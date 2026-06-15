#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <string>

#include "config.hpp"
#include "peripherals/flash_memory/flash_memory.hpp"
#include "peripherals/led/led.hpp"
#include "peripherals/relay/relay.hpp"
#include "setup.hpp"

static flash_memory::FlashMemory flashMemory;
static relay::Relay loadRelay(config::relay::GPIO, config::relay::INVERTED);
static led::Led builtinLed(config::builtin_led::GPIO,
                           config::builtin_led::INVERTED);

static const char *TAG = "app/main";

extern "C" void app_main() {
  if (setup(flashMemory, loadRelay, builtinLed) != ESP_OK) {
    ESP_LOGE(TAG, "setup failed; halting");
    return;
  }

  std::string deviceId;
  esp_err_t err = flashMemory.read(config::device::NVS_NAMESPACE,
                                   config::device::DEVICE_ID_KEY, deviceId);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "device_id unavailable (%s); provision the device",
             esp_err_to_name(err));
  } else {
    ESP_LOGI(TAG, "device_id: %s", deviceId.c_str());
  }

  // Idle loop: keeps app_main alive until the real application logic is added.
  while (true) {
    ESP_LOGI(TAG, "Device ID: %s", deviceId.c_str());
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
