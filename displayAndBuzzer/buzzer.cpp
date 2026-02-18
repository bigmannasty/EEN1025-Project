#include <buzzer.h>
#include <Arduino.h>
#include "pitches.h"

//Initialises state variable to not buzz
buzzerState currBuzzerState = NOBUZZ;

//Pin Configuration for buzzer
const short buzzer = 12;


// Use 0 to represent noTone()
// I included 0 at the start of the arrays as the playTheme() function skips past the first note, I basically moved first note to second note.
int themeMelody[] = {
  0,                                              
  NOTE_C5, NOTE_B5, NOTE_C5, NOTE_G4, NOTE_F4, 0,       // Phrase 1
  NOTE_C5, NOTE_B5, NOTE_C5, NOTE_G4, NOTE_F4, 0,       // Phrase 2
  NOTE_C5, NOTE_B5, 0, NOTE_B5, 0, NOTE_C5, 0,          // Phrase 3
  NOTE_G4, NOTE_C4, NOTE_AS4, 0                         // Ending
};

int themeDurations[] = {
  0,                                                    
  250, 250, 500, 500, 500, 100,                         // Phrase 1
  250, 250, 500, 500, 500, 100,                         // Phrase 2
  250, 250, 50, 250, 50, 750, 1,                        // Phrase 3 (1ms dummy rest)
  500, 500, 1250, 500                                   // Ending
};
short themeNoteCount = sizeof(themeMelody) / sizeof(themeMelody[0]);
short currentThemeNote = 0;
unsigned long lastThemeUpdate = 0;

void startTheme() {
  lastThemeUpdate = millis();
  currBuzzerState = THEME;
}

//Will continously play theme song
void playTheme() {
  unsigned long currentMillis = millis();

  // Check if it's time for the next note
  if (currentMillis - lastThemeUpdate >= themeDurations[currentThemeNote]) {
    //Resets stopwatch and moves to next note
    lastThemeUpdate = currentMillis;
    currentThemeNote++;             

    // If song has finished, restart to beginning
    if (currentThemeNote >= themeNoteCount) {
      noTone(buzzer);
      currentThemeNote = 0;
      return;
    }

    // Play the current note (or nothing if 0)
    if (themeMelody[currentThemeNote] == 0) {
      noTone(buzzer);
    } else {
      tone(buzzer, themeMelody[currentThemeNote]);
    }
  }
}

//Variables for buzz count and for no. times to buzz
short buzzCount = 0;   
short targetBuzz = 0; 

unsigned long lastBuzzTime = 0;
void playBuzz() {
  unsigned long currentMillis = millis();
  static bool buzzerOn = false;

  //Sets interval between buzzes
  short buzzInterval = 75;

  if (currentMillis - lastBuzzTime >= buzzInterval) {
    lastBuzzTime = currentMillis;

    if (!buzzerOn) {
      // START THE buzz
      tone(buzzer, NOTE_C5);
      buzzerOn = true;
    } else {
      // STOP THE buzz
      noTone(buzzer);
      buzzerOn = false;
      buzzCount++; // We finished one full cycle

      // Check if we hit the limit
      if (buzzCount >= targetBuzz) {
        currBuzzerState = NOBUZZ;
      }
    }
  }
}

//Sets number of time to buzz and resets variables from each node no. buzz
void startBuzz(int count) {
  targetBuzz = count;
  buzzCount = 0;
  lastBuzzTime = millis();
  currBuzzerState = BUZZ;
}

void updateBuzzer() {
  switch (currBuzzerState)
  {
    case NOBUZZ:
      noTone(buzzer);
      break;

    case THEME:
      playTheme();
      break;
    
    case BUZZ:
      playBuzz();
      break;

    case INIT:
      playInit();
      break;
  }
}

int initTune[] = {0, NOTE_D4, NOTE_C4, NOTE_C5, 0};
int initDuration[] = {100, 100, 100, 100, 100};

short currentInitNote;
unsigned long lastInitUpdate;

short initNoteCount = sizeof(initTune) / sizeof(initTune[0]);
void startInit() {
  currentInitNote = 0;
  lastInitUpdate = millis();
  currBuzzerState = INIT; // This "switches" the logic in the loop
}

void playInit(){
  unsigned long currentMillis = millis();

  if (currentMillis - lastInitUpdate >= initDuration[currentInitNote]) {
    lastInitUpdate = currentMillis; // Reset the stopwatch
    currentInitNote++;              // Move to the next note index

    // Check if the song is finished
    if (currentInitNote >= initNoteCount) {
      noTone(buzzer);
      currentInitNote = 0;
      currBuzzerState = NOBUZZ;
      return;
    }

    // Play the current note (or silence if 0)
    if (initTune[currentInitNote] == 0) {
      noTone(buzzer);
    } else {
      tone(buzzer, initTune[currentInitNote]);
    }
  }
}