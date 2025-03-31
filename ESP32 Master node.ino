#include <ArduinoBLE.h>  // ย้ายไปไว้ที่บรรทัดแรกสุดของโค้ด

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define CHARACTERISTIC_UUID "D90A7C02-9B21-4243-8372-3E523FA7978B"

BLEDevice peripheral;
BLECharacteristic characteristic;
bool isConnected = false;

void TaskBLEScan(void *pvParameters) {
    Serial.println("Scanning for BLE device...");
    BLE.scan();  // Start scanning
    
    while (!isConnected) {
        BLEDevice foundDevice = BLE.available();
        if (foundDevice) {
            Serial.println("Found BLE Device: " + String(foundDevice.address()));
            BLE.stopScan();  // Stop scan when device found
            
            Serial.println("Listing available services:");
            for (int i = 0; i < foundDevice.serviceCount(); i++) {
                BLEService service = foundDevice.service(i);
                Serial.println("Service UUID: " + String(service.uuid()));
            }
            
            Serial.println("Trying to connect...");
            for (int attempt = 0; attempt < 5 && !isConnected; attempt++) {
                Serial.print("Attempt ");
                Serial.print(attempt + 1);
                Serial.println(" to connect...");
                
                if (foundDevice.connect()) {
                    Serial.println("Connected to device!");
                    peripheral = foundDevice;
                    characteristic = peripheral.characteristic(CHARACTERISTIC_UUID);
                    isConnected = peripheral.connected();
                    
                    if (isConnected) {
                        Serial.println("Successfully connected!");
                        if (characteristic) {
                            Serial.println("Found characteristic!");
                        } else {
                            Serial.println("Characteristic not found!");
                        }
                    } else {
                        Serial.println("Connection lost immediately, retrying...");
                    }
                } else {
                    Serial.println("Connection failed. Retrying...");
                    delay(2000);
                }
            }
        }
        delay(500);
    }
    Serial.println("BLE connection established, ending scan task.");
    vTaskDelete(NULL);
}


void setup() {
    Serial.begin(115200);
    setCpuFrequencyMhz(80);
    Serial.println("Starting BLE Client...");

    if (!BLE.begin()) {
        Serial.println("Failed to initialize BLE!");
        while (1);
    }

    xTaskCreatePinnedToCore(
        TaskBLEScan,
        "BLEScan",
        4096,
        NULL,
        1,
        NULL,
        0
    );
}

void loop() {
    if (isConnected && peripheral.connected() && characteristic) {
        if (characteristic.canRead()) {
            char buffer[50] = {0};
            int length = characteristic.readValue(buffer, sizeof(buffer) - 1);
            buffer[length] = '\0';
            
            Serial.print("Received: ");
            Serial.println(buffer);
        }
    } else if (isConnected && !peripheral.connected()) {
        Serial.println("Disconnected! Restarting scan...");
        isConnected = false;
        BLE.begin();
        xTaskCreatePinnedToCore(
            TaskBLEScan,
            "BLEScan",
            4096,
            NULL,
            1,
            NULL,
            0
        );
    }
    delay(1000);
}
