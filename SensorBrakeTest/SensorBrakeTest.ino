const short motor1PWM = 37;
const short motor1Phase = 38;
const short motor2PWM = 39;
const short motor2Phase = 20;

const short distAnalogPin = 16;
const short trigPin = 17;

//Fastest speed and turn_gain mobot can have before errors
const int speedL = 242;
const int speedR = 255;


void setup() {
  Serial.begin(9600);
  pinMode(distAnalogPin, INPUT);
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);

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


void loop() {
  // put your main code here, to run repeatedly:
  // trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long distanceValue = pulseIn(distAnalogPin, HIGH, 30000); // 30ms ~ 5m max

  Serial.println(distanceValue);
  if (distanceValue <= 400){
    
    Serial.println("Turning 180");
    motorTurn(1);
  }
  else{
    motorDrive(speedL, speedR);
    Serial.println("Moving Forward...");
  }
  delay(200);
  pinMode(distAnalogPin, INPUT);
}
