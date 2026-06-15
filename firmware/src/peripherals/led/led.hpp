#pragma once

#include <driver/gpio.h>

#include "peripherals/digital_actuator/digital_actuator.hpp"

namespace led {

/**
 * @brief Module facade for LED control.
 *
 * An LED is driven by a digital output line, so it is implemented in terms of
 * the digital_actuator stack (port, service and GPIO adapter) via private
 * inheritance from digital_actuator::DigitalActuator. Private inheritance keeps
 * the level-based API (setLevel/getLevel) internal: consumers only see the
 * device-level @c turnOn() / @c turnOff() vocabulary (mapped to the line's
 * high/low levels) plus this class's own logging.
 */
class Led : private digital_actuator::DigitalActuator {
 public:
  /**
   * @brief Builds the LED bound to a GPIO and polarity.
   *
   * The pin is not touched yet; call @c begin() to configure it.
   *
   * @param gpio     GPIO wired to the LED.
   * @param inverted true when the LED is wired active-low. Defaults to false
   *                 (normal, active-high logic).
   */
  Led(gpio_num_t gpio, bool inverted = false);

  /**
   * @brief Initializes the LED on its configured GPIO.
   *
   * Must be called once before any other operation. The LED is left off.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t begin();

  /**
   * @brief Lights the LED and logs the change.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t turnOn();

  /**
   * @brief Turns the LED off and logs the change.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t turnOff();
};

}  // namespace led
