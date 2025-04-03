#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define SHOOT_CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"
#define COUNTER_ALL_UUID    "A1B2C3D4-E5F6-7890-1234-56789ABCDEF0"
#define COUNTER_ACC_UUID    "B2C3D4E5-F678-9012-3456-789ABCDEF012"
#define SOUND_LEVEL_UUID    "C3D4E5F6-7890-1234-5678-9ABCDEF01234"

#define TARGET_ADDRESS      "54:91:e9:a3:b9:b8"
#define LED 23

BLEDevice peripheral;
BLECharacteristic shootCharacteristic;
BLECharacteristic counterAllCharacteristic;
BLECharacteristic counterAccCharacteristic;
BLECharacteristic soundLevelCharacteristic;
bool isConnected = false;

void TaskBLEScan(void *pvParameters) {
    Serial.println("🔍 Scanning for BLE device...");
    BLE.scan();
    unsigned long startTime = millis();
    while (!isConnected && millis() - startTime < 20000) {
        BLEDevice foundDevice = BLE.available();
        if (foundDevice) {
            String foundAddress = String(foundDevice.address());
            Serial.println("📡 Found BLE Device: " + foundAddress);
            if (foundAddress.equalsIgnoreCase(TARGET_ADDRESS)) {
                BLE.stopScan();
                Serial.println("🔗 Trying to connect...");
                if (foundDevice.connect()) {
                    Serial.println("✅ Connected!");
                    peripheral = foundDevice;
                    isConnected = true;

                    if (!peripheral.discoverAttributes()) {
                        Serial.println("❌ Failed to discover attributes!");
                        return;
                    }

                    BLEService foundService = peripheral.service(SERVICE_UUID);
                    if (!foundService) {
                        Serial.println("❌ Service UUID not found!");
                        return;
                    }

                    shootCharacteristic = foundService.characteristic(SHOOT_CHARACTERISTIC_UUID);
                    counterAllCharacteristic = foundService.characteristic(COUNTER_ALL_UUID);
                    counterAccCharacteristic = foundService.characteristic(COUNTER_ACC_UUID);
                    soundLevelCharacteristic = foundService.characteristic(SOUND_LEVEL_UUID);

                    if (!shootCharacteristic || !counterAllCharacteristic || !counterAccCharacteristic || !soundLevelCharacteristic) {
                        Serial.println("❌ Some characteristics are missing!");
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
    pinMode(LED, OUTPUT);
    Serial.println("🚀 Starting BLE Client...");
    if (!BLE.begin()) {
        Serial.println("❌ Failed to initialize BLE!");
        while (1);
    }
    xTaskCreatePinnedToCore(TaskBLEScan, "BLEScan", 8192, NULL, 1, NULL, 1);
}

void loop() {
    if (isConnected && peripheral.connected()) {
        if (shootCharacteristic.canRead()) {
            char buffer[2] = {0};
            int length = shootCharacteristic.readValue(buffer, sizeof(buffer) - 1);
            buffer[length] = '\0';
            Serial.print("📩 Shot Status: ");
            Serial.println(buffer);
        }

        if (counterAllCharacteristic.canRead()) {
            int counterAll = 0;
            counterAllCharacteristic.readValue(&counterAll, sizeof(counterAll));
            Serial.print("🎯 Total Shots: ");
            Serial.println(counterAll);
        }

        if (counterAccCharacteristic.canRead()) {
            int counterAcc = 0;
            counterAccCharacteristic.readValue(&counterAcc, sizeof(counterAcc));
            Serial.print("🎯 Shots Since Last Reset: ");
            Serial.println(counterAcc);
        }

        if (soundLevelCharacteristic.canRead()) {
            int soundLevel = 0;
            soundLevelCharacteristic.readValue(&soundLevel, sizeof(soundLevel));
            Serial.print("🔊 Sound Level: ");
            Serial.println(soundLevel);
        }
    } else if (isConnected && !peripheral.connected()) {
        Serial.println("🔄 Disconnected! Restarting scan...");
        isConnected = false;
        BLE.begin();
        xTaskCreatePinnedToCore(TaskBLEScan, "BLEScan", 8192, NULL, 1, NULL, 1);
    }
    delay(1000);
}
