#include "connectivity/wifi/infrastructure/esp_wifi_station.hpp"

#include <esp_event.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_timer.h>
#include <esp_wifi.h>

#include <cstring>

#include "connectivity/wifi/tag.hpp"

namespace connectivity::wifi {

namespace {

// Single station per device, so the esp_wifi/event state lives at file scope.
StatusHandler g_onStatus;
bool g_connected = false;
bool g_initialized = false;
bool g_autoReconnect = false;
std::uint32_t g_reconnectDelayMs = 5000;
std::int8_t g_maxTxPower = 80;
esp_timer_handle_t g_reconnectTimer = nullptr;

void notify(Status status) {
  if (g_onStatus) {
    g_onStatus(status);
  }
}

void reconnectTimerCb(void *) {
  ESP_LOGI(TAG, "retrying connection");
  notify(Status::Connecting);
  esp_wifi_connect();
}

void scheduleReconnect() {
  if (g_reconnectTimer == nullptr) {
    return;
  }

  esp_timer_stop(g_reconnectTimer);
  esp_timer_start_once(g_reconnectTimer,
                       static_cast<std::uint64_t>(g_reconnectDelayMs) * 1000);
}

void wifiEventHandler(void *, esp_event_base_t base, std::int32_t id, void *) {
  if (base != WIFI_EVENT) {
    return;
  }

  if (id == WIFI_EVENT_STA_DISCONNECTED) {
    g_connected = false;
    notify(Status::Disconnected);
    if (g_autoReconnect) {
      ESP_LOGW(TAG, "disconnected; retrying in %u ms",
               static_cast<unsigned>(g_reconnectDelayMs));
      scheduleReconnect();
    }
  }
}

void ipEventHandler(void *, esp_event_base_t base, std::int32_t id,
                    void *data) {
  if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
    auto *event = static_cast<ip_event_got_ip_t *>(data);
    ESP_LOGI(TAG, "connected; got IP " IPSTR, IP2STR(&event->ip_info.ip));
    g_connected = true;
    notify(Status::Connected);
  }
}

}  // namespace

EspWifiStation::EspWifiStation(std::uint32_t reconnectDelayMs,
                               std::int8_t maxTxPower) {
  g_reconnectDelayMs = reconnectDelayMs;
  g_maxTxPower = maxTxPower;
}

esp_err_t EspWifiStation::begin(StatusHandler onStatus) {
  if (g_initialized) {
    return ESP_OK;
  }

  g_onStatus = std::move(onStatus);

  esp_err_t err = esp_netif_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_netif_init failed: %s", esp_err_to_name(err));

    return err;
  }

  err = esp_event_loop_create_default();
  if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
    ESP_LOGE(TAG, "esp_event_loop_create_default failed: %s",
             esp_err_to_name(err));

    return err;
  }

  esp_netif_create_default_wifi_sta();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  err = esp_wifi_init(&cfg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_wifi_init failed: %s", esp_err_to_name(err));

    return err;
  }

  err = esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifiEventHandler, nullptr, nullptr);
  if (err != ESP_OK) {
    return err;
  }

  err = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                            &ipEventHandler, nullptr, nullptr);
  if (err != ESP_OK) {
    return err;
  }

  esp_timer_create_args_t timerArgs = {};
  timerArgs.callback = &reconnectTimerCb;
  timerArgs.name = "wifi_reconnect";
  err = esp_timer_create(&timerArgs, &g_reconnectTimer);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_timer_create failed: %s", esp_err_to_name(err));

    return err;
  }

  err = esp_wifi_set_mode(WIFI_MODE_STA);
  if (err != ESP_OK) {
    return err;
  }

  err = esp_wifi_start();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_wifi_start failed: %s", esp_err_to_name(err));

    return err;
  }

  // Cap TX power after start (antenna workaround for ESP32-C3 SuperMini).
  err = esp_wifi_set_max_tx_power(g_maxTxPower);
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "esp_wifi_set_max_tx_power failed: %s", esp_err_to_name(err));
  } else {
    ESP_LOGI(TAG, "max tx power set to %d (0.25 dBm units)", g_maxTxPower);
  }

  g_initialized = true;
  ESP_LOGI(TAG, "wifi station started");

  return ESP_OK;
}

esp_err_t EspWifiStation::connect(const std::string &ssid,
                                  const std::string &password) {
  wifi_config_t cfg = {};
  std::strncpy(reinterpret_cast<char *>(cfg.sta.ssid), ssid.c_str(),
               sizeof(cfg.sta.ssid) - 1);
  std::strncpy(reinterpret_cast<char *>(cfg.sta.password), password.c_str(),
               sizeof(cfg.sta.password) - 1);
  cfg.sta.threshold.authmode =
      password.empty() ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;

  esp_err_t err = esp_wifi_set_config(WIFI_IF_STA, &cfg);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "esp_wifi_set_config failed: %s", esp_err_to_name(err));

    return err;
  }

  g_autoReconnect = true;
  ESP_LOGI(TAG, "connecting to '%s'", ssid.c_str());
  notify(Status::Connecting);

  return esp_wifi_connect();
}

esp_err_t EspWifiStation::disconnect() {
  g_autoReconnect = false;
  if (g_reconnectTimer != nullptr) {
    esp_timer_stop(g_reconnectTimer);
  }
  g_connected = false;

  return esp_wifi_disconnect();
}

bool EspWifiStation::isConnected() const { return g_connected; }

}  // namespace connectivity::wifi
