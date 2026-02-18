#include <display.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>



const short SCREEN_WIDTH = 128;
const short SCREEN_HEIGHT = 64;

//Pin Configuration for OLED
const short I2C_SDA = 8; //Serial Data
const short I2C_SCL = 9; //Serial Clock


//Constructs an object to talk to the screen
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

displayState currDisplayState = TEXT;

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

//
const short arrowX = 48;
const short arrowY = 24;

short currentNode = 0;
short nextNode = 0;

void nodeUpdate() {
	
  //Draws arrow
  display.clearDisplay();
  drawArrow32x16(arrowX, arrowY);
	
  //Draws Static Text
  display.setTextSize(7);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, 8);
  display.print(currentNode);
  display.setCursor(86, 8);
  display.print(nextNode);
  display.display();

  currentNode = nextNode;
  currDisplayState = IDLE;
}

//Sets nextNode and sets state for updateDisplay()
void startNodeUpdate(short node) {
  nextNode = node;
  currDisplayState = NODE;
}

//Variable & Function to change text on screen from "START" -> "PARK"
bool parked = false;
void textUpdate() {
  //Will display start text
  if (parked == false)
  {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(4);
    display.setCursor(0, 16);
    display.print("START");
    display.display();
  }
  else
  //Will display park text
  {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(5);
    display.setCursor(0, 16);
    display.print("PARK");
    display.display();
  }

}
//Given value parameter, will set parked and set state
void startText(bool value) {
  parked = value;
  currDisplayState = TEXT; // FORCE the state to TEXT so updateDisplay() sees it
  textUpdate();            // Update immediately
}

void wfcDisplay() {
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.print("WFC");
  
  // Simple animated loading bar or dots based on millis()
  int dots = (millis() / 500) % 4; 
  for(int i=0; i<dots; i++) display.print(".");
  display.display();
}


void updateDisplay() {
  switch(currDisplayState)
  {
    case IDLE:
      break;
    case NODE:
      nodeUpdate();
      break;
    case TEXT:
      textUpdate();
      break;
  }
}
