#include "Bluetooth.h"


BLECharacteristic *pCharacteristic;
BLECharacteristic *pPhoneChar;

bool deviceConnected = false;


void MyServerCallbacks::onConnect(BLEServer* pServer) {
    deviceConnected = true;
    //Serial.println("");
    //Serial.println("Bluetooth Connected!");
    pServer->getAdvertising()->start();
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
                  
  pPhoneChar = pService->createCharacteristic(
                      CHAR_PHONE_UUID,
                      BLECharacteristic::PROPERTY_READ |
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
  pPhoneChar->addDescriptor(new BLE2902());

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

//Could perform some logic so that if pos == 5, phone receives a parking notif instead of node number?
//String should be kept to below 23 bytes, otherwise phone will truncate msg
void sendToPhone(int pos=0)
{
  if (pos == 5)
  {
    String phoneMsg = "Mobot Parking...";
    pPhoneChar->setValue(phoneMsg.c_str());
    pPhoneChar->notify();
  }
  else
  {
    String phoneMsg = "Next node:" + String(pos);
    pPhoneChar->setValue(phoneMsg.c_str());
    pPhoneChar->notify();
  }
}
