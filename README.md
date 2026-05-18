# 🧠 ZAIRE SYSTEMS — SNOWMAN v1

Smart communication system for winter helmets *(retrofit sleeve-based design)*

---

## 🚀 Overview

Zaire Snowman v1 is a modular, retrofit communication system designed to upgrade existing snowboard/ski helmets — no need to replace the helmet you already trust.

### Core Features

- 🎧 Bluetooth audio (music + calls)
- 📡 Walkie-talkie communication (ESP-NOW)
- 🎛️ On-ear controls (glove-friendly)
- 🔋 External battery module (hidden in liner)
- 🌐 Web-based settings portal (no app needed)
- 🧠 Persistent settings (NVS storage)

---

## 🏗️ System Architecture

### 🧦 Form Factor

- Earmuff sleeve wraps over stock earmuff
- Integrated speaker (ear-facing)
- PCB + controls (outer-facing)
- Battery stored in helmet liner

---

## ⚙️ Features

- Walkie-talkie communication
- Bluetooth audio
- Sleeve-based retrofit design
- Web settings portal
- Persistent storage

---

## 🔌 Setup

Requirements:
- ESP32 (ESP32-D recommended)
- ESP-IDF v5+

```bash
idf.py build
idf.py flash
idf.py monitor
```

---

## 👤 Author

Yann Kabambi — Zaire Systems
