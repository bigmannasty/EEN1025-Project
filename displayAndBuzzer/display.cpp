#include <display.h>
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


const short SCREEN_WIDTH = 128;
const short SCREEN_HEIGHT = 64;
const short I2C_SDA = 8;
const short I2C_SCL = 9;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

displayState currDisplayState = TEXT;

short node = -1;
const short route[] = {0, 1, 3, 2, 5};

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

const short arrowX = 48;
const short arrowY = 24;
const short centre = 8;
short scrollVertical = 8;
const short uiInterval = 1;

unsigned long lastDisplayUpdate = 0;
short currentNode = 0;
short nextNode = 0;

void nodeUpdate() {
	
  //Draws arrow
	
  display.clearDisplay();
	
  drawArrow32x16(arrowX, arrowY);
	

	
  //Draws Static Text
	
  display.setTextSize(7);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, scrollVertical);
  display.print(currentNode);
  display.setCursor(86, scrollVertical);
  display.print(nextNode);
  display.display();
  currentNode = nextNode;
	
  currDisplayState = IDLE;
}

/*
void nodeUpdate() {
  unsigned long currentMillis = millis();

  //Check if enough time has elapsed to update animation
  if (currentMillis - lastDisplayUpdate >= uiInterval)
  {
    lastDisplayUpdate = currentMillis;

    //Draws arrow
    display.clearDisplay();
    drawArrow32x16(arrowX, arrowY);

    //Draws Scrolling Text
    display.setTextSize(7);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(4, scrollVertical);
    display.print(currentNode);
    display.setCursor(86, scrollVertical);
    display.print(nextNode);
    display.display();

    //Decrement y pos of node numbers
    scrollVertical -=4;

    //If scrolled to the top, jump to the bottom
    if (scrollVertical < -64 && scrollVertical > -100) {
      scrollVertical = 64;
    }

    if (scrollVertical == 8) {
      currDisplayState = IDLE;
      currentNode = nextNode;
    }
  }
}
*/
void startNodeUpdate(short node) {
  nextNode = node;
  scrollVertical = 4;
  lastDisplayUpdate = millis();
  currDisplayState = NODE;
}

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
  {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(5);
    display.setCursor(0, 16);
    display.print("PARK");
    display.display();
  }
}
void startText(bool value) {
  if (value == true)
  {
    parked = true;
    currDisplayState = TEXT;
  }
  else
  {
    currDisplayState = TEXT;
  }
}

void startWFC() {

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
      display.clearDisplay();
      break;
    case NODE:
      nodeUpdate();
      break;
    case TEXT:
      textUpdate();
      break;
  }
}
