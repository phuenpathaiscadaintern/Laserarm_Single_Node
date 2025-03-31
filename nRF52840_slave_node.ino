#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

// LED บนบอร์ด Xiao nRF52840 ใช้ขา 13
#define LED_BUILTIN  13  

BLEService customService(SERVICE_UUID);
BLECharacteristic customCharacteristic(CHARACTERISTIC_UUID, BLERead | BLENotify, 50);

void setup() {
    Serial.begin(115200);

    pinMode(LED_BUILTIN, OUTPUT);

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

    // ไฟแดงกระพริบเมื่อรอโฆษณา BLE
    flashRed();
}

void loop() {
    BLEDevice central = BLE.central();

    if (central) {
        Serial.print("Connected to: ");
        Serial.println(central.address());

        setGreen(); // เปลี่ยนเป็นไฟเขียวเมื่อเชื่อมต่อแล้ว

        while (central.connected()) {
            customCharacteristic.setValue("Hello world updated!");
            delay(1000);
        }

        Serial.println("Disconnected!");
        flashRed(); // กลับไปกระพริบไฟแดงเมื่อหลุดการเชื่อมต่อ
    }
}

// ฟังก์ชันให้ไฟแดงกระพริบ
void flashRed() {
    while (!BLE.connected()) {
        analogWrite(LED_BUILTIN, 255); // ไฟแดง
        delay(500);
        analogWrite(LED_BUILTIN, 0);   // ปิดไฟ
        delay(500);
    }
}

// ฟังก์ชันเปลี่ยนเป็นไฟสีเขียว
void setGreen() {
    analogWrite(LED_BUILTIN, 0); // ปิดไฟแดง
    // Xiao nRF52840 ไม่มีไฟเขียวโดยตรง อาจต้องใช้ค่าลดแสงแดงให้เป็นสีเขียวมากขึ้น
}
