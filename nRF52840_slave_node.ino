#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define SHOOT_CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"
#define COUNTER_ALL_UUID    "A1B2C3D4-E5F6-7890-1234-56789ABCDEF0"
#define COUNTER_ACC_UUID    "B2C3D4E5-F678-9012-3456-789ABCDEF012"
#define SOUND_LEVEL_UUID    "C3D4E5F6-7890-1234-5678-9ABCDEF01234"

BLEService customService(SERVICE_UUID);
BLECharacteristic shootCharacteristic(SHOOT_CHARACTERISTIC_UUID, BLERead | BLENotify, 1);
BLECharacteristic counterAllCharacteristic(COUNTER_ALL_UUID, BLERead | BLENotify, sizeof(int));
BLECharacteristic counterAccCharacteristic(COUNTER_ACC_UUID, BLERead | BLENotify | BLEWrite, sizeof(int));
BLECharacteristic soundLevelCharacteristic(SOUND_LEVEL_UUID, BLERead | BLENotify, sizeof(int));

const int MIC_PIN = A0;
const int SOUND_THRESHOLD = 100;
int counterAll = 0;
int counterAcc = 0;

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
    customService.addCharacteristic(counterAllCharacteristic);
    customService.addCharacteristic(counterAccCharacteristic);
    customService.addCharacteristic(soundLevelCharacteristic);
    BLE.addService(customService);

    shootCharacteristic.setValue("0");
    counterAllCharacteristic.setValue((byte*)&counterAll, sizeof(counterAll));
    counterAccCharacteristic.setValue((byte*)&counterAcc, sizeof(counterAcc));
    soundLevelCharacteristic.setValue((byte*)&counterAll, sizeof(counterAll));

    BLE.advertise();
    Serial.println("📡 Advertising BLE Service...");
}

void loop() {
    BLEDevice central = BLE.central();
    if (central) {
        Serial.print("✅ Connected to: ");
        Serial.println(central.address());

        while (central.connected()) {
            int soundLevel = analogRead(MIC_PIN);
            Serial.print("🔊 Sound Level: ");
            Serial.println(soundLevel);
            soundLevelCharacteristic.setValue((byte*)&soundLevel, sizeof(soundLevel));

            if (soundLevel > SOUND_THRESHOLD) {
                Serial.println("🔥 ยิงแล้ว!");
                shootCharacteristic.setValue("1");
                counterAll++;
                counterAcc++;
                counterAllCharacteristic.setValue((byte*)&counterAll, sizeof(counterAll));
                counterAccCharacteristic.setValue((byte*)&counterAcc, sizeof(counterAcc));
            } else {
                shootCharacteristic.setValue("0");
            }

            BLE.poll();
            delay(500);
        }

        Serial.println("🔄 Disconnected! Restarting advertisement...");
        BLE.advertise();
    }
}
