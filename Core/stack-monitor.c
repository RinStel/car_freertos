/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* TI includes. */
#include "ti_msp_dl_config.h"

/*-----------------------------------------------------------*/

#define STACK_HIGH_WATER_MARK_THRESHOLD 50

/*-----------------------------------------------------------*/

TaskHandle_t        xMonitorTaskHandle;
extern TaskHandle_t xControlTaskHandle;
extern TaskHandle_t xEncoderUpdaterTaskHandle;

/*-----------------------------------------------------------*/

void        main_StackMonitor(void);
static void prvStackMonitorTask(void *argument);

/*-----------------------------------------------------------*/

static void prvStackMonitorTask(void *argument)
{
    for (;;)
    {
        // 可在此处加入对更多任务的栈的监控
        if (uxTaskGetStackHighWaterMark(NULL) < STACK_HIGH_WATER_MARK_THRESHOLD ||
            uxTaskGetStackHighWaterMark(xControlTaskHandle) < STACK_HIGH_WATER_MARK_THRESHOLD ||
            (xEncoderUpdaterTaskHandle != NULL &&
             uxTaskGetStackHighWaterMark(xEncoderUpdaterTaskHandle) <
                 STACK_HIGH_WATER_MARK_THRESHOLD))
        {
            DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_STACK_WARNING_LED_PIN);
        }
        else
        {
            DL_GPIO_togglePins(GPIO_LEDS_PORT, GPIO_LEDS_STACK_WARNING_LED_PIN);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void main_StackMonitor(void)
{
    BaseType_t status = xTaskCreate(prvStackMonitorTask, "StackMonitor", configMINIMAL_STACK_SIZE,
                                    NULL, tskIDLE_PRIORITY + 1, &xMonitorTaskHandle);
    configASSERT(status == pdPASS);
}