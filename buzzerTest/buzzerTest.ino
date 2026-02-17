#include <buzzer.h>

void setup() {
  pinMode(buzzer, OUTPUT);
  startBuzz(2);
  //startTheme();
  //startInit();
  //startObstacle();
}

void loop() {
  updateBuzzer();
}
