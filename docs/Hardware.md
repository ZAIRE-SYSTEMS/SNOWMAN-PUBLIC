## Hardware Architecture Evolution

Snowman/Zaire Systems has gone through multiple hardware architecture revisions as the product direction became clearer.

The original concept started closer to a smart helmet platform with camera support, richer onboard processing, and expanded embedded features. During early exploration, the architecture included multiple controller options and even an Allwinner H3-based Linux processor for more advanced camera and processing workflows.

Over time, the product direction became more focused. The form factor shifted away from a full helmet platform toward a smaller wearable retrofit communication system for ski and snowboarding gear. Although the form factor changed, the core goal stayed the same:

> Build a practical wearable system focused on communication, connectivity, and simple user interaction.

## Controller Selection Iteration

The controller architecture evolved through several stages:

```txt
ESP32
  ↓
ESP32-S3
  ↓
ESP32-D
  ↓
ESP32-S3

Each stage was used to evaluate tradeoffs around firmware support, available peripherals, wireless behavior, board complexity, size, cost, power consumption, and development speed.
```

### ESP32
The original ESP32-based direction provided a strong starting point for Wi-Fi, Bluetooth, ESP-NOW communication, captive portal configuration, and general embedded prototyping.

### ESP32S3
The ESP32-S3 was explored for its stronger embedded feature set, USB support, expanded memory options, and better fit for more advanced firmware and peripheral integration.

### ESP32D
The design later moved back toward an ESP32-D-style architecture during simplification and testing, especially while narrowing the product around communication and connectivity instead of camera-heavy processing.

### Return to ESP32-S3
The project later returned to the ESP32-S3 as the preferred direction because it provides a strong balance of firmware capability, wireless support, peripheral flexibility, and future expansion while keeping the system simpler than a multi-processor Linux-based architecture.

### Allwinner H3 Exploration
At one stage, the architecture included an Allwinner H3 processor to support more advanced onboard processing, camera workflows, and Linux-based features.
This direction was eventually dialed back because it increased:
* board complexity
* power requirements
* firmware/software scope
* boot and storage complexity
* manufacturing risk
* MVP development time

The decision to remove the Allwinner H3 from the MVP architecture helped refocus the product around the features that mattered most: communication, connectivity, wearable usability, and reliable embedded operation.

### Camera Feature Scope Reduction
Earlier versions of the project included camera functionality when the product direction was closer to a smart helmet platform.
As the product evolved into a smaller wearable communication retrofit system, camera support was removed from the MVP scope. This reduced hardware complexity and allowed the system to focus on the core user problem:
* communicating with nearby riders
* maintaining Bluetooth audio support
* providing simple device configuration
* keeping the system wearable and practical
* reducing power and size constraints
