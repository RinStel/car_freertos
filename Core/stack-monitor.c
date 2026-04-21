/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* TI includes. */
#include "ti_msp_dl_config.h"

/*-----------------------------------------------------------*/

#define STACK_MONITOR_TASK_PRIORITY (tskIDLE_PRIORITY + 1U)
#define STACK_MONITOR_TASK_STACK_WORDS configMINIMAL_STACK_SIZE
#define STACK_HIGH_THR 50

/*-----------------------------------------------------------*/

TaskHandle_t        xMonitorTaskHandle;
extern TaskHandle_t xControlTaskHandle;
extern TaskHandle_t xMotorEncoderUpdaterTaskHandle;
extern TaskHandle_t xHCSR04TaskHandle;

/*-----------------------------------------------------------*/

void        main_StackMonitor(void);
static void prvStackMonitorTask(void *argument);

/*-----------------------------------------------------------*/

static void prvStackMonitorTask(void *argument)
{
    (void) argument;

    for (;;)
    {
        // 可在此处加入对更多任务的栈的监控
        if (uxTaskGetStackHighWaterMark(NULL) < STACK_HIGH_THR ||
            uxTaskGetStackHighWaterMark(xControlTaskHandle) < STACK_HIGH_THR ||
            uxTaskGetStackHighWaterMark(xMotorEncoderUpdaterTaskHandle) < STACK_HIGH_THR ||
            uxTaskGetStackHighWaterMark(xHCSR04TaskHandle) < STACK_HIGH_THR)
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
    BaseType_t status =
        xTaskCreate(prvStackMonitorTask, "StackMonitor", STACK_MONITOR_TASK_STACK_WORDS, NULL,
                    STACK_MONITOR_TASK_PRIORITY, &xMonitorTaskHandle);
    configASSERT(status == pdPASS);
}
