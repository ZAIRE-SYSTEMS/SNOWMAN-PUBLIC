# Wireless and Audio Design Decisions

## Overview

Snowman is designed as a wearable communication platform for ski and snowboarding environments. The product requires short-range peer-to-peer communication, Bluetooth audio support, simple setup, and practical wearable operation.

This document explains the major wireless and audio architecture decisions at a public, portfolio-safe level.

## Product Communication Goals

The system is designed to support:

- Local peer-to-peer communication between nearby users.
- Bluetooth audio support for user-facing audio.
- Simple setup without a required mobile application.
- Reliable operation in outdoor winter environments.
- Reduced RF design complexity during MVP development.
- A path toward future product certification.

## Why ESP-NOW for Walkie-Style Communication

Early product exploration considered multiple ways to support walkie-talkie behavior. Instead of using a traditional radio architecture for the MVP, Snowman uses ESP-NOW-style peer-to-peer communication.

### Reasons for this approach

- Reduces RF design complexity during early product development.
- Allows decentralized communication behavior between devices.
- Works well with ESP32-based prototyping.
- Avoids requiring cloud infrastructure for local communication.
- Supports fast iteration during firmware development.
- Keeps the MVP focused on product behavior instead of custom radio design.

### Tradeoffs

ESP-NOW is useful for early peer-to-peer communication, but it also comes with tradeoffs:

- Range depends heavily on antenna design, enclosure, environment, and orientation.
- It is not the same as a licensed or long-range radio solution.
- Production designs still require careful RF planning and regulatory review.
- Reliability must be validated in real outdoor conditions.

## Bluetooth Audio Module Strategy

Snowman separates peer-to-peer walkie communication from Bluetooth audio behavior. This allows the system to evolve the audio hardware independently from the peer communication layer.

### Bluetooth audio goals

- Support user-facing audio behavior.
- Keep audio module control isolated from the rest of the firmware.
- Allow future hardware revisions to change modules without rewriting the entire system.
- Use module-level abstraction to reduce firmware coupling.

## Module Iteration

During development, the Bluetooth audio hardware path evolved across multiple module options.

The public design lesson:

```txt
Prototype hardware choices can change.
Firmware architecture should expect that.
The system is structured so Bluetooth audio behavior is accessed through an interface layer instead of being tightly coupled to a single module.
```
## Example abstraction:
Application Layer
      |
      v
Audio Manager
      |
      v
Bluetooth Module Interface
      |
      v
Selected Bluetooth Audio Hardware

## Pre-Certified Module Strategy
For future production planning, the system favors pre-certified wireless modules where practical.

#### Why this matters
Wireless products must be designed with regulatory requirements in mind. Using pre-certified modules can help reduce RF design risk and simplify the path toward compliance compared with designing a custom radio from scratch.
This does not eliminate all certification responsibilities, but it can reduce risk during the transition from prototype to product.

## Captive Portal Configuration
Snowman uses a local web-based configuration portal instead of requiring a mobile app for initial setup.

### Reasons for this decision
* Reduces setup friction for users.
* Avoids requiring app store installation for basic configuration.
* Allows phone/laptop configuration over local Wi-Fi.
* Keeps the MVP focused on firmware and device behavior.
* Makes the system easier to test during development.

# System-Level Wireless/Audio Separation
The firmware treats wireless communication and audio control as separate responsibilities.
+-------------------------+
| User Controls           |
+-----------+-------------+
            |
            v
+-------------------------+
| Application State       |
+------+-----------+------+
       |           |
       v           v
+------------+  +----------------+
| Walkie     |  | Audio Manager  |
| Interface  |  |                |
+------------+  +----------------+
       |           |
       v           v
+------------+  +----------------+
| ESP-NOW    |  | Bluetooth      |
| Layer      |  | Audio Module   |
+------------+  +----------------+

# Public Preview Note
Exact command handling, pairing behavior, security logic, and production protocol details are intentionally excluded from this public repository.
