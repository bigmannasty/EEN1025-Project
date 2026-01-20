/*
NOTES:

1. Core 0 runs WiFI and BLE, use Core 1 for more heavy tasks

*/



#include <Adafruit_NeoPixel.h>
#define PIN 38 // Or 38, depending on your S3 DevKit version!
#define NUMPIXELS 1
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pixels.begin();
  Serial.begin (115200);

  xTaskCreatePinnedToCore (
    loop2,     // Function to implement the task
    "loop2",   // Name of the task
    1000,      // Stack size in bytes
    NULL,      // Task input parameter
    0,         // Priority of the task
    NULL,      // Task handle.
    1          // Core where the task should run
  );
}

void loop () {
  pixels.clear();
  pixels.show();
  delay(DELAYVAL);
  for(int i=0; i<NUMPIXELS; i++) {
   pixels.setPixelColor(i, pixels.Color(0, 5, 0));
   pixels.show();
   delay(DELAYVAL);
   }
 }

// the loop2 function also runs forver but as a parallel task
void loop2 (void* pvParameters) {
  while (1) {
    Serial.print ("Hello");
    delay (500); // wait for half a second
    Serial.println (" World");
    delay (500); // wait for half a second
  }
}
