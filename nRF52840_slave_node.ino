#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

#define LED_RED   2  // กำหนดขา LED สีแดง
#define LED_GREEN 3  // กำหนดขา LED สีเขียว

BLEService customService(SERVICE_UUID);
BLECharacteristic customCharacteristic(CHARACTERISTIC_UUID, BLERead | BLENotify, 50);

void setup() {
    Serial.begin(115200);
    
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);

    if (!BLE.begin()) {
        Serial.println("Starting BLE failed!");
        while (1);
    }

    BLE.setLocalName("XIAO nRF52840");
    BLE.setAdvertisedService(customService);
    customService.addCharacteristic(customCharacteristic);
    BLE.addService(customService);

    customCharacteristic.setValue("Hello world");
    
    BLE.advertise();
    Serial.println("Advertising started...");

    // เริ่มต้นให้ไฟแดงกระพริบเพื่อบอกว่ากำลังโฆษณา
    flashRed();
}

void loop() {
    BLEDevice central = BLE.central();

    if (central) {
        Serial.print("Connected to: ");
        Serial.println(central.address());

        digitalWrite(LED_RED, LOW);   // ปิดไฟแดง
        digitalWrite(LED_GREEN, HIGH); // เปิดไฟเขียว

        while (central.connected()) {
            customCharacteristic.setValue("Hello world updated!");
            delay(1000);
        }

        Serial.println("Disconnected!");

        // เมื่อหลุดการเชื่อมต่อ ให้กลับไปกระพริบไฟแดง
        flashRed();
    }
}

// ฟังก์ชันให้ไฟแดงกระพริบ
void flashRed() {
    while (!BLE.connected()) {
        digitalWrite(LED_RED, HIGH);
        delay(500);
        digitalWrite(LED_RED, LOW);
        delay(500);
    }
}
