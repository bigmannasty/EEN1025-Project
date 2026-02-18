#include <Wire.h>
#include <BLEDevice.h>
#include <buzzer.h>
#include <display.h>
#include "pitches.h"

// Add a global state variable for bluetooth logic
enum SystemState { 
  WAITING_FOR_CONN,
  CONNECTING,
  CONNECTED
};
SystemState currentState = WAITING_FOR_CONN;


//Service UUID tells which device to connect to
static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
//Char UUID what to do when connected
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
bool doConnect = false;


//class will search for devices advertising same service UUID, then will stop scanning, save device info and moves to connecting to device
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) {
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      startInit();
      textUpdate();
    }
  }
};

// Callback for when the server sends a notification
static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                            uint8_t* pData, size_t length, bool isNotify) {
  
    int value = *pData; // Simple cast for a single byte/integer
    Serial.print("Received Value: ");
    Serial.println(value);

    //Logic for nextPosition node value 
    if (value != 0)
    {
      startBuzz(value);
      startNodeUpdate(value);
      nextNode = value;
    }
    //When nextPos is 5 (going to park), play theme
    if (value == 5)
    {
      startTheme();
    }
    //When parked (received 5 twice), display PARK and stop buzzer
    else if (value == 5 && currentNode == 5)
    {
      startText(true);
      currBuzzerState = NOBUZZ;
    }
}

void setup() {
  Serial.begin(115200);

  // Start I2C on chosen pins
  Wire.begin(I2C_SDA, I2C_SCL);

  //If display fails, freeze ESP until hard reset
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }
  /*
  Initialise connection, start scanning, and start connecting 
  */
  BLEDevice::init("DCU-SAUR");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  //If successfully found connection,
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
        }
      }
    }
    doConnect = false; 
  }

  // UI Rendering Logic
  if (currentState == WAITING_FOR_CONN || currentState == CONNECTING) {
    wfcDisplay();
  } else {
    updateDisplay(); // Your normal "Connected" GUI
  }

  updateBuzzer();
}
