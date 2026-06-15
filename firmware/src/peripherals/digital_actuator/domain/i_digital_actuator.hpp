#pragma once

#include <esp_err.h>

#include "peripherals/digital_actuator/domain/level.hpp"

namespace digital_actuator {

/**
 * @brief Port for a single digital output line.
 *
 * Defines the contract that any digital-output backend must fulfill (a relay,
 * an LED, a transistor-driven load, ...). The contract speaks in logical
 * @c Level (High = active, Low = inactive); concrete adapters translate it to
 * the physical pin level, including inverted (active-low) wiring.
 */
class IDigitalActuator {
 public:
  virtual ~IDigitalActuator() = default;

  /**
   * @brief Drives the line to the given logical level.
   *
   * @param level Logical level to apply.
   * @return      ESP_OK on success, ESP_ERR_INVALID_STATE if the driver is not
   *              initialized, or an ESP-IDF error code on failure.
   */
  virtual esp_err_t setLevel(Level level) = 0;

  /**
   * @brief Returns the current logical level.
   *
   * @return Level::HIGH if the line is logically active, Level::LOW otherwise.
   */
  virtual Level getLevel() const = 0;
};

}  // namespace digital_actuator
