#include "relay.hpp"

namespace peripherals
{

    Relay::Relay(gpio_num_t pin, bool activeHigh) : DigitalOutput(pin, activeHigh)
    {
    }

} // namespace peripherals
