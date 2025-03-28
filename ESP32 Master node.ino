#include <bluefruit.h>

// UUID ของ Service และ Characteristic ต้องตรงกับ ESP32
#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

// สร้าง Service และ Characteristic
BLEService customService(SERVICE_UUID);
BLECharacteristic customCharacteristic(CHARACTERISTIC_UUID, 
                                       CHR_PROPS_READ | CHR_PROPS_NOTIFY, 
                                       50); // ขนาดสูงสุดของข้อความ

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Peripheral...");

    Bluefruit.begin();
    Bluefruit.setName("XIAO nRF52840");

    // เริ่มโฆษณา BLE Service
    Bluefruit.Advertising.addService(customService);
    Bluefruit.Advertising.addName();

    // สร้าง Service และ Characteristic
    customService.begin();
    customCharacteristic.begin();

    // กำหนดข้อมูลที่ต้องการส่ง
    String boardID = "Board ID: XIAO\n";
    String boardNumber = "Board Number: 1\n";
    String message = "Received Data: Hello world";

    String fullMessage = boardID + boardNumber + message;
    
    // เขียนค่าไปยัง Characteristic
    customCharacteristic.write(fullMessage.c_str(), fullMessage.length());

    // เริ่มโฆษณา
    Bluefruit.Advertising.start();
}

void loop() {
    delay(1000);
}
