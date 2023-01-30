#include <stdio.h>

#include "driver/gpio.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_websocket_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define BLINK_GPIO 10
#define NO_DATA_TIMEOUT_SEC 10

#define WEBSOCKET_PROTOCOL "wss://"
#define HOST_DOMAIN CONFIG_DISCO_HOST_DOMAIN

extern const uint8_t server_cert_pem_start[] asm("_binary_host_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_host_cert_pem_end");

static const char *TAG = "Websocket";

static TimerHandle_t shutdown_signal_timer;
static SemaphoreHandle_t shutdown_sema;

static void configure_led(void) {
  gpio_reset_pin(BLINK_GPIO);
  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
}

static void shutdown_signaler(TimerHandle_t xTimer) {
  ESP_LOGI(TAG, "No data received for %d seconds, signaling shutdown",
           NO_DATA_TIMEOUT_SEC);
  xSemaphoreGive(shutdown_sema);
}

static void websocket_event_handler(void *handler_args, esp_event_base_t base,
                                    int32_t event_id, void *event_data) {
  char *buf = NULL;

  esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
  switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
      ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
      break;
    case WEBSOCKET_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
      break;
    case WEBSOCKET_EVENT_DATA:
      ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
      ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
      ESP_LOGW(TAG, "Received=%.*s", data->data_len, (char *)data->data_ptr);

      buf = calloc(1, data->data_len + 1);
      strncpy(buf, data->data_ptr, data->data_len);

      if (data->data_len > 0) {
        if (strcmp(buf, "1") == 0) {
          ESP_LOGI(TAG, "On");
          gpio_set_level(BLINK_GPIO, 1);
        } else if (strcmp(buf, "0") == 0) {
          ESP_LOGI(TAG, "Off");
          gpio_set_level(BLINK_GPIO, 0);
        } else {
          ESP_LOGI(TAG, "Unknown");
        }
      }
      free(buf);

      xTimerReset(shutdown_signal_timer, portMAX_DELAY);
      break;
    case WEBSOCKET_EVENT_ERROR:
      ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
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
  };

  shutdown_signal_timer =
      xTimerCreate("Websocket shutdown timer",
                   NO_DATA_TIMEOUT_SEC * 1000 / portTICK_PERIOD_MS, pdFALSE,
                   NULL, shutdown_signaler);
  shutdown_sema = xSemaphoreCreateBinary();

  ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);

  esp_websocket_client_handle_t client =
      esp_websocket_client_init(&websocket_cfg);
  esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY,
                                websocket_event_handler, (void *)client);
  esp_websocket_client_start(client);
}
