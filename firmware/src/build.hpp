#pragma once

#include <esp_err.h>

#include "peripherals/flash_memory/flash_memory.hpp"
#include "peripherals/led/led.hpp"
#include "peripherals/relay/relay.hpp"

/**
 * @brief Initializes every peripheral instance of the application.
 *
 * Runs the begin() sequence in dependency order, binding each instance to its
 * configuration constants. The sequence aborts on the first failure.
 *
 * @param flashMemory Flash memory (NVS) facade. Initialized first, since other
 *                    modules rely on persistent storage.
 * @param loadRelay   Load relay facade.
 * @param builtinLed  Built-in LED facade.
 * @return            ESP_OK when every peripheral initialized, or the error of
 *                    the first failure.
 */
esp_err_t beginPeripherals(flash_memory::FlashMemory &flashMemory,
                           relay::Relay &loadRelay, led::Led &builtinLed);
