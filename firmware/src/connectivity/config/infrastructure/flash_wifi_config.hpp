#pragma once

#include "connectivity/config/domain/i_wifi_config.hpp"
#include "peripherals/flash_memory/application/flash_service.hpp"

namespace connectivity::config {

/**
 * @brief NVS namespace and keys for the WiFi configuration fields.
 *
 * Injected at construction so the adapter holds no hard-coded configuration;
 * main.cpp supplies the values from the central config header.
 */
struct WifiConfigKeys {
  const char *ns;
  const char *ssid;
  const char *password;
};

/**
 * @brief Flash-backed adapter for WiFi configuration.
 *
 * Implements @c IWifiConfig on top of the flash_memory service. Each operation
 * is a direct read-through / write-through to NVS (no in-RAM cache), mapping
 * each WiFi field to its injected NVS key.
 */
class FlashWifiConfig : public IWifiConfig {
 public:
  /**
   * @brief Builds the adapter over a flash service.
   *
   * @param flash Reference to the flash service. Must outlive this instance.
   * @param keys  NVS namespace and keys for the WiFi fields.
   */
  FlashWifiConfig(flash_memory::FlashService &flash,
                  const WifiConfigKeys &keys);

  esp_err_t getSsid(std::string &out) const override;
  esp_err_t setSsid(const std::string &value) override;

  esp_err_t getPassword(std::string &out) const override;
  esp_err_t setPassword(const std::string &value) override;

 private:
  flash_memory::FlashService &_flash;
  WifiConfigKeys _keys;
};

}  // namespace connectivity::config
