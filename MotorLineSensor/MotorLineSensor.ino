int AnalogValue[5]; //intialise 0s
int AnalogPin[5] = { 4, 5, 6, 7, 15 };  // keep 8 free for tone O/P music

//Initialisation of pins to control motor
int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

//intitialise threshold, base motor speed, and turn gain
const int WHITE_THRESHOLD = 1000;
const int BASE_SPEED = 125;
const int TURN_GAIN = 35;

void setup() {
  Serial.begin(9600);

  // Wheel initialisation
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


int weights[5] = {-2, -1, 0, 1, 2}; // set up the weightings gor each sensor
int correction; // corection var


void loop() {

  //intialise error and line detect vars
  int error = 0;
  bool lineDetected = false;

  //Code will retrieve sensor values continuously
  for (int i = 0; i < 5; i++) {
    AnalogValue[i] = analogRead(AnalogPin[i]);
  if (AnalogValue[i] <= WHITE_THRESHOLD) {
      error += weights[i];
      if (i == 2) {lineDetected = true;}
    }
  }

  motorDir(0); //set motor direction to forward

  //find required correction and set each wheel speed
  correction = error * TURN_GAIN;
  int leftSpeed = BASE_SPEED;
  int rightSpeed = BASE_SPEED;

  if (!lineDetected) {
    leftSpeed += correction;
    rightSpeed -= correction;
  }

  //yeah, i drive
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
