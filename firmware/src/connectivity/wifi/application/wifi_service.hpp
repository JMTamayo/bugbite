#pragma once

#include "connectivity/config/domain/i_wifi_config.hpp"
#include "connectivity/wifi/domain/i_wifi_station.hpp"

namespace connectivity::wifi {

/**
 * @brief Application service that drives the WiFi station from stored config.
 *
 * Reads the credentials from the configuration store and asks the station to
 * connect. The radio handles reconnection on its own with the stored config.
 */
class WifiService {
 public:
  /**
   * @brief Builds the service over a station and a configuration store.
   *
   * @param station WiFi station port. Must outlive this instance.
   * @param config  WiFi configuration store. Must outlive this instance.
   */
  WifiService(IWifiStation &station, config::IWifiConfig &config);

  /**
   * @brief Connects using the credentials in storage.
   *
   * @return ESP_OK if the attempt was launched, ESP_ERR_INVALID_STATE if there
   *         are no credentials yet, or an ESP-IDF error code on failure.
   */
  esp_err_t connect();

  /**
   * @brief Whether the station currently holds a connection.
   */
  bool isConnected() const;

 private:
  IWifiStation &_station;
  config::IWifiConfig &_config;
};

}  // namespace connectivity::wifi
