#ifndef buzzer_h
#define buzzer_h

enum buzzerState{
  NOBUZZ,
  THEME,
  BUZZ,
  INIT,
  OBSTACLE
};


extern buzzerState currBuzzerState;

extern const short buzzer;

extern int themeMelody[];
extern int themeDuration[];

extern int themeNoteCount;
extern int currentThemeNote;

extern unsigned long lastThemeUpdate;

void startTheme();
void playTheme();

extern int buzzCount;
extern int targetBuzz;

void playBuzz();
void startBuzz(int count);
void updateBuzzer();

extern int obstacleTune[];
extern int obstacleDuration[];

extern int currentObstacleNote;
extern unsigned long lastObstacleUpdate;

extern int obstacleNoteCount;

void startObstacle();
void playObstacle();

extern int initTune[];
extern int initDuration[];

extern int currentInitNote;
extern unsigned long lastInitUpdate;

extern int initNoteCount;
void startInit();
void playInit();


#endif

