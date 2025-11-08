/*
 * Simple WiFi helper for ESP32 (station mode)
 * Provides a tiny API to initialize and connect to an AP.
 */
#ifndef WIFI_H
#define WIFI_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize WiFi in station mode and attempt to connect to the given SSID/PASSWORD.
 * Returns true if connected within the internal timeout (10s), false otherwise.
 */
bool wifi_init_sta(const char *ssid, const char *password);

/* Wait until connected or timeout (ms). Returns true if connected. */
bool wifi_wait_connected(uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif

#endif // WIFI_H
