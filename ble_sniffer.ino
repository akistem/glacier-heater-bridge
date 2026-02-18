#include <NimBLEDevice.h>
#include <ETH.h>

/**
 * Vevor Diesel Heater - BLE Sniffer / Decoder
 * This code connects to the heater and prints every status update to Serial.
 */

// --- Olimex POE-ISO Ethernet Pins (Required for board stability) ---
#define ETH_PHY_ADDR  0
#define ETH_PHY_POWER 12
#define ETH_PHY_MDC   23
#define ETH_PHY_MDIO  18
#define ETH_TYPE      ETH_PHY_LAN8720
#define ETH_CLK_MODE  ETH_CLOCK_GPIO17_OUT

// --- BLE UUIDs ---
static const char* SERVICE_UUID = "0000ffe0-0000-1000-8000-00805f9b34fb";
static const char* CHAR_UUID    = "0000ffe1-0000-1000-8000-00805f9b34fb";

// --- Global State ---
static NimBLEClient*  pClient = nullptr;
static NimBLERemoteCharacteristic* pRemoteChar = nullptr;
bool connected = false;
bool doConnect = false;
NimBLEAdvertisedDevice* targetDevice = nullptr;

// --- Heater Status Structure (20 Bytes) ---
struct __attribute__((packed)) HeaterStatus {
    uint16_t magic;           // 0x55AA
    uint8_t  command;         // Should be 0x01 for response to ping
    uint8_t  power_status;    // 0=Off, 1=On
    uint8_t  error_code;
    uint8_t  run_state;       // 0=Warmup, 1=SelfTest, 2=Ignition, 3=Heating, 4=Shutdown
    uint16_t altitude;
    uint8_t  op_mode;         // 1=Manual, 2=Auto
    uint8_t  target_val;      // The Hz level or Temperature set on LCD
    uint8_t  current_pwr;     // Current pump speed
    uint16_t voltage;         // Decivolts (e.g. 126 = 12.6V)
    uint16_t heat_temp;       // Combustion chamber temp
    uint16_t room_temp;       // Ambient temp
    uint8_t  display_err;     // E-0X code
    uint8_t  padding;         // 0x00
    uint8_t  checksum;
};

// --- Helper: Print Raw Hex ---
void printHex(uint8_t* data, size_t len) {
    Serial.print("RAW HEX: ");
    for (size_t i = 0; i < len; i++) {
        if (data[i] < 0x10) Serial.print("0");
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

// --- Notification Handler (This is where the magic happens) ---
void notifyCallback(NimBLERemoteCharacteristic* pChar, uint8_t* pData, size_t length, bool isNotify) {
    if (length < 20) return;

    printHex(pData, length);

    HeaterStatus* s = (HeaterStatus*)pData;

    Serial.println("----------------------------------------");
    Serial.printf("Command Ref:  0x%02X\n", s->command);
    Serial.printf("Power Status: %s\n", (s->power_status == 1) ? "ON" : "OFF");
    
    const char* states[] = {"Warmup", "Self-Test", "Ignition", "Heating", "Shutting Down"};
    if (s->run_state <= 4) Serial.printf("Run State:    %s\n", states[s->run_state]);
    
    Serial.printf("Mode:         %s\n", (s->op_mode == 2) ? "Automatic (Temp)" : "Manual (Power)");
    Serial.printf("LCD Target:   %d %s\n", s->target_val, (s->op_mode == 2) ? "°C" : "Level");
    Serial.printf("Current Pump: Level %d\n", s->current_pwr);
    Serial.printf("Voltage:      %.1f V\n", s->voltage / 10.0);
    Serial.printf("Chamber Temp: %d °C\n", s->heat_temp);
    Serial.printf("Room Temp:    %d °C\n", s->room_temp);
    if (s->display_err > 0) Serial.printf("ERROR:        E-%02d\n", s->display_err);
    Serial.println("----------------------------------------");
}

// --- Scan Callbacks ---
class AdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        if (advertisedDevice->isAdvertisingService(NimBLEUUID(SERVICE_UUID))) {
            Serial.print("Found Heater! ");
            Serial.println(advertisedDevice->toString().c_str());
            NimBLEDevice::getScan()->stop();
            targetDevice = advertisedDevice;
            doConnect = true;
        }
    }
};

// --- Connection Callbacks ---
class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) { Serial.println("Connected."); }
    void onDisconnect(NimBLEClient* pClient) {
        connected = false;
        Serial.println("Disconnected. Scanning again...");
        NimBLEDevice::getScan()->start(0, false);
    }
};

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\nVevor BLE Sniffer Starting...");

    // Initialize Ethernet (needed for PoE board to manage power correctly)
    ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_TYPE, ETH_CLK_MODE);

    // Initialize BLE
    NimBLEDevice::init("CalgaryHeaterSniffer");
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    pScan->setInterval(45);
    pScan->setWindow(15);
    pScan->setActiveScan(true);
    pScan->start(0, false);
}

void loop() {
    // 1. Handle Connection
    if (doConnect) {
        doConnect = false;
        pClient = NimBLEDevice::createClient();
        pClient->setClientCallbacks(new ClientCallbacks());

        if (pClient->connect(targetDevice)) {
            NimBLERemoteService* pSvc = pClient->getService(SERVICE_UUID);
            if (pSvc) {
                pRemoteChar = pSvc->getCharacteristic(CHAR_UUID);
                if (pRemoteChar && pRemoteChar->canNotify()) {
                    pRemoteChar->subscribe(true, notifyCallback);
                    connected = true;
                }
            }
        }
    }

    // 2. Periodic Ping (0x01 Command) every 5 seconds
    static uint32_t lastPing = 0;
    if (connected && (millis() - lastPing > 5000)) {
        lastPing = millis();

        // Build the Ping packet: AA 55 0C 22 01 00 00 2F
        uint8_t ping[] = {0xAA, 0x55, 0x0C, 0x22, 0x01, 0x00, 0x00, 0x2F};
        
        if (pRemoteChar) {
            pRemoteChar->writeValue(ping, sizeof(ping), true);
            Serial.println(">> Ping sent (0x01 Read Status)");
        }
    }

    delay(10);
}
