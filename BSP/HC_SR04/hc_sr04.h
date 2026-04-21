#ifndef HC_SR04_H
#define HC_SR04_H

#include "ti_msp_dl_config.h"

/*-----------------------------------------------------------*/

extern volatile uint16_t usHCSR04Distance_mm;

/*-----------------------------------------------------------*/

void vHCSR04Trigger(void);
void vHCSR04Echo_IRQHandler(void);

/*-----------------------------------------------------------*/

void main_HCSR04(void);

#endif /* HC_SR04_H */
