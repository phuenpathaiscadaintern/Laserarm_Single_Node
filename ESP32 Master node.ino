// Master Node (ESP32 WROOM)
#include <ArduinoBLE.h>
#include "esp_task_wdt.h"

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
const char* CHARACTERISTIC_UUID = "D90A7C02-9B21-4243-8372-3E523FA7978B";
#define TARGET_ADDRESS      "54:91:e9:a3:b9:b8"

BLEDevice peripheral;
BLECharacteristic characteristic;
bool isConnected = false;

void TaskBLEScan(void *pvParameters) {
    Serial.println("🔍 Scanning for BLE device...");
    BLE.scan();
    unsigned long startTime = millis();
    while (!isConnected && millis() - startTime < 20000) {
        esp_task_wdt_reset();  
        BLEDevice foundDevice = BLE.available();
        if (foundDevice) {
            String foundAddress = String(foundDevice.address());
            Serial.println("📡 Found BLE Device: " + foundAddress);
            if (foundAddress.equalsIgnoreCase(TARGET_ADDRESS)) {
                BLE.stopScan();
                Serial.println("🔗 Trying to connect to " + String(TARGET_ADDRESS));
                if (foundDevice.connect()) {
                    Serial.println("✅ Connected to device!");
                    peripheral = foundDevice;
                    isConnected = true;
                    Serial.println("🔍 Discovering services...");
                    if (!peripheral.discoverAttributes()) {
                        Serial.println("❌ Failed to discover attributes!");
                        return;
                    }
                    Serial.println("✅ Services discovered!");
                    BLEService foundService = peripheral.service(SERVICE_UUID);
                    if (!foundService) {
                        Serial.println("❌ Service UUID not found!");
                        return;
                    }
                    Serial.println("✅ Found Service UUID!");
                    bool charFound = false;
                    for (int i = 0; i < foundService.characteristicCount(); i++) {
                      BLECharacteristic foundCharacteristic = foundService.characteristic(i);
                      Serial.print("🔎 Checking Characteristic UUID: ");
                      Serial.println(foundCharacteristic.uuid());

                    // ✅ เปลี่ยน equalsIgnoreCase() เป็น strcasecmp()
                    if (strcasecmp(foundCharacteristic.uuid(), CHARACTERISTIC_UUID) == 0) {
                      characteristic = foundCharacteristic;
                      charFound = true;
                      Serial.println("✅ Found Target Characteristic!");
                      break;
                    }
}

                    if (!charFound) {
                        Serial.println("❌ Target Characteristic not found!");
                        return;
                    }
                    Serial.println("🔗 BLE connection established.");
                    BLE.stopScan();
                    vTaskDelete(NULL);
                } else {
                    Serial.println("Connection failed. Retrying...");
                    delay(2000);
                }
            }
        }
        vTaskDelay(50);
    }
}

void setup() {
    Serial.begin(115200);
    setCpuFrequencyMhz(80);
    Serial.println("🚀 Starting BLE Client...");
    if (!BLE.begin()) {
        Serial.println("❌ Failed to initialize BLE!");
        while (1);
    }
    xTaskCreatePinnedToCore(
        TaskBLEScan,
        "BLEScan",
        8192,
        NULL,
        1,
        NULL,
        1
    );
}

void loop() {
    if (isConnected && peripheral.connected() && characteristic) {
        if (characteristic.canRead()) {
            char buffer[50] = {0};
            int length = characteristic.readValue(buffer, sizeof(buffer) - 1);
            buffer[length] = '\0';
            Serial.print("📩 Received: ");
            Serial.println(buffer);
        }
    } else if (isConnected && !peripheral.connected()) {
        Serial.println("🔄 Disconnected! Restarting scan...");
        isConnected = false;
        BLE.begin();
        xTaskCreatePinnedToCore(
            TaskBLEScan,
            "BLEScan",
            8192,
            NULL,
            1,
            NULL,
            1
        );
    }
    delay(1000);
}
