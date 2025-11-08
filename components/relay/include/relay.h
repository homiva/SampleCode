#pragma once

#include "driver/gpio.h"
#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize a relay on a specific GPIO pin.
 *
 * @param gpio_num GPIO pin connected to relay module input.
 * @param active_high true if relay is triggered by HIGH signal, false if LOW.
 * @return ESP_OK on success, ESP_FAIL on error.
 */
esp_err_t relay_init(gpio_num_t gpio_num, bool active_high);

/**
 * @brief Turn the relay ON.
 */
void relay_on(void);

/**
 * @brief Turn the relay OFF.
 */
void relay_off(void);

/**
 * @brief Toggle relay state.
 */
void relay_toggle(void);

/**
 * @brief Get current relay state.
 *
 * @return true if relay is ON, false if OFF.
 */
bool relay_is_on(void);

#ifdef __cplusplus
}
#endif
