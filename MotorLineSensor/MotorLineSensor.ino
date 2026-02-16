int AnalogValue[5]; //intialise 0s
int AnalogPin[5] = { 4, 5, 6, 7, 15 };  // keep 8 free for tone O/P music

//initialise the optical distance sensor values
int Distance1Value = 0;
int distAnalog1Pin = 16;

int Distance2Value = 0;
int distAnalog2Pin = 17;

//Initialisation of pins to control motor
int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 40;

//intitialise threshold, base motor speed, and turn gain
const int WHITE_THRESHOLD = 500;

//Fastest speed and turn_gain mobot can have before errors
int speedL = 242;
int speedR = 255;
const int TURN_GAIN = 110;

void setup() {
  Serial.begin(9600);

  // Wheel pin initialisation
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor1Phase, OUTPUT);
  pinMode(motor2PWM, OUTPUT);
  pinMode(motor2Phase, OUTPUT);
}

//function to drive each motor at controlled pwm
void motorDrive(int leftPWM, int rightPWM) {
  leftPWM = constrain(leftPWM, 0, 255);
  rightPWM = constrain(rightPWM, 0, 255);
  
  analogWrite(motor1PWM, leftPWM);
  analogWrite(motor2PWM, rightPWM);
}

//function to set direction of motors
void motorDir(int dir) {
  if (dir == 0){
    digitalWrite(motor1Phase, LOW);
    digitalWrite(motor2Phase, HIGH);
  }
  else {
    digitalWrite(motor1Phase, HIGH);
    digitalWrite(motor2Phase, LOW);
  }

}

//dir:0 cw, 1 anti-cw
void motorTurn(int dir) { 
  if (dir == 0){
    digitalWrite(motor1Phase, HIGH);
    digitalWrite(motor2Phase, HIGH);
  }
  else {
    digitalWrite(motor1Phase, LOW);
    digitalWrite(motor2Phase, LOW);
  }
  motorDrive(100,100);
}


int weights[5] = {-2, -1, 0, 1, 2}; // set up the weightings gor each sensor
int correction; // corection var
bool obstacleDetected; //distance sense boolean
int activeSensors; //var for detecting nodes

void loop() {

  //check distance sensor
  Distance1Value = analogRead(distAnalog1Pin);
  Distance2Value = analogRead(distAnalog2Pin);
  if ((Distance1Value >= 1500) && (Distance2Value >= 1500)) {
  if ((Distance1Value >= 1100) && (Distance2Value >= 1100)) {
    speedL = 0;
    speedR = 0;
  }

  motorDir(0); //set motor direction to forward

  //intialise error and line detect vars
  int error = 0;
  bool lineDetected = false;
  activeSensors = 0;


  //Code will retrieve sensor values continuously
  for (int i = 0; i < 5; i++) {
    AnalogValue[i] = analogRead(AnalogPin[i]);
  if (AnalogValue[i] <= WHITE_THRESHOLD) {
      error += weights[i];
      activeSensors++;
      if (i == 2) {lineDetected = true;}
    }
  }  
  //find required correction and set each wheel speed
  correction = error * TURN_GAIN;
  int leftSpeed = speedL;
  int rightSpeed = speedR;

  if (!lineDetected) {
    leftSpeed += correction;
    rightSpeed -= correction;
  }
  motorDrive(rightSpeed, leftSpeed);
}

/*
----
Notes:
Black >= 2600
White <= 500
Sensors range from 200 to 4095 typically

----
Operation:
1. Vehicle will not run until it detects the white line in the middle.

2. When middle sensor is below 300, vehicle will begin detecting line and moving forward.
  2a. Vehicle should automatically adjust angle so it is driving as parallel to white line as possible.

3. When a turn is detected, vehicle needs to change PWM to change angle according to white line.
  3a. If left turn detected, right wheel needs to slow down and vice versa.
  3b. This could be done with an error variable, if larger than 0, we need to turn right and if less than 0, we need to turn left.

----
*/
