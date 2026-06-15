#include "peripherals/digital_actuator/infrastructure/gpio_digital_actuator.hpp"

#include <esp_log.h>

#include "peripherals/digital_actuator/tag.hpp"

namespace digital_actuator {

esp_err_t GpioDigitalActuator::begin(gpio_num_t gpio, bool inverted) {
  if (_initialized) {
    return ESP_ERR_INVALID_STATE;
  }

  gpio_config_t cfg = {};
  cfg.pin_bit_mask = 1ULL << gpio;
  cfg.mode = GPIO_MODE_INPUT_OUTPUT;
  cfg.pull_up_en = GPIO_PULLUP_DISABLE;
  cfg.pull_down_en = GPIO_PULLDOWN_DISABLE;
  cfg.intr_type = GPIO_INTR_DISABLE;

  esp_err_t err = gpio_config(&cfg);
  if (err != ESP_OK) {
    return err;
  }

  _gpio = gpio;
  _inverted = inverted;
  _initialized = true;

  return setLevel(Level::LOW);
}

esp_err_t GpioDigitalActuator::setLevel(Level level) {
  if (!_initialized) {
    return ESP_ERR_INVALID_STATE;
  }

  bool high = level == Level::HIGH;
  uint32_t physical = (high != _inverted) ? 1 : 0;

  esp_err_t err = gpio_set_level(_gpio, physical);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "failed to set GPIO %d to %s: %s", _gpio,
             high ? "HIGH" : "LOW", esp_err_to_name(err));

    return err;
  }

  ESP_LOGD(TAG, "GPIO %d set to %s (pin level %d)", _gpio,
           high ? "HIGH" : "LOW", static_cast<int>(physical));

  return ESP_OK;
}

Level GpioDigitalActuator::getLevel() const {
  bool high = static_cast<bool>(gpio_get_level(_gpio)) != _inverted;

  return high ? Level::HIGH : Level::LOW;
}

}  // namespace digital_actuator
