.DEFAULT_GOAL := help

.PHONY: help run ports

help: ## Show available commands
	@grep -E '^[a-zA-Z_-]+:.*##' $(MAKEFILE_LIST) \
		| awk 'BEGIN {FS = ":.*##"}; {printf "  \033[36m%-10s\033[0m %s\n", $$1, $$2}'

sync: ## Install and sync project dependencies
	uv sync

run: ## Run the provisioner and flash the device_id to the ESP32
	uv run python src/main.py

ports: ## List available serial ports to identify the connected device
	uv run python -m serial.tools.list_ports --verbose
