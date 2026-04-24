#ifndef MOTOR_H
#define MOTOR_H

// #include "ti_msp_dl_config.h"
#include <stdint.h>

/*-----------------------------------------------------------*/

#define TIMER_PERIOD (4000)                                       // PWM周期
#define MOTOR_MAX_PWM_DUTY ((uint16_t) (50 * TIMER_PERIOD / 100)) // 50% 占空比对应的计数值

/*-----------------------------------------------------------*/

typedef enum
{
    ENCODER_LEFT = 0,
    ENCODER_RIGHT
} EncoderInst_t;

typedef struct
{
    EncoderInst_t inst;
    int32_t       count;
    uint8_t       preState;
} EncoderData_t;

/*-----------------------------------------------------------*/

void vMotorInit(void);
void vMotorSetPWM(int8_t left_speed, int8_t right_speed);
void vMotorStop(void);

/*-----------------------------------------------------------*/

int32_t vMotorEncoderGetCount(EncoderInst_t type);

void vMotorEncoderLeft_IRQHandler(void);
void vMotorEncoderRight_IRQHandler(void);

#endif /* MOTOR_H */