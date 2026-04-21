#include "ti_msp_dl_config.h"

#include "free_timer.h"

#define TIMER_PERIOD ((uint16_t) (FREE_TIMER_INST_LOAD_VALUE + 1U))

FreeTimer_t vFreeTimerStart(void)
{
    return (FreeTimer_t) DL_Timer_getTimerCount(FREE_TIMER_INST);
}

uint16_t usFreeTimerGetElapsed(FreeTimer_t start_time)
{
    uint16_t current_time = DL_Timer_getTimerCount(FREE_TIMER_INST);

    if (current_time >= start_time)
    {
        return current_time - start_time;
    }

    return (uint16_t) (current_time + TIMER_PERIOD - start_time);
}
