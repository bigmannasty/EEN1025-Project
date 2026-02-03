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
const int speedR = 150;
const int speedL = int(speedR * 0.95);
const int TURN_GAIN = 110;

int currentDir = 0; // 0 CW    1 ACW

bool obstacleDetected = false;
bool lineDetected = false;
bool nodeDetected = false;
int error = 0;
int activeSensors = 0;

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





void setup() {
  Serial.begin(9600);
  delay(500);  // <--- ADDED: Wait for Serial to initialize
    
  // Initialize motor
  initMotor();
  
  
  
}





bool passedJunc = true;
bool routeFinished = false;

void loop() {

  
  //distanceSense();

  lineDetected = false;
  nodeDetected = false;

  //Using pointers, we can change values of error and activeSensors within lineSense function

  error = 0;
  activeSensors = 0;

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

  if (nodeDetected == true) {
    motorTurn(0);
  }

/*

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
*/





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
