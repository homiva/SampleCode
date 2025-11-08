#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "hcsr04.h"

static const char *TAG = "hcsr04";

static int s_trigger_gpio = -1;
static int s_echo_gpio = -1;

esp_err_t hcsr04_init(int trigger_gpio, int echo_gpio)
{
    s_trigger_gpio = trigger_gpio;
    s_echo_gpio = echo_gpio;

    gpio_config_t trig_cfg = {
        .pin_bit_mask = (1ULL << trigger_gpio),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&trig_cfg));

    gpio_config_t echo_cfg = {
        .pin_bit_mask = (1ULL << echo_gpio),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,   // ✅ fixed
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&echo_cfg));

    gpio_set_level(s_trigger_gpio, 0);
    ESP_LOGI(TAG, "HC-SR04 initialized (TRIG=%d, ECHO=%d)", trigger_gpio, echo_gpio);
    return ESP_OK;
}

esp_err_t hcsr04_measure(float *distance_cm)
{
    if (!distance_cm || s_trigger_gpio < 0 || s_echo_gpio < 0)
        return ESP_ERR_INVALID_STATE;

    gpio_set_level(s_trigger_gpio, 0);
    esp_rom_delay_us(2);
    gpio_set_level(s_trigger_gpio, 1);
    esp_rom_delay_us(10);
    gpio_set_level(s_trigger_gpio, 0);

    int64_t start_time = esp_timer_get_time();
    while (gpio_get_level(s_echo_gpio) == 0) {
        if ((esp_timer_get_time() - start_time) > 1000000) {
            ESP_LOGW(TAG, "Timeout waiting for echo HIGH");
            return ESP_ERR_TIMEOUT;
        }
    }

    int64_t echo_start = esp_timer_get_time();
    while (gpio_get_level(s_echo_gpio) == 1) {
        if ((esp_timer_get_time() - echo_start) > 300000) {
            ESP_LOGW(TAG, "Timeout waiting for echo LOW");
            return ESP_ERR_TIMEOUT;
        }
    }
    int64_t echo_end = esp_timer_get_time();

    *distance_cm = ((echo_end - echo_start) * 0.0343f) / 2.0f;
    ESP_LOGI(TAG, "📏 Distance: %.2f cm", *distance_cm);

    vTaskDelay(pdMS_TO_TICKS(60));  // allow sensor to settle
    return ESP_OK;
}

