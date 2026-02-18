# GlacierNode: Industrial Vevor Heater Bridge

**GlacierNode** is a robust, PoE-powered BLE-to-Ethernet gateway designed for extreme climates. This project provides a "Truck LAN" integrated controller for Vevor Diesel Heaters, replacing unstable factory mobile apps with a high-reliability, wired hardware bridge.

## ❄️ Calgary-Ready Infrastructure
Built specifically for survival in Canadian winters (Calgary, AB), this implementation prioritizes wired stability and cold-weather component ratings to ensure your primary heat source never fails due to a "smart" app glitch or WiFi interference.

## 🤝 Acknowledgments & Lineage
This project is an ESP32 C++/Arduino port of the excellent reverse-engineering work done by the community. It draws heavily from the following projects:

*   **[vevor-heater-ble (iotmaestro)](https://github.com/iotmaestro/vevor-heater-ble):** The primary source for the BLE communication protocol, packet structures, and checksum logic.
*   **[homeassistant-vevor-heater (MSDATDE)](https://github.com/MSDATDE/homeassistant-vevor-heater):** Inspiration for high-level state management and integration patterns.

This repository takes those software-based discoveries and moves them onto dedicated, industrial-grade hardware for off-grid and "vanlife" reliability.

## 🚀 Key Features
- **Hardware-First:** Optimized for the **Olimex ESP32-POE-ISO-IND**, featuring a -40°C to +85°C operating range and 3000VDC galvanic isolation.
- **PoE Connectivity:** Wired backhaul via 802.3af/at PoE, eliminating WiFi congestion and power-management instability.
- **Stateless Synchronization:** Upon boot, the node synchronizes with the heater's current state, ensuring it never overrides internal safety cycles (e.g., if the ESP32 reboots during a cooldown).
- **Protocol Support:** Full implementation of the `0xAA 0x55` magic-byte protocol, including real-time telemetry for combustion chamber temperature, pump frequency, voltage, and error codes.
- **Starlink Optimized:** Designed to operate behind CGNAT and satellite backhaul with minimal data overhead via optimized HTTPS heartbeats.

## 🛠️ Hardware Requirements
- **Microcontroller:** [ESP32-POE-ISO-IND](https://www.olimex.com/Products/IoT/ESP32/ESP32-POE-ISO/) (Industrial Version mandatory for sub-zero climates).
- **Network:** Industrial PoE+ Switch (e.g., Linovision or Cudy GS1005P).
- **Power:** 12V-to-48V DC-DC Boost Converter (for PoE Switch power) and 8 AWG primary power runs.
- **Ducting:** 3" Silicone Brake Vent Hose with aluminum Cam-Lock bulkhead fittings for airtight recirculation.

## 📡 Protocol Snapshot
The Vevor heater communicates via BLE using a proprietary serial bridge on Service `0xFFE0` / Characteristic `0xFFE1`.

| Offset | Description | Value |
| :--- | :--- | :--- |
| 0-1 | Magic Header | `0xAA 0x55` |
| 2-3 | Passkey | `0x0C 0x22` (Default: 1234) |
| 4 | Command Type | `0x01` (Read) - `0x04` (Set) |
| 19 | Checksum | Sum of bytes 3-18 mod 256 |

## ⚖️ Safety & Disclaimer
**NOT FOR LIFE-SAFETY APPLICATIONS.**
This project is for monitoring and secondary control. Diesel heaters are combustion devices. This controller is designed to work *with* the heater's internal safety MCU, not replace it. Never override or interrupt the heater's internal shutdown/cooldown cycles, as this poses a significant fire and carbon monoxide risk.

## 📄 License
This project is licensed under the MIT License - see the LICENSE file for details.
