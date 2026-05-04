#include "esp_log.h"

#include "config.hpp"
#include "secrets.hpp"

static const char *TAG = "main";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "project=%s, device_id=%s",
             config::PROJECT_NAME,
             secrets::DEVICE_ID);
}
