#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "build.hpp"
#include "config.hpp"
#include "connectivity/config/infrastructure/flash_device_config.hpp"
#include "connectivity/config/infrastructure/flash_mqtt_config.hpp"
#include "connectivity/config/infrastructure/flash_wifi_config.hpp"
#include "connectivity/provisioning/provisioning.hpp"
#include "peripherals/flash_memory/flash_memory.hpp"
#include "peripherals/led/led.hpp"
#include "peripherals/relay/relay.hpp"

static const char *TAG = "app/main";

static flash_memory::FlashMemory flashMemory;
static connectivity::config::FlashDeviceConfig deviceConfig(
    flashMemory, {config::nvs::NAMESPACE, config::device::ID_KEY,
                  config::device::PROJECT_NAME});
static connectivity::config::FlashWifiConfig wifiConfig(
    flashMemory, {config::nvs::NAMESPACE, config::wifi::SSID_KEY,
                  config::wifi::PASSWORD_KEY});
static connectivity::config::FlashMqttConfig mqttConfig(
    flashMemory,
    {config::nvs::NAMESPACE, config::mqtt::HOST_KEY, config::mqtt::PORT_KEY,
     config::mqtt::USER_KEY, config::mqtt::PASSWORD_KEY});
static relay::Relay loadRelay(config::relay::GPIO, config::relay::INVERTED);
static led::Led builtinLed(config::builtin_led::GPIO,
                           config::builtin_led::INVERTED);
static connectivity::provisioning::Provisioning provisioning(
    deviceConfig, wifiConfig, mqttConfig,
    {.service = config::ble::SERVICE_UUID,
     .deviceId = config::ble::DEVICE_ID_UUID,
     .wifiSsid = config::ble::WIFI_SSID_UUID,
     .wifiPassword = config::ble::WIFI_PASSWORD_UUID,
     .mqttHost = config::ble::MQTT_HOST_UUID,
     .mqttPort = config::ble::MQTT_PORT_UUID,
     .mqttUser = config::ble::MQTT_USER_UUID,
     .mqttPassword = config::ble::MQTT_PASSWORD_UUID});

extern "C" void app_main() {
  if (beginPeripherals(flashMemory, loadRelay, builtinLed) != ESP_OK) {
    ESP_LOGE(TAG, "setup failed; halting");

    return;
  }

  esp_err_t err = provisioning.start();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "provisioning start failed: %s", esp_err_to_name(err));
  }

  // Idle loop: keeps app_main alive until the real application logic is added.
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
