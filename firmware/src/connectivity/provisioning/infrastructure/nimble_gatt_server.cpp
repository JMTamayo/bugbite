#include "connectivity/provisioning/infrastructure/nimble_gatt_server.hpp"

#include <esp_log.h>

#include <cstring>
#include <vector>

#include "connectivity/provisioning/tag.hpp"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

namespace connectivity::provisioning {

namespace {

// There is a single provisioning server per device, so the NimBLE host state
// lives here at file scope. The GATT table is built once and must outlive the
// host: the C structures below are kept alive for the program's lifetime.
ServiceSpec g_service;
std::vector<ble_uuid128_t> g_uuids;       // [0]=service, [1..]=chars
std::vector<ble_gatt_chr_def> g_chrDefs;  // n + 1 (zero terminator)
std::vector<std::vector<ble_gatt_dsc_def>> g_dscDefs;  // 2 each (desc + term)
ble_gatt_svc_def g_svcDefs[2];
const ble_uuid16_t g_userDescUuid = BLE_UUID16_INIT(0x2901);

std::string g_deviceName;
bool g_hostInitialized = false;
bool g_advertising = false;
std::uint8_t g_ownAddrType = 0;
std::uint16_t g_connHandle = BLE_HS_CONN_HANDLE_NONE;

ble_uuid128_t makeUuid128(const std::array<std::uint8_t, 16> &bytes) {
  ble_uuid128_t uuid = {};
  uuid.u.type = BLE_UUID_TYPE_128;
  std::memcpy(uuid.value, bytes.data(), bytes.size());

  return uuid;
}

// Dispatches a characteristic read/write to its spec handlers (arg points to
// the owning CharacteristicSpec).
int accessCb(std::uint16_t, std::uint16_t, struct ble_gatt_access_ctxt *ctxt,
             void *arg) {
  auto *spec = static_cast<CharacteristicSpec *>(arg);

  switch (ctxt->op) {
    case BLE_GATT_ACCESS_OP_READ_CHR: {
      std::string value;
      if (spec->onRead) {
        esp_err_t err = spec->onRead(value);
        if (err != ESP_OK) {
          ESP_LOGE(TAG, "read '%s' failed: %s", spec->name.c_str(),
                   esp_err_to_name(err));

          return BLE_ATT_ERR_UNLIKELY;
        }
      }
      int rc = os_mbuf_append(ctxt->om, value.data(), value.size());

      return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    case BLE_GATT_ACCESS_OP_WRITE_CHR: {
      std::uint16_t len = OS_MBUF_PKTLEN(ctxt->om);
      std::string value(len, '\0');
      std::uint16_t outLen = 0;
      int rc = ble_hs_mbuf_to_flat(ctxt->om, value.data(), len, &outLen);
      if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
      }
      value.resize(outLen);
      if (spec->onWrite) {
        esp_err_t err = spec->onWrite(value);
        if (err != ESP_OK) {
          ESP_LOGE(TAG, "write '%s' failed: %s", spec->name.c_str(),
                   esp_err_to_name(err));

          return BLE_ATT_ERR_UNLIKELY;
        }
      }
      ESP_LOGI(TAG, "characteristic '%s' updated (%u bytes)",
               spec->name.c_str(), static_cast<unsigned>(outLen));

      return 0;
    }
    default:
      return BLE_ATT_ERR_UNLIKELY;
  }
}

// Serves the 0x2901 user description (the characteristic name in arg).
int dscCb(std::uint16_t, std::uint16_t, struct ble_gatt_access_ctxt *ctxt,
          void *arg) {
  if (ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC) {
    const char *name = static_cast<const char *>(arg);
    int rc = os_mbuf_append(ctxt->om, name, std::strlen(name));

    return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
  }

  return BLE_ATT_ERR_UNLIKELY;
}

// Builds the NimBLE service/characteristic/descriptor tables from g_service.
// Pointers handed to NimBLE reference the file-scope containers, so they must
// not be reallocated afterwards.
void buildGattTable() {
  const std::size_t n = g_service.characteristics.size();

  g_uuids.clear();
  g_uuids.reserve(n + 1);
  g_uuids.push_back(makeUuid128(g_service.uuid));
  for (std::size_t i = 0; i < n; ++i) {
    g_uuids.push_back(makeUuid128(g_service.characteristics[i].uuid));
  }

  g_dscDefs.assign(n, std::vector<ble_gatt_dsc_def>(2, ble_gatt_dsc_def{}));
  g_chrDefs.assign(n + 1, ble_gatt_chr_def{});
  for (std::size_t i = 0; i < n; ++i) {
    CharacteristicSpec &c = g_service.characteristics[i];

    g_dscDefs[i][0].uuid = &g_userDescUuid.u;
    g_dscDefs[i][0].att_flags = BLE_ATT_F_READ;
    g_dscDefs[i][0].access_cb = dscCb;
    g_dscDefs[i][0].arg = const_cast<char *>(c.name.c_str());

    ble_gatt_chr_flags flags = 0;
    if (c.readable) {
      flags |= BLE_GATT_CHR_F_READ;
    }
    if (c.writable) {
      flags |= BLE_GATT_CHR_F_WRITE;
    }

    g_chrDefs[i].uuid = &g_uuids[i + 1].u;
    g_chrDefs[i].access_cb = accessCb;
    g_chrDefs[i].arg = &c;
    g_chrDefs[i].flags = flags;
    g_chrDefs[i].descriptors = g_dscDefs[i].data();
  }

  g_svcDefs[0] = ble_gatt_svc_def{};
  g_svcDefs[0].type = BLE_GATT_SVC_TYPE_PRIMARY;
  g_svcDefs[0].uuid = &g_uuids[0].u;
  g_svcDefs[0].characteristics = g_chrDefs.data();
  g_svcDefs[1] = ble_gatt_svc_def{};
}

int gapEventCb(struct ble_gap_event *event, void *arg);

void startAdvertising() {
  struct ble_hs_adv_fields fields = {};
  fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

  const char *name = ble_svc_gap_device_name();
  fields.name = reinterpret_cast<std::uint8_t *>(const_cast<char *>(name));
  fields.name_len = name ? std::strlen(name) : 0;
  fields.name_is_complete = 1;

  int rc = ble_gap_adv_set_fields(&fields);
  if (rc != 0) {
    ESP_LOGE(TAG, "ble_gap_adv_set_fields failed: rc=%d", rc);

    return;
  }

  struct ble_gap_adv_params adv_params = {};
  adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
  adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

  rc = ble_gap_adv_start(g_ownAddrType, nullptr, BLE_HS_FOREVER, &adv_params,
                         gapEventCb, nullptr);
  if (rc != 0) {
    ESP_LOGE(TAG, "ble_gap_adv_start failed: rc=%d", rc);

    return;
  }

  ESP_LOGI(TAG, "advertising as '%s'", name ? name : "");
}

int gapEventCb(struct ble_gap_event *event, void *) {
  switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
      if (event->connect.status == 0) {
        g_connHandle = event->connect.conn_handle;
        ESP_LOGI(TAG, "client connected");
      } else if (g_advertising) {
        startAdvertising();
      }

      return 0;
    case BLE_GAP_EVENT_DISCONNECT:
      g_connHandle = BLE_HS_CONN_HANDLE_NONE;
      ESP_LOGI(TAG, "client disconnected");
      if (g_advertising) {
        startAdvertising();
      }

      return 0;
    case BLE_GAP_EVENT_ADV_COMPLETE:
      if (g_advertising) {
        startAdvertising();
      }

      return 0;
    default:
      return 0;
  }
}

void onSync() {
  int rc = ble_hs_id_infer_auto(0, &g_ownAddrType);
  if (rc != 0) {
    ESP_LOGE(TAG, "ble_hs_id_infer_auto failed: rc=%d", rc);

    return;
  }

  if (g_advertising) {
    startAdvertising();
  }
}

void onReset(int reason) {
  ESP_LOGW(TAG, "nimble host reset; reason=%d", reason);
}

void hostTask(void *) {
  nimble_port_run();
  nimble_port_freertos_deinit();
}

}  // namespace

esp_err_t NimbleGattServer::start(const std::string &deviceName,
                                  const ServiceSpec &service) {
  g_deviceName = deviceName;
  g_advertising = true;

  if (g_hostInitialized) {
    ble_svc_gap_device_name_set(g_deviceName.c_str());
    startAdvertising();

    return ESP_OK;
  }

  g_service = service;

  esp_err_t err = nimble_port_init();
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "nimble_port_init failed: %s", esp_err_to_name(err));

    return err;
  }

  ble_hs_cfg.sync_cb = onSync;
  ble_hs_cfg.reset_cb = onReset;

  ble_svc_gap_init();
  ble_svc_gatt_init();

  buildGattTable();

  int rc = ble_gatts_count_cfg(g_svcDefs);
  if (rc != 0) {
    ESP_LOGE(TAG, "ble_gatts_count_cfg failed: rc=%d", rc);

    return ESP_FAIL;
  }

  rc = ble_gatts_add_svcs(g_svcDefs);
  if (rc != 0) {
    ESP_LOGE(TAG, "ble_gatts_add_svcs failed: rc=%d", rc);

    return ESP_FAIL;
  }

  rc = ble_svc_gap_device_name_set(g_deviceName.c_str());
  if (rc != 0) {
    ESP_LOGW(TAG, "ble_svc_gap_device_name_set failed: rc=%d", rc);
  }

  nimble_port_freertos_init(hostTask);
  g_hostInitialized = true;
  ESP_LOGI(TAG, "nimble host started");

  return ESP_OK;
}

esp_err_t NimbleGattServer::stop() {
  g_advertising = false;

  if (!g_hostInitialized) {
    return ESP_OK;
  }

  int rc = ble_gap_adv_stop();
  if (rc != 0 && rc != BLE_HS_EALREADY) {
    ESP_LOGW(TAG, "ble_gap_adv_stop failed: rc=%d", rc);
  }

  if (g_connHandle != BLE_HS_CONN_HANDLE_NONE) {
    ble_gap_terminate(g_connHandle, BLE_ERR_REM_USER_CONN_TERM);
  }

  ESP_LOGI(TAG, "provisioning stopped");

  return ESP_OK;
}

}  // namespace connectivity::provisioning
