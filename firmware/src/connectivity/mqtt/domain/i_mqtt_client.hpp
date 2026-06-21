#pragma once

#include <esp_err.h>

#include <functional>
#include <string>

namespace connectivity::mqtt {

/// Connection state of the MQTT client.
enum class Status { Disconnected, Connecting, Connected };

/// Notified on every client status transition.
using StatusHandler = std::function<void(Status)>;

/// An inbound MQTT message.
struct Message {
  std::string topic;
  std::string payload;
};

/// Notified for every received message on a subscribed topic.
using MessageHandler = std::function<void(const Message &)>;

/// Everything the client needs to connect to the broker.
struct ClientConfig {
  std::string uri;       // e.g. mqtt://host:port
  std::string clientId;  // the device id
  std::string username;  // empty for anonymous
  std::string password;  // empty for anonymous
};

/**
 * @brief Port for an MQTT client.
 *
 * Abstracts the broker connection (esp-mqtt here) behind connect/publish/
 * subscribe operations and asynchronous status/message callbacks. Keeps the
 * MQTT stack out of the application layer.
 */
class IMqttClient {
 public:
  virtual ~IMqttClient() = default;

  /**
   * @brief Initializes the client and starts connecting (non-blocking).
   *
   * The outcome and inbound messages arrive asynchronously via the callbacks.
   * The transport reconnects on its own while running.
   *
   * @param config    Broker URI and credentials.
   * @param onStatus  Callback invoked on every status transition.
   * @param onMessage Callback invoked for every received message.
   * @return          ESP_OK if the client started, or an ESP-IDF error code.
   */
  virtual esp_err_t begin(const ClientConfig &config, StatusHandler onStatus,
                          MessageHandler onMessage) = 0;

  /**
   * @brief Publishes a payload to a topic.
   *
   * @param topic   Destination topic.
   * @param payload Message payload (may be binary).
   * @param qos     MQTT QoS (0, 1 or 2).
   * @param retain  Whether the broker should retain the message.
   * @return        ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t publish(const std::string &topic,
                            const std::string &payload, int qos,
                            bool retain) = 0;

  /**
   * @brief Subscribes to a topic.
   *
   * Subscriptions are not restored automatically after a reconnect;
   * re-subscribe from the @c Connected status callback.
   *
   * @param topic Topic filter to subscribe to.
   * @param qos   Requested MQTT QoS.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t subscribe(const std::string &topic, int qos) = 0;

  /**
   * @brief Stops the client and disconnects from the broker.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t stop() = 0;

  /**
   * @brief Whether the client currently holds a broker connection.
   */
  virtual bool isConnected() const = 0;
};

}  // namespace connectivity::mqtt
