#include "led.hpp"

namespace peripherals
{

    Led::Led(gpio_num_t pin, bool activeHigh) : DigitalOutput(pin, activeHigh)
    {
    }

} // namespace peripherals
