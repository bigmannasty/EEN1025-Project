#include <WiFi.h>
// set up the weightings for each sensor
const int weights[5] = { -3, -1, 0, 1, 3 };



//initialise the distance sensor values
const int distAnalogPin = 16;
int DistanceValue = 0;
int lastDist = 0;
bool finalDrive = false;


//Initialisation of pins to control motor
const int motor1PWM = 37;
const int motor1Phase = 38;
const int motor2PWM = 39;
const int motor2Phase = 40;

// Added these global variables
String route = "";  // <--- ADDED: Store the route received from GET
int routeList[] = {0, 3, 2, 5};
int position = 0;  // <--- ADDED: Track current position index
bool routeCompleted = false;  // <--- ADDED: Flag for route completion


//node and wight arrays
int nodes[][5] = {{0, 1, 1, 0, 1}, {1, 0, 1, 1, 1}, {1, 1, 0, 1, 0}, {0, 1, 1, 0, 1}, {1, 1, 0, 1, 0}}; // direct node connections, 0 - no direct connection, 1 - direct connection
int nodeWeights[][5] = {{9, 2, 2, 9, 1}, {2, 9, 2, 4, 4}, {2, 2, 9, 1, 9}, {9, 4, 1, 9, 6}, {1, 4, 9, 6, 9}}; // route weightings for each node
int nodeDirection[][5] = {{9, 1, 1, 9, 0}, {1, 9, 1, 0, 0}, {0, 0, 9, 1, 9}, {9, 1, 0, 9, 1}, {1, 0, 9, 0, 9}}; // 0 clockwise  1 anti clockwise

int nodePathRank03[3] = {2, 1, 4}; // route paths for node 0 -> 3 and vice versa
int nodePathRank24[3] = {0, 1, 3}; // route paths for node 2 -> 4 and vice versa



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
const int TURN_GAIN = 80;

int currentDir = 1; // 0 CW    1 ACW

bool obstacleDetected = false;
bool lineDetected = false;
bool nodeDetected = false;
int error = 0;
int activeSensors = 0;
bool passedJunc = true;
bool routeFinished = false;
bool parkingFromNode1 = false;



//Initialises 0s for LineSensorValues
int lineValue[5];
const int lineSensePin[5] = { 4, 5, 6, 7, 15 };
const int WHITE_THRESHOLD = 800;




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
  delay(200);
  lineDetected = false;
  //while mid sensor isnt on line, keep on turnin
  while (!lineDetected) {
    if (analogRead(lineSensePin[2]) <= WHITE_THRESHOLD - 300) { motorDrive(0, 0); lineDetected = true; }
  }
  motorDir(0);
  if (currentDir == 0) { currentDir = 1; } // flip direction
  else { currentDir = 0; }
  
}







//check distance sensor & stop motor if too close to object
bool distanceSense() {
  int DistanceValue = analogRead(distAnalogPin);
  //Serial.println(DistanceValue);
  if (DistanceValue >= 1800) {
    motorDrive(0, 0);
    return true;
    /*
    while (obstacleDetected) {
      DistanceValue = analogRead(distAnalogPin);
      if (DistanceValue < 2000) obstacleDetected = false;
    }
    */
  }
  return false;
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


// Function to send GET request and get route
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


// Function to send POST request when arriving at node
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

  initMotor(); // Initialize motor
  motorDrive(0,0); // make sure that jawn dont move
  motorDir(0); //set motor direction to forward


  Serial.begin(9600);

  Serial.println("");
  Serial.println("");
  Serial.println("");
  

  /*
  // conncect up to wifi
  connectToWiFi();

  // get that route from server
  while (!getRoute()) {  // <--- CHANGED: Call getRoute() function instead of raw HTTP
    Serial.println("Retrying to get route in 3 seconds...");
    delay(3000);
  }
  
  // get first dest from route or whatever
  int firstComma = route.indexOf(',');
  if (firstComma != -1) {
    String firstPos = route.substring(0, firstComma);
    nextPos = firstPos.toInt();
  } else {
    nextPos = route.toInt();
  }
*/
  // set lineDetected to false because i forgot why
  lineDetected = false;
  if (routeList[-2] == 1) { parkingFromNode1 = true; }
  
}






void loop() {

  
  nodeDetected = false;
  lineDetected = false;
  error = 0;
  activeSensors = 0;


  //checking for line
  for (int i = 0; i < 5; i++) {
    lineValue[i] = analogRead(lineSensePin[i]);
    if (lineValue[i] <= WHITE_THRESHOLD) {
        error += weights[i];
        activeSensors++;
        if (i == 2) {lineDetected = true;}
    }
  }

  
  if (activeSensors >= 4) nodeDetected = true; // check for node

/*
  Serial.println("next pos");
  Serial.println(nextPos);
  Serial.println("start pos");
  Serial.println(startPos);
  Serial.println("passed junction");
  Serial.println(passedJunc);
  Serial.println("final drive");
  Serial.println(finalDrive);
  */


  if (nodeDetected == true && nextPos != 5) { // if reached a node

    motorDrive(0,0); // stop!!
    delay(50);

    if (passedJunc == true) { // if node is not a junction
      
      if (nextPos == routeList[currentRouteNodeIndex]) { // if the node youre at right now is an actual route node
        //routeFinished = sendArrival(nextPos);
        currentRouteNodeIndex++; // move onto the next routelist node
        startPos = nextPos; // new start node becomes the last dest node
        nextPos = routeList[currentRouteNodeIndex]; // new next node becomes the next required destination node in the route list
        // parking node loop
        if (nextPos == 5) { // when parking up
          if (startPos != 1) { parkingFromNode1 = false; }
          //Serial.println("Next Node 5");
          

          /*
          if (startPos == 1) { // if ending at 1
            motorDrive(speedL, speedR); // drive
            delay(750);
            if (currentDir != 0) { motor180(); } // make sure facing left
            motorDrive(speedL, speedR); // drive
            
          }
          


          motorDrive(speedL, speedR);
          int lastDist = 0;
          while (DistanceValue > 1500 && lastDist > 1500) { // while not at wall check for distance
            Serial.println("Checking for wall");
            lastDist = DistanceValue;
            DistanceValue = analogRead(distAnalogPin);
            Serial.println(DistanceValue);
          }
          */

          //routeFinished = sendArrival(nextPos);
        }
        if (nodes[startPos][nextPos] == 0 && nextPos != 5) { // if theres no direct connection between the 2 nodes
          if (startPos == 0 || startPos == 3) { nextPos = nodePathRank03[currentNodeRank]; } // if going 0 -> 3 or vice versa use the 1st ranking
          else { nextPos = nodePathRank24[currentNodeRank]; } // if going 2 -> 4 or vice versa use the 2nd ranking
        }
        
        passedJunc = false; // reset the junction var
        motorDrive(speedL, speedR); // get that jawn movin
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

        

        //Serial.println("main route loop");

      }


      else if (nextPos != routeList[currentRouteNodeIndex]) { // if the node reached was a midpoint
        startPos = nextPos; // reached node is new start
        nextPos = routeList[currentRouteNodeIndex]; // nextpos becomes the original main destination node from the routelist
        passedJunc = false; // reset junction var
        motorDrive(speedL, speedR);
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
        //Serial.println("sub route loop");

      }

/*
      Serial.print("Start: ");
      Serial.println(startPos);
      Serial.print("Next: ");
      Serial.println(nextPos);
      */

      if ( (startPos == 0 && nextPos == 4) || (startPos == 4 && nextPos == 0) ) { passedJunc = true; } // if starts at 0 and goes 4 or vice versa, next node is not junction
      else if ( (startPos == 2 && nextPos == 3) || (startPos == 3 && nextPos == 2) ) { passedJunc = true; } // if starts at 2 and goes 3 or vice versa, next node is not junction
      
      else if (nextPos == 5) { // node conditions for parking
        passedJunc = true; 

        if (startPos == 1 && parkingFromNode1) { // if going 1 -> 5
          if (currentDir == 1) { motor180(); } // if facing to the right turn around
          passedJunc = false; // will come to that 1 junc
        }

        else if (startPos == 1 && !parkingFromNode1) { // if passing through 1 while -> 5
          if (currentDir == 1) { // if direction is anti 
            if (routeList[currentRouteNodeIndex - 1] == 0) { motorTurn(1); }
            else { motorTurn(0); }
          } 
          
          else if (currentDir == 0) { 
            if (routeList[currentRouteNodeIndex - 1] == 0) { motorTurn(0); }
            else { motorTurn(1); }
          }

        }

        else { // if going other nodes -> 5
          if (currentDir == 0) { motorTurn(0); } // turn right when clockwise
          else if (currentDir == 1) { motorTurn(1); } // turn left when anti clock

          if (startPos == 0 || startPos == 2) { nextPos = 1; }
        }

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

      if (currentDir != nodeDirection[startPos][nextPos] && nextPos != 5 && routeList[currentRouteNodeIndex] != 5) { motor180(); } // check for correct direction, if not going the right way flip round


    }




    else if (passedJunc == false) { // if at a junction

      //Serial.println("at junc loop");

      motorDrive(speedL, speedR);
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

      if (currentDir == 0 && nextPos == 1) { // if going clockwise to node 1
        motorTurn(0); // turn right at the next junction
        //Serial.println("turn right");
        if (startPos == 4) { currentDir = 1; }
      }

      else if (currentDir == 1 && nextPos == 1) { // if going anti-clockwise to node 1
        motorTurn(1); // turn left at the next junction
        //Serial.println("turn left");
        if (startPos == 0) { currentDir = 0; }
      }


      if (startPos == 1 && (nextPos == 2 || nextPos == 4)) { // if going from node 1 to node 2 or 4
        motorTurn(1); // turn left at the next junction
        //Serial.println("turn left");
        if (nextPos == 2) { currentDir = 1; }
        else { currentDir = 1; }
      }

      else if (startPos == 1 && (nextPos == 0 || nextPos == 3)) { // if going from node 1 to node 0 or 3
        motorTurn(0); // turn righ at the next junction
        //Serial.println("turn right");
        if (nextPos == 0) { currentDir = 0; }
        else { currentDir = 0; }
      }

      passedJunc = true; // indicate that the junction has been passed

    }


  }

  else if (nodeDetected == true && passedJunc == true && nextPos == 1) {
    motorDrive(0,0);
    delay(200);
    nextPos = 5;
    finalDrive = true;
  }

  else if (nodeDetected == true && passedJunc == false && startPos == 1 && nextPos == 5) {
    motorDrive(0,0);
    delay(200);
    finalDrive = true;
  }

  else if (nodeDetected == true && passedJunc == true && startPos == 1 && nextPos == 5) {
    motorDrive(0,0);
    delay(200);
    finalDrive = true;
  }
  

  //if (distanceSense()) { obstacleDetected = true; }

/*
  //turn-around at an obstacle
  if (obstacleDetected == true) {
    Serial.println("Obstacle Detected");
    //motor180();
    delay(2000);
    if (distanceSense() == false) { obstacleDetected = false; }
    else { while (1) { motorDrive(0,0); } }
  }
*/
  
  if (finalDrive) {
    int DistanceValue = analogRead(distAnalogPin);
    /*
    Serial.println("DistanceValue");
    Serial.println(DistanceValue);
    Serial.println("lastDist");
    Serial.println(lastDist);
    */
    if (DistanceValue >= 1000 && lastDist >= 1000) {
      motorDrive(0,0);
      //sendArrival(nextPos);
      while (1) {}
    }
    lastDist = DistanceValue;
  }
  

  //find required correction and set each wheel speed
  int correction = error * TURN_GAIN;
  int leftSpeed = speedL;
  int rightSpeed = speedR;


  if (!lineDetected && !finalDrive) {
    leftSpeed += correction;
    rightSpeed -= correction;
  }

  //yeah, i drive
  motorDrive(rightSpeed, leftSpeed);
}
