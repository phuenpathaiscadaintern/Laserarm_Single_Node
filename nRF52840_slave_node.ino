#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

// กำหนดขา LED บนบอร์ด
#define LED_BUILTIN  13  
#define LED_RED      22
#define LED_GREEN    23

// สร้าง Service และ Characteristic
BLEService customService(SERVICE_UUID);
BLECharacteristic customCharacteristic(CHARACTERISTIC_UUID, BLERead | BLENotify, 50);

void setup() {
    Serial.begin(115200);

    // ตั้งค่า LED เป็น Output
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);

    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1);
    }

    // ตั้งค่าชื่อโฆษณา
    BLE.setLocalName("XIAO nRF52840");
    BLE.setAdvertisedService(customService);
    
    // เพิ่ม Characteristic ให้กับ Service
    customService.addCharacteristic(customCharacteristic);
    BLE.addService(customService);

    // ตั้งค่าข้อความเริ่มต้น
    customCharacteristic.setValue("Hello world");

    // เริ่มโฆษณา BLE
    BLE.advertise();
    Serial.println("BLE Advertising started...");

    // กระพริบไฟแดงเมื่อรอโฆษณา BLE
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

// ฟังก์ชันให้ไฟแดงกระพริบเมื่อรอโฆษณา BLE
void flashRed() {
    while (!BLE.connected()) {
        digitalWrite(LED_RED, HIGH);
        delay(500);
        digitalWrite(LED_RED, LOW);
        delay(500);
    }
}

// ฟังก์ชันเปลี่ยนเป็นไฟสีเขียวเมื่อเชื่อมต่อ
void setGreen() {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
}
