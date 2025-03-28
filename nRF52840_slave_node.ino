#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

// สร้าง Service และ Characteristic
BLEService customService(SERVICE_UUID);
BLECharacteristic customCharacteristic(CHARACTERISTIC_UUID, 
                                       BLERead | BLENotify, 
                                       50); // ขนาดสูงสุดของข้อความ

void setup() {
    Serial.begin(115200);
    
    if (!BLE.begin()) {
        Serial.println("Starting BLE failed!");
        while (1);
    }

    BLE.setLocalName("XIAO nRF52840");
    BLE.setAdvertisedService(customService);
    
    customService.addCharacteristic(customCharacteristic);
    BLE.addService(customService);

    // กำหนดค่าเริ่มต้นใน Characteristic
    customCharacteristic.setValue("Hello world");
    
    // เริ่มโฆษณา BLE
    BLE.advertise();
    Serial.println("Advertising started...");
}

void loop() {
    // ตัวอุปกรณ์จะทำงานใน background และโฆษณา
    BLE.poll(); // ตรวจสอบการเชื่อมต่อและแจ้งเตือนในแต่ละรอบ

    // กำหนดค่าข้อความใหม่ใน Characteristic
    customCharacteristic.setValue("Hello world updated!");
    
    // รอเวลา 1 วินาที แล้วค่อยส่งการอัปเดตใหม่ไป
    delay(1000);
}
