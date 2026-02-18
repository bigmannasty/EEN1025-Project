#include <ServerCom.h>
#include <Motor.h>
#include <Bluetooth.h>
#include <SonicSensor.h>


// set up the weightings for each sensor
const int weights[5] = { -3, -1, 0, 1, 3 };

const int TURN_GAIN = 50;

//initialise the distance sensor values
//const int distAnalogPin = 16;
int DistanceValue = 0;
int lastDist = 0;
bool finalDrive = false;


//const int trigPin = 16;
//const int echoPin = 17;


//node and wight arrays
int nodes[][5] = {{0, 1, 1, 0, 1}, {1, 0, 1, 1, 1}, {1, 1, 0, 1, 0}, {0, 1, 1, 0, 1}, {1, 1, 0, 1, 0}}; // direct node connections, 0 - no direct connection, 1 - direct connection
int nodeWeights[][5] = {{9, 2, 2, 9, 1}, {2, 9, 2, 4, 4}, {2, 2, 9, 1, 9}, {9, 4, 1, 9, 6}, {1, 4, 9, 6, 9}}; // route weightings for each node
int nodeDirection[][5] = {{9, 1, 1, 9, 0}, {1, 9, 1, 0, 0}, {0, 0, 9, 1, 9}, {9, 1, 0, 9, 1}, {1, 0, 9, 0, 9}}; // 0 clockwise  1 anti clockwise

int nodePathRank03[3] = {2, 1, 4}; // route paths for node 0 -> 3 and vice versa
int nodePathRank24[3] = {0, 1, 3}; // route paths for node 2 -> 4 and vice versa


int currentNodeRank = 0; //current index of the node rank array
int currentRouteNodeIndex = 0; //index of the node the mobot is at in the overall route


int startPos = -1;
int nextPos = 0;


extern bool obstacleDetected = false;
extern bool nodeDetected = false;


int error = 0; // error from sensor weight
int activeSensors = 0; // how many active sensors on a given scan
bool passedJunc = true; // denoted the passing of a junction
bool routeFinished = false; // take a big guess, probably redundant ngl
bool parkingFromNode1 = false; // if going 1 -> direct aka if the last main route node is 1

/*
bool parkingFromN34PreFDStage1 = false; // parking from node 3 or 4 before final drive aka on the big bend 1st stage
bool parkingFromN34PreFDStage2 = false; // parking from node 3 or 4 before final drive aka on the big bend 2nd stage
*/


bool returnToStart = false; // flag for when returning to a start node after detecting an obstacle



void nodeDecLoop() {
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



void setup() {

  initMotor(); // Initialize motor
  motorDrive(0,0); // make sure that jawn dont move
  motorDir(0); //set motor direction to forward


  Serial.begin(9600);

  Serial.println("");
  Serial.println("");
  Serial.println("");
  

  
  // conncect up to wifi
  connectToWiFi();

  // get that route from server
  while (!getRoute()) {  // <--- CHANGED: Call getRoute() function instead of raw HTTP
    Serial.println("Retrying to get route in 3 seconds...");
    delay(3000);
  }
  

  bluetoothSetup();


  // set lineDetected to false because i forgot why
  lineDetected = false;
  if (routeList[-2] == 1) { parkingFromNode1 = true; }

  pinSetup();
  
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


  // node detection logic
  if (nodeDetected == true && nextPos != 5) { // if reached a node

    motorDrive(0,0); // stop!!
    delay(50);

    if (passedJunc == true && !returnToStart) { // if node is not a junction
      
      // condition logic for when non-junction main route nodes detected
      if (nextPos == routeList[currentRouteNodeIndex]) { // if the node youre at right now is an actual route node
        routeFinished = sendArrival(nextPos);
        currentRouteNodeIndex++; // move onto the next routelist node
        startPos = nextPos; // new start node becomes the last dest node
        nextPos = routeList[currentRouteNodeIndex]; // new next node becomes the next required destination node in the route list
        sendNode(nextPos); // send nextpos to other esp
        sendToPhone(nextPos);
        // parking node loop

        if (nextPos == 5) { // when parking up
          if (startPos != 1) { parkingFromNode1 = false; }
          
        }

        // checking for direct node connections
        if (nodes[startPos][nextPos] == 0 && nextPos != 5) { // if theres no direct connection between the 2 nodes
          
          if (startPos == 0 || startPos == 3) { // if going 0 -> 3 or vice versa use the 1st ranking
            currentNodeRank = 0;
            while (nodes[startPos][nextPos] == 0) {
              nextPos = nodePathRank03[currentNodeRank];
              if (nodes[startPos][nextPos] == 0) { currentNodeRank++; }
            }
          } 
          
          else if (startPos == 2 || startPos == 4) {  // if going 2 -> 4 or vice versa use the 2nd ranking
            currentNodeRank = 0;
            while (nodes[startPos][nextPos] == 0) {
              nextPos = nodePathRank24[currentNodeRank];
              if (nodes[startPos][nextPos] == 0) { currentNodeRank++; }
            }
          }

          else if (startPos == 1) {

          }
        }
        
        passedJunc = false; // reset the junction var
        motorDrive(speedL, speedR); // get that jawn movin
        nodeDecLoop(); // go til off node

        //Serial.println("main route loop");

      }

      // condition logic for when non-junction main route nodes detected
      else if (nextPos != routeList[currentRouteNodeIndex]) { // if the node reached was a midpoint
        startPos = nextPos; // reached node is new start
        nextPos = routeList[currentRouteNodeIndex]; // nextpos becomes the original main destination node from the routelist
        passedJunc = false; // reset junction var
        motorDrive(speedL, speedR);
        nodeDecLoop();
        //Serial.println("sub route loop");

      }


      // junction condition logic
      if ( (startPos == 0 && nextPos == 4) || (startPos == 4 && nextPos == 0) ) { passedJunc = true; } // if starts at 0 and goes 4 or vice versa, next node is not junction
      else if ( (startPos == 2 && nextPos == 3) || (startPos == 3 && nextPos == 2) ) { passedJunc = true; } // if starts at 2 and goes 3 or vice versa, next node is not junction
      

      // stuff for parking !!!!
      else if (nextPos == 5) { // node conditions for parking
        passedJunc = true; 

        if (startPos == 1 && parkingFromNode1) { // if going 1 -> 5
          if (currentDir == 1) { motor180(); } // if facing to the right turn around
          passedJunc = false; // will come to that 1 junc
        }


        // when parking from 0/2 while at 1
        else if (startPos == 1 && !parkingFromNode1) { // if passing through 1 while -> 5
          if (routeList[currentRouteNodeIndex - 1] == 0 || routeList[currentRouteNodeIndex - 1] == 4) { motorTurn(1); } // if parking from 0, turn left
          else { motorTurn(0); } // if parking from 2, turn right
          }


        else if (startPos != 3 && startPos != 4) { // if going other nodes -> 5
          //motorTurn(currentDir); // turn left or right based on direction UNCOMMENT THIS FOR ADVANCED PARKING

          //if (currentDir == 0) { motorTurn(0); } // turn right when clockwise
          //else if (currentDir == 1) { motorTurn(1); } // turn left when anti clock

          nextPos = 1; // set next position to 1
          if (startPos == 0 && currentDir == 0) { motor180(); }
          else if (startPos == 2 && currentDir == 1) { motor180(); }
        }

        else if (startPos == 3) { nextPos = 2; if (currentDir == 1) { motor180(); } } // if parking from 3 go to 2 and 180 if need be

        else { nextPos = 0; if (currentDir == 0) { motor180(); } } // if parking from 4 go to 0 and 180 if need be

        motorDrive(speedL, speedR);
        nodeDecLoop();
      }



      if (currentDir != nodeDirection[startPos][nextPos] && nextPos != 5 && routeList[currentRouteNodeIndex] != 5) { motor180(); } // check for correct direction, if not going the right way flip round


    }

    // IN THE CASE AN OBS WAS DETECTED DISREGARD THE NEXT NODE SINCE ITS JUST GONNA BE STARTPOS
    else if (passedJunc == true && returnToStart) {
      returnToStart = true;
    }



    // at junction logic
    else if (passedJunc == false) { // if at a junction

      motorDrive(speedL, speedR);
      nodeDecLoop();

      
      // if going to 1, turn the current direction at the junction
      if (nextPos == 1) { 
        motorTurn(currentDir);
        if (startPos == 0) { currentDir = 0; } // if coming anti-clock from 0, change direction to left
        else if (startPos == 4) { currentDir = 1; } // if coming anti-clock from 4, change direction to right
      } 


      // if going from 1 to other nodes
      else if (startPos == 1) {
        if (nextPos == 2 || nextPos == 4) { motorTurn(1); currentDir = 1; } // if going to 2 or 4, turn left and set the direction to anti clock (1)
        else if (nextPos == 0 || nextPos == 3) { motorTurn(0); currentDir = 0; } // if going to 0 or 3, turn right and set the direction to clockwise (0)
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

  // OBSTACLE DETECTION CONDITIONS
  if (obstacleDetected) {
    motorDrive(0,0);
    if ( ( (startPos == 0 || startPos == 3) && currentDir == 0 ) || ( (startPos == 2 || startPos == 4) && currentDir == 1) ) { // obstacle on the straights 0/4 or 2/3
      nodes[startPos][nextPos] = 0; // set node connection to 0 denoting that there is no direct connection
      nodes[nextPos][startPos] = 0;
      returnToStart = true;
      if (nodes[startPos][1] == 1) { nextPos = 1; }
      else {
        for (int i; nodes[startPos][nextPos] == 0; i++) { // find the next available node to go to
          nextPos = nodes[startPos][i];
        }
      }
    }

    else if (startPos != 1) { // IF GOING FROM A NORMAL  NODE
      if (startPos == 0 || startPos == 2) { nodes[0][2] = 0; nodes[2][0] = 0; } // cancel connection between mirrors 0/2
      else if (startPos == 3 || startPos == 4) { nodes[3][4] = 0; nodes[4][3] = 0; } // cancel connection between mirrors 3/4

      if (passedJunc == false) { // if obstacle is on a bend before a junc
        nodes[startPos][1] = 0; // cancel start node connection to node 1
        nodes[1][startPos] = 0;
        returnToStart = true; // go back whence you came
        passedJunc = true;

        for (int i; nodes[startPos][nextPos] == 0; i++) { // find the next available node to go to
          nextPos = nodes[startPos][i];
        }

      }

      else if (nextPos != 1) { // if obstacle is on a bend after a junc and not going 1
         
        nodes[nextPos][1] = 0; // cancel next node connection to node 1
        nodes[1][nextPos] = 0;
        
        passedJunc = false; // you'll meet a junction again

        if (nodes[startPos][1] == 1) { nextPos = 1; returnToStart = false; } // do not return. 
        else {
          for (int i; nodes[startPos][nextPos] == 0; i++) { // find the next available node to go to
          nextPos = nodes[startPos][i];
          }
          returnToStart = true; // go back whence you came
        }

      }

      else if (nextPos == 1) { // if obstacle is either side of 1
        if (startPos == 0 || startPos == 2) { nodes[0][1] = 0; nodes[1][0] = 0; nodes[2][1] = 0; nodes[1][2] = 0; } // cancel connection between mirrors 0/2 and 1
        else if (startPos == 3 || startPos == 4) { nodes[3][1] = 0; nodes[1][3] = 0; nodes[4][1] = 0; nodes[1][4] = 0; } // cancel connection between mirrors 3/4 and 1

        passedJunc = false;
        nextPos = startPos;
        startPos = 1;



      }

    }



    else if (startPos == 1) { // if starting at node 1
      if (passedJunc = false) { // if pre junctiong obstacle
        if (currentDir == 1) { nodes[0][1] = 0; nodes[1][0] = 0; nodes[2][1] = 0; nodes[1][2] = 0; } // if going right to 0 or 2 block em off
        else if (currentDir == 0) { nodes[3][1] = 0; nodes[1][3] = 0; nodes[4][1] = 0; nodes[1][4] = 0; } // else if going left to 3 or 4 block em off

        passedJunc = true;
        startPos = nextPos;
        nextPos = 1;
        
      }

      else if (passedJunc == true) {
        nodes[1][nextPos] = 0;
        nodes[nextPos][1] = 0;

        if (nextPos == 0 || nextPos == 2) { nodes[0][2] = 0; nodes[2][0] = 0; }
        else if (startPos == 3 || startPos == 4) { nodes[3][4] = 0; nodes[4][3] = 0; }

        passedJunc == false;
        startPos = nextPos;
        nextPos = 1;
        
      }

    }


    motor180();
    
  }

  /*
  if (finalDrive) {
    DistanceValue = analogRead(distAnalogPin);
    if (DistanceValue >= 2250 && lastDist >= 2250) {
      motorDrive(0,0);
      sendArrival(nextPos);
      sendNode(nextPos);
      while (1) {}
    }
    lastDist = DistanceValue;
  }
  */

  if (finalDrive) {
    trigPulse();
    DistanceValue = distanceSense();
    Serial.println("Dis");
    Serial.println(DistanceValue);
    if (DistanceValue <= 800 && DistanceValue > 300 && lastDist <= 800) {
      motorDrive(0,0);
      sendArrival(nextPos);
      sendNode(nextPos);
      while (1) {}
    }
    if (DistanceValue > 300) lastDist = DistanceValue;
  }

  
  if (activeSensors < 2) { error *= 2; } // if only either edge sensors active then double the error for turning




  /*  ADVANCED PARKING STUFF LEAVE FOR NOW, COME BACK LATER FOR THIS JAWN!!!!!!!!!!!!!!!!!!!!!!!

  if ( ((startPos == 3 && error < -4 ) || (startPos == 4 && error > 4)) && nextPos == 5) { parkingFromN34PreFDStage1 = true; } // if on the first part of the bend after node 3/4 flag stage 1

  if (parkingFromN34PreFDStage1 && lineDetected && (error < 5 || error > -5)) { parkingFromN34PreFDStage1 = false; parkingFromN34PreFDStage2 = true; } // if on the straight post first part flag stage 2

  if (parkingFromN34PreFDStage2 && (error > 4 || error < -4)) { // if on the last part of the big bend 
    motorDrive(speedL, speedR);
    delay(1000);
    motorDrive(0,0);
    if (startPos ==  3) { digitalWrite(motor1Phase, HIGH); digitalWrite(motor2Phase, HIGH); }
    else { digitalWrite(motor1Phase, LOW); digitalWrite(motor2Phase, LOW); }
    motorDrive(speedL, speedR);
    delay(500);
    motorDir(0);
    finalDrive = true;

  }
  */

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

