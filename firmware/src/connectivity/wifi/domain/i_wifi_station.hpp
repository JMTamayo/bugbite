#pragma once

#include <esp_err.h>

#include <functional>
#include <string>

namespace connectivity::wifi {

/// Connection state of the WiFi station.
enum class Status { Disconnected, Connecting, Connected };

/// Notified on every station status transition.
using StatusHandler = std::function<void(Status)>;

/**
 * @brief Port for a WiFi station radio.
 *
 * Abstracts the connection mechanics (esp_wifi here) behind start/connect/stop
 * operations and an asynchronous status callback. Keeps the radio stack out of
 * the application layer.
 */
class IWifiStation {
 public:
  virtual ~IWifiStation() = default;

  /**
   * @brief One-time initialization of the network stack and event handlers.
   *
   * @param onStatus Callback invoked on every status transition.
   * @return         ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t begin(StatusHandler onStatus) = 0;

  /**
   * @brief Connects to the given network (non-blocking).
   *
   * On drop, the station retries on its own. Status is reported via the
   * callback registered in @c begin().
   *
   * @param ssid     Network SSID.
   * @param password Network password (empty for an open network).
   * @return         ESP_OK if the attempt was launched, or an ESP-IDF error.
   */
  virtual esp_err_t connect(const std::string &ssid,
                            const std::string &password) = 0;

  /**
   * @brief Stops auto-reconnect and disconnects from the network.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t disconnect() = 0;

  /**
   * @brief Whether the station currently holds an IP-level connection.
   */
  virtual bool isConnected() const = 0;
};

}  // namespace connectivity::wifi
