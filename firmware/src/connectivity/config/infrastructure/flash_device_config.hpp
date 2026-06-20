#pragma once

#include "connectivity/config/domain/i_device_config.hpp"
#include "peripherals/flash_memory/application/flash_service.hpp"

namespace connectivity::config {

/**
 * @brief NVS coordinates and build-time values for the device identity.
 *
 * Injected at construction so the adapter holds no hard-coded configuration;
 * main.cpp supplies the values from the central config header.
 */
struct DeviceConfigParams {
  const char *ns;
  const char *idKey;
  const char *projectName;
};

/**
 * @brief Flash-backed adapter for device identity.
 *
 * Implements @c IDeviceConfig. The device_id is read-through from NVS; the
 * project name is a build-time constant supplied at construction.
 */
class FlashDeviceConfig : public IDeviceConfig {
 public:
  /**
   * @brief Builds the adapter over a flash service.
   *
   * @param flash  Reference to the flash service. Must outlive this instance.
   * @param params NVS coordinates and the project name.
   */
  FlashDeviceConfig(flash_memory::FlashService &flash,
                    const DeviceConfigParams &params);

  /**
   * @brief Reads the device id from storage.
   *
   * @param out Receives the value on success.
   * @return    ESP_OK, ESP_ERR_NVS_NOT_FOUND if not provisioned, or an ESP-IDF
   *            error.
   */
  esp_err_t getId(std::string &out) const override;

  /**
   * @brief Returns the injected project name.
   *
   * @param out Receives the project name.
   * @return    ESP_OK on success.
   */
  esp_err_t getProjectName(std::string &out) const override;

 private:
  flash_memory::FlashService &_flash;
  DeviceConfigParams _params;
};

}  // namespace connectivity::config
