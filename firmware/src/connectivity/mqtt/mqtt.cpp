#include "connectivity/mqtt/mqtt.hpp"

#include <utility>

namespace connectivity::mqtt {

Mqtt::Mqtt(config::IMqttConfig &config, config::IDeviceConfig &device,
           std::string defaultPort)
    : _service(_client, config, device, std::move(defaultPort)) {}

esp_err_t Mqtt::start() {
  return _service.connect(
      [this](Status status) {
        if (_userStatus) {
          _userStatus(status);
        }
      },
      [this](const Message &message) {
        if (_userMessage) {
          _userMessage(message);
        }
      });
}

esp_err_t Mqtt::stop() { return _service.stop(); }

esp_err_t Mqtt::publish(const std::string &topic, const std::string &payload,
                        int qos, bool retain) {
  return _service.publish(topic, payload, qos, retain);
}

esp_err_t Mqtt::subscribe(const std::string &topic, int qos) {
  return _service.subscribe(topic, qos);
}

bool Mqtt::isConnected() const { return _service.isConnected(); }

void Mqtt::onStatusChange(StatusHandler handler) {
  _userStatus = std::move(handler);
}

void Mqtt::onMessage(MessageHandler handler) {
  _userMessage = std::move(handler);
}

}  // namespace connectivity::mqtt
