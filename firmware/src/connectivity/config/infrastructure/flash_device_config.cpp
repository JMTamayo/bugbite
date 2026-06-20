#include "connectivity/config/infrastructure/flash_device_config.hpp"

namespace connectivity::config {

FlashDeviceConfig::FlashDeviceConfig(flash_memory::FlashService &flash,
                                     const DeviceConfigParams &params)
    : _flash(flash), _params(params) {}

esp_err_t FlashDeviceConfig::getId(std::string &out) const {
  return _flash.read(_params.ns, _params.idKey, out);
}

esp_err_t FlashDeviceConfig::getProjectName(std::string &out) const {
  out = _params.projectName;

  return ESP_OK;
}

}  // namespace connectivity::config
