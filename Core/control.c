#include "stdio.h"

/* Kernel includes. */
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

/* TI includes. */
#include "ti_msp_dl_config.h"

#include "track.h"

/*-----------------------------------------------------------*/

TaskHandle_t xControlTaskHandle;

/*-----------------------------------------------------------*/

void main_Control(void);
void prvControlTask(void *argument);

/*-----------------------------------------------------------*/

void prvControlTask(void *argument)
{
    for (;;)
    {
        TrackData_t xTrackData;
        vTrackUpdate(&xTrackData);
        printf("%d", xTrackData.current_pos);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void main_Control(void)
{
    xTaskCreate(prvControlTask, "Control", 512, NULL, configMAX_PRIORITIES - 4,
                &xControlTaskHandle);
}