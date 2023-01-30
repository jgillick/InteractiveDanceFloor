#include <stdio.h>

#include "cJSON.h"
#include "esp_app_desc.h"
#include "esp_crt_bundle.h"
#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "spi_flash_mmap.h"

#define FIRMWARE_UPGRADE_HOST CONFIG_DISCO_HOST_DOMAIN
#define FIRMWARE_UPGRADE_PATH "/firmware?version=%s"
#define HTTP_OUTPUT_BUFFER 512

static const char *TAG = "OTA Update";
extern const uint8_t server_cert_pem_start[] asm("_binary_ca_cert_pem_start");
extern const uint8_t server_cert_pem_end[] asm("_binary_ca_cert_pem_end");

esp_err_t check_for_upgrade(char **upgrade_path) {
  // Construct URL path with the app version
  const esp_app_desc_t *app_desc;
  app_desc = esp_app_get_description();
  char path[strlen(FIRMWARE_UPGRADE_PATH) + strlen(app_desc->version) + 1];
  sprintf(path, FIRMWARE_UPGRADE_PATH, app_desc->version);
  ESP_LOGI(TAG, "Checking for upgrade: https://%s%s", FIRMWARE_UPGRADE_HOST,
           path);

  // Setup server request
  char output_buffer[HTTP_OUTPUT_BUFFER] = {0};
  int content_length = 0;
  esp_http_client_config_t config = {
      .host = FIRMWARE_UPGRADE_HOST,
      .path = path,
      .transport_type = HTTP_TRANSPORT_OVER_SSL,
      .cert_pem = (const char *)server_cert_pem_start,
  };
  esp_http_client_handle_t client = esp_http_client_init(&config);
  esp_err_t err = esp_http_client_open(client, 0);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
    esp_http_client_close(client);
    return ESP_FAIL;
  }

  // Process connection response
  content_length = esp_http_client_fetch_headers(client);
  if (content_length < 0) {
    ESP_LOGE(TAG, "HTTP client fetch headers failed");
  } else {
    int data_read = esp_http_client_read_response(client, output_buffer,
                                                  HTTP_OUTPUT_BUFFER);
    if (data_read >= 0) {
      cJSON *data = cJSON_Parse(output_buffer);
      if (cJSON_GetObjectItem(data, "updateAvailable")) {
        int has_update = cJSON_GetObjectItem(data, "updateAvailable")->valueint;
        if (has_update) {
          ESP_LOGI(TAG, "Update available");
          *upgrade_path =
              cJSON_GetObjectItem(data, "firmwarePath")->valuestring;
        }
      }
    }
  }

  esp_http_client_close(client);
  return ESP_OK;
}

esp_err_t ota_upgrade(const char *upgrade_path) {
  if (upgrade_path == NULL) {
    ESP_LOGE(TAG, "No upgrade path!");
    return ESP_FAIL;
  }
  ESP_LOGI(TAG, "Upgrading firmware from: %s", upgrade_path);

  // Upgrade
  esp_http_client_config_t config = {
      .host = FIRMWARE_UPGRADE_HOST,
      .path = upgrade_path,
      .cert_pem = (char *)server_cert_pem_start,
  };
  esp_https_ota_config_t ota_config = {
      .http_config = &config,
  };
  esp_err_t ret = esp_https_ota(&ota_config);
  if (ret == ESP_OK) {
    ESP_LOGD(TAG, "Upgraded and restarting");
    esp_restart();
  }
  return ESP_OK;
}

esp_err_t init_ota_upgrade(void) {
  char *upgrade_path = NULL;
  ESP_ERROR_CHECK(check_for_upgrade(&upgrade_path));

  if (upgrade_path != NULL) {
    ota_upgrade(upgrade_path);
  } else {
    ESP_LOGI(TAG, "The firmware is up-to-date");
  }

  return ESP_OK;
}
