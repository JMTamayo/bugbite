#include "peripherals/flash_memory/flash_memory.hpp"

#include <esp_log.h>
#include <nvs_flash.h>

#include "peripherals/flash_memory/tag.hpp"

namespace flash_memory {

FlashMemory::FlashMemory() : FlashService(_storage) {}

esp_err_t FlashMemory::begin() {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_LOGW(TAG, "NVS unusable (%s), erasing partition", esp_err_to_name(err));
    err = nvs_flash_erase();
    if (err == ESP_OK) {
      err = nvs_flash_init();
    }
  }

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_flash_init failed: %s", esp_err_to_name(err));

    return err;
  }

  ESP_LOGI(TAG, "NVS initialized");

  return ESP_OK;
}

}  // namespace flash_memory
