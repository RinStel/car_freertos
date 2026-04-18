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
    TrackData_t xTrackData;
    for (;;)
    {
        vTrackUpdate(&xTrackData);
        printf("%d, ", xTrackData.current_pos);
        switch (xTrackData.status)
        {
        case TRACK_LINE_NORMAL:
            printf("NORMAL\n");
            break;
        case TRACK_LINE_LEFT_TURN:
            printf("LEFT\n");
            break;
        case TRACK_LINE_RIGHT_TURN:
            printf("RIGHT\n");
            break;
        case TRACK_LINE_CROSS:
            printf("CROSS\n");
            break;
        case TRACK_LINE_LOST:
            printf("LOST\n");
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(1000 / 50));
    }
}

void main_Control(void)
{
    xTaskCreate(prvControlTask, "Control", 512, NULL, configMAX_PRIORITIES - 4,
                &xControlTaskHandle);
}