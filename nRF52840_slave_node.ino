#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

BLEService customService(SERVICE_UUID);
BLECharacteristic shootCharacteristic(CHARACTERISTIC_UUID, BLERead | BLENotify, 20);

const int MIC_PIN = A0;  // พินของไมโครโฟน
const int SOUND_THRESHOLD = 700;  // ค่าเสียงที่ใช้เป็นเกณฑ์การยิง (0 - 1023)

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
    customService.addCharacteristic(shootCharacteristic);
    BLE.addService(customService);
    
    shootCharacteristic.setValue("ยังไม่ได้ยิง");

    BLE.advertise();
    Serial.println("📡 Advertising BLE Service...");
}

void loop() {
    BLEDevice central = BLE.central();
    if (central) {
        Serial.print("✅ Connected to: ");
        Serial.println(central.address());

        while (central.connected()) {
            int soundLevel = analogRead(MIC_PIN);  // อ่านค่าจากไมโครโฟน
            Serial.print("🔊 Sound Level: ");
            Serial.println(soundLevel);

            if (soundLevel > SOUND_THRESHOLD) {
                Serial.println("🔥 ยิงแล้ว!");
                shootCharacteristic.setValue("ยิงแล้ว");
            } else {
                shootCharacteristic.setValue("ยังไม่ได้ยิง");
            }

            BLE.poll();  
            delay(500);  // ตรวจสอบเสียงทุกๆ 500ms
        }

        Serial.println("🔄 Disconnected! Restarting advertisement...");
        BLE.advertise();
    }
}
