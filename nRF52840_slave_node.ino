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
    while (!Serial);
    Serial.println("Starting Slave Node...");

    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    
    setRed(); // เริ่มต้นให้ไฟแดงติดก่อน

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

    flashRed(); // ไฟแดงกระพริบขณะโฆษณา
}

void loop() {
    BLEDevice central = BLE.central();

    if (central) {
        Serial.print("Connected to: ");
        Serial.println(central.address());
        setGreen(); // ไฟเขียวติดเมื่อเชื่อมต่อแล้ว

        while (central.connected()) {
            Serial.println("Sending: Hello world updated!");
            customCharacteristic.setValue("Hello world updated!");
            delay(1000);
        }

        Serial.println("Disconnected! Restarting advertisement...");
        setRed(); // ไฟแดงติดค้างเมื่อหลุดการเชื่อมต่อ
        BLE.advertise();
    }
}

// ไฟแดงกระพริบ 6 ครั้งตอนเริ่มต้น
void flashRed() {
    Serial.println("Flashing RED LED...");
    for (int i = 0; i < 6; i++) {
        digitalWrite(LED_RED, HIGH);
        delay(300);
        digitalWrite(LED_RED, LOW);
        delay(300);
    }
}

// ไฟเขียวติดเมื่อเชื่อมต่อสำเร็จ
void setGreen() {
    Serial.println("Connected! Turning GREEN LED ON...");
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
}

// ไฟแดงติดค้างเมื่อรอการเชื่อมต่อใหม่
void setRed() {
    Serial.println("Disconnected! Turning RED LED ON...");
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH);
}
