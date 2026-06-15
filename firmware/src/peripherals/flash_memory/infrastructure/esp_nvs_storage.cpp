#include "peripherals/flash_memory/infrastructure/esp_nvs_storage.hpp"

#include <esp_log.h>
#include <nvs.h>

#include "peripherals/flash_memory/tag.hpp"

namespace flash_memory {

esp_err_t EspNvsStorage::read(const std::string &ns, const std::string &key,
                              std::string &out) const {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(ns.c_str(), NVS_READONLY, &handle);
  if (err == ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGD(TAG, "namespace '%s' not found", ns.c_str());

    return err;
  }
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_open failed for ns='%s': %s", ns.c_str(),
             esp_err_to_name(err));

    return err;
  }

  size_t size = 0;
  err = nvs_get_str(handle, key.c_str(), nullptr, &size);
  if (err == ESP_ERR_NVS_NOT_FOUND) {
    ESP_LOGD(TAG, "key '%s' not found in ns='%s'", key.c_str(), ns.c_str());
    nvs_close(handle);

    return err;
  }
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_get_str size failed for key='%s': %s", key.c_str(),
             esp_err_to_name(err));
    nvs_close(handle);

    return err;
  }

  out.resize(size);
  err = nvs_get_str(handle, key.c_str(), out.data(), &size);
  nvs_close(handle);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_get_str read failed for key='%s': %s", key.c_str(),
             esp_err_to_name(err));

    return err;
  }

  // nvs_get_str includes the null terminator in `size`
  out.resize(size - 1);

  return ESP_OK;
}

esp_err_t EspNvsStorage::write(const std::string &ns, const std::string &key,
                               const std::string &value) {
  nvs_handle_t handle;
  esp_err_t err = nvs_open(ns.c_str(), NVS_READWRITE, &handle);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_open failed for ns='%s': %s", ns.c_str(),
             esp_err_to_name(err));

    return err;
  }

  err = nvs_set_str(handle, key.c_str(), value.c_str());
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_set_str failed for key='%s': %s", key.c_str(),
             esp_err_to_name(err));
    nvs_close(handle);

    return err;
  }

  err = nvs_commit(handle);
  nvs_close(handle);

  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nvs_commit failed: %s", esp_err_to_name(err));
  }

  return err;
}

}  // namespace flash_memory
