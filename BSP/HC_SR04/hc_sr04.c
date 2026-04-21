#include "FreeRTOS.h"
#include "task.h"

#include "ti_msp_dl_config.h"

#include "free_timer.h"
#include "hc_sr04.h"

/*-----------------------------------------------------------*/

#define HCSR04_MAX_DISTANCE_MM (4000U)
#define ALPHA_FILTER 0.15f

/*-----------------------------------------------------------*/

TaskHandle_t xHCSR04TaskHandle;

/*-----------------------------------------------------------*/

static volatile FreeTimer_t xHCSR04EchoStartTime;
static volatile uint8_t     ucHCSR04EchoActive;
volatile uint16_t           usHCSR04Distance_mm;

/*-----------------------------------------------------------*/

static inline float_t prvLowPassFilter(float_t old_value, float_t new_value, float_t alpha)
{
    return (alpha) * (new_value) + (1 - (alpha)) * (old_value);
}

void vHCSR04Trigger(void)
{
    taskENTER_CRITICAL();
    ucHCSR04EchoActive = 0U;
    // 发送至少10us的高电平触发信号
    DL_GPIO_setPins(GPIO_HCSR04_PORT, GPIO_HCSR04_TRIG_PIN);
    delay_cycles(CPUCLK_FREQ / 1000000 * 10); // 10us 延时
    DL_GPIO_clearPins(GPIO_HCSR04_PORT, GPIO_HCSR04_TRIG_PIN);
    taskEXIT_CRITICAL();
}

void vHCSR04Echo_IRQHandler(void)
{
    static volatile uint16_t usHCSR04LastDistance_mm = 0U;

    if (DL_GPIO_readPins(GPIO_HCSR04_PORT, GPIO_HCSR04_ECHO_PIN))
    {
        // 上升沿，记录开始时间
        xHCSR04EchoStartTime = vFreeTimerStart();
        ucHCSR04EchoActive   = 1U;
    }
    else if (ucHCSR04EchoActive != 0U)
    {
        // 下降沿，计算距离
        uint16_t echoDuration_us = usFreeTimerGetElapsed(xHCSR04EchoStartTime);
        ucHCSR04EchoActive       = 0U;

        if (echoDuration_us <= (HCSR04_MAX_DISTANCE_MM * 2000U / 340U))
        {
            usHCSR04Distance_mm = (uint16_t) prvLowPassFilter(
                usHCSR04LastDistance_mm, ((float_t) echoDuration_us * 340 / 2000), ALPHA_FILTER);
        }
    }
}

static void prvHCSR04Task(void *argument)
{
    const TickType_t xFrequency = pdMS_TO_TICKS(1000 / 30); // 30Hz
    TickType_t       xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        vHCSR04Trigger();
    }
}

void main_HCSR04()
{
    BaseType_t status =
        xTaskCreate(prvHCSR04Task, "HCSR04", configMINIMAL_STACK_SIZE, NULL, 4, &xHCSR04TaskHandle);
    configASSERT(status == pdPASS);
}