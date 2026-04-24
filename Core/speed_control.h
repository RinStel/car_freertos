#ifndef SPEED_CONTROL_H
#define SPEED_CONTROL_H

#include <math.h>

void vMotorSpeedInit(void);
void vMotorSpeedSet(float_t left_speed_mmps, float_t right_speed_mmps);

#endif /* SPEED_CONTROL_H */
