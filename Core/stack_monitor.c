/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* TI includes. */
#include "ti_msp_dl_config.h"

/*-----------------------------------------------------------*/

#define STACK_MONITOR_TASK_PRIORITY (tskIDLE_PRIORITY + 1U)
#define STACK_MONITOR_TASK_STACK_WORDS configMINIMAL_STACK_SIZE
#define STACK_HIGH_WATER_THR (30U)

/*-----------------------------------------------------------*/

TaskHandle_t        xMonitorTaskHandle;
extern TaskHandle_t xControlTaskHandle;
extern TaskHandle_t xMotorEncoderUpdaterTaskHandle;
extern TaskHandle_t xHCSR04TaskHandle;
extern TaskHandle_t xDebugTaskHandle;
extern TaskHandle_t xTrackUpdateTaskHandle;

/*-----------------------------------------------------------*/

void        main_StackMonitor(void);
static void prvStackMonitorTask(void *argument);

/*-----------------------------------------------------------*/

static inline bool prvStackLow(TaskHandle_t xTaskHandle)
{
    // 通过检查高水位来判断是否可能发生栈溢出，注意这不是绝对的
    return uxTaskGetStackHighWaterMark(xTaskHandle) < STACK_HIGH_WATER_THR;
}

static void prvStackMonitorTask(void *argument)
{
    (void) argument;

    for (;;)
    {
        // 可在此处加入对更多任务的栈的监控
        if (prvStackLow(xMonitorTaskHandle) || prvStackLow(xControlTaskHandle) ||
            prvStackLow(xMotorEncoderUpdaterTaskHandle) || prvStackLow(xHCSR04TaskHandle) ||
            prvStackLow(xDebugTaskHandle) || prvStackLow(xTrackUpdateTaskHandle))
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
