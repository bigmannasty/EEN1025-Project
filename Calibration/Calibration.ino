//Initialisation of pins to control motor
int motor1PWM = 37;
int motor1Phase = 38;
int motor2PWM = 39;
int motor2Phase = 20;

int speedL = 242;
int speedR = 255;
int dir;

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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Wheel initialisation
  pinMode(motor1PWM, OUTPUT);
  pinMode(motor1Phase, OUTPUT);
  pinMode(motor2PWM, OUTPUT);
  pinMode(motor2Phase, OUTPUT);
  
  dir = 0;
  motorDir(dir);
  motorDrive(0, 0);

}

void loop() {
  delay(10000);
  motorDrive(speedL, speedR);
  delay(2000);
  motorDrive(0, 0);
}
