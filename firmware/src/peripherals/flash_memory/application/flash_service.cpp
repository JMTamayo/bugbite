#include "peripherals/flash_memory/application/flash_service.hpp"

#include <esp_log.h>

#include "peripherals/flash_memory/tag.hpp"

namespace flash_memory {

FlashService::FlashService(IFlashStorage &storage) : _storage(storage) {}

esp_err_t FlashService::read(const std::string &ns, const std::string &key,
                             std::string &out) const {
  ESP_LOGD(TAG, "read ns='%s' key='%s'", ns.c_str(), key.c_str());

  return _storage.read(ns, key, out);
}

esp_err_t FlashService::write(const std::string &ns, const std::string &key,
                              const std::string &value) {
  ESP_LOGD(TAG, "write ns='%s' key='%s'", ns.c_str(), key.c_str());

  return _storage.write(ns, key, value);
}

}  // namespace flash_memory
