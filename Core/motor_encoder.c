/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* TI includes. */
#include "ti_msp_dl_config.h"

#include "motor.h"
#include "motor_encoder.h"

/*-----------------------------------------------------------*/

MotorEncoderData_t xMotorEncoderData;

static void prvMotorEncoderUpdater(void *argument)
{
    MotorEncoderData_t *pxMotorEncoderData = (MotorEncoderData_t *) argument;

    TickType_t       xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000 / 100);

    int32_t last_left_count  = 0;
    int32_t last_right_count = 0;

    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);

        pxMotorEncoderData->left_dist  = vMotorEncoderGetCount(ENCODER_LEFT);
        pxMotorEncoderData->right_dist = vMotorEncoderGetCount(ENCODER_RIGHT);

        float_t tick_to_hz = (float_t) configTICK_RATE_HZ / (float_t) xFrequency;

        pxMotorEncoderData->left_speed =
            (float_t) (pxMotorEncoderData->left_dist - last_left_count) * tick_to_hz;
        pxMotorEncoderData->right_speed =
            (float_t) (pxMotorEncoderData->right_dist - last_right_count) * tick_to_hz;

        last_left_count  = pxMotorEncoderData->left_dist;
        last_right_count = pxMotorEncoderData->right_dist;
    }
}

void main_MotorEncoderUpdater(void)
{
    BaseType_t status = xTaskCreate(prvMotorEncoderUpdater, "MotorEncoderUpdater", 768,
                                    &xMotorEncoderData, 6, NULL);
    configASSERT(status == pdPASS);
}
