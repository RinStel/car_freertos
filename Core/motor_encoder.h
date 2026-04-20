#ifndef MOTOR_ENCODER_H
#define MOTOR_ENCODER_H

#include "ti_msp_dl_config.h"

typedef struct
{
    int32_t left_count;
    int32_t left_dist;
    float_t left_speed;

    int32_t right_count;
    int32_t right_dist;
    float_t right_speed;
} MotorEncoderData_t;

extern MotorEncoderData_t xMotorEncoderData;

void main_MotorEncoderUpdater(void);

#endif /* MOTOR_ENCODER_H */