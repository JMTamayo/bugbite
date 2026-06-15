#pragma once

#include <driver/gpio.h>

namespace config {

namespace device {

constexpr const char *NVS_NAMESPACE = "config";
constexpr const char *DEVICE_ID_KEY = "device_id";

}  // namespace device

namespace relay {

constexpr gpio_num_t GPIO = GPIO_NUM_2;
constexpr bool INVERTED = true;

}  // namespace relay

namespace builtin_led {

constexpr gpio_num_t GPIO = GPIO_NUM_8;
constexpr bool INVERTED = true;

}  // namespace builtin_led

}  // namespace config
