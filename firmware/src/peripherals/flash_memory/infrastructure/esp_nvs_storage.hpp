#pragma once

#include "peripherals/flash_memory/domain/i_flash_storage.hpp"

namespace flash_memory {

/**
 * @brief ESP-IDF NVS adapter for @c IFlashStorage.
 *
 * Implements the storage port using the ESP-IDF Non-Volatile Storage (NVS)
 * API. Each operation opens an NVS handle, performs the read or write, and
 * closes the handle immediately.
 */
class EspNvsStorage : public IFlashStorage {
 public:
  /**
   * @brief Reads a string value from NVS.
   *
   * Opens the namespace in read-only mode, queries the required buffer
   * size, reads the value, and closes the handle. A missing namespace or
   * key is an expected condition and is not logged as an error.
   *
   * @param ns  NVS namespace that contains the key.
   * @param key Key to look up.
   * @param out Receives the stored value on success. Left in an
   *            unspecified state on failure.
   * @return    ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if the namespace
   *            or key does not exist, or an ESP-IDF NVS error code on
   *            failure.
   */
  esp_err_t read(const std::string &ns, const std::string &key,
                 std::string &out) const override;

  /**
   * @brief Writes a string value to NVS.
   *
   * Opens the namespace in read-write mode, sets the value, commits the
   * change, and closes the handle.
   *
   * @param ns    NVS namespace where the key will be stored.
   * @param key   Key to write.
   * @param value Value to associate with the key.
   * @return      ESP_OK on success, or an ESP-IDF NVS error code on failure.
   */
  esp_err_t write(const std::string &ns, const std::string &key,
                  const std::string &value) override;
};

}  // namespace flash_memory
