#ifndef FIREBASE_H
#define FIREBASE_H

#include "esp_err.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize Firebase component (optional for future token setup)
 */
esp_err_t firebase_init(void);

/**
 * @brief Send water tank data (distance and relay/motor state)
 * 
 * @param distance_cm  Distance measured by ultrasonic sensor
 * @param motor_on     True if relay is active
 * @return esp_err_t   ESP_OK on success, ESP_FAIL otherwise
 */
esp_err_t firebase_send_data(float distance_cm, bool motor_on);

/**
 * @brief Read manual control flag from Firebase (e.g., "motor_control")
 * 
 * @return int  1 = manual ON, 0 = manual OFF, -1 = error/no data
 */
int firebase_read_motor_command(void);

// Read manual control flag (1 = manual mode, 0 = automatic)
int firebase_read_manual_control(void);

#ifdef __cplusplus
}
#endif

#endif // FIREBASE_H
