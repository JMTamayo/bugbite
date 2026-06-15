#pragma once

#include <driver/gpio.h>

#include "peripherals/digital_actuator/domain/i_digital_actuator.hpp"

namespace digital_actuator {

/**
 * @brief GPIO adapter for @c IDigitalActuator.
 *
 * Implements the port by driving a single GPIO configured as input/output, so
 * the current level is read back from the pad rather than cached. Supports
 * inverted (active-low) wiring: the logical level requested by consumers is
 * translated to the matching physical level.
 */
class GpioDigitalActuator : public IDigitalActuator {
 public:
  GpioDigitalActuator() = default;

  GpioDigitalActuator(const GpioDigitalActuator &) = delete;
  GpioDigitalActuator &operator=(const GpioDigitalActuator &) = delete;

  /**
   * @brief Configures the GPIO as input/output and leaves the line low.
   *
   * @param gpio     GPIO wired to the output line.
   * @param inverted true when the line is wired active-low (it reaches its
   *                 high/active level when the GPIO is driven low).
   * @return         ESP_OK on success, ESP_ERR_INVALID_STATE if already
   *                 initialized, or an ESP-IDF error code on failure.
   */
  esp_err_t begin(gpio_num_t gpio, bool inverted);

  /**
   * @brief Drives the line to the given logical level.
   *
   * Applies the polarity mapping: with inverted wiring a logical HIGH drives
   * the GPIO low.
   *
   * @param level Logical level to apply.
   * @return      ESP_OK on success, ESP_ERR_INVALID_STATE if @c begin() has
   *              not succeeded, or an ESP-IDF error code on failure.
   */
  esp_err_t setLevel(Level level) override;

  /**
   * @brief Returns the current logical level.
   *
   * Reads the physical level driven on the pad and undoes the polarity
   * mapping, so no software copy of the level is kept.
   *
   * @return Level::HIGH if the line is logically active, Level::LOW otherwise.
   */
  Level getLevel() const override;

 private:
  gpio_num_t _gpio = GPIO_NUM_NC;
  bool _inverted = false;
  bool _initialized = false;
};

}  // namespace digital_actuator
