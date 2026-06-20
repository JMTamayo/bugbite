#pragma once

#include <cstdint>

#include "connectivity/wifi/domain/i_wifi_station.hpp"

namespace connectivity::wifi {

/**
 * @brief esp_wifi station adapter.
 *
 * Implements @c IWifiStation in station mode. There is a single station per
 * device, so the esp_wifi/event-loop state lives at file scope in the
 * implementation; this keeps the WiFi stack headers out of the rest of the
 * codebase. On disconnect it retries indefinitely with a fixed delay.
 */
class EspWifiStation : public IWifiStation {
 public:
  /**
   * @brief Builds the adapter.
   *
   * @param reconnectDelayMs Delay between reconnection attempts.
   * @param maxTxPower        Maximum TX power in 0.25 dBm units, applied after
   *                          the station starts (antenna workaround).
   */
  EspWifiStation(std::uint32_t reconnectDelayMs, std::int8_t maxTxPower);

  /**
   * @brief Initializes the network stack and starts the station.
   *
   * Sets up esp_netif, the default event loop, the WiFi driver and the WiFi/IP
   * event handlers, then starts the station in STA mode. Idempotent: a no-op if
   * already initialized.
   *
   * @param onStatus Callback invoked on every status transition.
   * @return         ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t begin(StatusHandler onStatus) override;

  /**
   * @brief Launches a non-blocking connection attempt.
   *
   * Stores the credentials in the driver and calls esp_wifi_connect(); the
   * outcome arrives asynchronously via the status callback (Connected on
   * GOT_IP, Disconnected otherwise). Enables auto-reconnect.
   *
   * @param ssid     Network SSID.
   * @param password Network password (empty selects an open network).
   * @return         ESP_OK if the attempt was launched, or an ESP-IDF error.
   */
  esp_err_t connect(const std::string &ssid,
                    const std::string &password) override;

  /**
   * @brief Disables auto-reconnect, cancels any pending retry and disconnects.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t disconnect() override;

  /**
   * @brief Whether the station currently holds an IP-level connection.
   *
   * Set when an IP is acquired and cleared on disconnect.
   */
  bool isConnected() const override;
};

}  // namespace connectivity::wifi
