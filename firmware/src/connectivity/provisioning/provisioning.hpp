#pragma once

#include <esp_err.h>

#include <string>

#include "connectivity/config/domain/i_device_config.hpp"
#include "connectivity/config/domain/i_mqtt_config.hpp"
#include "connectivity/config/domain/i_wifi_config.hpp"
#include "connectivity/provisioning/application/provisioning_service.hpp"
#include "connectivity/provisioning/infrastructure/nimble_gatt_server.hpp"

namespace connectivity::provisioning {

/**
 * @brief Module facade for BLE provisioning.
 *
 * Composition root of the provisioning module: owns the GATT server adapter and
 * the application service, wiring the configuration ports and UUIDs together.
 * Exposes a start/stop lifecycle so provisioning availability can be managed
 * flexibly (e.g. provisioning windows, event-triggered activation) in the
 * future; for now the application starts it once at boot.
 */
class Provisioning {
 public:
  /**
   * @brief Builds the provisioning module.
   *
   * @param device Device identity store. Must outlive this instance.
   * @param wifi   WiFi configuration store. Must outlive this instance.
   * @param mqtt   MQTT configuration store. Must outlive this instance.
   * @param uuids  UUIDs for the service and characteristics.
   */
  Provisioning(config::IDeviceConfig &device, config::IWifiConfig &wifi,
               config::IMqttConfig &mqtt, const ProvisioningUuids &uuids);

  /**
   * @brief Brings the BLE server up and starts advertising.
   *
   * Reads the device_id to build the advertised name (project name plus a short
   * device_id suffix). Fails if the device has not been provisioned (no
   * device_id), since the device cannot be identified over BLE in that case.
   *
   * @return ESP_OK on success, ESP_ERR_INVALID_STATE if the device_id is
   *         missing or empty, or an ESP-IDF error code on failure.
   */
  esp_err_t start();

  /**
   * @brief Stops advertising and drops active connections.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t stop();

 private:
  config::IDeviceConfig &_device;
  ProvisioningService _service;
  NimbleGattServer _server;
};

}  // namespace connectivity::provisioning
