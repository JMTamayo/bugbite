#include "connectivity/provisioning/provisioning.hpp"

#include <esp_log.h>

#include "connectivity/provisioning/tag.hpp"

namespace connectivity::provisioning {

Provisioning::Provisioning(config::IDeviceConfig &device,
                           config::IWifiConfig &wifi, config::IMqttConfig &mqtt,
                           const ProvisioningUuids &uuids)
    : _device(device), _service(device, wifi, mqtt, uuids) {}

esp_err_t Provisioning::start() {
  std::string deviceId;
  esp_err_t err = _device.getId(deviceId);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "device_id unavailable (%s); cannot start provisioning",
             esp_err_to_name(err));

    return err;
  }

  if (deviceId.empty()) {
    ESP_LOGE(TAG, "device_id is empty; cannot start provisioning");

    return ESP_ERR_INVALID_STATE;
  }

  std::string projectName;
  _device.getProjectName(projectName);

  std::string suffix = deviceId.substr(0, deviceId.find('-'));
  std::string deviceName = projectName + "-" + suffix;

  return _server.start(deviceName, _service.buildService());
}

esp_err_t Provisioning::stop() { return _server.stop(); }

}  // namespace connectivity::provisioning
