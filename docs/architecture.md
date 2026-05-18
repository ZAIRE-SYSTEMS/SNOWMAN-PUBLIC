# Snowman Firmware Architecture

## Overview

Snowman is a wearable embedded communication platform designed for ski and snowboarding environments. The system combines short-range peer-to-peer communication, Bluetooth audio support, local device configuration, user controls, battery-powered operation, and embedded system state management.

This repository contains a sanitized public preview of the firmware architecture. Full production source code, security logic, hardware files, protocol details, and product-specific implementation have been removed.

## System Goals

The firmware architecture is designed around the following goals:

- Support wearable communication without requiring a mobile app for setup.
- Provide peer-to-peer walkie-talkie style communication between nearby devices.
- Integrate Bluetooth audio for user-facing audio functionality.
- Maintain responsive user controls suitable for glove-friendly operation.
- Store persistent configuration locally using non-volatile storage.
- Support battery-powered operation with power-state awareness.
- Keep the firmware modular enough to evolve across PCB and hardware revisions.

## High-Level Architecture

The system is organized into independent firmware modules that communicate through shared state, event flags, queues, and task-level coordination.

```txt
+--------------------------------------------------+
|                  Application Layer               |
|--------------------------------------------------|
| Button Logic | Audio Control | Walkie Logic       |
| Settings UI  | Device State  | Power Management   |
+--------------------------------------------------+
|                  Middleware Layer                |
|--------------------------------------------------|
| NVS Storage  | Captive Portal | ESP-NOW Interface  |
| Bluetooth Module Interface | Sensor Interface     |
+--------------------------------------------------+
|                  Hardware Layer                  |
|--------------------------------------------------|
| GPIO | UART | I2C | I2S | SPI | Battery/Power     |
+--------------------------------------------------+
|                  ESP-IDF / FreeRTOS              |
+--------------------------------------------------+
```


# System Manager

All systems are manage via flags and RTOS. Each button has its own xTaskCreate() following a flag logic.

Zaire init via:
* Initialize NVS and local file storage.
* Load user and system configuration.
* Initialize hardware interfaces.
* Start FreeRTOS tasks.
* Maintain system-level state.


