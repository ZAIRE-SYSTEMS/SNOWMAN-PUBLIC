# Snowman Power Architecture

## Overview

Snowman is a battery-powered wearable embedded communication platform. The power system is designed around a single-cell lithium battery, USB-C charging, regulated logic power, pushbutton-controlled system power, battery fuel-gauge monitoring, and firmware-visible low-battery behavior.

This document is a public, portfolio-safe overview of the Snowman power architecture. It intentionally avoids exposing full schematics, complete PCB layout details, production validation data, or manufacturing files.

## Power Design Goals

The power system is designed to support:

- Battery-powered wearable operation.
- Simple user-controlled power on/off behavior through a physical button.
- Stable logic voltage for the ESP32-S3 and supporting peripherals.
- USB-C charging input for a single-cell lithium battery.
- Firmware-readable battery state-of-charge information.
- Low-battery alerts through the device feedback system.
- A clean transition between user input, firmware state, and hardware power control.
- A practical architecture that can be tested and revised across prototype PCB versions.

## High-Level Power Block Diagram

```txt
USB-C Input
    |
    |-- CC1/CC2 Rd resistors to GND
    |
    v
TP4056-MS Battery Charger
    |
    v
3.7 V Lithium Battery
    |
    +--------------------+
    |                    |
    v                    v
MAX17048 Fuel Gauge      LTC2954 Pushbutton Power Controller
    |                    |
    | I2C                | Enable / Kill / Interrupt style control
    v                    v
ESP32-S3 Main MCU ---> TPS62849 Buck Regulator ---> 3.3 V-class System Rail
```

## Main Power Components

| Component | Role |
|---|---|
| LTC2954ITS8 | Pushbutton on/off controller for user-controlled system power |
| MAX17048G+T10 | I2C lithium battery fuel gauge for state-of-charge monitoring |
| TPS62849DLCR | High-efficiency buck regulator for the main regulated logic rail |
| TP4056-MS | Single-cell lithium battery charger |
| USB-C Connector | 5 V charging input |
| 5.1 kΩ CC Resistors | USB-C sink-device pull-down resistors on CC1 and CC2 |
| 3.7 V 3200 mAh Battery | Main rechargeable energy source |

## Battery

The current design uses a single-cell lithium battery:

```txt
Nominal Voltage: 3.7 V
Capacity:        3200 mAh
Energy:          11.84 Wh
```

This battery size was selected to support a wearable runtime target while keeping the system small enough for a ski/snowboarding retrofit form factor.

The battery powers the system through the regulation and power-control stages. Firmware also monitors the battery level through the MAX17048 fuel gauge over I2C.

## USB-C Charging Input

Snowman uses USB-C as the charging input.

The USB-C connector is configured as a sink-only device using pull-down resistors on the configuration channel pins:

```txt
CC1 -> 5.1 kΩ -> GND
CC2 -> 5.1 kΩ -> GND
```

This allows the USB-C source to detect the device as a sink and provide 5 V charging input.

For this MVP-stage design, USB-C is used for power input rather than high-speed data or USB Power Delivery negotiation. The design keeps charging simple and focused on safe, low-complexity battery recharge behavior.

## Battery Charging: TP4056-MS

The TP4056-MS is used as the single-cell lithium battery charging IC.

Design role:

- Accepts 5 V from the USB-C input.
- Charges the 3.7 V lithium battery.
- Provides a simple charging architecture for early wearable prototypes.
- Keeps the charging subsystem separate from the ESP32-S3 application firmware.

Design notes:

- Final charge current depends on the programming resistor and thermal limits.
- PCB layout should respect charger thermal behavior.
- The charger should be validated with the selected battery pack.
- Battery protection requirements should be reviewed for the final pack and production design.

## Pushbutton Power Control: LTC2954ITS8

The LTC2954ITS8 is used as the power on/off controller. It allows the device to be powered on or powered down using a physical button instead of relying only on firmware.

Design role:

- Provides a user-facing power button behavior.
- Helps prevent accidental power toggles through timing control.
- Can signal the main MCU during power-button events.
- Allows the MCU to participate in controlled shutdown behavior.
- Separates critical power-control behavior from normal application logic.

The LTC2954 is connected to the main MCU so firmware can detect power-button behavior and respond properly.

Example firmware behavior:

```txt
User holds power button
        |
        v
LTC2954 detects valid press duration
        |
        v
MCU receives power-event signal
        |
        v
Firmware saves state / prepares shutdown
        |
        v
System power is released cleanly
```

### Timing Capacitor Choice

The design uses a 470 nF timing capacitor to create an estimated hold time of approximately 6 seconds for wake/shutdown behavior.

Design intent:

- Avoid accidental power-on events.
- Avoid accidental shutdowns from quick button taps.
- Make power control usable with gloves.
- Provide enough time for firmware to react to power-button events.
- Keep behavior consistent with an outdoor wearable use case.

Public note: the exact timing should be verified against the final LTC2954 configuration, capacitor tolerance, temperature behavior, and measured prototype behavior.

## Battery Fuel Gauge: MAX17048G+T10

The MAX17048G+T10 is used to monitor the single-cell lithium battery over I2C.

Design role:

- Measures battery voltage and state-of-charge behavior.
- Provides firmware-visible battery percentage information.
- Allows the ESP32-S3 to detect low-battery conditions.
- Supports user feedback such as low-battery beep alerts.
- Helps the device communicate battery state without guessing from voltage alone.

Firmware-level behavior:

```txt
MAX17048 reports battery data over I2C
        |
        v
ESP32-S3 reads battery percentage / voltage
        |
        v
Firmware compares battery level against threshold
        |
        v
Low battery state triggers buzzer or UI feedback
```

Example user feedback behavior:

```txt
Battery level below threshold
        |
        v
Firmware sets LOW_BATTERY flag
        |
        v
Buzzer/indicator task plays low-battery alert
```

This allows the system to provide simple but useful battery feedback without requiring a screen or phone app.

## Main Logic Regulation: TPS62849DLCR

The TPS62849DLCR is used to regulate the battery voltage into a stable logic rail for the ESP32-S3 and supporting electronics.

Design intent:

- Convert the single-cell lithium battery voltage into a stable 3.3 V-class rail.
- Improve system stability as the battery voltage changes during discharge.
- Support efficient battery-powered operation.
- Provide a cleaner supply for the MCU and digital peripherals.

Important implementation note:

The exact TPS62849 output option should be verified before final fabrication. Some distributor listings identify TPS62849DLCR as a fixed 3.4 V regulator. For Snowman, the design intent is a stable ESP32-S3-compatible logic rail. The final selected regulator variant and output voltage should be validated against the ESP32-S3 module requirements and all connected peripherals.

## Power-State Behavior

Snowman power behavior is divided into hardware-level and firmware-level responsibilities.

### Hardware-Level Responsibilities

- USB-C receives 5 V charging input.
- TP4056-MS manages lithium battery charging.
- LTC2954 manages pushbutton on/off control.
- TPS62849 provides the regulated logic rail.
- MAX17048 monitors battery state.

### Firmware-Level Responsibilities

- Read fuel-gauge data over I2C.
- Track low-battery state.
- Trigger low-battery alerts.
- Manage low-power firmware states where supported.
- Respond to power-button events from the LTC2954.
- Save important device state before shutdown where appropriate.
- Control user feedback such as buzzer patterns.

## Example Runtime Power Flow

```txt
Battery connected
        |
        v
User holds power button
        |
        v
LTC2954 enables system power
        |
        v
TPS62849 generates stable logic rail
        |
        v
ESP32-S3 boots firmware
        |
        v
Firmware initializes MAX17048 over I2C
        |
        v
Device monitors battery level during operation
```

## Example Shutdown Flow

```txt
User holds power button
        |
        v
LTC2954 detects shutdown request
        |
        v
MCU receives power-event signal
        |
        v
Firmware prepares system for shutdown
        |
        v
Device saves settings/state if needed
        |
        v
LTC2954 releases system power
```

## Low-Battery Alert Strategy

The low-battery alert is designed to be simple and useful in a wearable environment.

Possible alert behavior:

- Periodic beep when battery drops below threshold.
- Different buzzer pattern for critical battery level.
- Optional UI/captive portal battery display.
- Optional low-power behavior when battery is very low.

Example public logic:

```c
if (battery_percent <= LOW_BATTERY_PERCENT) {
    device_state.low_battery = true;
    buzzer_play_low_battery_pattern();
}
```

The exact thresholds and patterns are intentionally excluded from this public document.

## Design Tradeoffs

### Why use a dedicated pushbutton power controller?

A dedicated power controller makes the system more robust than relying only on the MCU to control its own power state. It also allows predictable user-facing power behavior and helps prevent accidental button events.

### Why use a fuel gauge instead of voltage-only measurement?

Lithium battery voltage does not map linearly to remaining capacity. A fuel gauge gives firmware a better estimate of state of charge and allows cleaner low-battery user feedback.

### Why use a buck regulator?

A buck regulator helps maintain a stable logic rail as battery voltage changes. This is important for wireless communication, MCU stability, and wearable reliability.

### Why keep charging simple?

For MVP hardware, a simple single-cell charging system helps reduce design scope and keeps the product focused on communication, connectivity, and usability.

## Validation Checklist

Before final production hardware, the power system should be validated for:

- USB-C 5 V detection behavior.
- TP4056-MS charge current and thermal behavior.
- Battery charge/discharge behavior.
- TPS62849 output voltage under load.
- ESP32-S3 boot stability during wireless activity.
- MAX17048 I2C communication reliability.
- Low-battery alert behavior.
- LTC2954 power-on and power-off timing.
- Button behavior with gloves.
- Shutdown behavior during active firmware tasks.
- Battery connector safety and mechanical reliability.
- Regulator behavior during Wi-Fi/Bluetooth current bursts.

## What Is Not Included Publicly

This document intentionally does not include:

- Full schematics.
- PCB layout files.
- Gerbers.
- Exact net names.
- Full pin mapping.
- Battery protection circuit details.
- Final production thresholds.
- Complete firmware power-state logic.
- Detailed manufacturing validation data.

## Summary

The Snowman power system is designed to support a practical wearable embedded product: USB-C charging, lithium battery operation, pushbutton power control, fuel-gauge monitoring, regulated MCU power, and firmware-driven user feedback.

The design balances MVP simplicity with product-focused power architecture, allowing the device to remain focused on its core goal: reliable communication and connectivity in a wearable winter-sports form factor.
