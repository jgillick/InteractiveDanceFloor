#include <stdio.h>

#include "driver/gpio.h"
#include "esp_app_desc.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_websocket_client.h"
#include "nvs_flash.h"
#include "ota_upgrade.h"
#include "websocket.h"
#include "wifi.h"

static const char *TAG = "Main";

void app_main(void) {
  ESP_LOGI(TAG, "Startup...");
  esp_log_level_set("wifi", ESP_LOG_WARN);

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());

  const esp_app_desc_t *app_desc;
  app_desc = esp_app_get_description();

  ESP_LOGI(TAG, "Program starting (ver: %s)", app_desc->version);
  // ESP_LOGI(TAG, "We upgraded brah!");

  ESP_ERROR_CHECK(wifi_connect());
  ESP_ERROR_CHECK(init_ota_upgrade());
  // websocket_start();
  // http_request();

  while (1) {
  }
}
