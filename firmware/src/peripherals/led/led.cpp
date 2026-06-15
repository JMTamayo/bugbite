#include "peripherals/led/led.hpp"

#include <esp_log.h>

#include "peripherals/led/tag.hpp"

namespace led {

Led::Led(gpio_num_t gpio, bool inverted)
    : digital_actuator::DigitalActuator(gpio, inverted) {}

esp_err_t Led::begin() {
  esp_err_t err = digital_actuator::DigitalActuator::begin();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "led initialization failed: %s", esp_err_to_name(err));

    return err;
  }

  ESP_LOGI(TAG, "led ready (%s logic)", _inverted ? "inverted" : "normal");

  return ESP_OK;
}

esp_err_t Led::turnOn() {
  esp_err_t err = setLevel(digital_actuator::Level::HIGH);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to turn on led: %s", esp_err_to_name(err));

    return err;
  }

  ESP_LOGI(TAG, "led on");

  return ESP_OK;
}

esp_err_t Led::turnOff() {
  esp_err_t err = setLevel(digital_actuator::Level::LOW);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to turn off led: %s", esp_err_to_name(err));

    return err;
  }

  ESP_LOGI(TAG, "led off");

  return ESP_OK;
}

}  // namespace led
