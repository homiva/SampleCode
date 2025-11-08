#include "relay.h"
#include "esp_log.h"

static const char *TAG = "relay";

static gpio_num_t s_relay_gpio = -1;
static bool s_active_high = true;
static bool s_state = false;

esp_err_t relay_init(gpio_num_t gpio_num, bool active_high)
{
    s_relay_gpio = gpio_num;
    s_active_high = active_high;

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << gpio_num),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIO %d", gpio_num);
        return err;
    }

    // Immediately set known safe state (relay OFF)
    gpio_set_level(s_relay_gpio, s_active_high ? 0 : 1);
    s_state = false;

    ESP_LOGI(TAG, "Relay initialized (GPIO=%d, active_%s)",
             gpio_num, s_active_high ? "HIGH" : "LOW");
    return ESP_OK;
}

void relay_on(void)
{
    if (s_relay_gpio < 0) return;
    gpio_set_level(s_relay_gpio, s_active_high ? 1 : 0);
    s_state = true;
    ESP_LOGI(TAG, "Relay ON");
}

void relay_off(void)
{
    if (s_relay_gpio < 0) return;
    gpio_set_level(s_relay_gpio, s_active_high ? 0 : 1);
    s_state = false;
    ESP_LOGI(TAG, "Relay OFF");
}

void relay_toggle(void)
{
    if (s_state)
        relay_off();
    else
        relay_on();
}

bool relay_is_on(void)
{
    return s_state;
}
