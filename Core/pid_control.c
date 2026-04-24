#include "pid_control.h"
#include "utils.h"

void vPIDInit(PID_Contorl_t *pxPid, float_t kp, float_t ki, float_t kd, float_t min_output,
              float_t max_output)
{
    pxPid->kp         = kp;
    pxPid->ki         = ki;
    pxPid->kd         = kd;
    pxPid->min_output = min_output;
    pxPid->max_output = max_output;

    vPIDReset(pxPid);
}

void vPIDReset(PID_Contorl_t *pxPid)
{
    pxPid->last_error = 0.0f;
    pxPid->integral   = 0.0f;
    pxPid->output     = 0.0f;
}

float_t fPIDStep(PID_Contorl_t *pxPid, float_t target, float_t measured)
{
    float_t error = target - measured;

    pxPid->integral = pxPid->integral * PID_INTEGRAL_DECAY_FACTOR + error;

    float_t output =
        pxPid->kp * error + pxPid->ki * pxPid->integral + pxPid->kd * (error - pxPid->last_error);
    output = fClamp(output, pxPid->min_output, pxPid->max_output);

    pxPid->last_error = error;
    pxPid->output     = output;

    return output;
}
