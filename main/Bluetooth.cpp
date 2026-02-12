#include "Bluetooth.h"


BLECharacteristic *pCharacteristic;

bool deviceConnected = false;


void MyServerCallbacks::onConnect(BLEServer* pServer) {
    deviceConnected = true;
    //Serial.println("");
    //Serial.println("Bluetooth Connected!");
}

void MyServerCallbacks::onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    //Serial.println("");
    //Serial.println("Phone Disconnected. Restarting Advertising...");
    pServer->getAdvertising()->start();
}

void bluetoothSetup() {
  BLEDevice::init("ESP32_S3_Server");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pServer->setCallbacks(new MyServerCallbacks());

  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
}

void sendNode(int pos) {
  pCharacteristic->setValue(pos);
  pCharacteristic->notify(); // Push the new value to the client
  /*
  Serial.println("");
  Serial.print("Bluetooth Sent: ");
  Serial.println(pos);
  Serial.println("");
  */
}
