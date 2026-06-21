#pragma once

#include "peripherals/flash_memory/application/flash_service.hpp"
#include "peripherals/flash_memory/infrastructure/esp_nvs_storage.hpp"

namespace flash_memory {

/**
 * @brief Module facade for flash memory access.
 *
 * Extends @c FlashService as the composition root of the flash_memory module.
 * Owns the @c EspNvsStorage adapter, wires it into the service, and exposes
 * the module lifecycle via @c begin().
 *
 * Consumers depend only on this class; read and write operations are
 * inherited from @c FlashService.
 */
class FlashMemory : public FlashService {
 public:
  /**
   * @brief Builds the facade, wiring the owned NVS adapter into the service.
   */
  FlashMemory();

  /**
   * @brief Initializes the NVS subsystem.
   *
   * Must be called once before any read or write operation. If the
   * partition is unusable (no free pages or a newer NVS format), it is
   * erased and initialized again; any provisioned data is lost.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t begin();

 private:
  EspNvsStorage _storage;
};

}  // namespace flash_memory
