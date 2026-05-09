#include "digital_output.hpp"

namespace peripherals
{

    DigitalOutput::DigitalOutput(gpio_num_t pin, bool activeHigh) : pin_(pin), activeHigh_(activeHigh)
    {
    }

    DigitalOutput::~DigitalOutput()
    {
        off();
    }

    void DigitalOutput::begin()
    {
        gpio_config_t cfg = {
            .pin_bit_mask = 1ULL << pin_,
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_DISABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
        };
        gpio_config(&cfg);
        off();
    }

    void DigitalOutput::on()
    {
        gpio_set_level(pin_, activeHigh_ ? 1 : 0);
        state_ = true;
    }

    void DigitalOutput::off()
    {
        gpio_set_level(pin_, activeHigh_ ? 0 : 1);
        state_ = false;
    }

    void DigitalOutput::toggle()
    {
        state_ ? off() : on();
    }

    bool DigitalOutput::isOn() const
    {
        return state_;
    }

} // namespace peripherals
