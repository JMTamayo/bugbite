#pragma once

#include <esp_err.h>

#include <string>

namespace flash_memory {

/**
 * @brief Port for key-value persistent storage.
 *
 * Defines the contract that any storage backend must fulfill.
 * Concrete adapters implement this interface to decouple the
 * application layer from the underlying storage technology.
 */
class IFlashStorage {
 public:
  virtual ~IFlashStorage() = default;

  /**
   * @brief Reads a string value from storage.
   *
   * @param ns  NVS namespace that contains the key.
   * @param key Key to look up.
   * @param out Receives the stored value on success. Left in an
   *            unspecified state on failure.
   * @return    ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if the namespace
   *            or key does not exist, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t read(const std::string &ns, const std::string &key,
                         std::string &out) const = 0;

  /**
   * @brief Writes a string value to storage.
   *
   * @param ns    NVS namespace where the key will be stored.
   * @param key   Key to write.
   * @param value Value to associate with the key.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t write(const std::string &ns, const std::string &key,
                          const std::string &value) = 0;
};

}  // namespace flash_memory
