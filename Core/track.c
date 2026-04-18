/* TI includes. */
#include "ti_msp_dl_config.h"

#include "BSP/gpio/gpio.h"
#include <stdint.h>

#include "track.h"

/*-----------------------------------------------------------*/

#define DEBOUNCE_THRESHOLD 3

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

void vTrackUpdate(TrackData_t *xTrackData)
{
    xTrackData->_last_pos    = xTrackData->current_pos;
    xTrackData->_last_status = xTrackData->status;

    uint8_t ucDebounced = prvDebounceUpdate(ucTraceReadData());

    // 从中间开始，向两侧寻找最近的有效点
    int8_t center_idx = GPIO_TRACE_SENSOR_NUM / 2;
    int8_t index      = center_idx;
    while (index >= 0)
    {
        if (ucDebounced & (1 << index))
        {
            break;
        }
        else if (ucDebounced & (1 << (GPIO_TRACE_SENSOR_NUM - 1 - index)))
        {
            // 在另一侧找到有效点
            index = GPIO_TRACE_SENSOR_NUM - 1 - index;
            break;
        }
        index--;
    }
    if (index < 0)
    {
        // 没有找到有效点，保持上次位置不变
        xTrackData->status = TRACK_LINE_LOST;
        return;
    }

    // 计算并平均距离中心点最近的一团信号
    int8_t  left_index     = index + 1;
    int8_t  right_index    = index - 1;
    int16_t pos_weight_sum = (index - center_idx) * TRACK_SENSOR_SPACE;
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
        xTrackData->status = TRACK_LINE_NORMAL;
        break;
    case TRACK_LINE_LEFT_TURN:
    case TRACK_LINE_RIGHT_TURN:
        if (left_index == GPIO_TRACE_SENSOR_NUM && right_index == -1)
        {
            xTrackData->status = TRACK_LINE_CROSS;
        }
        else if (abs(xTrackData->current_pos) <= TRACK_SENSOR_SPACE)
        {
            xTrackData->status = TRACK_LINE_NORMAL;
        }
        else
        {
            xTrackData->status = xTrackData->_last_status;
        }
        break;
    case TRACK_LINE_CROSS:
        if (left_index != GPIO_TRACE_SENSOR_NUM && right_index != -1 &&
            abs(xTrackData->current_pos) > TRACK_SENSOR_SPACE)
        {
            xTrackData->status =
                (xTrackData->current_pos < 0) ? TRACK_LINE_LEFT_TURN : TRACK_LINE_RIGHT_TURN;
        }
        else
        {
            xTrackData->status = TRACK_LINE_CROSS;
        }
        break;
    case TRACK_LINE_NORMAL:
        if (left_index == GPIO_TRACE_SENSOR_NUM && right_index != -1)
        {
            xTrackData->status = TRACK_LINE_LEFT_TURN;
        }
        else if (left_index != GPIO_TRACE_SENSOR_NUM && right_index == -1)
        {
            xTrackData->status = TRACK_LINE_RIGHT_TURN;
        }
        else
        {
            xTrackData->status = TRACK_LINE_NORMAL;
        }
        break;
    }
}

/*-----------------------------------------------------------*/