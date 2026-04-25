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
    int8_t           current_pos; // 线目前所在的位置，左负右正
    TrackLineState_t status;
    float_t left_speed_offset; // 左侧速度的叠加偏移，单位是与原速度的比率
    float_t right_speed_offset;

    int8_t           _last_pos;
    TrackLineState_t _last_status;
} TrackData_t;

/*-----------------------------------------------------------*/

// 获取处理后的 Track 数据
void vTrackUpdate(volatile TrackData_t *xTrackData);

/*-----------------------------------------------------------*/

#endif /* TRACK_H */
