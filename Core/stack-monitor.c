/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* TI includes. */
#include "ti_msp_dl_config.h"

/*-----------------------------------------------------------*/

/*
 * 一些句柄
 */
TaskHandle_t xMonitorTaskHandle;

/*-----------------------------------------------------------*/

void prvStackMonitorTask(void *argument);

void main_StackMonitor(void);

/*-----------------------------------------------------------*/

void prvStackMonitorTask(void *argument)
{
    DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_STACK_WARNING_LED_PIN);
    vTaskDelay(pdMS_TO_TICKS(500));
    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_STACK_WARNING_LED_PIN);
    vTaskDelay(pdMS_TO_TICKS(500));
    DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_STACK_WARNING_LED_PIN);
    vTaskDelay(pdMS_TO_TICKS(500));
    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_STACK_WARNING_LED_PIN);
    vTaskDelay(pdMS_TO_TICKS(500));
    DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_STACK_WARNING_LED_PIN);
    vTaskDelay(pdMS_TO_TICKS(500));
    DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_STACK_WARNING_LED_PIN);

    for (;;)
    {
        // 可在此处加入对更多任务的栈的监控
        if (uxTaskGetStackHighWaterMark(NULL) < 50)
        {
            DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_STACK_WARNING_LED_PIN);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void main_StackMonitor(void)
{
    xTaskCreate(prvStackMonitorTask, "StackMonitor", configMINIMAL_STACK_SIZE, NULL,
                tskIDLE_PRIORITY + 1, &xMonitorTaskHandle);
}