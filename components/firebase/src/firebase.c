#include "firebase.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"

static const char *TAG = "firebase";

//#define FIREBASE_BASE_URL "https://smartwatertank-a695b-default-rtdb.firebaseio.com"
#define FIREBASE_BASE_URL "https://smartwatertank-3efdd-default-rtdb.firebaseio.com"
// ================================================================
// 🔹 Initialize Firebase
// ================================================================
esp_err_t firebase_init(void)
{
    ESP_LOGI(TAG, "✅ Firebase module initialized");
    return ESP_OK;
}

// ================================================================
// 🔹 Send distance and motor state to Firebase
// ================================================================
esp_err_t firebase_send_data(float distance, bool motor_on)
{
    char url[256];
    snprintf(url, sizeof(url), "%s/data.json", FIREBASE_BASE_URL);

    char json[256];
    snprintf(json, sizeof(json),
             "{\"distance_cm\": %.2f, \"motor_state\": \"%s\"}",
             distance, motor_on ? "ON" : "OFF");

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_PATCH,   // ✅ Use PATCH to prevent overwrite
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 7000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, json, strlen(json));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "✅ Sent to Firebase (%.2f cm, %s) HTTP %d",
                 distance, motor_on ? "ON" : "OFF", status);
    } else {
        ESP_LOGE(TAG, "❌ Firebase upload failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}

// ================================================================
// 🔹 Read Manual Control Mode (Auto / Manual)
// ================================================================
int firebase_read_manual_control(void)
{
    const char *url = FIREBASE_BASE_URL "/data/manual_control.json";
    esp_http_client_config_t config = {
        .url = url,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 10000
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "❌ Failed to open connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return 0;
    }

    // Perform the request
    esp_http_client_fetch_headers(client);
    int total_len = esp_http_client_get_content_length(client);
    ESP_LOGI(TAG, "📦 manual_control content_length = %d", total_len);

    char buffer[16];
    int read_len = esp_http_client_read_response(client, buffer, sizeof(buffer) - 1);

    int control = 0;
    if (read_len > 0) {
        buffer[read_len] = '\0';
        ESP_LOGI(TAG, "🔥 RAW manual_control response: [%s]", buffer);
        if (strchr(buffer, '1'))
            control = 1;
    } else {
        ESP_LOGW(TAG, "⚠️ No data read for manual_control (len=%d)", read_len);
    }

    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return control;
}




// ================================================================
// 🔹 Read Manual Command (Motor ON / OFF in Manual Mode)
// ================================================================
int firebase_read_motor_command(void)
{
    const char *url = FIREBASE_BASE_URL "/data/manual_command.json";
    esp_http_client_config_t config = {
        .url = url,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 10000
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_open(client, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "❌ Failed to open connection: %s", esp_err_to_name(err));
        esp_http_client_cleanup(client);
        return 0;
    }

    esp_http_client_fetch_headers(client);
    int total_len = esp_http_client_get_content_length(client);
    ESP_LOGI(TAG, "📦 manual_command content_length = %d", total_len);

    char buffer[32];
    int read_len = esp_http_client_read_response(client, buffer, sizeof(buffer) - 1);

    int command = 0;
    if (read_len > 0) {
        buffer[read_len] = '\0';
        ESP_LOGI(TAG, "🔥 RAW manual_command response: [%s]", buffer);

        // Trim spaces/newlines if any
        for (int i = 0; i < read_len; i++) {
            if (buffer[i] == '\r' || buffer[i] == '\n' || buffer[i] == ' ')
                buffer[i] = '\0';
        }

        if (strcmp(buffer, "1") == 0)
            command = 1;
        else if (strcmp(buffer, "0") == 0)
            command = 0;
        else
            ESP_LOGW(TAG, "⚠️ Unexpected command format: '%s'", buffer);
    } else {
        ESP_LOGW(TAG, "⚠️ No data read for manual_command (len=%d)", read_len);
    }

    esp_http_client_close(client);
    esp_http_client_cleanup(client);
    return command;
}

