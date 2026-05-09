#pragma once

#include "digital_output.hpp"

namespace peripherals
{

    /// @brief Driver for a GPIO-controlled relay.
    ///
    /// Thin concrete subclass of DigitalOutput for relay peripherals. All GPIO control
    /// logic (begin, on, off, toggle, isOn) and safe shutdown (off on destruction)
    /// are inherited from the base class.
    ///
    /// Both active-high and active-low coil configurations are supported via the
    /// @p activeHigh constructor parameter — active-low is common in relay modules
    /// that use an inverting optocoupler.
    class Relay final : public DigitalOutput
    {
    public:
        /// @brief Construct a new Relay driver.
        /// @param pin        GPIO number connected to the relay coil control input.
        /// @param activeHigh Set to true if a HIGH level energizes the relay (active-high);
        ///                   set to false if a LOW level energizes it (active-low).
        Relay(gpio_num_t pin, bool activeHigh);
    };

} // namespace peripherals
