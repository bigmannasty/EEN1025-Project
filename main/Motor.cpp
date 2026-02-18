#include <Motor.h>
#include <Arduino.h>
//#include "Sensors.h"

//Initialisation of pins to control motor
const int motor1PWM = 37;
const int motor1Phase = 38;
const int motor2PWM = 39;
const int motor2Phase = 40;


//Fastest speed and turn_gain mobot can have before errors
const int speedR = 255;
const int speedL = int(speedR * 0.95);


int currentDir = 1; // 0 CW    1 ACW


//Initialises 0s for LineSensorValues
int lineValue[5];
const int lineSensePin[5] = { 4, 5, 6, 7, 15 };
const int WHITE_THRESHOLD = 800;

bool lineDetected = false;



// Wheel initialisation
void initMotor() {
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor1Phase, OUTPUT);
  pinMode(motor2PWM, OUTPUT);
  pinMode(motor2Phase, OUTPUT);
}


//drive each motor at controlled pwm
void motorDrive(int leftPWM, int rightPWM) {
  leftPWM = constrain(leftPWM, 0, 255);
  rightPWM = constrain(rightPWM, 0, 255);

  analogWrite(motor1PWM, leftPWM);
  analogWrite(motor2PWM, rightPWM);
}


//set direction of motors
//dir:0 cw, 1 anti-cw
void motorDir(int dir) {
  if (dir == 0) {
    digitalWrite(motor1Phase, LOW);
    digitalWrite(motor2Phase, HIGH);
  } else {
    digitalWrite(motor1Phase, HIGH);
    digitalWrite(motor2Phase, LOW);
  }
}


//0 right   1 left
void motorTurn(int dir) {
  //move forward a bit
  motorDrive(speedL, speedR);
  delay(100);
  motorDrive(0, 0);
  //set motor direction for turn
  if (dir == 0) {
    digitalWrite(motor1Phase, HIGH);
    digitalWrite(motor2Phase, HIGH);
  } 
  
  else {
    digitalWrite(motor1Phase, LOW);
    digitalWrite(motor2Phase, LOW);
  }
  //while mid sensor isnt on line, keep on turnin
  motorDrive(speedL, speedR);
  delay(50);
  while (lineDetected) {
    int midSensor = analogRead(lineSensePin[2]);
    if (midSensor > WHITE_THRESHOLD) lineDetected = false;
  }

  while (!lineDetected) {
    int midSensor = analogRead(lineSensePin[2]);
    if (midSensor <= WHITE_THRESHOLD) lineDetected = true;
  }
  motorDrive(0, 0);
  motorDir(0);
}



// function to do a 180 
void motor180() {
  //motorDrive(100,100);//move forward a bit before turn
  //delay(50);
  //Serial.println("Doing 180");
  motorDrive(0,0);
  motorDir(1);
  motorDrive(speedL, speedR);
  delay(200);
  digitalWrite(motor1Phase, HIGH);
  digitalWrite(motor2Phase, HIGH);
  delay(400);
  lineDetected = false;
  //while mid sensor isnt on line, keep on turnin
  while (!lineDetected) {
    if (analogRead(lineSensePin[2]) <= WHITE_THRESHOLD - 300) { motorDrive(0, 0); lineDetected = true; }
  }
  motorDir(0);
  if (currentDir == 0) { currentDir = 1; } // flip direction
  else { currentDir = 0; }
  
}






