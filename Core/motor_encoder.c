/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* TI includes. */
#include "ti_msp_dl_config.h"

#include "motor.h"
#include "motor_encoder.h"

/*-----------------------------------------------------------*/

#define MOTOR_ENCODER_TASK_PRIORITY 8U
#define MOTOR_ENCODER_TASK_STACK_WORDS (configMINIMAL_STACK_SIZE)
#define MOTOR_ENCODER_TASK_FREQUENCY_HZ (250U)

/*-----------------------------------------------------------*/

// 500线GMR编码器
// 使用AB两相并在上升/下降沿都计数（x4），每圈计数=500*4=2000
// 编码器连接在电机轴上，减速比1:28，则每转车轮计数=2000*28=56000
#define MOTOR_ENCODER_COUNTS_PER_WHEEL_REV (500 * 4 * 28)

// 轮子半径，单位毫米
#define WHEEL_RADIUS (32.5f)

#define LOW_PASS_FILTER_ALPHA 0.2f
#define PI (3.14159265358979323846f)

MotorEncoderData_t xMotorEncoderData;

TaskHandle_t xMotorEncoderUpdaterTaskHandle;

/*-----------------------------------------------------------*/

static inline float_t prvLowPassFilter(float_t old_value, float_t new_value, float_t alpha)
{
    return (alpha) * (new_value) + (1 - (alpha)) * (old_value);
}

static inline float_t prvEncoderCountToDistance_mm(int32_t count)
{
    const float_t rev  = (float_t) count / MOTOR_ENCODER_COUNTS_PER_WHEEL_REV; // 转数
    const float_t c_mm = 2.0f * PI * WHEEL_RADIUS; // 轮子周长，单位毫米

    return rev * c_mm;
}

static inline float_t prvEncoderDeltaCountToSpeed_mmps(int32_t deltaCount)
{
    static const float_t dt_s = (float_t) pdMS_TO_TICKS(1000 / MOTOR_ENCODER_TASK_FREQUENCY_HZ) /
                                (float_t) configTICK_RATE_HZ;
    return prvEncoderCountToDistance_mm(deltaCount) / dt_s;
}

static void prvMotorEncoderUpdater(void *argument)
{
    MotorEncoderData_t *pxMotorEncoderData = (MotorEncoderData_t *) argument;

    int32_t last_left_count  = vMotorEncoderGetCount(ENCODER_LEFT);
    int32_t last_right_count = vMotorEncoderGetCount(ENCODER_RIGHT);

    pxMotorEncoderData->left_count  = last_left_count;
    pxMotorEncoderData->right_count = last_right_count;
    pxMotorEncoderData->left_speed  = 0;
    pxMotorEncoderData->right_speed = 0;

    const TickType_t xFrequency    = pdMS_TO_TICKS(1000 / MOTOR_ENCODER_TASK_FREQUENCY_HZ);
    TickType_t       xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        xTaskDelayUntil(&xLastWakeTime, xFrequency);

        pxMotorEncoderData->left_count  = vMotorEncoderGetCount(ENCODER_LEFT);
        pxMotorEncoderData->right_count = vMotorEncoderGetCount(ENCODER_RIGHT);

        pxMotorEncoderData->left_dist =
            prvEncoderCountToDistance_mm(pxMotorEncoderData->left_count);
        pxMotorEncoderData->right_dist =
            prvEncoderCountToDistance_mm(pxMotorEncoderData->right_count);

        float_t now_left_speed =
            prvEncoderDeltaCountToSpeed_mmps(pxMotorEncoderData->left_count - last_left_count);
        float_t now_right_speed =
            prvEncoderDeltaCountToSpeed_mmps(pxMotorEncoderData->right_count - last_right_count);

        pxMotorEncoderData->left_speed =
            prvLowPassFilter(pxMotorEncoderData->left_speed, now_left_speed, LOW_PASS_FILTER_ALPHA);
        pxMotorEncoderData->right_speed = prvLowPassFilter(pxMotorEncoderData->right_speed,
                                                           now_right_speed, LOW_PASS_FILTER_ALPHA);

        last_left_count  = pxMotorEncoderData->left_count;
        last_right_count = pxMotorEncoderData->right_count;
    }
}

void main_MotorEncoderUpdater(void)
{
    BaseType_t status = xTaskCreate(prvMotorEncoderUpdater, "MotorEncoder",
                                    MOTOR_ENCODER_TASK_STACK_WORDS, &xMotorEncoderData,
                                    MOTOR_ENCODER_TASK_PRIORITY, &xMotorEncoderUpdaterTaskHandle);
    configASSERT(status == pdPASS);
}
