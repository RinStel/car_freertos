#include "stdio.h"

#include "ti_msp_dl_config.h"

static char sEchoData;

void UART_DEBUG_INST_IRQHandler(void)
{
    switch (DL_UART_getPendingInterrupt(UART_DEBUG_INST))
    {
    case DL_UART_MAIN_IIDX_RX:
        sEchoData = DL_UART_Main_receiveData(UART_DEBUG_INST);
        DL_UART_Main_transmitData(UART_DEBUG_INST, sEchoData);
        break;
    default:
        break;
    }
}

/*-----------------------------------------------------------*/
// 支持 printf 输出到 UART_DEBUG_INST

int fputc(int c, FILE *stream)
{
    DL_UART_Main_transmitDataBlocking(UART_DEBUG_INST, (uint8_t) c);
    return c;
}

int fputs(const char *restrict s, FILE *restrict stream)
{
    uint16_t i = 0;
    while (s[i] != '\0')
    {
        DL_UART_Main_transmitDataBlocking(UART_DEBUG_INST, (uint8_t) s[i]);
        i++;
    }
    return i;
}

int puts(const char *_ptr)
{
    return fputs(_ptr, stdout) + fputc('\n', stdout);
}

/*-----------------------------------------------------------*/