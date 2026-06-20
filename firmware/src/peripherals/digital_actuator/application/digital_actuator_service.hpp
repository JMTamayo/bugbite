#pragma once

#include "peripherals/digital_actuator/domain/i_digital_actuator.hpp"
#include "peripherals/digital_actuator/domain/level.hpp"

namespace digital_actuator {

/**
 * @brief Application service for digital-output operations.
 *
 * Exposes the level operations through the @c IDigitalActuator port. Consumers
 * depend on this service rather than on any concrete adapter.
 */
class DigitalActuatorService {
 public:
  /**
   * @brief Constructs the service with an injected backend.
   *
   * @param driver Reference to the driver adapter. Must outlive this instance.
   */
  DigitalActuatorService(IDigitalActuator &driver);

  /**
   * @brief Drives the line to the given logical level.
   *
   * @param level Logical level to apply.
   * @return      ESP_OK on success, ESP_ERR_INVALID_STATE if the driver is not
   *              initialized, or an ESP-IDF error code on failure.
   */
  esp_err_t setLevel(Level level);

  /**
   * @brief Returns the current logical level.
   *
   * @return Level::HIGH if the line is logically active, Level::LOW otherwise.
   */
  Level getLevel() const;

 private:
  IDigitalActuator &_driver;
};

}  // namespace digital_actuator
