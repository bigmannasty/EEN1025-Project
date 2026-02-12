#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

bool deviceConnected = false;

// Handles Connection and Disconnection
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      Serial.println("Phone Connected!");
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      Serial.println("Phone Disconnected. Restarting Advertising...");
      pServer->getAdvertising()->start(); // Allow other devices to find it again
    }
};


class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      // Use the native BLE pointer to get the data
      uint8_t* pData = pCharacteristic->getData();
      size_t len = pCharacteristic->getLength();

      if (len > 0) {
        // Convert the raw data to a standard string
        std::string value((char*)pData, len);

        Serial.println("*********");
        Serial.print("New Value Received: ");
        
        // Convert string to integer
        int receivedInt = atoi(value.c_str());
        
        Serial.println(receivedInt);
        Serial.println("*********");
      }
    }
};
void setup() {
  Serial.begin(115200);
  BLEDevice::init("DCU-SAUR");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks()); // Set the server callbacks

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());
  
  // Add a Descriptor (required by some phones for stable connections)
  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  
  // These specific intervals help iOS and Android "trust" the connection
  pAdvertising->setMinPreferred(0x06);  
  pAdvertising->setMaxPreferred(0x12);
  
  BLEDevice::startAdvertising();
  Serial.println("Waiting for connection...");
}

void loop() {
  // Keep the loop empty
}