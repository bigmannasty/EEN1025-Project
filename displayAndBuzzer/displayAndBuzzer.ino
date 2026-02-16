#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include "pitches.h"

//OLED Display Variables
#define OLED_ADDR 0x3C
const short SCREEN_WIDTH = 128;
const short SCREEN_HEIGHT = 64;
const short I2C_SDA = 8;
const short I2C_SCL = 9;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);


static BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
bool doConnect = false;

//Pin Configurations
const short buzzer = 12;

//Variables for Display
const short arrowX = 48;
const short arrowY = 24;
const short centre = 8;

short currNode = 0;

//Function to draw arrow on display
void drawArrow32x16(int x, int y) {
  const short width = 32;
  const short height = 16;

  const short shaftLength = 20;      // body length
  const short shaftThickness = 6;    // thickness of shaft

  const short centerY = y + height / 2;

  // Shaft (rectangle)
  display.fillRect(x, centerY - shaftThickness / 2,
                   shaftLength, shaftThickness,
                   SSD1306_WHITE);

  // Arrow head (triangle)
  display.fillTriangle(
    x + shaftLength, y,              // top back of head
    x + shaftLength, y + height,     // bottom back of head
    x + width, centerY,              // tip
    SSD1306_WHITE
  );
}


void updateUI(short currNode, short nextNode)
{
  buzz(currNode);
  display.clearDisplay();
  display.setTextSize(7);
  drawArrow32x16(arrowX, arrowY);
  display.setCursor(4, 8);
  display.print(currNode);
  display.setCursor(86, 8);
  display.print(nextNode);
  display.display();
}
/*
void updateUI(short currNode, short nextNode)
{
  buzz(currNode);
  short scrollVertical = 8;
  while (scrollVertical >= -64)
  {
    display.setTextSize(7);
    drawArrow32x16(arrowX, arrowY);
    display.setCursor(4, scrollVertical);
    display.print(currNode);
    display.setCursor(86, scrollVertical);
    display.print(nextNode);
    display.display();
    delay(1);
    scrollVertical -= 4;     
    display.clearDisplay();
  }
  scrollVertical = 64;
  while (scrollVertical >= 8)
  {
    drawArrow32x16(arrowX, arrowY);
    display.setCursor(4, scrollVertical);
    display.print(currNode);
    display.setCursor(86, scrollVertical);
    display.print(nextNode);
    display.display();
    delay(1);
    scrollVertical -= 4;     
    display.clearDisplay();
  }

}
*/

void endTextandTheme()
{
  //Jurrasic Park Theme
  tone(buzzer, NOTE_C5);  //C
  delay(250);
  tone(buzzer, NOTE_B5);  //B
  delay(250);
  tone(buzzer, NOTE_C5);  //C
  delay(500);
  tone(buzzer, NOTE_G4);  //G
  delay(500);
  tone(buzzer, NOTE_F4);  //f
  delay(500);

  noTone(buzzer);
  delay(100);

  tone(buzzer, NOTE_C5);  //C
  delay(250);
  tone(buzzer, NOTE_B5);  //B
  delay(250);
  tone(buzzer, NOTE_C5);  //C
  delay(500);
  tone(buzzer, NOTE_G4);  //G
  delay(500);
  tone(buzzer, NOTE_F4);  //f
  delay(500);

  noTone(buzzer);
  delay(100);

  tone(buzzer, NOTE_C5);  //C
  delay(250);
  tone(buzzer, NOTE_B5);  //B
  delay(250);
  noTone(buzzer);
  delay(50);
  tone(buzzer, NOTE_B5);  //B
  delay(250);
  noTone(buzzer);
  delay(50);
  tone(buzzer, NOTE_C5);  //C
  delay(750);

  noTone(buzzer);
  tone(buzzer, NOTE_G4);  //G
  delay(500);
  tone(buzzer, NOTE_C4);  //low c
  delay(500);
  tone(buzzer, NOTE_AS4);  //Bflat
  delay(1250);
  noTone(buzzer);
  delay(500);

  noTone(buzzer);
  //Display
  display.clearDisplay();
  display.setTextSize(5);
  display.setCursor(0, 16);
  display.print("PARK");
  display.display();
}

void startTextandBuzz()
{
  //Display
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.print("START");
  display.display();

  //Buzzer
  tone(buzzer, NOTE_D4);
  delay(100);
  tone(buzzer, NOTE_C4);
  delay(100);
  tone(buzzer, NOTE_A5);
  delay(100);
  noTone(buzzer);
}

//Function to animate numbers on display
/*
void updateUI(const short route[],short node) {
  buzz(node);
  short scrollVertical = 8;
  while (scrollVertical >= -64)
  {
    display.setTextSize(7);
    drawArrow32x16(arrowX, arrowY);
    display.setCursor(4, scrollVertical);
    display.print(route[node]);
    display.setCursor(86, scrollVertical);
    display.print(route[node+1]);
    display.display();
    delay(5);
    scrollVertical -= 2;     
    display.clearDisplay();
  }
  scrollVertical = 64;
  while (scrollVertical >= 8)
  {
    drawArrow32x16(arrowX, arrowY);
    display.setCursor(4, scrollVertical);
    display.print(route[node+1]);
    display.setCursor(86, scrollVertical);
    display.print(route[node+2]);
    display.display();
    delay(5);
    scrollVertical -= 2;     
    display.clearDisplay();
  }
}
*/

//Function that buzzes for each node passed
void buzz(int nodeIndex) {

  for (int i=0; i<nodeIndex; i++)
  {
    delay(75);
    tone(buzzer, NOTE_A3);
    delay(75);
    noTone(buzzer);
  }
}

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
  
    int value = *pData; // Simple cast for a single byte/integer
    Serial.print("Received Value: ");
    Serial.println(value);
    if (value != 0)
    {
      updateUI(currNode, value);
      currNode = value;
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
  startTextandBuzz();
  BLEDevice::init("DCU-SAUR");
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void loop() {
  if (doConnect) {
    BLEClient* pClient = BLEDevice::createClient();
    pClient->connect(myDevice);
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService != nullptr) {
      pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
      if (pRemoteCharacteristic != nullptr && pRemoteCharacteristic->canNotify()) {
        pRemoteCharacteristic->registerForNotify(notifyCallback);
      }
    }
    doConnect = false;
  }
  delay(1000);
  
}

/*
TO DO:
Implement stop watch instead of delays
implement GUI for WFC
Seperate the end text and theme, put in a parked flag so song continuously plays while going to park
*/