#pragma once

#include <driver/gpio.h>

#include "peripherals/digital_actuator/digital_actuator.hpp"

namespace relay {

/**
 * @brief Module facade for relay control.
 *
 * A relay is driven by a digital output line, so it is implemented in terms of
 * the digital_actuator stack (port, service and GPIO adapter) via private
 * inheritance from digital_actuator::DigitalActuator. Private inheritance keeps
 * the level-based API (setLevel/getLevel) internal: consumers only see the
 * device-level @c turnOn() / @c turnOff() vocabulary (mapped to the line's
 * high/low levels) plus this class's own logging.
 */
class Relay : private digital_actuator::DigitalActuator {
 public:
  /**
   * @brief Builds the relay bound to a GPIO and polarity.
   *
   * The pin is not touched yet; call @c begin() to configure it.
   *
   * @param gpio     GPIO wired to the relay control line.
   * @param inverted true when the relay is wired active-low. Defaults to
   *                 false (normal, active-high logic).
   */
  Relay(gpio_num_t gpio, bool inverted = false);

  /**
   * @brief Initializes the relay on its configured GPIO.
   *
   * Must be called once before any other operation. The relay is left off
   * (load de-energized).
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t begin();

  /**
   * @brief Energizes the load and logs the change.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t activate();

  /**
   * @brief De-energizes the load and logs the change.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t deactivate();
};

}  // namespace relay
