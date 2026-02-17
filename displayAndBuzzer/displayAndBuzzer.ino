#include <Wire.h>
#include <BLEDevice.h>
#include <buzzer.h>
#include <display.h>
#include "pitches.h"

/*
TO DO:
Make two different states for animation (one for scrolling up, other for resetting pos, updating nodes and scrolling up)
*/



// Add a global state variable
enum SystemState { 
  WAITING_FOR_CONN,
  CONNECTING,
  CONNECTED
};
SystemState currentState = WAITING_FOR_CONN;

static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
bool doConnect = false;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

// Callback for when the server sends a notification
static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                            uint8_t* pData, size_t length, bool isNotify) {
  
    int node = *pData; // Simple cast for a single byte/integer
    
    Serial.println(node);
    if (node != 0)
    {
      startBuzz(node);
      startNodeUpdate(node);
    }
    if (node == 5)
    {
      startTheme();
    }
    if (node == 5 && currentNode == 5)
    {
      parked = true;
      currDisplayState = TEXT;
      currBuzzerState = NOBUZZ;
    }
}

void setup() {
  Serial.begin(115200);

  // Start I2C on chosen pins
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }
  textUpdate();
  startInit();
  BLEDevice::init("DCU-SAUR");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
  display.clearDisplay();
}

void loop() {
  if (doConnect) {
    currentState = CONNECTING;
    BLEClient* pClient = BLEDevice::createClient();
    
    if (pClient->connect(myDevice)) {
      BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
      if (pRemoteService != nullptr) {
        pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
        if (pRemoteCharacteristic != nullptr && pRemoteCharacteristic->canNotify()) {
          pRemoteCharacteristic->registerForNotify(notifyCallback);
          currentState = CONNECTED; // Successfully hooked up
          currDisplayState = TEXT;
        }
      }
    }
    doConnect = false; 
  }

  // UI Rendering Logic
  if (currentState == WAITING_FOR_CONN || currentState == CONNECTING) {
    wfcDisplay();
  } else {
    updateDisplay();
    }
  
  
  updateBuzzer();
}