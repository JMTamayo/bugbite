#include "connectivity/mqtt/application/mqtt_service.hpp"

#include <esp_log.h>
#include <nvs.h>

#include <utility>

#include "connectivity/mqtt/tag.hpp"

namespace connectivity::mqtt {

MqttService::MqttService(IMqttClient &client, config::IMqttConfig &config,
                         config::IDeviceConfig &device, std::string defaultPort)
    : _client(client),
      _config(config),
      _device(device),
      _defaultPort(std::move(defaultPort)) {}

esp_err_t MqttService::connect(StatusHandler onStatus,
                               MessageHandler onMessage) {
  std::string host;
  esp_err_t err = _config.getHost(host);
  if (err == ESP_ERR_NVS_NOT_FOUND || (err == ESP_OK && host.empty())) {
    ESP_LOGW(TAG, "no MQTT host; provision via BLE first");

    return ESP_ERR_INVALID_STATE;
  }
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to read host: %s", esp_err_to_name(err));

    return err;
  }

  std::string clientId;
  err = _device.getId(clientId);
  if (err == ESP_ERR_NVS_NOT_FOUND || (err == ESP_OK && clientId.empty())) {
    ESP_LOGW(TAG, "no device_id; cannot set MQTT client id");

    return ESP_ERR_INVALID_STATE;
  }
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to read device_id: %s", esp_err_to_name(err));

    return err;
  }

  std::string port;
  err = _config.getPort(port);
  if (err == ESP_ERR_NVS_NOT_FOUND || (err == ESP_OK && port.empty())) {
    port = _defaultPort;
  } else if (err != ESP_OK) {
    return err;
  }

  std::string user;
  err = _config.getUser(user);
  if (err == ESP_ERR_NVS_NOT_FOUND) {
    user.clear();
  } else if (err != ESP_OK) {
    return err;
  }

  std::string password;
  err = _config.getPassword(password);
  if (err == ESP_ERR_NVS_NOT_FOUND) {
    password.clear();
  } else if (err != ESP_OK) {
    return err;
  }

  ClientConfig config{"mqtt://" + host + ":" + port, clientId, user, password};

  return _client.begin(config, std::move(onStatus), std::move(onMessage));
}

esp_err_t MqttService::publish(const std::string &topic,
                               const std::string &payload, int qos,
                               bool retain) {
  return _client.publish(topic, payload, qos, retain);
}

esp_err_t MqttService::subscribe(const std::string &topic, int qos) {
  return _client.subscribe(topic, qos);
}

esp_err_t MqttService::stop() { return _client.stop(); }

bool MqttService::isConnected() const { return _client.isConnected(); }

}  // namespace connectivity::mqtt
