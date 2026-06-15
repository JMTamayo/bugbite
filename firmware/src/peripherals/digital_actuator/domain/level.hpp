#pragma once

#include <cstdint>

namespace digital_actuator {

/**
 * @brief Logical level of a digital output line.
 *
 * HIGH means the active state, LOW the inactive one. Concrete adapters map
 * this logical level to the physical pin level, applying inverted
 * (active-low) wiring when configured.
 */
enum class Level : uint8_t {
  LOW = 0,
  HIGH = 1,
};

}  // namespace digital_actuator
