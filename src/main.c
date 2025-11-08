#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"

#include "wifi.h"
#include "hcsr04.h"
#include "relay.h"
#include "firebase.h"

// -------------------- Wi-Fi --------------------
#define WIFI_SSID "JioFiber-C5SAq"
#define WIFI_PASS "12345678"

// -------------------- RELAY --------------------
#define RELAY_GPIO GPIO_NUM_26
#define RELAY_ACTIVE_HIGH false  // false for active LOW relay

// -------------------- LEVEL THRESHOLDS --------------------
#define WATER_LOW_LEVEL_CM 50.0f  // Turn ON relay if above this (tank empty)
#define WATER_HIGH_LEVEL_CM 10.0f // Turn OFF relay if below this (tank full)

static const char *TAG = "SMART_TANK";

// ==========================================================
// 🔹 Ultrasonic + Relay + Firebase Task
// ==========================================================
static void smart_tank_task(void *pv)
{
    float distance_cm = 0.0f;

    firebase_init();

    while (1)
    {
        int manual_mode = firebase_read_manual_control();  // 0 = auto, 1 = manual
        int manual_cmd  = firebase_read_motor_command();   // 0 = off, 1 = on
        ESP_LOGI(TAG, "🧭 Mode=%s, Cmd=%d", manual_mode ? "MANUAL" : "AUTO", manual_cmd);

        if (hcsr04_measure(&distance_cm) == ESP_OK)
        {
            ESP_LOGI(TAG, "📏 Water Level: %.2f cm", distance_cm);

            if (manual_mode == 1)
            {
                ESP_LOGI(TAG, "🖐 Manual mode active");
                if (manual_cmd == 1)
                {
                    ESP_LOGI(TAG, "🖱 Motor ON");
                    relay_on();
                }
                else
                {
                    ESP_LOGI(TAG, "🖱 Motor OFF");
                    relay_off();
                }
            }
            else
            {
                ESP_LOGI(TAG, "⚙️ Auto mode active");
                if (distance_cm > WATER_LOW_LEVEL_CM)
                {
                    ESP_LOGI(TAG, "💧 Low water → Motor ON");
                    relay_on();
                }
                else if (distance_cm < WATER_HIGH_LEVEL_CM)
                {
                    ESP_LOGI(TAG, "🛑 Tank full → Motor OFF");
                    relay_off();
                }
            }

            firebase_send_data(distance_cm, relay_is_on());
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}


// ==========================================================
// 🔹 Main Application
// ==========================================================
void app_main(void)
{
    ESP_LOGI(TAG, "🚀 Starting Smart Water Tank Controller");

    // --- Wi-Fi ---
    if (!wifi_init_sta(WIFI_SSID, WIFI_PASS))
    {
        ESP_LOGE(TAG, "❌ Wi-Fi connection failed");
        return;
    }
    ESP_LOGI(TAG, "✅ Wi-Fi connected");

    // --- HC-SR04 ---
    if (hcsr04_init(25, 27) != ESP_OK)
    {
        ESP_LOGE(TAG, "❌ HC-SR04 init failed");
        return;
    }

    // --- Relay ---
    if (relay_init(RELAY_GPIO, RELAY_ACTIVE_HIGH) != ESP_OK)
    {
        ESP_LOGE(TAG, "❌ Relay init failed");
        return;
    }
    relay_off();  // ensure motor is off initially

    // --- Start Task ---
    xTaskCreate(smart_tank_task, "smart_tank_task", 8192, NULL, 5, NULL);
}
