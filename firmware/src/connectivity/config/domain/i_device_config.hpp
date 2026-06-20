#pragma once

#include <esp_err.h>

#include <string>

namespace connectivity::config {

/**
 * @brief Port for device identity information.
 *
 * Read-only access to the device identity: the device_id provisioned at the
 * factory (resolved from storage) and the build-time project name. Consumers
 * such as the provisioning module use it to derive the advertised name, and the
 * future MQTT module will derive the client id from the device_id.
 */
class IDeviceConfig {
 public:
  virtual ~IDeviceConfig() = default;

  /**
   * @brief Reads the device id.
   *
   * @param out Receives the device_id on success.
   * @return    ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if the device has not
   *            been provisioned, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t getId(std::string &out) const = 0;

  /**
   * @brief Reads the project name.
   *
   * @param out Receives the project name.
   * @return    ESP_OK on success.
   */
  virtual esp_err_t getProjectName(std::string &out) const = 0;
};

}  // namespace connectivity::config
