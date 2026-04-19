/* TI includes. */
#include "ti_msp_dl_config.h"

#include "gpio.h"

#include "motor.h"

/*-----------------------------------------------------------*/

uint8_t ucTraceReadData(void)
{
    uint8_t ucTraceData = 0;

    // 最高位保留为 0
    ucTraceData |= DL_GPIO_readPins(GPIO_TRACE_L3_PORT, GPIO_TRACE_L3_PIN) ? (1 << 6) : 0;
    ucTraceData |= DL_GPIO_readPins(GPIO_TRACE_L2_PORT, GPIO_TRACE_L2_PIN) ? (1 << 5) : 0;
    ucTraceData |= DL_GPIO_readPins(GPIO_TRACE_L1_PORT, GPIO_TRACE_L1_PIN) ? (1 << 4) : 0;
    ucTraceData |= DL_GPIO_readPins(GPIO_TRACE_C_PORT, GPIO_TRACE_C_PIN) ? (1 << 3) : 0;
    ucTraceData |= DL_GPIO_readPins(GPIO_TRACE_R1_PORT, GPIO_TRACE_R1_PIN) ? (1 << 2) : 0;
    ucTraceData |= DL_GPIO_readPins(GPIO_TRACE_R2_PORT, GPIO_TRACE_R2_PIN) ? (1 << 1) : 0;
    ucTraceData |= DL_GPIO_readPins(GPIO_TRACE_R3_PORT, GPIO_TRACE_R3_PIN) ? (1 << 0) : 0;

    return ucTraceData;
}

void GROUP1_IRQHandler(void)
{
    switch (DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
    case GPIO_MOTOR_E1A_IIDX:
    case GPIO_MOTOR_E1B_IIDX:
        vMotorEncoderLeft_IRQHandler();
        break;
    case GPIO_MOTOR_E2A_IIDX:
    case GPIO_MOTOR_E2B_IIDX:
        vMotorEncoderRight_IRQHandler();
        break;

    default:
        break;
    }
}