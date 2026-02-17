#ifndef SonicSensor_h
#define SonicSensor_h

extern const int trigPin;
extern const int echoPin;

void pinSetup();

void trigPulse();

int distanceSense();


#endif