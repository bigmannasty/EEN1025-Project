#include "pitches.h"
const int buzzer = 12;

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

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
}
