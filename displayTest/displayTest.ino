#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const short SCREEN_WIDTH = 128;
const short SCREEN_HEIGHT = 64;
#define OLED_ADDR 0x3C
const short I2C_SDA = 8;
const short I2C_SCL = 9;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

const short button = 1;

short node = -1;
const short route[] = {0, 1, 3, 2, 5};

int buttonState = LOW;
int lastState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

//Function to Draw arrow on display
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
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.print("START");
  display.display();
}

const short arrowX = 48;
const short arrowY = 24;

const short centre = 8;

void updateUI(short node) {
  short scrollVertical = 8;
  while (scrollVertical >= -64)
  {
    display.setTextSize(7);
    drawArrow32x16(arrowX, arrowY);
    display.setCursor(4, scrollVertical);
    display.print(node);
    display.setCursor(86, scrollVertical);
    display.print(node);
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
    display.print(node);
    display.setCursor(86, scrollVertical);
    display.print(node);
    display.display();
    delay(1);
    scrollVertical -= 4;     
    display.clearDisplay();
  }
}

void loop() {

  for (int i = 0; i < 5; i++)
  {
    updateUI(i);
    delay(150);
  }
}
/*
TO DO:
1a. Displays PARK when ending route and plays music from buzzer
1b. Once parked, display END

2. At each node, plays buzzer sound relating to node number
*/

