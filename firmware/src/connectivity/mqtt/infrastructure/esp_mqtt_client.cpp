#include "connectivity/mqtt/infrastructure/esp_mqtt_client.hpp"

#include <esp_log.h>
#include <mqtt_client.h>

#include <cstdint>
#include <utility>

#include "connectivity/mqtt/tag.hpp"

namespace connectivity::mqtt {

namespace {

// Single client per device, so the esp-mqtt handle and state live at file
// scope.
esp_mqtt_client_handle_t g_client = nullptr;
bool g_connected = false;
StatusHandler g_onStatus;
MessageHandler g_onMessage;
ClientConfig g_config;  // keeps the credential strings alive

void notify(Status status) {
  if (g_onStatus) {
    g_onStatus(status);
  }
}

void eventHandler(void *, esp_event_base_t, std::int32_t id, void *data) {
  auto *event = static_cast<esp_mqtt_event_handle_t>(data);

  switch (static_cast<esp_mqtt_event_id_t>(id)) {
    case MQTT_EVENT_CONNECTED:
      g_connected = true;
      ESP_LOGI(TAG, "connected to broker");
      notify(Status::Connected);
      break;
    case MQTT_EVENT_DISCONNECTED:
      g_connected = false;
      ESP_LOGW(TAG, "disconnected; retrying");
      notify(Status::Disconnected);
      break;
    case MQTT_EVENT_BEFORE_CONNECT:
      notify(Status::Connecting);
      break;
    case MQTT_EVENT_DATA:
      if (event->topic != nullptr && g_onMessage) {
        Message msg;
        msg.topic.assign(event->topic, event->topic_len);
        msg.payload.assign(event->data, event->data_len);
        g_onMessage(msg);
      }
      break;
    case MQTT_EVENT_ERROR:
      ESP_LOGE(TAG, "mqtt error");
      break;
    default:
      break;
  }
}

}  // namespace

esp_err_t EspMqttClient::begin(const ClientConfig &config,
                               StatusHandler onStatus,
                               MessageHandler onMessage) {
  if (g_client != nullptr) {
    return ESP_OK;
  }

  g_config = config;
  g_onStatus = std::move(onStatus);
  g_onMessage = std::move(onMessage);

  esp_mqtt_client_config_t cfg = {};
  cfg.broker.address.uri = g_config.uri.c_str();
  cfg.credentials.client_id = g_config.clientId.c_str();
  if (!g_config.username.empty()) {
    cfg.credentials.username = g_config.username.c_str();
  }
  if (!g_config.password.empty()) {
    cfg.credentials.authentication.password = g_config.password.c_str();
  }

  g_client = esp_mqtt_client_init(&cfg);
  if (g_client == nullptr) {
    ESP_LOGE(TAG, "esp_mqtt_client_init failed");

    return ESP_FAIL;
  }

  esp_err_t err = esp_mqtt_client_register_event(g_client, MQTT_EVENT_ANY,
                                                 &eventHandler, nullptr);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_mqtt_client_register_event failed: %s",
             esp_err_to_name(err));

    return err;
  }

  notify(Status::Connecting);
  err = esp_mqtt_client_start(g_client);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_mqtt_client_start failed: %s", esp_err_to_name(err));

    return err;
  }

  ESP_LOGI(TAG, "mqtt client started (%s)", g_config.uri.c_str());

  return ESP_OK;
}

esp_err_t EspMqttClient::publish(const std::string &topic,
                                 const std::string &payload, int qos,
                                 bool retain) {
  if (g_client == nullptr) {
    return ESP_ERR_INVALID_STATE;
  }

  int msgId = esp_mqtt_client_publish(g_client, topic.c_str(), payload.data(),
                                      payload.size(), qos, retain ? 1 : 0);

  return msgId < 0 ? ESP_FAIL : ESP_OK;
}

esp_err_t EspMqttClient::subscribe(const std::string &topic, int qos) {
  if (g_client == nullptr) {
    return ESP_ERR_INVALID_STATE;
  }

  int msgId = esp_mqtt_client_subscribe_single(g_client, topic.c_str(), qos);

  return msgId < 0 ? ESP_FAIL : ESP_OK;
}

esp_err_t EspMqttClient::stop() {
  if (g_client == nullptr) {
    return ESP_OK;
  }

  g_connected = false;

  return esp_mqtt_client_stop(g_client);
}

bool EspMqttClient::isConnected() const { return g_connected; }

}  // namespace connectivity::mqtt
