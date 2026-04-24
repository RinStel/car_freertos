#include "stdio.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* TI includes. */
#include "ti_msp_dl_config.h"

#include "hc_sr04.h"
#include "motor_encoder.h"
#include "speed_control.h"
#include "track.h"

/*-----------------------------------------------------------*/

#define CONTROL_TASK_PRIORITY 7U
#define CONTROL_TASK_STACK_WORDS (configMINIMAL_STACK_SIZE)
#define CONTROL_TASK_FREQUENCY_HZ 100U

#define DEBUG_TASK_PRIORITY (tskIDLE_PRIORITY + 2)
#define DEBUG_TASK_STACK_WORDS (configMINIMAL_STACK_SIZE + 128U)
#define DEBUG_TASK_FREQUENCY_HZ 2U

/*-----------------------------------------------------------*/

TaskHandle_t xControlTaskHandle;
TaskHandle_t xDebugTaskHandle;

volatile TrackData_t xTrackData = {
    .current_pos = 0,
    .status      = TRACK_LINE_NORMAL,
};

/*-----------------------------------------------------------*/

void main_Control(void);
void prvControlTask(void *argument);

/*-----------------------------------------------------------*/

void prvControlTask(void *argument)
{
    (void) argument;

    TickType_t       xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000 / CONTROL_TASK_FREQUENCY_HZ);

    // Test
    vMotorSpeedInit();
    float_t speed_mmps = 100; // 10 cm/s

    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Test
        vMotorSpeedSet(speed_mmps * (1.0f + xTrackData.left_speed_offset),
                       speed_mmps * (1.0f + xTrackData.right_speed_offset));
        vTrackUpdate(&xTrackData);
    }
}

void prvDebugTask(void *argument)
{
    (void) argument;

    TickType_t       xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000 / 2);

    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);

        printf("%3d, ", xTrackData.current_pos);
        // printf("%u, ", vFreeTimerStart());
        printf("%ld, ", (long) xMotorEncoderData.left_dist);
        printf("%ld, ", (long) xMotorEncoderData.right_dist);
        printf("%ld, ", (long) xMotorEncoderData.left_speed);
        printf("%ld, ", (long) xMotorEncoderData.right_speed);
        printf("%d, ", usHCSR04Distance_mm);
        switch (xTrackData.status)
        {
        case TRACK_LINE_NORMAL:
            printf("NORMAL\n");
            break;
        case TRACK_LINE_LEFT:
            printf("LEFT\n");
            break;
        case TRACK_LINE_RIGHT:
            printf("RIGHT\n");
            break;
        case TRACK_LINE_LEFT_TURN:
            printf("LEFT_TURN\n");
            break;
        case TRACK_LINE_RIGHT_TURN:
            printf("RIGHT_TURN\n");
            break;
        case TRACK_LINE_CROSS:
            printf("CROSS\n");
            break;
        case TRACK_LINE_LOST:
            printf("LOST\n");
            break;
        }
    }
}

void main_Control(void)
{
    BaseType_t status = xTaskCreate(prvControlTask, "Control", configMINIMAL_STACK_SIZE, NULL,
                                    CONTROL_TASK_PRIORITY, &xControlTaskHandle);
    configASSERT(status == pdPASS);

    status = xTaskCreate(prvDebugTask, "Debug", DEBUG_TASK_STACK_WORDS, NULL, DEBUG_TASK_PRIORITY,
                         &xDebugTaskHandle);
    configASSERT(status == pdPASS);
}
