#include <stdio.h>

#include "driver/gpio.h"
#include "esp_app_desc.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_websocket_client.h"
#include "nvs_flash.h"
#include "ota_upgrade.h"
#include "websocket.h"
#include "wifi.h"

static const char *TAG = "Main";

esp_err_t print_mac_address(void) {
  uint8_t mac[6] = {0};
  if (esp_base_mac_addr_get(mac) != ESP_OK) {
    esp_efuse_mac_get_default(mac);
  }
  ESP_LOGI(TAG, "MAC address: %.2X:%.2X:%.2X:%.2X:%.2X:%.2X", mac[0], mac[1],
           mac[2], mac[3], mac[4], mac[5]);

  return ESP_OK;
}

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
  print_mac_address();

  // Connect to wifi
  ESP_ERROR_CHECK(wifi_connect());

  // Check for firware updaets
  ESP_ERROR_CHECK(init_ota_upgrade());

  // Connect to web sockets
  websocket_start();
}
