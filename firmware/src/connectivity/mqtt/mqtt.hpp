#pragma once

#include <string>

#include "connectivity/config/domain/i_device_config.hpp"
#include "connectivity/config/domain/i_mqtt_config.hpp"
#include "connectivity/mqtt/application/mqtt_service.hpp"
#include "connectivity/mqtt/domain/i_mqtt_client.hpp"
#include "connectivity/mqtt/infrastructure/esp_mqtt_client.hpp"

namespace connectivity::mqtt {

/**
 * @brief Module facade for MQTT connectivity.
 *
 * Composition root of the mqtt module: owns the esp-mqtt adapter and the
 * application service, wiring them to the MQTT and device configuration stores.
 * Connects asynchronously using the stored broker settings (client_id =
 * device_id) and exposes publish/subscribe primitives plus connection status.
 */
class Mqtt {
 public:
  /**
   * @brief Builds the MQTT module.
   *
   * @param config      MQTT configuration store. Must outlive this instance.
   * @param device      Device identity store. Must outlive this instance.
   * @param defaultPort Broker port used when none has been provisioned.
   */
  Mqtt(config::IMqttConfig &config, config::IDeviceConfig &device,
       std::string defaultPort);

  /**
   * @brief Connects to the broker using the stored settings.
   *
   * @return ESP_OK if the client started, ESP_ERR_INVALID_STATE if the host or
   *         device id are missing, or an ESP-IDF error code on failure.
   */
  esp_err_t start();

  /**
   * @brief Stops the client and disconnects.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t stop();

  /**
   * @brief Publishes a payload to a topic.
   */
  esp_err_t publish(const std::string &topic, const std::string &payload,
                    int qos = 0, bool retain = false);

  /**
   * @brief Subscribes to a topic (re-subscribe from the Connected callback).
   */
  esp_err_t subscribe(const std::string &topic, int qos = 0);

  /**
   * @brief Whether the client currently holds a broker connection.
   */
  bool isConnected() const;

  /**
   * @brief Registers a callback notified on every status transition.
   */
  void onStatusChange(StatusHandler handler);

  /**
   * @brief Registers a callback notified for every received message.
   */
  void onMessage(MessageHandler handler);

 private:
  EspMqttClient _client;
  MqttService _service;
  StatusHandler _userStatus;
  MessageHandler _userMessage;
};

}  // namespace connectivity::mqtt
