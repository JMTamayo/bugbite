#pragma once

#include <driver/gpio.h>

#include "peripherals/digital_actuator/application/digital_actuator_service.hpp"
#include "peripherals/digital_actuator/infrastructure/gpio_digital_actuator.hpp"

namespace digital_actuator {

/**
 * @brief Reusable facade for an on/off digital actuator.
 *
 * Extends @c DigitalActuatorService as the composition root of the module.
 * Owns the @c GpioDigitalActuator adapter, wires it into the service, and
 * exposes the module lifecycle via @c begin().
 *
 * This class carries no peripheral-specific identity (no logging): concrete
 * peripherals such as relay::Relay or led::Led derive from it to add their
 * own naming and logging. The level operations are inherited from
 * @c DigitalActuatorService.
 *
 * The constructor is protected on purpose: this facade is not meant to be
 * instantiated on its own, only as the base of a concrete peripheral.
 */
class DigitalActuator : public DigitalActuatorService {
 protected:
  /**
   * @brief Builds the actuator bound to a GPIO and polarity.
   *
   * The pin is not touched yet; call @c begin() to configure it. Protected so
   * only derived peripherals can construct it.
   *
   * @param gpio     GPIO wired to the actuator control line.
   * @param inverted true when the actuator is wired active-low. Defaults to
   *                 false (normal, active-high logic).
   */
  DigitalActuator(gpio_num_t gpio, bool inverted = false);

  /**
   * @brief Initializes the actuator on its configured GPIO.
   *
   * Must be called once before any other operation. The actuator is left off.
   * Protected so only derived peripherals can run it.
   *
   * @return ESP_OK on success, or an ESP-IDF error code on failure.
   */
  esp_err_t begin();

  gpio_num_t _gpio;
  bool _inverted;

 private:
  GpioDigitalActuator _driver;
};

}  // namespace digital_actuator
