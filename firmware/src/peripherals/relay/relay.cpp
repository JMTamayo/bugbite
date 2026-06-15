#include "peripherals/relay/relay.hpp"

#include <esp_log.h>

#include "peripherals/relay/tag.hpp"

namespace relay {

Relay::Relay(gpio_num_t gpio, bool inverted)
    : digital_actuator::DigitalActuator(gpio, inverted) {}

esp_err_t Relay::begin() {
  esp_err_t err = digital_actuator::DigitalActuator::begin();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "relay initialization failed: %s", esp_err_to_name(err));

    return err;
  }

  ESP_LOGI(TAG, "relay ready (%s logic)", _inverted ? "inverted" : "normal");

  return ESP_OK;
}

esp_err_t Relay::activate() {
  esp_err_t err = setLevel(digital_actuator::Level::HIGH);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to activate relay: %s", esp_err_to_name(err));

    return err;
  }

  ESP_LOGI(TAG, "relay activated");

  return ESP_OK;
}

esp_err_t Relay::deactivate() {
  esp_err_t err = setLevel(digital_actuator::Level::LOW);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to deactivate relay: %s", esp_err_to_name(err));

    return err;
  }

  ESP_LOGI(TAG, "relay deactivated");

  return ESP_OK;
}

}  // namespace relay
