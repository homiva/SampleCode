HC-SR04 component

Wiring
- VCC -> 5V
- GND -> GND
- TRIG -> Connect to ESP32 GPIO (e.g. GPIO18)
- ECHO -> Connect to ESP32 GPIO (e.g. GPIO19) but IMPORTANT: HC-SR04 echo outputs 5V. Use a voltage divider or level shifter to bring ECHO down to 3.3V before connecting to ESP32.

Notes
- Do NOT connect ECHO directly to an ESP32 GPIO without level shifting; it may damage the chip.
- The code uses busy-wait timing with esp_timer_get_time(). It is simple and accurate for most hobby projects.

API
- `bool hcsr04_init(int trigger_gpio, int echo_gpio);` Initialize pins.
- `bool hcsr04_measure_cm(float *distance_cm, uint32_t timeout_ms);` Measure distance in cm.

Example `src/main.c` snippet
```c
#include "hcsr04.h"

void app_main(void)
{
    if (!hcsr04_init(18, 19)) {
        ESP_LOGE("app", "hcsr04 init failed");
        return;
    }

    while (1) {
        float d;
        if (hcsr04_measure_cm(&d, 200)) {
            ESP_LOGI("app", "Distance: %.2f cm", d);
        } else {
            ESP_LOGW("app", "Measurement failed");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
```
