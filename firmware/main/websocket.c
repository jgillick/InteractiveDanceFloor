#include <stdio.h>

#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_websocket_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define BLINK_GPIO 10
#define NO_DATA_TIMEOUT_SEC 10

#define WEBSOCKET_PROTOCOL "wss://"
#define HOST_DOMAIN CONFIG_DISCO_HOST_DOMAIN

extern const uint8_t server_cert_pem_start[] asm("_binary_host_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_host_cert_pem_end");

static const char *TAG = "WebSocket";

static void configure_led(void) {
  gpio_reset_pin(BLINK_GPIO);
  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

static void websocket_event_handler(void *handler_args, esp_event_base_t base,
                                    int32_t event_id, void *event_data) {
  char *buf = NULL;

  esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
  switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
      ESP_LOGI(TAG, "Connected");
      break;
    case WEBSOCKET_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "Disconnected");
      break;
    case WEBSOCKET_EVENT_DATA:
      if (data->op_code != 10) {
        ESP_LOGI(TAG, "Received message (opcode: %d): %.*s", data->op_code,
                 data->data_len, (char *)data->data_ptr);

        buf = calloc(1, data->data_len + 1);
        strncpy(buf, data->data_ptr, data->data_len);

        if (data->data_len > 0) {
          if (strcmp(buf, "1") == 0) {
            ESP_LOGI(TAG, "Value: On");
            gpio_set_level(BLINK_GPIO, 1);
          } else if (strcmp(buf, "0") == 0) {
            ESP_LOGI(TAG, "Value: Off");
            gpio_set_level(BLINK_GPIO, 0);
          } else {
            ESP_LOGI(TAG, "Value: Unknown");
          }
        }
        free(buf);
      }

      break;
    case WEBSOCKET_EVENT_ERROR:
      ESP_LOGI(TAG, "Error");
      break;
  }
}

void websocket_start(void) {
  configure_led();

  char url[strlen(WEBSOCKET_PROTOCOL) + strlen(CONFIG_DISCO_HOST_DOMAIN) + 1];
  sprintf(url, "%s%s", WEBSOCKET_PROTOCOL, CONFIG_DISCO_HOST_DOMAIN);
  esp_websocket_client_config_t websocket_cfg = {
      .uri = url,
      .cert_pem = (const char *)server_cert_pem_start,
      .network_timeout_ms = 100,
      .reconnect_timeout_ms = 5000,
  };

  ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);

  esp_websocket_client_handle_t client =
      esp_websocket_client_init(&websocket_cfg);
  esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY,
                                websocket_event_handler, (void *)client);
  esp_websocket_client_start(client);
}
