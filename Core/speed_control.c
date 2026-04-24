#include "speed_control.h"
#include "motor.h"
#include "motor_encoder.h"
#include "pid_control.h"

static PID_Contorl_t xMotorSpeedPidLeft, xMotorSpeedPidRight;

void vMotorSpeedInit(void)
{
    vMotorInit();
    vPIDInit(&xMotorSpeedPidLeft, 6.0f, 0.3f, 0.0f, -MOTOR_MAX_PWM_DUTY, MOTOR_MAX_PWM_DUTY,
             0.9999f);
    vPIDInit(&xMotorSpeedPidRight, 6.0f, 0.3f, 0.0f, -MOTOR_MAX_PWM_DUTY, MOTOR_MAX_PWM_DUTY,
             0.9999f);
}

void vMotorSpeedSet(float_t left_speed_mmps, float_t right_speed_mmps)
{
    int8_t left_pwm =
        (int8_t) fPIDStep(&xMotorSpeedPidLeft, left_speed_mmps, xMotorEncoderData.left_speed);
    int8_t right_pwm =
        (int8_t) fPIDStep(&xMotorSpeedPidRight, right_speed_mmps, xMotorEncoderData.right_speed);

    vMotorSetPWM(left_pwm, right_pwm);
}
