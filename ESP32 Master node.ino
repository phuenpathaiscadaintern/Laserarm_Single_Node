#include <ArduinoBLE.h>

#define SERVICE_UUID        "87E01439-99BE-45AA-9410-DB4D3F23EA99"
#define UUID_CHARACTERISTIC "12345678-1234-5678-1234-56789ABCDEF0"

BLEDevice peripheral;
BLECharacteristic characteristic;
BLECharacteristic uuidCharacteristic;
bool isConnected = false;
String targetCharacteristicUUID = "";

void TaskBLEScan(void *pvParameters) {
    Serial.println("Scanning for BLE device...");
    BLE.scan();  // Start scanning
    
    unsigned long startTime = millis();
    while (!isConnected && millis() - startTime < 20000) {
        BLEDevice foundDevice = BLE.available();
        if (foundDevice) {
            Serial.println("Found BLE Device: " + String(foundDevice.address()));
            BLE.stopScan();
            
            Serial.println("Trying to connect...");
            if (foundDevice.connect()) {
                Serial.println("Connected to device!");
                peripheral = foundDevice;
                isConnected = true;

                Serial.println("Discovering services...");
                if (peripheral.discoverAttributes()) {
                    Serial.println("Services discovered!");
                    
                    Serial.println("Reading UUID Characteristic...");
                    for (int i = 0; i < peripheral.characteristicCount(); i++) {
                        BLECharacteristic chara = peripheral.characteristic(i);
                        if (chara.uuid() == UUID_CHARACTERISTIC) {
                            char buffer[50] = {0};  
                            int length = chara.readValue(buffer, sizeof(buffer) - 1);
                            buffer[length] = '\0'; 
                            targetCharacteristicUUID = String(buffer);
                            Serial.println("Received Target Characteristic UUID: " + targetCharacteristicUUID);
                        }
                    }

                    Serial.println("Listing available Characteristics:");
                    for (int i = 0; i < peripheral.characteristicCount(); i++) {
                        BLECharacteristic chara = peripheral.characteristic(i);
                        Serial.println("Characteristic UUID: " + String(chara.uuid()));

                        if (String(chara.uuid()) == targetCharacteristicUUID) {
                            characteristic = chara;
                            Serial.println("✅ Found target characteristic!");
                        }
                    }

                    if (!characteristic) {
                        Serial.println("❌ Target Characteristic not found!");
                    }
                } else {
                    Serial.println("Failed to discover services.");
                }
            } else {
                Serial.println("Connection failed. Retrying...");
                delay(2000);
            }
        }
        vTaskDelay(10);  // เพิ่ม delay ให้กับ watchdog timer
    }
    Serial.println("BLE connection established.");
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
        8192,  // เพิ่ม Stack Size
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
            8192,  // เพิ่ม Stack Size
            NULL,
            1,
            NULL,
            0
        );
    }
    delay(1000);  // ป้องกัน Watchdog
}
