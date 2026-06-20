#include "connectivity/config/infrastructure/flash_mqtt_config.hpp"

namespace connectivity::config {

FlashMqttConfig::FlashMqttConfig(flash_memory::FlashService &flash,
                                 const MqttConfigKeys &keys)
    : _flash(flash), _keys(keys) {}

esp_err_t FlashMqttConfig::getHost(std::string &out) const {
  return _flash.read(_keys.ns, _keys.host, out);
}

esp_err_t FlashMqttConfig::setHost(const std::string &value) {
  return _flash.write(_keys.ns, _keys.host, value);
}

esp_err_t FlashMqttConfig::getPort(std::string &out) const {
  return _flash.read(_keys.ns, _keys.port, out);
}

esp_err_t FlashMqttConfig::setPort(const std::string &value) {
  return _flash.write(_keys.ns, _keys.port, value);
}

esp_err_t FlashMqttConfig::getUser(std::string &out) const {
  return _flash.read(_keys.ns, _keys.user, out);
}

esp_err_t FlashMqttConfig::setUser(const std::string &value) {
  return _flash.write(_keys.ns, _keys.user, value);
}

esp_err_t FlashMqttConfig::getPassword(std::string &out) const {
  return _flash.read(_keys.ns, _keys.password, out);
}

esp_err_t FlashMqttConfig::setPassword(const std::string &value) {
  return _flash.write(_keys.ns, _keys.password, value);
}

}  // namespace connectivity::config
