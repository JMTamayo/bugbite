#pragma once

#include "connectivity/mqtt/domain/i_mqtt_client.hpp"

namespace connectivity::mqtt {

/**
 * @brief esp-mqtt client adapter.
 *
 * Implements @c IMqttClient over the ESP-IDF esp-mqtt component. There is a
 * single client per device, so the client handle and state live at file scope
 * in the implementation; this keeps the MQTT stack headers out of the rest of
 * the codebase. esp-mqtt reconnects to the broker on its own.
 */
class EspMqttClient : public IMqttClient {
 public:
  /**
   * @brief Initializes the esp-mqtt client and starts it.
   *
   * Builds the client config from @p config, registers the event handler and
   * starts the client (non-blocking). Idempotent: a no-op if already started.
   *
   * @param config    Broker URI and credentials.
   * @param onStatus  Callback invoked on every status transition.
   * @param onMessage Callback invoked for every received message.
   * @return          ESP_OK if the client started, or an ESP-IDF error code.
   */
  esp_err_t begin(const ClientConfig &config, StatusHandler onStatus,
                  MessageHandler onMessage) override;

  /**
   * @brief Publishes a payload to a topic.
   *
   * @return ESP_OK on success, ESP_ERR_INVALID_STATE if not started, or
   *         ESP_FAIL if the client rejected the message.
   */
  esp_err_t publish(const std::string &topic, const std::string &payload,
                    int qos, bool retain) override;

  /**
   * @brief Subscribes to a topic.
   *
   * @return ESP_OK on success, ESP_ERR_INVALID_STATE if not started, or
   *         ESP_FAIL on failure.
   */
  esp_err_t subscribe(const std::string &topic, int qos) override;

  /**
   * @brief Stops the client and disconnects from the broker.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t stop() override;

  /**
   * @brief Whether the client currently holds a broker connection.
   */
  bool isConnected() const override;
};

}  // namespace connectivity::mqtt
