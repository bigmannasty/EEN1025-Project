#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic;
int route[] = {0, 2, 4, 3, 1, 5, 6};
int val = 0;

void setup() {
  Serial.begin(9600);
  BLEDevice::init("ESP32_S3_Server");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
  Serial.println("Waiting for a client to connect...");
}

void loop() {

  if (route[val] != 6)
  {
    
    pCharacteristic->setValue(route[val]);
    pCharacteristic->notify(); // Push the new value to the client
    Serial.print("Sent: ");
    Serial.println(route[val]);
    delay(3000); 
    val++; 
  }

  if (route[val] == 6) {
    pCharacteristic->setValue(route[val]);
    pCharacteristic->notify(); // Push the new value to the client
    Serial.print("Sent: ");
    Serial.println(route[val]);
    while (1) {}
  }
  
}
