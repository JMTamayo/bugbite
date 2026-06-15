# Provisioner

CLI tool that writes the device configuration into the ESP32 flash memory using an NVS partition. Part of the [bugbite](../README.md) project.

## How it works

```
.env → Config → CSV → NVS binary → flash → verify
```

1. **Config** — loads and validates all settings from the `.env` file.
2. **CSV** — generates the NVS key-value file consumed by `esp_idf_nvs_partition_gen`.
3. **NVS binary** — compiles the CSV into a binary partition ready to be flashed.
4. **Flash** — writes the binary to the NVS partition address using `esptool`.
5. **Verify** — reads back the partition from the device and confirms the `device_id` is present.

## Structure

```
provisioner/
├── src/
│   ├── config/
│   │   ├── __init__.py       # Re-exports Config and get_logger
│   │   ├── config.py         # Pydantic-settings Config class
│   │   └── logger.py         # Rich-based colored logger
│   ├── main.py               # Provisioning pipeline
│   ├── .env                  # Local environment variables (not committed)
│   └── .env.example          # Template for .env
├── Makefile
└── pyproject.toml
```

## Setup

```bash
cp src/.env.example src/.env
# Edit src/.env with your device values
make sync
```

## Usage

```bash
make run    # Flash device_id to the ESP32 and verify
make ports  # List available serial ports to identify the device
make help   # Show all available commands
```

## Environment variables

| Variable | Required | Default | Description |
|---|---|---|---|
| `DEVICE_PORT` | Yes | — | Serial port of the ESP32 (e.g. `/dev/cu.usbmodem101`) |
| `CHIP` | Yes | — | ESP32 chip variant (e.g. `esp32-c3`, `esp32`, `esp32-s3`) |
| `NVS_FLASH_ADDRESS` | Yes | — | Flash offset of the NVS partition in hex (e.g. `0x9000`). Must match the firmware partition table. |
| `NVS_PARTITION_SIZE` | Yes | — | NVS partition size in bytes (e.g. `24576` for 0x6000). Must match the firmware partition table. |
| `NVS_NAMESPACE` | No | `config` | NVS namespace. Must match `nvs_open()` calls in the firmware. |
| `DEVICE_ID` | Yes | — | UUID that uniquely identifies the device. |

## NVS contract

The provisioner writes the following keys under the `config` namespace (or the value of `NVS_NAMESPACE`). The firmware must read them using the same namespace and key names.

| Namespace | Key | Type | Description |
|---|---|---|---|
| `config` | `device_id` | `string` | Unique device identifier (UUID) |

## Example output

```
INFO     Starting provisioner
DEBUG    Device ID   : 00000000-0000-0000-0000-000000000000
DEBUG    Port        : /dev/cu.usbmodem101
DEBUG    NVS address : 0x9000
DEBUG    NVS size    : 24576 bytes
INFO     CSV written to out/nvs_config.csv
INFO     Binary written to out/nvs_config.bin (24576 bytes)
INFO     Provisioning complete — device_id: 00000000-0000-0000-0000-000000000000
INFO     Verification passed — device_id 00000000-0000-0000-0000-000000000000 found in flash
```
