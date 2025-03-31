#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

// ใช้ BLEWrite เพื่อให้ ESP32 อ่านค่าได้
BLEService customService(SERVICE_UUID);
BLECharacteristic customCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 50);

void setup() {
    Serial.begin(115200);

    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1);
    }

    BLE.setLocalName("XIAO nRF52840");
    BLE.setAdvertisedService(customService);
    
    // เพิ่ม Characteristic ให้กับ Service
    customService.addCharacteristic(customCharacteristic);
    BLE.addService(customService);

    // ตั้งค่าข้อความเริ่มต้น
    customCharacteristic.setValue("Hello world");

    // เริ่มโฆษณา
    BLE.advertise();
    Serial.println("BLE Advertising started...");
}

void loop() {
    BLEDevice central = BLE.central();

    if (central) {
        Serial.print("Connected to: ");
        Serial.println(central.address());

        while (central.connected()) {
            customCharacteristic.setValue("Hello world updated!");
            delay(1000);
        }

        Serial.println("Disconnected! Restarting advertisement...");
        BLE.advertise();
    }
}
