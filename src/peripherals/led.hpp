#pragma once

#include "digital_output.hpp"

namespace peripherals
{

    /// @brief Driver for a GPIO-controlled LED.
    ///
    /// Thin concrete subclass of DigitalOutput for LED peripherals. All GPIO control
    /// logic (begin, on, off, toggle, isOn) is inherited from the base class.
    class Led final : public DigitalOutput
    {
    public:
        /// @brief Construct a new Led driver.
        /// @param pin        GPIO number the LED is connected to.
        /// @param activeHigh Set to true if a HIGH level illuminates the LED (active-high);
        ///                   set to false if a LOW level illuminates it (active-low).
        Led(gpio_num_t pin, bool activeHigh);
    };

} // namespace peripherals
