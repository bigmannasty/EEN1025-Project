#ifndef Motor_h
#define Motor_h


extern int currentDir;

extern const int speedR;
extern const int speedL;

extern int lineValue[5];
extern const int lineSensePin[5];
extern const int WHITE_THRESHOLD;

extern bool lineDetected;

void initMotor();

void motorDrive(int leftPWM, int rightPWM);

void motorDir(int dir);

void motorTurn(int dir);

void motor180();

#endif