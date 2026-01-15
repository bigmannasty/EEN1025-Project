int AnalogValue[5]; //intialise 0s
int AnalogPin[5] = { 4, 5, 6, 7, 15 };  // keep 8 free for tone O/P music

//Initialisation of pins to control motor
int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

const int WHITE_THRESHOLD = 300;
const int BASE_SPEED = 50;
const int TURN_GAIN = 25;

void setup() {
  Serial.begin(9600);

  // Wheel initialisation
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor1Phase, OUTPUT);
  pinMode(motor2PWM, OUTPUT);
  pinMode(motor2Phase, OUTPUT);
}

void motorDrive(int leftPWM, int rightPWM) {
  leftPWM = constrain(leftPWM, 0, 255);
  rightPWM = constrain(rightPWM, 0, 255);
  
  analogWrite(motor1PWM, leftPWM);
  analogWrite(motor2PWM, rightPWM);
}

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

void motor_turn(int PWM_l, int PWM_r) {
  digitalWrite(motor1Phase, HIGH);
  digitalWrite(motor2Phase, LOW);

  digitalWrite(motor1PWM, PWM_l);
  digitalWrite(motor2PWM, PWM_r);
}


void loop() {

  int error = 0;
  int weights[5] = {-2, -1, 0, 1, 2};
  bool lineDetected = false;

  //Code will retrieve sensor values continuously
  for (int i = 0; i < 5; i++) {
    AnalogValue[i] = analogRead(AnalogPin[i]);
  
  if (AnalogValue[i] <= WHITE_THRESHOLD) {
      error += weights[i];
      lineDetected = true;
    }
  }

  int correction = error * TURN_GAIN;

  int leftSpeed = BASE_SPEED - correction;
  int rightSpeed = BASE_SPEED + correction;

  motorDrive(leftSpeed, rightSpeed);

  


/*
  //Middle Sensor
  if (AnalogValue[2] <= 300) {
    
    //Vehicle will start when detecting white line from middle sensor
    motorBWD(100);

    //Left Sensors
    if ((AnalogValue[0] >= 2000) && (AnalogValue[1] >= 1500)) {
        //Motor Turn Left
        motor_turn(80, 40);
        delay(100);
      }

    //Right Sensors
    if ((AnalogValue[4] >= 2000) && (AnalogValue[3] >= 1500)) {
        //Motor Turn Right
        motor_turn(40, 80);
        delay(100);
      }
  } else {
    //Turn motors off as soon as middle sensor no longer detects a white line (vehicle has gone off course)
  }
  */
}

/*
----
Notes:
Black >= 2600
White <= 300
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
TO DO:
1. Optimise the ratio of turning for vehicle by using sensors closer to middle
2. Backward movement so vehicle can go back on track on its own?
3. Try see if line detection can be changed into interrupts instead of if statements for stability

*/
