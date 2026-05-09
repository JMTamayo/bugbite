#pragma once

#include "driver/gpio.h"

namespace peripherals
{

    /// @brief Abstract base class for a single-channel GPIO digital output peripheral.
    ///
    /// Implements the common GPIO control logic shared by all digital output actuators:
    /// hardware initialization, active-level abstraction, and state tracking.
    /// Concrete subclasses inherit this behaviour. The destructor drives the output
    /// to the off state before the object is released, ensuring all actuators are
    /// left inactive regardless of the subclass.
    ///
    /// The constructor is protected to prevent direct instantiation — only derived
    /// classes may be constructed.
    class DigitalOutput
    {
    public:
        /// @brief Destructor — drives the output to the off state before releasing the object.
        virtual ~DigitalOutput();

        /// @brief Configure the GPIO as a push-pull output and drive it to the initial (off) state.
        ///
        /// Must be called once after construction and before any other method.
        /// Separating initialization from construction allows objects to be declared
        /// at any scope and have their hardware setup deferred until needed.
        void begin();

        /// @brief Drive the output to its active (on) state.
        void on();

        /// @brief Drive the output to its inactive (off) state.
        void off();

        /// @brief Toggle the current output state.
        void toggle();

        /// @brief Return true if the output is currently in the active (on) state.
        [[nodiscard]] bool isOn() const;

    protected:
        /// @brief Construct a new DigitalOutput.
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
