#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

// ขา LED บนบอร์ด XIAO nRF52840
#define LED_BUILTIN 13

BLEService customService(SERVICE_UUID);
BLECharacteristic customCharacteristic(CHARACTERISTIC_UUID, BLERead | BLEWrite | BLENotify, 50);

void setup() {
    Serial.begin(115200);
    while (!Serial);  // รอ Serial พร้อมก่อนดำเนินการต่อ
    Serial.println("Starting Slave Node...");

    pinMode(LED_BUILTIN, OUTPUT);

    if (!BLE.begin()) {
        Serial.println("Failed to start BLE!");
        while (1);
    }

    BLE.setLocalName("XIAO nRF52840");
    BLE.setAdvertisedService(customService);
    customService.addCharacteristic(customCharacteristic);
    BLE.addService(customService);

    customCharacteristic.setValue("Hello world");

    Serial.print("Advertising Service UUID: ");
    Serial.println(SERVICE_UUID);
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
            Serial.println("Sending: Hello world updated!");
            customCharacteristic.setValue("Hello world updated!");
            delay(1000);
        }

        Serial.println("Disconnected! Restarting advertisement...");
        BLE.advertise();
    }
}

void flashRed() {
    Serial.println("Flashing RED LED...");
    while(!BLE.connected()) {
        analogWrite(LED_BUILTIN, 255);
        delay(500);
        analogWrite(LED_BUILTIN, 0);
        delay(500);
    }
}

void setGreen() {
    Serial.println("Connected! Turning GREEN LED ON...");
    analogWrite(LED_BUILTIN, LOW);
}
