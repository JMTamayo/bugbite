#include "setup.hpp"

#include <esp_log.h>

static const char *TAG = "app/setup";

esp_err_t setup(flash_memory::FlashMemory &flashMemory, relay::Relay &loadRelay,
                led::Led &builtinLed) {
  esp_err_t err = flashMemory.begin();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "flash memory initialization failed: %s",
             esp_err_to_name(err));
    return err;
  }

  err = loadRelay.begin();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "load relay initialization failed: %s", esp_err_to_name(err));
    return err;
  }

  err = builtinLed.begin();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "builtin led initialization failed: %s",
             esp_err_to_name(err));
    return err;
  }

  ESP_LOGI(TAG, "setup completed");

  return ESP_OK;
}
