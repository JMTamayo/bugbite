#pragma once

#include "connectivity/provisioning/domain/i_gatt_server.hpp"

namespace connectivity::provisioning {

/**
 * @brief NimBLE adapter implementing the GATT server port.
 *
 * Realizes a @c ServiceSpec as a NimBLE GATT service and drives connectable,
 * undirected advertising. There is a single provisioning server per device, so
 * the NimBLE host state is owned at file scope in the implementation; this
 * keeps the BLE stack headers out of the rest of the codebase.
 *
 * No security is configured (plaintext link): characteristics carry no
 * encryption/authentication flags and no pairing/bonding is performed.
 */
class NimbleGattServer : public IGattServer {
 public:
  /**
   * @brief Brings up the NimBLE host (first call) and starts advertising.
   *
   * Registers the GATT service from @p service on the first call; later calls
   * only re-start advertising under @p deviceName.
   *
   * @param deviceName Advertised device name.
   * @param service    Service and characteristics to expose.
   * @return           ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t start(const std::string &deviceName,
                  const ServiceSpec &service) override;

  /**
   * @brief Stops advertising and drops the active connection.
   *
   * The NimBLE host stays initialized so a later @c start() is cheap.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t stop() override;
};

}  // namespace connectivity::provisioning
