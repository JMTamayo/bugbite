from pathlib import Path
from uuid import UUID

from pydantic import computed_field
from pydantic_settings import BaseSettings, SettingsConfigDict

_ENV_FILE = Path(__file__).parent.parent / ".env"


class Config(BaseSettings):
    """Provisioner settings loaded from the `.env` file.

    Attributes:
        DEVICE_PORT: Serial port where the ESP32 is connected (e.g. ``/dev/cu.usbmodem101``).
        CHIP: ESP32 chip variant (e.g. ``esp32``, ``esp32-c3``, ``esp32-s3``).
            Must match the physical chip on the board.
        NVS_FLASH_ADDRESS: Flash address where the NVS partition starts, in hex (e.g. ``0x9000``).
            Must match the value defined in the partition table of the firmware.
        NVS_PARTITION_SIZE: Size of the NVS partition in bytes (e.g. ``24576`` for 0x6000).
            Must match the value defined in the partition table of the firmware.
        NVS_NAMESPACE: NVS namespace used to group the device configuration keys.
            Must match the namespace used in ``nvs_open()`` calls in the firmware. Defaults to ``config``.
        DEVICE_ID: Unique identifier for the device, stored as a UUID.
            Written to NVS under the ``NVS_NAMESPACE`` namespace with key ``device_id``.
        OUT_DIR: Directory where generated files are written. Defaults to ``out/``.
        CSV_FILE: Path to the generated NVS CSV file.
        BIN_FILE: Path to the compiled NVS binary.
    """

    model_config = SettingsConfigDict(env_file=_ENV_FILE, env_file_encoding="utf-8")

    DEVICE_PORT: str
    CHIP: str

    NVS_FLASH_ADDRESS: str
    NVS_PARTITION_SIZE: int
    NVS_NAMESPACE: str = "config"

    DEVICE_ID: UUID

    OUT_DIR: Path = Path("out")

    @computed_field
    @property
    def CSV_FILE(self) -> Path:
        return self.OUT_DIR / "nvs_config.csv"

    @computed_field
    @property
    def BIN_FILE(self) -> Path:
        return self.OUT_DIR / "nvs_config.bin"
