#include "pitches.h"

const int buzzer = 12;


enum buzzerState{
  IDLE,
  THEME,
  BUZZ,
  INIT,
  OBSTACLE
};

buzzerState currBuzzerState = IDLE;

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
int themeNoteCount = sizeof(themeMelody) / sizeof(themeMelody[0]);
int currentThemeNote = 0;
unsigned long lastThemeUpdate = 0;

void startTheme() {
  currentThemeNote = 0;
  lastThemeUpdate = millis();
  currBuzzerState = THEME; // This "switches" the logic in the loop
}

//Will continously play theme song
void playTheme() {
  unsigned long currentMillis = millis();

  // Check if it's time for the next note
  if (currentMillis - lastThemeUpdate >= themeDurations[currentThemeNote]) {
    lastThemeUpdate = currentMillis; // Reset the stopwatch
    currentThemeNote++;              // Move to the next note index

    // Check if the song is finished
    if (currentThemeNote >= themeNoteCount) {
      noTone(buzzer);
      currentThemeNote = 0;
      return;
    }

    // Play the current note (or silence if 0)
    if (themeMelody[currentThemeNote] == 0) {
      noTone(buzzer);
    } else {
      tone(buzzer, themeMelody[currentThemeNote]);
    }
  }
}

//Variables for buzz count and for no. times to buzz
int buzzCount = 0;   
int targetBuzz = 0; 

unsigned long lastBuzzTime = 0;
void playBuzz() {
  unsigned long currentMillis = millis();
  static bool buzzerOn = false;

  //Sets interval between buzzes
  int buzzInterval = 75;

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
        currBuzzerState = IDLE;
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
    case IDLE:
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

    case OBSTACLE:
      playObstacle();
      break;
  }
}

int obstacleTune[] = {0, NOTE_C5, NOTE_C4, NOTE_D2, 0};
int obstacleDuration[] = {100, 100, 100, 200, 100};

int currentObstacleNote;
unsigned long lastObstacleUpdate;

int obstacleNoteCount = sizeof(obstacleTune) / sizeof(obstacleTune[0]);
void startObstacle() {
  currentObstacleNote = 0;
  lastObstacleUpdate = millis();
  currBuzzerState = OBSTACLE; // This "switches" the logic in the loop
}

void playObstacle(){
  unsigned long currentMillis = millis();

  if (currentMillis - lastObstacleUpdate >= obstacleDuration[currentObstacleNote]) {
    lastObstacleUpdate = currentMillis; // Reset the stopwatch
    currentObstacleNote++;              // Move to the next note index

    // Check if the song is finished
    if (currentObstacleNote >= obstacleNoteCount) {
      noTone(buzzer);
      currentObstacleNote = 0;
      currBuzzerState = IDLE;
      return;
    }

    // Play the current note (or silence if 0)
    if (obstacleTune[currentObstacleNote] == 0) {
      noTone(buzzer);
    } else {
      tone(buzzer, obstacleTune[currentObstacleNote]);
    }
  }
}

int initTune[] = {0, NOTE_D4, NOTE_C4, NOTE_C5, 0};
int initDuration[] = {100, 100, 100, 100, 100};

int currentInitNote;
unsigned long lastInitUpdate;

int initNoteCount = sizeof(initTune) / sizeof(initTune[0]);
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
      currBuzzerState = IDLE;
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

void setup() {
  pinMode(buzzer, OUTPUT);
  //startBuzz(2);
  //startTheme();
  //startInit();
  //startObstacle();
}

void loop() {
  updateBuzzer();
}
