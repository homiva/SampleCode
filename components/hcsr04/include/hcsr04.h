#ifndef HCSR04_H
#define HCSR04_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the HC-SR04 ultrasonic sensor
 *
 * @param trigger_gpio GPIO number connected to TRIG pin
 * @param echo_gpio GPIO number connected to ECHO pin
 * @return esp_err_t
 *         - ESP_OK on success
 *         - ESP_FAIL if configuration failed
 */
esp_err_t hcsr04_init(int trigger_gpio, int echo_gpio);

/**
 * @brief Measure distance using the HC-SR04 sensor
 *
 * @param distance_cm Pointer to float where measured distance (in cm) will be stored
 * @return esp_err_t
 *         - ESP_OK if successful
 *         - ESP_ERR_TIMEOUT if no echo detected
 */
esp_err_t hcsr04_measure(float *distance_cm);

#ifdef __cplusplus
}
#endif

#endif // HCSR04_H
