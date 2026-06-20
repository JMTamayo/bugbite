#pragma once

#include <esp_err.h>

#include <string>

namespace connectivity::config {

/**
 * @brief Port for persistent WiFi configuration.
 *
 * Read/write access to the WiFi credentials backing store. The provisioning
 * module writes these values; the future WiFi module reads them to connect.
 * Each getter resolves the value from storage; @c ESP_ERR_NVS_NOT_FOUND means
 * the field has not been provisioned yet.
 */
class IWifiConfig {
 public:
  virtual ~IWifiConfig() = default;

  /**
   * @brief Reads the configured SSID.
   *
   * @param out Receives the SSID on success.
   * @return    ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if unset, or an
   *            ESP-IDF error code on failure.
   */
  virtual esp_err_t getSsid(std::string &out) const = 0;

  /**
   * @brief Stores the SSID.
   *
   * @param value SSID to persist.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t setSsid(const std::string &value) = 0;

  /**
   * @brief Reads the configured password.
   *
   * @param out Receives the password on success.
   * @return    ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if unset, or an
   *            ESP-IDF error code on failure.
   */
  virtual esp_err_t getPassword(std::string &out) const = 0;

  /**
   * @brief Stores the password.
   *
   * @param value Password to persist.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t setPassword(const std::string &value) = 0;
};

}  // namespace connectivity::config
