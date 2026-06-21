#pragma once

#include <string>

#include "connectivity/config/domain/i_device_config.hpp"
#include "connectivity/config/domain/i_mqtt_config.hpp"
#include "connectivity/mqtt/domain/i_mqtt_client.hpp"

namespace connectivity::mqtt {

/**
 * @brief Application service that drives the MQTT client from stored config.
 *
 * Reads the broker settings from the MQTT configuration store and the client id
 * from the device identity store (client_id = device_id), assembles the
 * @c ClientConfig and asks the client to connect. publish/subscribe/stop are
 * thin pass-throughs to the client.
 */
class MqttService {
 public:
  /**
   * @brief Builds the service.
   *
   * @param client      MQTT client port. Must outlive this instance.
   * @param config      MQTT configuration store. Must outlive this instance.
   * @param device      Device identity store. Must outlive this instance.
   * @param defaultPort Broker port used when none has been provisioned.
   */
  MqttService(IMqttClient &client, config::IMqttConfig &config,
              config::IDeviceConfig &device, std::string defaultPort);

  /**
   * @brief Connects using the stored broker settings and device id.
   *
   * @param onStatus  Callback invoked on every status transition.
   * @param onMessage Callback invoked for every received message.
   * @return          ESP_OK if the client started, ESP_ERR_INVALID_STATE if the
   *                  host or device id are missing, or an ESP-IDF error code.
   */
  esp_err_t connect(StatusHandler onStatus, MessageHandler onMessage);

  esp_err_t publish(const std::string &topic, const std::string &payload,
                    int qos, bool retain);
  esp_err_t subscribe(const std::string &topic, int qos);
  esp_err_t stop();
  bool isConnected() const;

 private:
  IMqttClient &_client;
  config::IMqttConfig &_config;
  config::IDeviceConfig &_device;
  std::string _defaultPort;
};

}  // namespace connectivity::mqtt
