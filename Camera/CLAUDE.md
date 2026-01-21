# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

This project uses PlatformIO with the ESP-WROVER-KIT board and Arduino framework.

```bash
# Build the project
pio run

# Upload to device
pio run --target upload

# Monitor serial output (115200 baud)
pio device monitor

# Build and upload in one step
pio run --target upload && pio device monitor

# Clean build artifacts
pio run --target clean
```

## Project Architecture

**Current State:** WiFi connectivity demonstrator using WPA2 Enterprise (EdUroam) authentication. The "Camera" name reflects intended future camera integration.

**Source Structure:**
- `src/main.cpp` - Main application with WiFi connection logic
- `src/secrets.h` - Credentials file (gitignored) - must be created locally with EAP_IDENTITY, EAP_PASSWORD, and SSID defines

**Hardware Target:** ESP-WROVER-KIT with I2C LCD display (GPIO21=SDA, GPIO22=SCL)

**Simulation:** Wokwi simulator configured via `diagram.json` and `wokwi.toml`

## Secrets Management

Create `src/secrets.h` with:
```cpp
#define EAP_IDENTITY "your_identity"
#define EAP_PASSWORD "your_password"
#define SSID "network_name"
```

This file is gitignored to prevent credential exposure.

## Parent Project Context

This is part of `/home/rocky/coding_projects/esp32_learning/` which contains multiple ESP32 learning projects (Display, JoyStick, StepperMotor, Wifi_Scan, edurom).
