#include "peripherals/digital_actuator/application/digital_actuator_service.hpp"

namespace digital_actuator {

DigitalActuatorService::DigitalActuatorService(IDigitalActuator &driver)
    : _driver(driver) {}

esp_err_t DigitalActuatorService::setLevel(Level level) {
  return _driver.setLevel(level);
}

Level DigitalActuatorService::getLevel() const { return _driver.getLevel(); }

}  // namespace digital_actuator
