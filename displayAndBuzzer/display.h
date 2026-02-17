#ifndef display_h
#define display_h
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


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
extern const int uiInterval;

void updateUI(short node);

extern unsigned long lastDisplayUpdate;

extern short currentNode;
extern short nextNode;
void nodeUpdate();
void startNodeUpdate(short node);

extern bool parked;

void textUpdate();
void updateDisplay();

#endif