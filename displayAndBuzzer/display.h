#ifndef display_h
#define display_h
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


enum displayState{
  TEXT,
  IDLE,
  NODE,
};

extern displayState currDisplayState;

extern const short SCREEN_WIDTH;
extern const short SCREEN_HEIGHT;
extern const short I2C_SDA;
extern const short I2C_SCL;
#define OLED_ADDR 0x3C

extern Adafruit_SSD1306 display;

extern short node;
extern const short route[];

void drawArrow32x16(int x, int y);

extern const short arrowX;
extern const short arrowY;
extern const short centre;
extern short scrollVertical;
extern const short uiInterval;

void updateUI(short node);
void startText(bool parked);

extern unsigned long lastDisplayUpdate;

extern short currentNode;
extern short nextNode;

void nodeUpdate();
void startNodeUpdate(short node);

extern bool parked;

void textUpdate();
void updateDisplay();

void wfcDisplay();

#endif