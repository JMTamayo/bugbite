#pragma once

#include "driver/gpio.h"

namespace config
{

    inline constexpr const char *PROJECT_NAME = "bugbite";

    /// @brief GPIO pin assignments and active-level settings for onboard peripherals.
    ///
    /// Active-level flags decouple the logical on/off semantics from the physical
    /// wiring polarity, allowing the same driver code to handle both active-high
    /// and active-low hardware configurations without modification.
    namespace pins
    {

        /// Built-in LED on the ESP32-C3-SUPERMINI (GPIO8).
        /// Most SUPERMINI variants wire this LED active-low — verify against your
        /// board's schematic if the behaviour seems inverted.
        inline constexpr gpio_num_t LED = GPIO_NUM_8;
        inline constexpr bool LED_ACTIVE_HIGH = false;

        /// Relay coil control output (GPIO2, general-purpose, conflict-free).
        inline constexpr gpio_num_t RELAY = GPIO_NUM_2;
        inline constexpr bool RELAY_ACTIVE_HIGH = true;

    } // namespace pins

} // namespace config
