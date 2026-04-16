#include "ti_msp_dl_config.h"

#include "FreeRTOS.h"

static char sEchoData;

void UART_0_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(UART_0_INST))
    {
    case DL_UART_MAIN_IIDX_RX:
        sEchoData = DL_UART_Main_receiveData(UART_0_INST);
        DL_UART_Main_transmitData(UART_0_INST, sEchoData);
        break;
    default:
        break;
    }
}
