#include "connectivity/config/infrastructure/flash_wifi_config.hpp"

namespace connectivity::config {

FlashWifiConfig::FlashWifiConfig(flash_memory::FlashService &flash,
                                 const WifiConfigKeys &keys)
    : _flash(flash), _keys(keys) {}

esp_err_t FlashWifiConfig::getSsid(std::string &out) const {
  return _flash.read(_keys.ns, _keys.ssid, out);
}

esp_err_t FlashWifiConfig::setSsid(const std::string &value) {
  return _flash.write(_keys.ns, _keys.ssid, value);
}

esp_err_t FlashWifiConfig::getPassword(std::string &out) const {
  return _flash.read(_keys.ns, _keys.password, out);
}

esp_err_t FlashWifiConfig::setPassword(const std::string &value) {
  return _flash.write(_keys.ns, _keys.password, value);
}

}  // namespace connectivity::config
