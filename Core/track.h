#ifndef TRACK_H
#define TRACK_H

/* TI includes. */
#include "ti_msp_dl_config.h"

#include "BSP/gpio/gpio.h"

/*-----------------------------------------------------------*/

typedef enum
{
    TRACK_LINE_NORMAL = 0,
    TRACK_LINE_LEFT,
    TRACK_LINE_RIGHT,
    TRACK_LINE_LEFT_TURN,
    TRACK_LINE_RIGHT_TURN,
    TRACK_LINE_CROSS,
    TRACK_LINE_LOST
} TrackLineState_t;

typedef struct
{
    int8_t           current_pos;
    TrackLineState_t status;
    int8_t          last_turn_direction; // 检测到的最后一次可能的转弯方向，0:无，>0:左，<0:右

    int8_t           _last_pos;
    TrackLineState_t _last_status;
} TrackData_t;

/*-----------------------------------------------------------*/

// 获取处理后的 Track 数据
void vTrackUpdate(TrackData_t *xTrackData);

/*-----------------------------------------------------------*/

#endif /* TRACK_H */
