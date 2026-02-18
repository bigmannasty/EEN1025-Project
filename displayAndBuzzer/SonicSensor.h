#ifndef SonicSensor_h
#define SonicSensor_h

extern const short trigPin;
extern const short echoPin;

void pinSetup();

void trigPulse();

int distanceSense();


#endif