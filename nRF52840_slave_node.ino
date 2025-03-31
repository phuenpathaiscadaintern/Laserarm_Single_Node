#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

// ขา LED บนบอร์ด XIAO nRF52840
#define LED_RED   22  
#define LED_GREEN 23  

BLEService customService(SERVICE_UUID);
BLECharacteristic customCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 50);

void setup() {
    Serial.begin(115200);
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);

    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1);
    }

    BLE.setLocalName("XIAO nRF52840");
    BLE.setAdvertisedService(customService);
    customService.addCharacteristic(customCharacteristic);
    BLE.addService(customService);

    customCharacteristic.setValue("Hello world");

    BLE.advertise();
    Serial.println("BLE Advertising started...");

    flashRed();
}

void loop() {
    BLEDevice central = BLE.central();

    if (central) {
        Serial.print("Connected to: ");
        Serial.println(central.address());
        setGreen();

        while (central.connected()) {
            customCharacteristic.setValue("Hello world updated!");
            delay(1000);
        }

        Serial.println("Disconnected! Restarting advertisement...");
        setRed();
        BLE.advertise();
    }
}

void flashRed() {
    for (int i = 0; i < 6; i++) {  // กระพริบ 3 ครั้ง แล้วหยุด
        digitalWrite(LED_RED, HIGH);
        delay(500);
        digitalWrite(LED_RED, LOW);
        delay(500);
    }
}

void setGreen() {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
}

void setRed() {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH);
}
