#include "speed_control.h"
#include "motor.h"
#include "motor_encoder.h"
#include "pid_control.h"

static PID_Contorl_t xMotorSpeedPidLeft, xMotorSpeedPidRight;

void vMotorSpeedInit(void)
{
    vMotorInit();
    vPIDInit(&xMotorSpeedPidLeft, 12.5f, 0.9f, 15.0f, -MOTOR_MAX_PWM_DUTY, MOTOR_MAX_PWM_DUTY,
             0.99995f);
    vPIDInit(&xMotorSpeedPidRight, 12.5f, 0.9f, 15.0f, -MOTOR_MAX_PWM_DUTY, MOTOR_MAX_PWM_DUTY,
             0.99995f);
}

void vMotorSpeedSet(float_t left_speed_mmps, float_t right_speed_mmps)
{
    int16_t left_pwm =
        (int16_t) fPIDStep(&xMotorSpeedPidLeft, left_speed_mmps, xMotorEncoderData.left_speed);
    int16_t right_pwm =
        (int16_t) fPIDStep(&xMotorSpeedPidRight, right_speed_mmps, xMotorEncoderData.right_speed);

    vMotorSetPWM(left_pwm, right_pwm);
}
