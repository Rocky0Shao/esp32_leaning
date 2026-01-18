# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP32 PlatformIO project for controlling a 28BYJ-48 stepper motor via joystick input. Uses an ESP-WROVER-KIT board with Arduino framework.

## Build Commands

```bash
# Build the project
pio run

# Upload firmware to ESP32 (requires device connected on /dev/ttyUSB0)
pio run --target upload

# Monitor serial output (115200 baud)
pio device monitor

# Build and upload in one command
pio run --target upload && pio device monitor
```

## Hardware Configuration

- **Board**: ESP-WROVER-KIT (esp32)
- **Joystick**: Analog on GPIO 34 (X), 35 (Y), button on GPIO 32
- **Stepper (ULN2003)**: GPIO 26, 27, 14, 0 for IN1-IN4
- **I2C LCD**: SDA=21, SCL=22 (configured in diagram.json for Wokwi simulation)

Note: ADC1 pins (32-39) are used for joystick because ADC2 conflicts with WiFi.

## Wokwi Simulation

The project includes Wokwi configuration for simulation. The `diagram.json` defines the virtual circuit and `wokwi.toml` points to the firmware location.
