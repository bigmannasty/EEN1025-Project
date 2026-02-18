#ifndef buzzer_h
#define buzzer_h

enum buzzerState{
  NOBUZZ,
  THEME,
  BUZZ,
  INIT,
};


extern buzzerState currBuzzerState;

extern const short buzzer;

extern int themeMelody[];
extern int themeDuration[];

extern short themeNoteCount;
extern short currentThemeNote;

extern unsigned long lastThemeUpdate;

void startTheme();
void playTheme();

extern short buzzCount;
extern short targetBuzz;

void playBuzz();
void startBuzz(int count);
void updateBuzzer();

extern int initTune[];
extern int initDuration[];

extern short currentInitNote;
extern short initNoteCount;
extern unsigned long lastInitUpdate;

void startInit();
void playInit();


#endif

