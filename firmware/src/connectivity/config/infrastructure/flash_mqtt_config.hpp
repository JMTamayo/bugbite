#pragma once

#include "connectivity/config/domain/i_mqtt_config.hpp"
#include "peripherals/flash_memory/application/flash_service.hpp"

namespace connectivity::config {

/**
 * @brief NVS namespace and keys for the MQTT configuration fields.
 *
 * Injected at construction so the adapter holds no hard-coded configuration;
 * main.cpp supplies the values from the central config header.
 */
struct MqttConfigKeys {
  const char *ns;
  const char *host;
  const char *port;
  const char *user;
  const char *password;
};

/**
 * @brief Flash-backed adapter for MQTT configuration.
 *
 * Implements @c IMqttConfig on top of the flash_memory service. Each operation
 * is a direct read-through / write-through to NVS (no in-RAM cache), mapping
 * each MQTT field to its injected NVS key.
 */
class FlashMqttConfig : public IMqttConfig {
 public:
  /**
   * @brief Builds the adapter over a flash service.
   *
   * @param flash Reference to the flash service. Must outlive this instance.
   * @param keys  NVS namespace and keys for the MQTT fields.
   */
  FlashMqttConfig(flash_memory::FlashService &flash,
                  const MqttConfigKeys &keys);

  /**
   * @brief Reads the broker host from storage.
   *
   * @param out Receives the value on success.
   * @return    ESP_OK, ESP_ERR_NVS_NOT_FOUND if unset, or an ESP-IDF error.
   */
  esp_err_t getHost(std::string &out) const override;

  /**
   * @brief Persists the broker host.
   *
   * @param value Value to store.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t setHost(const std::string &value) override;

  /**
   * @brief Reads the broker port from storage.
   *
   * @param out Receives the value on success.
   * @return    ESP_OK, ESP_ERR_NVS_NOT_FOUND if unset, or an ESP-IDF error.
   */
  esp_err_t getPort(std::string &out) const override;

  /**
   * @brief Persists the broker port.
   *
   * @param value Value to store.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t setPort(const std::string &value) override;

  /**
   * @brief Reads the user from storage.
   *
   * @param out Receives the value on success.
   * @return    ESP_OK, ESP_ERR_NVS_NOT_FOUND if unset, or an ESP-IDF error.
   */
  esp_err_t getUser(std::string &out) const override;

  /**
   * @brief Persists the user.
   *
   * @param value Value to store.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t setUser(const std::string &value) override;

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
  MqttConfigKeys _keys;
};

}  // namespace connectivity::config
