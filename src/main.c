#include "tft_display.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void app_main(void)
{
    tft_display_init();
    vTaskDelay(pdMS_TO_TICKS(1000));
    tft_display_draw_text("Hello from Homiva!", 10, 50);
}
