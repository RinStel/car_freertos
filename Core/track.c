/* TI includes. */
#include "ti_msp_dl_config.h"

#include "gpio.h"
#include <stdint.h>

#include "pid_control.h"
#include "track.h"

/*-----------------------------------------------------------*/

#define DEBOUNCE_THRESHOLD 2

// 传感器的间距，返回的 current_pos 就是据此推算的，注意单侧传感器数量乘上这个数不能超过127
#define TRACK_SENSOR_SPACE 20

#define abs(x) ((x) > 0 ? (x) : -(x))

/*-----------------------------------------------------------*/

// 依次单路积分去抖
static uint8_t prvDebounceUpdate(uint8_t ucTrace);

typedef struct
{
    uint8_t count;
    uint8_t state;
} DebonceState_t;

PID_Contorl_t xTrackPID = {.kp                    = 0.013f,
                           .ki                    = 0.0f,
                           .kd                    = 0.01f,
                           .integral_decay_factor = 1.0f,
                           .min_output            = -1.0f,
                           .max_output            = 1.0f,
                           .last_error            = 0.0f,
                           .integral              = 0.0f,
                           .output                = 0.0f};

/*-----------------------------------------------------------*/

static uint8_t prvDebounceUpdate(uint8_t ucTrace)
{
    static DebonceState_t db_state[GPIO_TRACE_SENSOR_NUM];
    uint8_t               debounced = 0;

    for (int i = 0; i < GPIO_TRACE_SENSOR_NUM; i++)
    {
        if (ucTrace & (1 << i))
        {
            if (db_state[i].count < DEBOUNCE_THRESHOLD)
            {
                db_state[i].count++;
            }
        }
        else
        {
            if (db_state[i].count > 0)
            {
                db_state[i].count--;
            }
        }

        if (db_state[i].count >= DEBOUNCE_THRESHOLD)
        {
            db_state[i].state = 1;
        }
        else if (db_state[i].count == 0)
        {
            db_state[i].state = 0;
        }
        debounced |= (db_state[i].state << i);
    }

    return debounced;
}

void vTrackUpdate(volatile TrackData_t *xTrackData)
{
    xTrackData->_last_pos    = xTrackData->current_pos;
    xTrackData->_last_status = xTrackData->status;

    uint8_t ucDebounced = prvDebounceUpdate(ucTraceReadData());

    // 从中间开始，向两侧寻找最近的有效点
    int8_t center_idx  = GPIO_TRACE_SENSOR_NUM / 2;
    int8_t closest_idx = center_idx;
    while (closest_idx >= 0)
    {
        if (ucDebounced & (1 << closest_idx))
        {
            break;
        }
        else if (ucDebounced & (1 << (GPIO_TRACE_SENSOR_NUM - 1 - closest_idx)))
        {
            // 在另一侧找到有效点
            closest_idx = GPIO_TRACE_SENSOR_NUM - 1 - closest_idx;
            break;
        }
        closest_idx--;
    }

    // 计算并平均有效点旁的信号
    int8_t  left_index     = closest_idx + 1;
    int8_t  right_index    = closest_idx - 1;
    int16_t pos_weight_sum = (closest_idx - center_idx) * TRACK_SENSOR_SPACE;
    while (left_index < GPIO_TRACE_SENSOR_NUM && ucDebounced & (1 << left_index))
    {
        pos_weight_sum += (left_index - center_idx) * TRACK_SENSOR_SPACE;
        left_index++;
    }
    while (right_index >= 0 && ucDebounced & (1 << right_index))
    {
        pos_weight_sum += (right_index - center_idx) * TRACK_SENSOR_SPACE;
        right_index--;
    }
    xTrackData->current_pos = pos_weight_sum / (left_index - right_index - 1);

    // 简易的左右转判定（ToDo）
    switch (xTrackData->_last_status)
    {
    case TRACK_LINE_LOST:
        if (closest_idx >= 0)
        {
            xTrackData->status = TRACK_LINE_NORMAL;
        }
        break;
    case TRACK_LINE_LEFT:
        if (closest_idx < 0)
        {
            xTrackData->status = TRACK_LINE_LEFT_TURN;
            break;
        }
    case TRACK_LINE_RIGHT:
        if (closest_idx < 0)
        {
            xTrackData->status = TRACK_LINE_RIGHT_TURN;
            break;
        }

        if (left_index == GPIO_TRACE_SENSOR_NUM && right_index == -1)
        {
            xTrackData->status = TRACK_LINE_CROSS;
        }
        else if (abs(xTrackData->current_pos) <= TRACK_SENSOR_SPACE)
        {
            xTrackData->status = TRACK_LINE_NORMAL;
        }
        break;
    case TRACK_LINE_LEFT_TURN:
    case TRACK_LINE_RIGHT_TURN:
        if (abs(xTrackData->current_pos) <= TRACK_SENSOR_SPACE)
        {
            xTrackData->status = TRACK_LINE_NORMAL;
        }
        break;
    case TRACK_LINE_CROSS:
        // if (left_index != GPIO_TRACE_SENSOR_NUM && right_index != -1 &&
        //     abs(xTrackData->current_pos) > TRACK_SENSOR_SPACE)
        // {
        //     xTrackData->status = (xTrackData->current_pos < 0) ? TRACK_LINE_LEFT :
        //     TRACK_LINE_RIGHT;
        // }
        if (!(left_index == GPIO_TRACE_SENSOR_NUM && right_index == -1))
        {
            xTrackData->status = TRACK_LINE_NORMAL;
        }
        break;
    case TRACK_LINE_NORMAL:
        if (closest_idx < 0)
        {
            xTrackData->status = TRACK_LINE_LOST;
            break;
        }

        if (left_index == GPIO_TRACE_SENSOR_NUM && right_index == -1)
        {
            xTrackData->status = TRACK_LINE_CROSS;
        }
        else if (left_index == GPIO_TRACE_SENSOR_NUM && right_index != -1)
        {
            xTrackData->status = TRACK_LINE_LEFT;
        }
        else if (left_index != GPIO_TRACE_SENSOR_NUM && right_index == -1)
        {
            xTrackData->status = TRACK_LINE_RIGHT;
        }
        break;
    }

    // 计算左右两侧的轮速差
    xTrackData->left_speed_offset  = 0.0f;
    xTrackData->right_speed_offset = 0.0f;
    if (xTrackData->status == TRACK_LINE_LOST)
    {
        xTrackData->left_speed_offset  = -0.8f;
        xTrackData->right_speed_offset = -0.8f;
        vPIDReset(&xTrackPID);
    }
    else if (xTrackData->status == TRACK_LINE_LEFT_TURN)
    {
        xTrackData->left_speed_offset  = -1.0f;
        xTrackData->right_speed_offset = 0.0f;
        vPIDReset(&xTrackPID);
    }
    else if (xTrackData->status == TRACK_LINE_RIGHT_TURN)
    {
        xTrackData->left_speed_offset  = 0.0f;
        xTrackData->right_speed_offset = -1.0f;
        vPIDReset(&xTrackPID);
    }
    else
    {
        fPIDStep(&xTrackPID, 0, xTrackData->current_pos);
        if (xTrackPID.output < 0)
        {
            xTrackData->left_speed_offset = xTrackPID.output;
        }
        else
        {
            xTrackData->right_speed_offset = -xTrackPID.output;
        }
    }
}

/*-----------------------------------------------------------*/