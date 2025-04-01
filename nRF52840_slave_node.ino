#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

BLEService customService(SERVICE_UUID);
BLECharacteristic customCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 50);

void setup() {
    Serial.begin(115200);
    while (!Serial);
    Serial.println("🚀 Starting Slave Node...");

    if (!BLE.begin()) {
        Serial.println("❌ Failed to start BLE!");
        while (1);
    }

    BLE.setLocalName("XIAO nRF52840");
    BLE.setAdvertisedService(customService);
    customService.addCharacteristic(customCharacteristic);
    BLE.addService(customService);
    customCharacteristic.setValue("Waiting for connection...");

    Serial.print("📡 Advertising Service UUID: ");
    Serial.println(customService.uuid());
    Serial.print("🔍 Characteristic UUID: ");
    Serial.println(customCharacteristic.uuid());

    BLE.advertise();
}

void loop() {
    BLEDevice central = BLE.central();
    if (central) {
        Serial.print("✅ Connected to: ");
        Serial.println(central.address());

        while (central.connected()) {
            Serial.println("📤 Sending message...");
            customCharacteristic.setValue("Hello world updated!");
            BLE.poll(); // ✅ ทำให้ BLE ทำงานต่อเนื่อง
            delay(1000);
        }

        Serial.println("🔄 Disconnected! Restarting advertisement...");
        BLE.advertise();
    }
}
