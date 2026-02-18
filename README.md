GlacierNode: Industrial Vevor Heater Bridge
GlacierNode is a robust, PoE-powered BLE-to-Ethernet gateway designed for extreme climates. This project provides a "Truck LAN" integrated controller for Vevor Diesel Heaters, replacing unstable factory mobile apps with a high-reliability, wired hardware bridge.
❄️ Calgary-Ready Infrastructure
Built specifically for survival in Canadian winters (Calgary, AB), this implementation prioritizes wired stability and cold-weather component ratings to ensure your primary heat source never fails due to a "smart" app glitch.
🤝 Acknowledgments & Lineage
This project is an ESP32 C++/Arduino port of the excellent reverse-engineering work done by the community. It draws heavily from the following projects:
vevor-heater-ble (iotmaestro): The primary source for the BLE communication protocol, packet structures, and checksum logic.
homeassistant-vevor-heater (MSDATDE): Inspiration for high-level state management and integration patterns.
This repository takes those software-based discoveries and moves them onto dedicated, industrial-grade hardware.
🚀 Key Features
Hardware-First: Designed for the Olimex ESP32-POE-ISO-IND, featuring -40°C to +85°C operating range and 3000VDC galvanic isolation.
PoE Connectivity: Wired backhaul via 802.3af/at PoE, eliminating WiFi interference and power-management sleep issues.
Stateless Synchronization: Upon boot, the node "asks" the heater for its current state, ensuring it never overrides existing safety cycles (e.g., if the ESP32 reboots during a cooldown).
Protocol Support: Full implementation of the AA 55 magic-byte protocol, including real-time telemetry for combustion chamber temperature, pump frequency, voltage, and error codes.
🛠️ Hardware Requirements
Microcontroller: ESP32-POE-ISO-IND (Industrial Version).
Network: PoE-enabled switch (e.g., Linovision Industrial or Cudy GS1005P).
Backhaul: Designed to work behind CGNAT environments like Starlink Mini.
📡 Protocol Snapshot
The Vevor heater communicates via BLE using a proprietary serial bridge on Service 0xFFE0 / Characteristic 0xFFE1.
Offset	Description	Value
0-1	Magic Header	0xAA 0x55
2-3	Passkey	0x0C 0x22 (Default: 1234)
4	Command Type	0x01 (Read) - 0x04 (Set)
19	Checksum	Sum of bytes 3-18 mod 256
⚖️ Safety & Disclaimer
NOT FOR LIFE-SAFETY APPLICATIONS.
This project is for monitoring and secondary control. Diesel heaters are combustion devices. This controller is designed to work with the heater's internal safety MCU, not replace it. Never override the heater's internal shutdown/cooldown cycles.
