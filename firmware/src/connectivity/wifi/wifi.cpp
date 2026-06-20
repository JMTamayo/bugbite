#include "connectivity/wifi/wifi.hpp"

namespace connectivity::wifi {

Wifi::Wifi(config::IWifiConfig &config, std::uint32_t reconnectDelayMs,
           std::int8_t maxTxPower)
    : _station(reconnectDelayMs, maxTxPower), _service(_station, config) {}

esp_err_t Wifi::start() {
  esp_err_t err = _station.begin([this](Status status) {
    if (_userHandler) {
      _userHandler(status);
    }
  });
  if (err != ESP_OK) {
    return err;
  }

  return _service.connect();
}

esp_err_t Wifi::stop() { return _station.disconnect(); }

bool Wifi::isConnected() const { return _service.isConnected(); }

void Wifi::onStatusChange(StatusHandler handler) {
  _userHandler = std::move(handler);
}

}  // namespace connectivity::wifi
