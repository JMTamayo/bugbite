#include "connectivity/provisioning/application/provisioning_service.hpp"

#include <nvs.h>

namespace connectivity::provisioning {

namespace {

// Wraps a getter so a not-yet-provisioned field reads back as an empty value
// instead of surfacing ESP_ERR_NVS_NOT_FOUND over BLE.
ReadHandler readThrough(std::function<esp_err_t(std::string &)> getter) {
  return [getter](std::string &out) -> esp_err_t {
    esp_err_t err = getter(out);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
      out.clear();

      return ESP_OK;
    }

    return err;
  };
}

}  // namespace

ProvisioningService::ProvisioningService(config::IDeviceConfig &device,
                                         config::IWifiConfig &wifi,
                                         config::IMqttConfig &mqtt,
                                         const ProvisioningUuids &uuids)
    : _device(device), _wifi(wifi), _mqtt(mqtt), _uuids(uuids) {}

ServiceSpec ProvisioningService::buildService() const {
  config::IDeviceConfig *device = &_device;
  config::IWifiConfig *wifi = &_wifi;
  config::IMqttConfig *mqtt = &_mqtt;

  ServiceSpec service;
  service.uuid = _uuids.service;
  service.characteristics = {
      {_uuids.deviceId, "device_id", true, false,
       readThrough([device](std::string &o) { return device->getId(o); }),
       nullptr},
      {_uuids.wifiSsid, "wifi_ssid", true, true,
       readThrough([wifi](std::string &o) { return wifi->getSsid(o); }),
       [wifi](const std::string &v) { return wifi->setSsid(v); }},
      {_uuids.wifiPassword, "wifi_password", false, true, nullptr,
       [wifi](const std::string &v) { return wifi->setPassword(v); }},
      {_uuids.mqttHost, "mqtt_host", true, true,
       readThrough([mqtt](std::string &o) { return mqtt->getHost(o); }),
       [mqtt](const std::string &v) { return mqtt->setHost(v); }},
      {_uuids.mqttPort, "mqtt_port", true, true,
       readThrough([mqtt](std::string &o) { return mqtt->getPort(o); }),
       [mqtt](const std::string &v) { return mqtt->setPort(v); }},
      {_uuids.mqttUser, "mqtt_user", true, true,
       readThrough([mqtt](std::string &o) { return mqtt->getUser(o); }),
       [mqtt](const std::string &v) { return mqtt->setUser(v); }},
      {_uuids.mqttPassword, "mqtt_password", false, true, nullptr,
       [mqtt](const std::string &v) { return mqtt->setPassword(v); }},
  };

  return service;
}

}  // namespace connectivity::provisioning
