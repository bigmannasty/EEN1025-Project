int AnalogValue[5]; //intialise 0s
int AnalogPin[5] = { 4, 5, 6, 7, 15 };  // keep 8 free for tone O/P music


//sonar distance sensor vars
const int trigPin = 17;   // GPIO15
const int echoPin = 16;   // GPIO16
unsigned long duration;
float distanceCm;


/*
//initialise the optical distance sensor values
int DistanceValue = 0;
int distAnalogPin = 16;
*/

//Initialisation of pins to control motor
int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

//intitialise threshold, base motor speed, and turn gain
const int WHITE_THRESHOLD = 500;

//Fastest speed and turn_gain mobot can have before errors
const int speedL = 242;
const int speedR = 255;
const int TURN_GAIN = 110;

void setup() {
  Serial.begin(9600);

  // Wheel pin initialisation
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor1Phase, OUTPUT);
  pinMode(motor2PWM, OUTPUT);
  pinMode(motor2Phase, OUTPUT);

  //sonar sensor pin initialisation
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
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


void sonicSensorTrig() {
  // trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

}

int weights[5] = {-2, -1, 0, 1, 2}; // set up the weightings gor each sensor
int correction; // corection var
bool obstacleDetected; //distance sense boolean
int activeSensors; //var for detecting nodes
bool nodeDetected = false;


void loop() {

  /*
  //check distance sensor
  DistanceValue = analogRead(distAnalogPin);
  Serial.print("Distance:");
  Serial.print(" ");
  Serial.print(DistanceValue);
  Serial.println("");
  if (DistanceValue >= 1000) {
    motorDrive(0,0);
    obstacleDetected = true;
    while (obstacleDetected) {
      DistanceValue = analogRead(distAnalogPin);
      if (DistanceValue < 2000) obstacleDetected = false;
    }
  }
  */

  motorDir(0); //set motor direction to forward


  sonicSensorTrig();
  // read echo pulse width (timeout to prevent lockup)
  duration = pulseIn(echoPin, HIGH, 30000); // 30ms ~ 5m max
  //if (duration != 0) { distanceCm = (duration * 0.0343f) / 2.0f; }

    
  Serial.print("Duration: ");
  Serial.print(duration, 1);
  Serial.println(" ");


  /*

  if (duration <= 600) {
    motorDrive(0,0);
    obstacleDetected = true;
    while (obstacleDetected) {
      motorDrive(0,0);
      sonicSensorTrig();
      duration = pulseIn(echoPin, HIGH, 30000);
      if (duration != 0) { distanceCm = (duration * 0.0343f) / 2.0f; }
      Serial.print("Duration: ");
      Serial.print(duration, 1);
      Serial.println(" ");
      if (duration > 800) { obstacleDetected = false; }
      delay(200);
    }
  }
  //}

  */

  if (duration <= 600 && duration > 10) {
    motorDrive(0,0);
    obstacleDetected = true;
    Serial.print("Duration: ");
    Serial.print(duration, 1);
    Serial.println(" ");
    
  }

  if (duration > 800) { obstacleDetected = false; }


  

  //intialise error and line detect vars
  int error = 0;
  bool lineDetected = false;
  activeSensors = 0;

/*
  //Code will retrieve sensor values continuously
  for (int i = 0; i < 5; i++) {
    AnalogValue[i] = analogRead(AnalogPin[i]);
  if (AnalogValue[i] <= WHITE_THRESHOLD) {
      error += weights[i];
      activeSensors++;
      if (i == 2) {lineDetected = true;}
    }
  }

  if (activeSensors >= 4) nodeDetected = true; //checking for node
*/
  
/*
  //turn-around on node loop
  if (nodeDetected = true) {
    motorDrive(100,100);//move forward a bit before turn
    delay(50);
    motorTurn(0);
    delay(500);
    lineDetected = false;
    //while mid sensor isnt on line, keep on turnin
    while (!lineDetected) {
      int midSensor = analogRead(AnalogPin[2]);
      if (midSensor <= WHITE_THRESHOLD) lineDetected = true;
      motorTurn(0);
    }
  }
  */
  

  //find required correction and set each wheel speed
  correction = error * TURN_GAIN;
  int leftSpeed = speedL;
  int rightSpeed = speedR;

  if (!lineDetected) {
    leftSpeed += correction;
    rightSpeed -= correction;
  }

  //yeah, i drive
  if (!obstacleDetected) {
    motorDrive(rightSpeed, leftSpeed);
  }
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
