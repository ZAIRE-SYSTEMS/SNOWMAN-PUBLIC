
---

# `docs/firmware_tasks.md`

```md
# Firmware Task Layout

## Overview

Snowman uses ESP-IDF and FreeRTOS to organize runtime behavior into separate tasks. This allows the firmware to manage user input, wireless communication, audio control, feedback, configuration, and power behavior without blocking the entire system.

This document describes the public task architecture at a high level. Exact production implementation details have been removed.

## Task Design Goals

The task architecture is designed to:

- Keep user input responsive.
- Avoid blocking audio or wireless workflows.
- Separate system responsibilities clearly.
- Allow future hardware revisions to reuse the same logic.
- Support real-time behavior where timing matters.
- Keep debugging simple through module-level isolation.

## Public Task Overview

| Task | Purpose | Priority Concept |
|---|---|---|
| System Init | Initializes hardware, storage, and system modules | Startup only |
| Button/Input Task | Reads physical controls and generates input events | High responsiveness |
| Walkie Task | Handles peer-to-peer communication state | Real-time communication |
| Audio Control Task | Coordinates Bluetooth/audio control behavior | Medium/high |
| Settings Portal Task | Hosts local configuration portal | On-demand |
| Indicator Task | Drives buzzer, LED, or feedback behavior | Low/medium |
| Power Task | Tracks battery/power state and low-power behavior | Medium |
| Diagnostics Task | Optional logging and debug monitoring | Low |

## Task Interaction Model

Tasks communicate through:

- Shared device state
- FreeRTOS queues
- Event groups
- Mutex-protected configuration data
- Module-level function calls
- State flags for simple runtime behavior

Example structure:

```txt
+------------------+
| Button Task      |
+--------+---------+
         |
         v
+------------------+        +------------------+
| Input Event      | -----> | Device State     |
| Handler          |        | Manager          |
+--------+---------+        +--------+---------+
         |                           |
         v                           v
+------------------+        +------------------+
| Walkie Task      |        | Audio Task       |
+------------------+        +------------------+
