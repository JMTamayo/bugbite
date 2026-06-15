import csv
import subprocess

from config import Config, get_logger

log = get_logger(__name__)


def generate_csv(config: Config) -> None:
    config.OUT_DIR.mkdir(exist_ok=True)

    with open(config.CSV_FILE, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(["key", "type", "encoding", "value"])
        writer.writerow([config.NVS_NAMESPACE, "namespace", "", ""])
        writer.writerow(["device_id", "data", "string", str(config.DEVICE_ID)])

    log.info("CSV written to [bold]%s[/bold]", config.CSV_FILE)
    log.debug("\n%s", config.CSV_FILE.read_text())


def compile_binary(config: Config) -> None:
    result = subprocess.run(
        [
            "python",
            "-m",
            "esp_idf_nvs_partition_gen",
            "generate",
            str(config.CSV_FILE),
            str(config.BIN_FILE),
            str(config.NVS_PARTITION_SIZE),
        ],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        log.error("nvs_partition_gen failed:\n%s", result.stderr)
        raise RuntimeError(result.stderr)

    size = config.BIN_FILE.stat().st_size
    log.info("Binary written to [bold]%s[/bold] (%d bytes)", config.BIN_FILE, size)


def flash(config: Config) -> None:
    result = subprocess.run(
        [
            "python",
            "-m",
            "esptool",
            "--chip",
            config.CHIP,
            "--port",
            config.DEVICE_PORT,
            "write_flash",
            config.NVS_FLASH_ADDRESS,
            str(config.BIN_FILE),
        ],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        log.error("esptool failed:\n%s", result.stderr)
        raise RuntimeError(result.stderr)

    log.info("Provisioning complete — device_id: [bold]%s[/bold]", config.DEVICE_ID)


def verify(config: Config) -> None:
    read_back = config.OUT_DIR / "nvs_readback.bin"

    result = subprocess.run(
        [
            "python",
            "-m",
            "esptool",
            "--chip",
            config.CHIP,
            "--port",
            config.DEVICE_PORT,
            "read_flash",
            config.NVS_FLASH_ADDRESS,
            str(config.NVS_PARTITION_SIZE),
            str(read_back),
        ],
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        log.error("read_flash failed:\n%s", result.stderr)
        raise RuntimeError(result.stderr)

    content = read_back.read_bytes()
    device_id_bytes = str(config.DEVICE_ID).encode()

    if device_id_bytes in content:
        log.info(
            "Verification [bold green]passed[/bold green] — device_id [bold]%s[/bold] found in flash",
            config.DEVICE_ID,
        )
    else:
        log.error(
            "Verification [bold red]failed[/bold red] — device_id [bold]%s[/bold] not found in flash",
            config.DEVICE_ID,
        )
        raise RuntimeError("device_id not found in flash")


if __name__ == "__main__":
    config = Config()

    log.info("Starting provisioner")
    log.debug("Device ID   : %s", config.DEVICE_ID)
    log.debug("Port        : %s", config.DEVICE_PORT)
    log.debug("NVS address : %s", config.NVS_FLASH_ADDRESS)
    log.debug("NVS size    : %d bytes", config.NVS_PARTITION_SIZE)

    generate_csv(config)
    compile_binary(config)
    flash(config)
    verify(config)
