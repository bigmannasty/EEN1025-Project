#include "pitches.h"



const int buzzer = 12;


void theme() {
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

void buzz(int node) {

  for (int i=0; i<node; i++)
  {
    delay(75);
    tone(buzzer, NOTE_C4);
    delay(75);
    noTone(buzzer);
  }
}

void obstacle() {

  tone(buzzer, NOTE_C5);
  delay(100);
  tone(buzzer, NOTE_C4);
  delay(100);
  tone(buzzer, NOTE_D4);
  delay(100);
  noTone(buzzer);
}

void start() {

  tone(buzzer, NOTE_D4);
  delay(100);
  tone(buzzer, NOTE_C4);
  delay(100);
  tone(buzzer, NOTE_C5);
  delay(100);
  noTone(buzzer);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzer, OUTPUT);
  start();
}

void loop() {
}
