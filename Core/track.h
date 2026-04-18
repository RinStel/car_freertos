#ifndef TRACK_H
#define TRACK_H

/* TI includes. */
#include "ti_msp_dl_config.h"

#include "BSP/gpio/gpio.h"

/*-----------------------------------------------------------*/

typedef struct
{
    int16_t current_pos;
    int16_t next_pred_pos;
    uint8_t is_low_confidence;
} TrackData_t;

/*-----------------------------------------------------------*/

// 获取处理后的 Track 数据
void vTrackUpdate(TrackData_t *xTrackData);

/*-----------------------------------------------------------*/

#endif /* TRACK_H */
