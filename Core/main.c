/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* TI includes */
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti_msp_dl_config.h"

/*-----------------------------------------------------------*/

#if (configCHECK_FOR_STACK_OVERFLOW)
volatile TaskHandle_t g_stackOverflowTask = NULL;
volatile const char * g_stackOverflowTaskName = NULL;
volatile char g_stackOverflowTaskNameBuf[ configMAX_TASK_NAME_LEN ] = { 0 };
#endif

/*
 * Set up the hardware ready to run this demo.
 */
static void prvSetupHardware(void);

extern void main_StackMonitor(void);
extern void main_Control(void);
extern void main_MotorEncoderUpdater(void);
extern void main_HCSR04(void);

/*-----------------------------------------------------------*/

int main(void)
{
    /* Prepare the hardware to run this demo. */
    prvSetupHardware();

    main_StackMonitor();
    main_HCSR04();
    main_Control();
    main_MotorEncoderUpdater();

    /* Start the tasks. */
    vTaskStartScheduler();

    for (;;)
    {
    }
}
/*-----------------------------------------------------------*/

static void prvSetupHardware(void)
{
    SYSCFG_DL_init();

    NVIC_ClearPendingIRQ(UART_DEBUG_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_DEBUG_INST_INT_IRQN);

    NVIC_ClearPendingIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
}
/*-----------------------------------------------------------*/

// #if (configSUPPORT_STATIC_ALLOCATION == 1)
// /*
//  *  ======== vApplicationGetIdleTaskMemory ========
//  *  When static allocation is enabled, the app must provide this callback
//  *  function for use by the Idle task.
//  */
// void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
//     StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
// {
//     static StaticTask_t xIdleTaskTCB;
//     static StackType_t uxIdleTaskStack[configIDLE_TASK_STACK_DEPTH];

//     *ppxIdleTaskTCBBuffer   = &xIdleTaskTCB;
//     *ppxIdleTaskStackBuffer = uxIdleTaskStack;
//     *pulIdleTaskStackSize   = configIDLE_TASK_STACK_DEPTH;
// }

// #if (configUSE_TIMERS == 1)
// /*
//  *  ======== vApplicationGetTimerTaskMemory ========
//  *  When static allocation is enabled, and timers are used, the app must provide
//  *  this callback function for use by the Timer Service task.
//  */
// void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
//     StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
// {
//     static StaticTask_t xTimerTaskTCB;
//     static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

//     *ppxTimerTaskTCBBuffer   = &xTimerTaskTCB;
//     *ppxTimerTaskStackBuffer = uxTimerTaskStack;
//     *pulTimerTaskStackSize   = configTIMER_TASK_STACK_DEPTH;
// }
// #endif

// #endif

#if (configCHECK_FOR_STACK_OVERFLOW)
/*
 *  ======== vApplicationStackOverflowHook ========
 *  When stack overflow checking is enabled the application must provide a
 *  stack overflow hook function. This default hook function is declared as
 *  weak, and will be used by default, unless the application specifically
 *  provides its own hook function.
 */
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    /* Strong definition to override weak hooks in middleware.
     * Record culprit for debugger/ROV inspection, then halt.
     */
    taskDISABLE_INTERRUPTS();

    g_stackOverflowTask = pxTask;
    g_stackOverflowTaskName = pcTaskName;

    if (pcTaskName != NULL)
    {
        for (unsigned int i = 0; i < (unsigned int)(configMAX_TASK_NAME_LEN - 1U); i++)
        {
            char c = pcTaskName[i];
            g_stackOverflowTaskNameBuf[i] = c;
            if (c == '\0')
            {
                break;
            }
        }
        g_stackOverflowTaskNameBuf[configMAX_TASK_NAME_LEN - 1U] = '\0';
    }

    while (1)
    {
    }
}
#endif

/*-----------------------------------------------------------*/
