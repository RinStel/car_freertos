#include "stdio.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* TI includes. */
#include "ti_msp_dl_config.h"

#include "motor.h"
#include "motor_encoder.h"
#include "track.h"

/*-----------------------------------------------------------*/

TaskHandle_t xControlTaskHandle;

/*-----------------------------------------------------------*/

void main_Control(void);
void prvControlTask(void *argument);

/*-----------------------------------------------------------*/

void prvControlTask(void *argument)
{
    TickType_t       xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000 / 50);

    TrackData_t xTrackData = {
        .current_pos         = 0,
        .status              = TRACK_LINE_NORMAL,
        .last_turn_direction = 0,
    };

    // Test
    vMotorInit();
    int8_t speed = 40;
    // int8_t speed_step = 1;

    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Test
        vMotorSetSpeed(speed, speed);
        // speed += speed_step;
        // if (speed >= 100)
        // {
        //     speed      = 99;
        //     speed_step = -speed_step;
        // }
        // else if (speed <= -100)
        // {
        //     speed      = -99;
        //     speed_step = -speed_step;
        // }

        vTrackUpdate(&xTrackData);
        printf("%3d, ", xTrackData.current_pos);
        // printf("%u, ", vFreeTimerStart());
        printf("%ld, ", (long) xMotorEncoderData.left_dist);
        printf("%ld, ", (long) xMotorEncoderData.right_dist);
        printf("%ld, ", (long) xMotorEncoderData.left_speed);
        printf("%ld, ", (long) xMotorEncoderData.right_speed);
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
    BaseType_t status = xTaskCreate(prvControlTask, "Control", configMINIMAL_STACK_SIZE + 128, NULL,
                                    7, &xControlTaskHandle);
    configASSERT(status == pdPASS);
}
