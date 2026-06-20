#include "connectivity/wifi/application/wifi_service.hpp"

#include <esp_log.h>
#include <nvs.h>

#include "connectivity/wifi/tag.hpp"

namespace connectivity::wifi {

WifiService::WifiService(IWifiStation &station, config::IWifiConfig &config)
    : _station(station), _config(config) {}

esp_err_t WifiService::connect() {
  std::string ssid;
  esp_err_t err = _config.getSsid(ssid);
  if (err == ESP_ERR_NVS_NOT_FOUND || (err == ESP_OK && ssid.empty())) {
    ESP_LOGW(TAG, "no WiFi credentials; provision via BLE first");

    return ESP_ERR_INVALID_STATE;
  }
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to read ssid: %s", esp_err_to_name(err));

    return err;
  }

  std::string password;
  err = _config.getPassword(password);
  if (err == ESP_ERR_NVS_NOT_FOUND) {
    password.clear();
  } else if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to read password: %s", esp_err_to_name(err));

    return err;
  }

  return _station.connect(ssid, password);
}

bool WifiService::isConnected() const { return _station.isConnected(); }

}  // namespace connectivity::wifi
