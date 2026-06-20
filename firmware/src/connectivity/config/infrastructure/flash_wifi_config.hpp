#pragma once

#include "connectivity/config/domain/i_wifi_config.hpp"
#include "peripherals/flash_memory/application/flash_service.hpp"

namespace connectivity::config {

/**
 * @brief NVS namespace and keys for the WiFi configuration fields.
 *
 * Injected at construction so the adapter holds no hard-coded configuration;
 * main.cpp supplies the values from the central config header.
 */
struct WifiConfigKeys {
  const char *ns;
  const char *ssid;
  const char *password;
};

/**
 * @brief Flash-backed adapter for WiFi configuration.
 *
 * Implements @c IWifiConfig on top of the flash_memory service. Each operation
 * is a direct read-through / write-through to NVS (no in-RAM cache), mapping
 * each WiFi field to its injected NVS key.
 */
class FlashWifiConfig : public IWifiConfig {
 public:
  /**
   * @brief Builds the adapter over a flash service.
   *
   * @param flash Reference to the flash service. Must outlive this instance.
   * @param keys  NVS namespace and keys for the WiFi fields.
   */
  FlashWifiConfig(flash_memory::FlashService &flash,
                  const WifiConfigKeys &keys);

  /**
   * @brief Reads the SSID from storage.
   *
   * @param out Receives the value on success.
   * @return    ESP_OK, ESP_ERR_NVS_NOT_FOUND if unset, or an ESP-IDF error.
   */
  esp_err_t getSsid(std::string &out) const override;

  /**
   * @brief Persists the SSID.
   *
   * @param value Value to store.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t setSsid(const std::string &value) override;

  /**
   * @brief Reads the password from storage.
   *
   * @param out Receives the value on success.
   * @return    ESP_OK, ESP_ERR_NVS_NOT_FOUND if unset, or an ESP-IDF error.
   */
  esp_err_t getPassword(std::string &out) const override;

  /**
   * @brief Persists the password.
   *
   * @param value Value to store.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t setPassword(const std::string &value) override;

 private:
  flash_memory::FlashService &_flash;
  WifiConfigKeys _keys;
};

}  // namespace connectivity::config
