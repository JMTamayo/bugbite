# bugbite

Bite down on the circuit. A hackable, edge-ready smart relay to take absolute command over your home appliances.

## Repository layout

This is a monorepo with two self-contained subprojects:

| Path | Stack | Description |
|---|---|---|
| [`firmware/`](firmware/) | C++ / ESP-IDF (PlatformIO) | Device firmware for the ESP32-C3-SuperMini. |
| [`provisioner/`](provisioner/) | Python | Host CLI tool that writes the device configuration into the ESP32 NVS partition. |

Each subproject has its own README, build tooling, and `.gitignore`.
