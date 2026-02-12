#ifndef ServerCom_h
#define ServerCom_h

extern int routeList[]; // Route as an int array

void connectToWiFi();

//Attempt to connect to server
bool connectToServer();

String readResponse();

int getStatusCode(String& response);

String getResponseBody(String& response);

// Function to send GET request and get route
bool getRoute();

// Function to send POST request when arriving at node
bool sendArrival(int position);

#endif