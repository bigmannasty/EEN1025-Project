#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const short SCREEN_WIDTH = 128;
const short SCREEN_HEIGHT = 64;
#define OLED_ADDR 0x3C

const short I2C_SDA = 8;
const short I2C_SCL = 9;
const short button = 1;
short node = 0;

const short route[] = {0, 1, 3, 2, 5};

int buttonState = LOW;
int lastState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

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

void setup() {
  Serial.begin(115200);

  // Start I2C on chosen pins
  Wire.begin(I2C_SDA, I2C_SCL);
  pinMode(button, INPUT_PULLDOWN);
  //attachInterrupt(button, handleButtonISR, FALLING);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }
  display.clearDisplay();
}

const short arrowX = 48;
const short arrowY = 24;

const short centre = 8;

void scrollText(short route[],short node) {
  short scrollVertical = 8;
  display.setTextSize(7);
  display.setTextColor(SSD1306_WHITE);
  while (scrollVertical >= -64)
  {
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
  node++;
  scrollVertical = 64;
  while (scrollVertical >= 8)
  {
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
  delay(100);

}
void loop() {

  int reading = digitalRead(button);

  if (reading != lastState)
  {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay)
  {
      if (reading != buttonState)
      {
          buttonState = reading;
      }
  }
  if (node>3)
  {
    node = 0;
    scrollText(route, node);
  }
  lastState = reading;
  if (lastState == HIGH)
  {
    node++;
    scrollText(route, node);
  }
}
/*
TO DO:
1. Displays START when beginning route
  1a. Displays END when ending route and plays music from buzzer

2. At each node, plays buzzer sound relating to node number
*/

