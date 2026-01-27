/*
NOTES:

for sensor calibration, we can use serial plotter and find the high and low ranges of sensors, letting us calibrate them!
*/

#include <WiFi.h>
// set up the weightings for each sensor
const int weights[5] = { -2, -1, 0, 1, 2 };

//Initialisation of pins to control motor
const int motor1PWM = 37;
const int motor1Phase = 38;
const int motor2PWM = 39;
const int motor2Phase = 20;

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

void motorTurn(int dir) {
  if (dir == 0) {
    digitalWrite(motor1Phase, HIGH);
    digitalWrite(motor2Phase, HIGH);
  } else {
    digitalWrite(motor1Phase, LOW);
    digitalWrite(motor2Phase, LOW);
  }
  motorDrive(100, 100);
}

//initialise the distance sensor values
const int distAnalogPin = 16;
//check distance sensor & stop motor if too close to object
void distanceSense() {
  int DistanceValue = analogRead(distAnalogPin);
  if (DistanceValue >= 1000) {
    motorDrive(0, 0);
    bool obstacleDetected = true;
    while (obstacleDetected) {
      DistanceValue = analogRead(distAnalogPin);
      if (DistanceValue < 2000) obstacleDetected = false;
    }
  }
}

//Initialises 0s for LineSensorValues
int lineValue[5];

const int lineSensePin[5] = { 4, 5, 6, 7, 15 };
const int WHITE_THRESHOLD = 500;
void lineSense(int *error, int *activeSensors) {
  //intialise error and line detect vars
  *error = 0;
  *activeSensors = 0;

  //Code will retrieve sensor values continuously
  for (int i = 0; i < 5; i++) {
    lineValue[i] = analogRead(lineSensePin[i]);
    if (lineValue[i] <= WHITE_THRESHOLD) {
      *error += weights[i];
      *activeSensors++;
    }
  }
}

bool middleLine() {
  if (lineValue[2] <= WHITE_THRESHOLD) {
    return true;
  } else return false;
}


void nodeDetected(int activeSensors) {
  //checking for node, turning around if true
  if (activeSensors >= 4) {
    //move forward a bit before turn
    motorDrive(100, 100);
    delay(50);
    motorTurn(0);
    delay(500);
    bool lineDetected = false;
    //while mid sensor isnt on line, keep on turnin
    while (!lineDetected) {
      motorTurn(0);
      lineDetected = middleLine();
    }
  }
}

//WiFi Details
const char ssid[] = "iot";
const char password[] = "spraylike69untrimming";
WiFiClient client;

void connectToWiFi() {
  Serial.print("Connecting to Network: ");
  Serial.print(ssid);
  Serial.flush();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    Serial.flush();
    delay(300);
  }
  Serial.println("Connected");
  Serial.print("Obtaining IP address");
  Serial.flush();

  while (WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    Serial.flush();
    delay(300);
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

//Server details
const char server[] = "3.250.38.184";
const int port = 8000;

//Attempt to connect to server
bool connect() {
  if (!client.connect(server, port)) {
    Serial.println("Error connecting to Server...");
    return false;
  } else {
    return true;
  }
}

// read buffer size for HTTP response
#define BUFSIZE 512

String readResponse() {
  char buffer[BUFSIZE];
  memset(buffer, 0, BUFSIZE);
  client.readBytes(buffer, BUFSIZE);
  String response(buffer);
  return response;
}

int getStatusCode(String& response) {
  String code = response.substring(9, 12);
  return code.toInt();
}

String getResponseBody(String& response) {
  int split = response.indexOf("\r\n\r\n");
  String body = response.substring(split + 4, response.length());
  body.trim();
  return body;
}

void setup() {
  Serial.begin(9600);
  connectToWiFi();
  connect();
  initMotor();

  // post body
  String postBody("position=");
  postBody += position;  // send post request and headers
  client.println("GET /api/getRoute/wjdn3008 HTTP/1.1");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(postBody.length());
  client.println();  // send post body
  client.println(postBody);
}

//Fastest speed and turn_gain mobot can have before errors
const int speedR = 255;
const int speedL = int(speedR * 0.95);
const int TURN_GAIN = 110;
void loop() {

  int destination;                   // read response
  String response = readResponse();  // get status code
  int statusCode = getStatusCode(response);
  if (statusCode == 200) {                    // success, read body
    String body = getResponseBody(response);  // check if at final destination
    if (!body.equals("Finished")) { destination = body.toInt(); }
  }

  distanceSense();

  bool lineDetected = middleLine();

  //Using pointers, we can change values of error and activeSensors within lineSense function
  int error;
  int activeSensors;
  lineSense(&error, &activeSensors);

  //If less than half of sensor detecting a line, stop mobot and keep checking for line
  while (activeSensors <= 2){
    motorDrive(0, 0);
    lineSense(&error, &activeSensors);
  }

  //set motor direction to forward
  motorDir(0);

  //If node is detected, will perform U-turn
  nodeDetected(activeSensors);

  //find required correction and set each wheel speed
  int correction = error * TURN_GAIN;
  int leftSpeed = speedL;
  int rightSpeed = speedR;

  if (!lineDetected) {
    leftSpeed += correction;
    rightSpeed -= correction;
  }

  //yeah, i drive
  motorDrive(rightSpeed, leftSpeed);
}
