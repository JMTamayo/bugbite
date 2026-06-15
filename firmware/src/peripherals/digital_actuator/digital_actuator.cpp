#include "peripherals/digital_actuator/digital_actuator.hpp"

namespace digital_actuator {

DigitalActuator::DigitalActuator(gpio_num_t gpio, bool inverted)
    : DigitalActuatorService(_driver), _gpio(gpio), _inverted(inverted) {}

esp_err_t DigitalActuator::begin() { return _driver.begin(_gpio, _inverted); }

}  // namespace digital_actuator
