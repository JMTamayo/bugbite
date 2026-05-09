#pragma once

#include "driver/gpio.h"

namespace peripherals
{

    /// @brief Abstract base class for a single-channel GPIO digital output peripheral.
    ///
    /// Implements the common GPIO control logic shared by all digital output actuators:
    /// active-level abstraction, state tracking, and safe shutdown on destruction.
    /// GPIO hardware configuration is performed in the constructor, guaranteeing the
    /// pin is always ready to use regardless of whether begin() is overridden by a subclass.
    ///
    /// The constructor is protected to prevent direct instantiation — only derived
    /// classes may be constructed.
    class DigitalOutput
    {
    public:
        /// @brief Destructor — drives the output to the off state before releasing the object.
        virtual ~DigitalOutput();

        /// @brief Drive the output to its initial (off) state.
        ///
        /// Call once after construction to establish a known starting state.
        /// GPIO hardware configuration is already handled by the constructor,
        /// so subclasses may override this method for additional setup without
        /// affecting the underlying pin configuration.
        virtual void begin();

        /// @brief Drive the output to its active (on) state.
        void on();

        /// @brief Drive the output to its inactive (off) state.
        void off();

        /// @brief Toggle the current output state.
        void toggle();

        /// @brief Return true if the output is currently in the active (on) state.
        [[nodiscard]] bool isOn() const;

    protected:
        /// @brief Construct a new DigitalOutput and configure the underlying GPIO.
        ///
        /// Configures the pin as a push-pull output immediately, so the hardware is
        /// ready to use as soon as the object exists.
        /// @param pin        GPIO number to control.
        /// @param activeHigh true if a HIGH level activates the output (active-high);
        ///                   false if a LOW level activates it (active-low).
        DigitalOutput(gpio_num_t pin, bool activeHigh);

    private:
        gpio_num_t pin_;
        bool activeHigh_;
        bool state_ = false;
    };

} // namespace peripherals
