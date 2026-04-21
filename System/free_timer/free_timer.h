#ifndef FREE_TIMER_H
#define FREE_TIMER_H

/*
 * 自由定时器，用于实现精确的微秒级计时
 */

#include "ti_msp_dl_config.h"

typedef uint16_t FreeTimer_t;

FreeTimer_t vFreeTimerStart(void);
// 获取时间
uint16_t usFreeTimerGetElapsed(FreeTimer_t start_time);

#endif /* FREE_TIMER_H */
