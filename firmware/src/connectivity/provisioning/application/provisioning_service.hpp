#pragma once

#include <array>
#include <cstdint>

#include "connectivity/config/domain/i_device_config.hpp"
#include "connectivity/config/domain/i_mqtt_config.hpp"
#include "connectivity/config/domain/i_wifi_config.hpp"
#include "connectivity/provisioning/domain/i_gatt_server.hpp"

namespace connectivity::provisioning {

/**
 * @brief 128-bit UUIDs for the provisioning service and its characteristics.
 *
 * Injected at construction so the module holds no hard-coded protocol values;
 * main.cpp supplies them from the central config header.
 */
struct ProvisioningUuids {
  std::array<std::uint8_t, 16> service;
  std::array<std::uint8_t, 16> deviceId;
  std::array<std::uint8_t, 16> wifiSsid;
  std::array<std::uint8_t, 16> wifiPassword;
  std::array<std::uint8_t, 16> mqttHost;
  std::array<std::uint8_t, 16> mqttPort;
  std::array<std::uint8_t, 16> mqttUser;
  std::array<std::uint8_t, 16> mqttPassword;
};

/**
 * @brief Application service that defines the provisioning GATT contract.
 *
 * Builds the @c ServiceSpec exposing WiFi and MQTT parameters as
 * characteristics, binding each one's read/write handlers to the configuration
 * store. Read handlers are read-through to flash; write handlers are
 * write-through to flash. Passwords are write-only at the BLE layer (no read
 * handler), even though they remain readable from flash for the WiFi/MQTT
 * modules.
 */
class ProvisioningService {
 public:
  /**
   * @brief Builds the service over the configuration ports.
   *
   * @param device Device identity store (exposes device_id, read-only).
   * @param wifi   WiFi configuration store. Must outlive this instance.
   * @param mqtt   MQTT configuration store. Must outlive this instance.
   * @param uuids  UUIDs for the service and characteristics.
   */
  ProvisioningService(config::IDeviceConfig &device, config::IWifiConfig &wifi,
                      config::IMqttConfig &mqtt,
                      const ProvisioningUuids &uuids);

  /**
   * @brief Assembles the GATT service description.
   *
   * @return A @c ServiceSpec with the provisioning characteristics and their
   *         handlers wired to the configuration store.
   */
  ServiceSpec buildService() const;

 private:
  config::IDeviceConfig &_device;
  config::IWifiConfig &_wifi;
  config::IMqttConfig &_mqtt;
  ProvisioningUuids _uuids;
};

}  // namespace connectivity::provisioning
