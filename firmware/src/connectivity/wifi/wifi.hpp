#pragma once

#include <cstdint>

#include "connectivity/config/domain/i_wifi_config.hpp"
#include "connectivity/wifi/application/wifi_service.hpp"
#include "connectivity/wifi/domain/i_wifi_station.hpp"
#include "connectivity/wifi/infrastructure/esp_wifi_station.hpp"

namespace connectivity::wifi {

/**
 * @brief Module facade for WiFi station connectivity.
 *
 * Composition root of the wifi module: owns the esp_wifi adapter and the
 * application service, wiring them to the WiFi configuration store. Connects
 * asynchronously using the stored credentials and keeps the link up with
 * indefinite reconnection. Exposes the connection status for the application.
 */
class Wifi {
 public:
  /**
   * @brief Builds the WiFi module.
   *
   * @param config           WiFi configuration store. Must outlive this
   *                         instance.
   * @param reconnectDelayMs Delay between reconnection attempts.
   * @param maxTxPower        Maximum TX power in 0.25 dBm units (antenna
   *                          workaround for ESP32-C3 SuperMini boards).
   */
  Wifi(config::IWifiConfig &config, std::uint32_t reconnectDelayMs,
       std::int8_t maxTxPower);

  /**
   * @brief Brings the station up and connects using stored credentials.
   *
   * @return ESP_OK if the connection attempt was launched,
   * ESP_ERR_INVALID_STATE if there are no credentials yet, or an ESP-IDF error
   * code on failure.
   */
  esp_err_t start();

  /**
   * @brief Stops auto-reconnect and disconnects.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t stop();

  /**
   * @brief Whether the station currently holds a connection.
   */
  bool isConnected() const;

  /**
   * @brief Registers a callback notified on every status transition.
   */
  void onStatusChange(StatusHandler handler);

 private:
  EspWifiStation _station;
  WifiService _service;
  StatusHandler _userHandler;
};

}  // namespace connectivity::wifi
