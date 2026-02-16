#include "pitches.h"

const int buzzer = 12;

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
bool playingTheme = false; // Set this to true to start the song

void playTheme() {
  if (!playingTheme) return;

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




void buzz(int node) {

  unsigned long currentMillis = millis();

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
  playingTheme = true;
}

void loop() {
  playTheme();
}
