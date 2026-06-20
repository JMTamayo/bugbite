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
  esp_err_t start(const std::string &deviceName,
                  const ServiceSpec &service) override;
  esp_err_t stop() override;
};

}  // namespace connectivity::provisioning
