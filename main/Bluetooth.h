#ifndef Bluetooth_h
#define Bluetooth_h

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHAR_PHONE_UUID     "d2109600-b6a3-48b4-8395-927943715d23"

extern bool deviceConnected;

class MyServerCallbacks: public BLEServerCallbacks {
public:
    void onConnect(BLEServer* pServer) override;
    void onDisconnect(BLEServer* pServer) override;
};

void bluetoothSetup();

void sendNode(int pos);
void sendToPhone(int pos);


#endif
