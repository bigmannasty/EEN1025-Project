#include <SonicSensor.h>
#include <Arduino.h>

const int trigPin = 16;
const int echoPin = 17;

void pinSetup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
}

void trigPulse() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

int distanceSense() {
  return pulseIn(echoPin, HIGH, 30000);
}