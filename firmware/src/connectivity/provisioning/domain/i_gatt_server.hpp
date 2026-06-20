#pragma once

#include <esp_err.h>

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace connectivity::provisioning {

/// Produces the value returned on a characteristic read.
using ReadHandler = std::function<esp_err_t(std::string &out)>;

/// Consumes the value received on a characteristic write.
using WriteHandler = std::function<esp_err_t(const std::string &in)>;

/**
 * @brief Transport-agnostic description of a GATT characteristic.
 *
 * Carries its 128-bit UUID, a human-readable name (exposed as the 0x2901 user
 * descriptor), the access flags and the handlers invoked on read/write. A
 * handler is only used when the matching flag is set.
 */
struct CharacteristicSpec {
  std::array<std::uint8_t, 16> uuid;
  std::string name;
  bool readable;
  bool writable;
  ReadHandler onRead;
  WriteHandler onWrite;
};

/**
 * @brief Transport-agnostic description of a GATT service.
 */
struct ServiceSpec {
  std::array<std::uint8_t, 16> uuid;
  std::vector<CharacteristicSpec> characteristics;
};

/**
 * @brief Port for a startable/stoppable GATT server.
 *
 * Keeps the BLE stack out of the application layer: the provisioning service
 * builds a @c ServiceSpec and any adapter (NimBLE here) realizes it.
 */
class IGattServer {
 public:
  virtual ~IGattServer() = default;

  /**
   * @brief Brings the server up and starts advertising.
   *
   * @param deviceName Advertised device name.
   * @param service    Service and characteristics to expose.
   * @return           ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t start(const std::string &deviceName,
                          const ServiceSpec &service) = 0;

  /**
   * @brief Stops advertising and drops active connections.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t stop() = 0;
};

}  // namespace connectivity::provisioning
