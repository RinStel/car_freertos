/* TI includes. */
#include "ti_msp_dl_config.h"

#include "motor.h"

/*-----------------------------------------------------------*/

#define MOTOR_MAX_PWM_DUTY (1024 * 25 / 100) // 25% 占空比对应的计数值

/*-----------------------------------------------------------*/

static volatile EncoderData_t xEncoderLeft, xEncoderRight;

/*
 * 编码器的状态转换表，根据先前状态和当前状态计算增量
 * 状态编码:
 * - 0b00: E1A=0 E1B=0
 * - 0b01: E1A=0 E1B=1
 * - 0b10: E1A=1 E1B=0
 * - 0b11: E1A=1 E1B=1
 * 规定 A 相领先 B 相的状态转换为正向，反之为反向
 * 正向: 00 -> 10 -> 11 -> 01 -> 00
 * 反向: 00 -> 01 -> 11 -> 10 -> 00
 */
static const int8_t ucEncoderStateTable[4][4] = {
    {0, -1, 1, 0},
    {1, 0, 0, -1},
    {-1, 0, 0, 1},
    {0, 1, -1, 0},
};

/*-----------------------------------------------------------*/

#define abs(x) ((x) > 0 ? (x) : -(x))

void vMotorInit(void)
{
    DL_GPIO_clearPins(GPIO_MOTOR_PORT, GPIO_MOTOR_STBY_PIN);

    /* 初始化编码器类型与初始相位, 避免第一次中断产生计数偏移 */
    xEncoderLeft.type     = ENCODER_LEFT;
    xEncoderLeft.count    = 0;
    xEncoderLeft.preState = (DL_GPIO_readPins(GPIO_MOTOR_PORT, GPIO_MOTOR_E1A_PIN) ? 0b10U : 0) |
                            (DL_GPIO_readPins(GPIO_MOTOR_PORT, GPIO_MOTOR_E1B_PIN) ? 0b01U : 0);

    xEncoderRight.type     = ENCODER_RIGHT;
    xEncoderRight.count    = 0;
    xEncoderRight.preState = (DL_GPIO_readPins(GPIO_MOTOR_PORT, GPIO_MOTOR_E2A_PIN) ? 0b10U : 0) |
                             (DL_GPIO_readPins(GPIO_MOTOR_PORT, GPIO_MOTOR_E2B_PIN) ? 0b01U : 0);

    DL_TimerG_startCounter(MOTOR_PWM_INST);
}

void vMotorSetSpeed(int8_t left_speed, int8_t right_speed)
{
    if (left_speed > 0)
    {
        DL_GPIO_setPins(GPIO_MOTOR_PORT, GPIO_MOTOR_AIN1_PIN);
        DL_GPIO_clearPins(GPIO_MOTOR_PORT, GPIO_MOTOR_AIN2_PIN);
    }
    else if (left_speed < 0)
    {
        DL_GPIO_clearPins(GPIO_MOTOR_PORT, GPIO_MOTOR_AIN1_PIN);
        DL_GPIO_setPins(GPIO_MOTOR_PORT, GPIO_MOTOR_AIN2_PIN);
    }
    else
    {
        DL_GPIO_clearPins(GPIO_MOTOR_PORT, GPIO_MOTOR_AIN1_PIN | GPIO_MOTOR_AIN2_PIN);
    }

    if (right_speed > 0)
    {
        DL_GPIO_setPins(GPIO_MOTOR_PORT, GPIO_MOTOR_BIN1_PIN);
        DL_GPIO_clearPins(GPIO_MOTOR_PORT, GPIO_MOTOR_BIN2_PIN);
    }
    else if (right_speed < 0)
    {
        DL_GPIO_clearPins(GPIO_MOTOR_PORT, GPIO_MOTOR_BIN1_PIN);
        DL_GPIO_setPins(GPIO_MOTOR_PORT, GPIO_MOTOR_BIN2_PIN);
    }
    else
    {
        DL_GPIO_clearPins(GPIO_MOTOR_PORT, GPIO_MOTOR_BIN1_PIN | GPIO_MOTOR_BIN2_PIN);
    }

    DL_GPIO_setPins(GPIO_MOTOR_PORT, GPIO_MOTOR_STBY_PIN);

    DL_TimerG_setCaptureCompareValue(MOTOR_PWM_INST, abs(left_speed) * MOTOR_MAX_PWM_DUTY / 100,
                                     DL_TIMER_CC_0_INDEX);
    DL_TimerG_setCaptureCompareValue(MOTOR_PWM_INST, abs(right_speed) * MOTOR_MAX_PWM_DUTY / 100,
                                     DL_TIMER_CC_1_INDEX);
}

void vMotorStop(void)
{
    DL_GPIO_clearPins(GPIO_MOTOR_PORT, GPIO_MOTOR_STBY_PIN);
    DL_GPIO_clearPins(GPIO_MOTOR_PORT, GPIO_MOTOR_AIN1_PIN | GPIO_MOTOR_AIN2_PIN |
                                           GPIO_MOTOR_BIN1_PIN | GPIO_MOTOR_BIN2_PIN);
}

void prvMotorEncoderUpdate(volatile EncoderData_t *encoder)
{
    uint8_t currentState = 0;
    if (encoder->type == ENCODER_LEFT)
    {
        currentState |= DL_GPIO_readPins(GPIO_MOTOR_PORT, GPIO_MOTOR_E1A_PIN) ? 0b10U : 0;
        currentState |= DL_GPIO_readPins(GPIO_MOTOR_PORT, GPIO_MOTOR_E1B_PIN) ? 0b01U : 0;
    }
    else
    {
        currentState |= DL_GPIO_readPins(GPIO_MOTOR_PORT, GPIO_MOTOR_E2A_PIN) ? 0b10U : 0;
        currentState |= DL_GPIO_readPins(GPIO_MOTOR_PORT, GPIO_MOTOR_E2B_PIN) ? 0b01U : 0;
    }

    int8_t delta = ucEncoderStateTable[encoder->preState][currentState];
    encoder->count += delta;
    encoder->preState = currentState;
}

int32_t vMotorEncoderGetCount(Encoder_t type)
{
    if (type == ENCODER_LEFT)
    {
        return xEncoderLeft.count;
    }
    else
    {
        return xEncoderRight.count;
    }
}

void vMotorEncoderLeft_IRQHandler(void)
{
    prvMotorEncoderUpdate(&xEncoderLeft);
}

void vMotorEncoderRight_IRQHandler(void)
{
    prvMotorEncoderUpdate(&xEncoderRight);
}
