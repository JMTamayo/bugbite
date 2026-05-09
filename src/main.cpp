#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "config.hpp"
#include "secrets.hpp"
#include "peripherals/led.hpp"

static const char *TAG = "main";

extern "C" void app_main(void)
{
    peripherals::Led builtinLed(config::pins::LED, config::pins::LED_ACTIVE_HIGH);
    builtinLed.begin();

    ESP_LOGI(TAG, "project=%s, device_id=%s", config::PROJECT_NAME, secrets::DEVICE_ID);

    while (true)
    {
        builtinLed.toggle();
        ESP_LOGI(TAG, "led_state=%s", builtinLed.isOn() ? "ON" : "OFF");
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
