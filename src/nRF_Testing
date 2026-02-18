# Lab Log: BLE Protocol Verification

## 🎯 Objective
To verify the `0xAA 0x55` communication protocol and the `1234` passkey logic discovered by the `iotmaestro` and `MSDATDE` projects on a 2024/2025 Vevor Diesel Heater unit.

## 🔬 Test Environment
- **Device:** Vevor 5kW Diesel Heater (2025 Model)
- **Ambient Temp:** -20°C (Blizzard Conditions - Calgary, AB)
- **Sniffer Tool:** nRF Connect for Mobile (Android/iOS)
- **Power Source:** 30Ah LiFePO4 Battery Pack

## 🧪 Experiment 1: The "Ping" Test
**Hypothesis:** Sending a "Read Status" command (0x01) with the default passkey `1234` will trigger a 20-byte notification.

**Command (Hex):** `AA550C220100002F`

**Results:**
- [ ] Response Received (AA 55 01...)
- [ ] Voltage matches multimeter/Victron data.
- [ ] Room Temp matches ambient sensors.

## 🧪 Experiment 2: Manual Power Level (Hz) Control
**Hypothesis:** The heater must be placed in Mode 1 (Manual) before Hz levels can be adjusted remotely.

**Step 1: Set Manual Mode**
`AA550C2202010031`

**Step 2: Set Power Level 10 (High Burn)**
`AA550C22040A003C`

## 📸 Evidence & Documentation
*(Insert your screenshots here)*

| Phone UI (nRF Connect) | Heater LCD Response | Description |
| :--- | :--- | :--- |
| ![Phone Screenshot](./screenshots/ping_sent.jpg) | ![LCD Photo](./controller_logs/lcd_response.jpg) | Verification of Level 10 Command |

## 📝 Observations
- Observation 1: Does the heater respond instantly?
- Observation 2: Does the BLE overide the setting from the supplied remote?
