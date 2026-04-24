#ifndef PID_CONTROL_H
#define PID_CONTROL_H

#include <math.h>

#define PID_INTEGRAL_DECAY_FACTOR 0.9995f

typedef struct
{
    float_t kp;
    float_t ki;
    float_t kd;

    float_t min_output;
    float_t max_output;

    float_t last_error;
    float_t integral;

    float_t output;
} PID_Contorl_t;

void    vPIDInit(PID_Contorl_t *pxPid, float_t kp, float_t ki, float_t kd, float_t min_output,
                 float_t max_output);
void    vPIDReset(PID_Contorl_t *pxPid);
float_t fPIDStep(PID_Contorl_t *pxPid, float_t target, float_t measured);

#endif // PID_CONTROL_H