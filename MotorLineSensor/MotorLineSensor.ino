/*
NOTES:

for sensor calibration, we can use serial plotter and find the high and low ranges of sensors, letting us calibrate them!
*/

#include <WiFi.h>
// set up the weightings for each sensor
const int weights[5] = { -3, -1, 0, 1, 3 };

//Initialisation of pins to control motor
const int motor1PWM = 37;
const int motor1Phase = 38;
const int motor2PWM = 39;
const int motor2Phase = 20;

// Added these global variables
String route = "";  // <--- ADDED: Store the route received from GET
int routeList[] = {0,0,0,0,0};
int position = 0;  // <--- ADDED: Track current position index
bool routeCompleted = false;  // <--- ADDED: Flag for route completion


//node and wight arrays
int nodes[][5] = {{0, 1, 1, 0, 1}, {1, 0, 1, 1, 1}, {1, 1, 0, 1, 0}, {0, 1, 1, 0, 1}, {1, 1, 0, 1, 0}};
int nodeWeights[][5] = {{9, 2, 2, 9, 1}, {2, 9, 2, 4, 4}, {2, 2, 9, 1, 9}, {9, 4, 1, 9, 5}, {1, 4, 9, 5, 9}};



int weightRankTotal[] = {9, 9, 9}; //weight of each midpoint for the mobot to take
int routeRank[] = {-1, -1, -1}; //rank of the midpoints for the mobot to take next on the route
int currentNodeRank = 0; //current index of the node rank array
int currentRouteNodeIndex = 0; //index of the node the mobot is at in the overall route
int currentWeightTotal = 0;
bool routeClear = false;

int startPos = -1;
int nextPos = 0;


//Fastest speed and turn_gain mobot can have before errors
const int speedR = 255;
const int speedL = int(speedR * 0.95);
const int TURN_GAIN = 110;

int currentDir = 0; // 0 CW    1 ACW

bool obstacleDetected = false;


//Initialises 0s for LineSensorValues
int lineValue[5];
const int lineSensePin[5] = { 4, 5, 6, 7, 15 };
const int WHITE_THRESHOLD = 500;




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
  motorDrive(200, 200);
  delay(100);
  motorDrive(0,0);
  //set motor direction for turn
  if (dir == 0) {
    digitalWrite(motor1Phase, HIGH);
    digitalWrite(motor2Phase, HIGH);
  } 
  
  else {
    digitalWrite(motor1Phase, LOW);
    digitalWrite(motor2Phase, LOW);
  }
  bool lineDetected = false;
  //while mid sensor isnt on line, keep on turnin
  motorDrive(100, 100);
  while (!lineDetected) {
    int midSensor = analogRead(lineSensePin[2]);
    if (midSensor <= WHITE_THRESHOLD) lineDetected = true;
  }
  motorDir(0);
}







//initialise the distance sensor values
const int distAnalogPin = 16;
//check distance sensor & stop motor if too close to object
void distanceSense() {
  int DistanceValue = analogRead(distAnalogPin);
  if (DistanceValue >= 1000) {
    motorDrive(0, 0);
    obstacleDetected = true;
    while (obstacleDetected) {
      DistanceValue = analogRead(distAnalogPin);
      if (DistanceValue < 2000) obstacleDetected = false;
    }
  }
}










/*
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
*/









bool routeFind() {

  int weightRankTotal[] = {9, 9, 9};
  int routeRank[] = {-1, -1, -1};
  int currentNodeRank = 0; 
  bool routeClear = false;

  if (nodes[startPos][nextPos] == 1) { //if there's a direct connection
    bool routeClear = true;
    return true;
    //direct connections can be longer than other indirect connections, add code to check for this TODO LATER!!!!!!!!!!!!!!
  }

  if (nodes[startPos][nextPos] == 0) {
    for (int i; i < 5; i++) {
      if (nodes[startPos][i] == 1) { //if direct connection to index-node in the list check weighting and update

        currentWeightTotal = nodeWeights[startPos][i]; //getting weight total for current midpoint
        currentWeightTotal += nodeWeights[i][nextPos];

        for (int j; j < 3; i++) {
          
          //update rank total
          if (currentWeightTotal < weightRankTotal[j]) {
            int temp = weightRankTotal[j];
            weightRankTotal[j] = currentWeightTotal;
            int rtemp = routeRank[j];
            routeRank[j] = i;
            

            //if replacing rank 1, new temps and push the list down 2 positions
            if (j == 0) {
              int temp2 = weightRankTotal[1];
              weightRankTotal[1] = temp;
              weightRankTotal[2] = temp2;
              int rtemp2 = routeRank[1];
              routeRank[1] = rtemp;
              routeRank[2] = rtemp2;
            }

            //if replacing rank 2, push the list down 1 position
            else if (j == 1) {
              weightRankTotal[2] = weightRankTotal[1];
              weightRankTotal[1] = temp;
              routeRank[2] = routeRank[1];
              routeRank[1] = rtemp;
            }
          }
        }
      }
    }
    bool routeClear = true;
    return true;
  }
}


void updateNextPos() {
  if (routeRank[currentNodeRank] != -1) { //if an indirect connection was made
    nextPos = routeRank[currentNodeRank];
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
bool connectToServer() {  // <--- CHANGED: Renamed for clarity
  Serial.print("Connecting to server: ");  // <--- ADDED: Debug info
  Serial.print(server);
  Serial.print(":");
  Serial.println(port);
  
  if (!client.connect(server, port)) {
    Serial.println("Connection failed!");
    return false;
  }
  
  Serial.println("Connected to server!");  // <--- ADDED: Success message
  return true;
}






// read buffer size for HTTP response
#define BUFSIZE 512

String readResponse() {
  String response = "";
  unsigned long timeout = millis();
  
  while (client.connected() && millis() - timeout < 5000) {  // <--- CHANGED: Timeout handling
    while (client.available()) {
      char c = client.read();
      response += c;
      timeout = millis();  // Reset timeout on data received
    }
  }
  
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






// ADDED: Function to send GET request and get route
bool getRoute() {
  if (!connectToServer()) return false;
  
  Serial.println("Sending GET request for route...");
  
  // Send proper HTTP GET request
  client.println("GET /api/getRoute/wjdn3008 HTTP/1.1");
  client.println("Host: 3.250.38.184");  // <--- ADDED: Required HTTP header
  client.println("Connection: close");  // <--- ADDED: Close connection after
  client.println();  // End of headers
  
  delay(100);  // Give server time to respond
  
  // Read response
  String response = readResponse();
  int statusCode = getStatusCode(response);
  
  Serial.print("GET Response Status: ");
  Serial.println(statusCode);
  
  if (statusCode == 200) {
    route = getResponseBody(response);  // <--- Store route globally
    int j = 0;
    for (int i = 0; route[i] != '\0'; i++) {
      if (route[i] != ',') {
        routeList[j] += route[i] - 48;
        j++;
      }
    }
    Serial.print("Route received: ");
    Serial.println(route);
    client.stop();
    return true;
  } else {
    Serial.println("Failed to get route");
    client.stop();
    return false;
  }
}







// ADDED: Function to send POST request when arriving at node
bool sendArrival(int position) {
  if (!connectToServer()) return false;
  
  Serial.print("Sending POST for position: ");
  Serial.println(position);
  
  // Prepare POST body
  String postBody = "position=" + String(position);
  
  // Send HTTP POST request
  client.println("POST /api/arrived/wjdn3008 HTTP/1.1");
  client.println("Host: 3.250.38.184");  // <--- ADDED: Required header
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");  // <--- FIXED: Proper Content-Length
  client.println(postBody.length());
  client.println("Connection: close");  // <--- ADDED: Close connection
  client.println();  // End of headers
  client.println(postBody);  // Send body
  
  delay(100);  // Give server time to respond
  
  // Read response
  String response = readResponse();
  int statusCode = getStatusCode(response);
  
  Serial.print("POST Response Status: ");
  Serial.println(statusCode);
  
  if (statusCode == 200) {
    String body = getResponseBody(response);
    Serial.print("Server response: ");
    Serial.println(body);
    client.stop();
    
    // Check if finished
    if (body.equals("Finished")) {
      Serial.println("Route completed!");
      return true;  // Finished
    }
    return false;  // Not finished
  } 
  
  else {
    Serial.println("Failed to send arrival");
    client.stop();
    return false;
  }
}











void setup() {
  Serial.begin(9600);
  delay(500);  // <--- ADDED: Wait for Serial to initialize
  

  
  connectToWiFi();
  
  // Initialize motor
  initMotor();
  

  
  // Get route from server
  while (!getRoute()) {  // <--- CHANGED: Call getRoute() function instead of raw HTTP
    Serial.println("Retrying to get route in 3 seconds...");
    delay(3000);
  }
  
  // Assuming route is a comma-separated list of destinations
  // e.g., "1,3,2,4"
  // Parse the first destination
  int firstComma = route.indexOf(',');
  if (firstComma != -1) {
    String firstPos = route.substring(0, firstComma);
    nextPos = firstPos.toInt();
  } else {
    nextPos = route.toInt();
  }
  
  Serial.print("First node: ");
  Serial.println(nextPos);
  
}





bool passedJunc = true;
bool routeFinished = false;

void loop() {

  
  //distanceSense();

  bool lineDetected = false;
  bool nodeDetected = false;

  //Using pointers, we can change values of error and activeSensors within lineSense function

  int error = 0;
  int activeSensors = 0;

  //lineSense(&error, &activeSensors);


  //set motor direction to forward
  motorDir(0);


  
  

  //checking for node, turning around if true
  
  for (int i = 0; i < 5; i++) {
    lineValue[i] = analogRead(lineSensePin[i]);
  if (lineValue[i] <= WHITE_THRESHOLD) {
      error += weights[i];
      activeSensors++;
      if (i == 2) {lineDetected = true;}
    }
  }

  

  /*
  for (int i = 0; i < 5; i++) {
    Serial.println("route");
    Serial.println(routeList[i]);  

  }
  */

  if (activeSensors >= 4) nodeDetected = true; // check for node

  if (nodeDetected == true) { // if reached a node

    if (passedJunc == true) { // if node is not a junction

      motorDrive(0,0);
      
      if (nextPos == routeList[currentRouteNodeIndex]) { // if the node youre at right now is an actual route node
        currentRouteNodeIndex++; // move onto the next routelist node
        routeFinished = sendArrival(nextPos);
        startPos = nextPos; // new start node becomes the last dest node
        nextPos = routeList[currentRouteNodeIndex]; // new next node becomes the next required destination node in the route list
        //routeFind(); // find the new route for the current start and next nodes
        //updateNextPos();
        passedJunc = false;
        while (nodeDetected) { // while still on node 
          activeSensors = 0;
          for (int i = 0; i < 5; i++) {
            lineValue[i] = analogRead(lineSensePin[i]);
            if (lineValue[i] <= WHITE_THRESHOLD) {
                error += weights[i];
                activeSensors++;
            }
          }
          if (activeSensors < 3) {nodeDetected = false;}
        }

      }


      else if (nextPos != routeList[currentRouteNodeIndex]) { // if the node reached was a midpoint
        startPos = nextPos; // reached node is new start
        nextPos = routeList[currentRouteNodeIndex]; // nextpos becomes the original main destination node from the routelist
        passedJunc = false; // reset junction var

      }

      if ( (startPos == 0 && nextPos == 4) || (startPos == 4 && nextPos == 0) ) { passedJunc = true; } // if starts at 0 and goes 4 or vice versa, next node is not junction
      else if ( (startPos == 2 && nextPos == 3) || (startPos == 3 && nextPos == 2) ) { passedJunc = true; } // if starts at 2 and goes 3 or vice versa, next node is not junction

    }




    if (passedJunc == false) { // if at a junction

      if (currentDir == 0 && nextPos == 1) { // if going clockwise to node 1
        motorTurn(0); // turn right at the next junction
        Serial.println("turn right");
      }

      else if (currentDir == 1 && nextPos == 1) { // if going anti-clockwise to node 1
        motorTurn(1); // turn left at the next junction
        Serial.println("turn left");
      }


      if (startPos == 1 && (nextPos == 2 || nextPos == 4)) { // if going from node 1 to node 2 or 4
        motorTurn(1); // turn left at the next junction
        Serial.println("turn left");
      }

      else if (startPos == 1 && (nextPos == 0 || nextPos == 3)) { // if going from node 1 to node 0 or 3
        motorTurn(0); // turn righ at the next junction
        Serial.println("turn right");
      }

      passedJunc = true; // indicate that the junction has been passed

    }


  }



/*
  if (nodeDetected == true) {
    motorDrive(0, 0);
    if (currentDir == 0 && nextPos == 1) {
      motorDrive(200, 200);
      delay(50);
      if (currentDir == 0) {
        digitalWrite(motor1Phase, HIGH);
        digitalWrite(motor2Phase, HIGH);
      }
      motorDrive(200, 200);
      //move forward a bit before turn
      bool lineDetected = false;
      //while mid sensor isnt on line, keep on turnin
      while (!lineDetected) {
        int midSensor = analogRead(AnalogPin[2]);
        if (midSensor <= WHITE_THRESHOLD) lineDetected = true;
      }
    }
    motorDir(0);
  }
  */



  //turn-around at an obstacle
  if (obstacleDetected == true) {
    motorTurn(0);
    delay(500);
    lineDetected = false;
    //while mid sensor isnt on line, keep on turnin
    while (!lineDetected) {
      int midSensor = analogRead(lineSensePin[2]);
      if (midSensor <= WHITE_THRESHOLD) lineDetected = true;
      motorTurn(0);
    }
  }
  
  

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
