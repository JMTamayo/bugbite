#pragma once

#include <string>

#include "peripherals/flash_memory/domain/i_flash_storage.hpp"

namespace flash_memory {

/**
 * @brief Application service for flash storage operations.
 *
 * Exposes read and write operations through the @c IFlashStorage port.
 * Consumers depend on this service rather than on any concrete adapter.
 */
class FlashService {
 public:
  /**
   * @brief Constructs the service with an injected storage backend.
   *
   * @param storage Reference to the storage adapter. Must outlive this
   * instance.
   */
  FlashService(IFlashStorage &storage);

  /**
   * @brief Reads a string value from the storage backend.
   *
   * @param ns  NVS namespace that contains the key.
   * @param key Key to look up.
   * @param out Receives the stored value on success. Left in an
   *            unspecified state on failure.
   * @return    ESP_OK on success, ESP_ERR_NVS_NOT_FOUND if the namespace
   *            or key does not exist, or an ESP-IDF error code on failure.
   */
  esp_err_t read(const std::string &ns, const std::string &key,
                 std::string &out) const;

  /**
   * @brief Writes a string value to the storage backend.
   *
   * @param ns    NVS namespace where the key will be stored.
   * @param key   Key to write.
   * @param value Value to associate with the key.
   * @return      ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t write(const std::string &ns, const std::string &key,
                  const std::string &value);

 private:
  IFlashStorage &_storage;
};

}  // namespace flash_memory
