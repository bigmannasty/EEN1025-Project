#include <Wire.h>
#include <display.h>

void setup() {
  Serial.begin(115200);

  // Start I2C on chosen pins
  Wire.begin(I2C_SDA, I2C_SCL);

  //Prevents ESP32 from working if display setup has failed
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }
  startNodeUpdate(2);
  //startText();
}

void loop() {
  updateDisplay();
}