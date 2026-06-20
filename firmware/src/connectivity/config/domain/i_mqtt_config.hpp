#pragma once

#include <esp_err.h>

#include <string>

namespace connectivity::config {

/**
 * @brief Port for persistent MQTT configuration.
 *
 * Read/write access to the MQTT connection settings backing store. The
 * provisioning module writes these values; the future MQTT module reads them
 * to connect. Each getter resolves the value from storage;
 * @c ESP_ERR_NVS_NOT_FOUND means the field has not been provisioned yet.
 *
 * The port is stored and exposed as a string, mirroring the underlying
 * key-value storage. The MQTT module is expected to parse it to a numeric
 * port when it consumes the value.
 */
class IMqttConfig {
 public:
  virtual ~IMqttConfig() = default;

  /**
   * @brief Reads the configured broker host.
   *
   * @param out Receives the host on success.
   * @return    ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if unset, or an
   *            ESP-IDF error code on failure.
   */
  virtual esp_err_t getHost(std::string &out) const = 0;

  /**
   * @brief Stores the broker host.
   *
   * @param value Host to persist.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t setHost(const std::string &value) = 0;

  /**
   * @brief Reads the configured broker port.
   *
   * @param out Receives the port on success.
   * @return    ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if unset, or an
   *            ESP-IDF error code on failure.
   */
  virtual esp_err_t getPort(std::string &out) const = 0;

  /**
   * @brief Stores the broker port.
   *
   * @param value Port to persist.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t setPort(const std::string &value) = 0;

  /**
   * @brief Reads the configured user.
   *
   * @param out Receives the user on success.
   * @return    ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if unset, or an
   *            ESP-IDF error code on failure.
   */
  virtual esp_err_t getUser(std::string &out) const = 0;

  /**
   * @brief Stores the user.
   *
   * @param value User to persist.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t setUser(const std::string &value) = 0;

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
