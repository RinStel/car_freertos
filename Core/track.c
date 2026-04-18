/* TI includes. */
#include "ti_msp_dl_config.h"

#include "BSP/gpio/gpio.h"

#include "track.h"

/*-----------------------------------------------------------*/

#define DEBOUNCE_THRESHOLD 3

/*-----------------------------------------------------------*/

// 依次单路积分去抖
static uint8_t prvDebounceUpdate(uint8_t ucTraceData);

typedef struct
{
    uint8_t count;
    uint8_t state;
} DebonceState_t;

/*-----------------------------------------------------------*/

static uint8_t prvDebounceUpdate(uint8_t ucTraceData)
{
    static DebonceState_t db_state[GPIO_TRACE_SENSOR_NUM];
    uint8_t               debouncedData = 0;

    for (int i = 0; i < GPIO_TRACE_SENSOR_NUM; i++)
    {
        if (ucTraceData & (1 << i))
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
        debouncedData |= (db_state[i].state << i);
    }

    return debouncedData;
}

void vTrackUpdate(TrackData_t *xTrackData)
{
    uint8_t ucDebouncedData = prvDebounceUpdate(ucTraceReadData());

    xTrackData->current_pos       = ucDebouncedData; // ToDo
    xTrackData->next_pred_pos     = 0;               // TODO: 实现预测功能
    xTrackData->is_low_confidence = 0;               // TODO: 实现置信度评估功能
}

/*-----------------------------------------------------------*/