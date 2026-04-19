#ifndef MOTOR_H
#define MOTOR_H

/* TI includes. */
#include "ti_msp_dl_config.h"

/*-----------------------------------------------------------*/

typedef enum
{
    ENCODER_LEFT = 0,
    ENCODER_RIGHT
} Encoder_t;

typedef struct
{
    Encoder_t type;
    uint32_t  count;
    uint8_t   preState;
} EncoderData_t;

/*-----------------------------------------------------------*/

void vMotorInit(void);
void vMotorSetSpeed(int8_t left_speed, int8_t right_speed);
void vMotorStop(void);

/*-----------------------------------------------------------*/

uint32_t vMotorEncoderGetCount(Encoder_t type);

void vMotorEncoderLeft_IRQHandler(void);
void vMotorEncoderRight_IRQHandler(void);

#endif /* MOTOR_H */