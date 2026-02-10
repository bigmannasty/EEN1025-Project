#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h> 
#include "pitches.h"

// wi-fi details 
char ssid[] = "iot"; 
char password[] = "needlings84wheezily"; 
WiFiClient client;

//Server details
const char server[] = "3.250.38.184";
const int port = 8000;

// read buffer size for HTTP response
#define BUFSIZE 512

//Routing Details
String route = "";
int routeList[] = {0, 0, 0, 0, 0};
int nextPos;


//OLED Display Variables
#define OLED_ADDR 0x3C
const short SCREEN_WIDTH = 128;
const short SCREEN_HEIGHT = 64;
const short I2C_SDA = 8;
const short I2C_SCL = 9;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

//Test Route Configuration : TO BE REMOVED
const short route[] = {0, 1, 3, 2, 5};
short nodeIndex = -1;

//Pin Configurations
const short button = 1;
const short buzzer = 12;


//Button Configuration
int buttonState = LOW;
int lastState = LOW;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;


//Variables for Display
const short arrowX = 48;
const short arrowY = 24;
const short centre = 8;

//Function to draw arrow on display
void drawArrow32x16(int x, int y) {
  const short width = 32;
  const short height = 16;

  const short shaftLength = 20;      // body length
  const short shaftThickness = 6;    // thickness of shaft

  const short centerY = y + height / 2;

  // Shaft (rectangle)
  display.fillRect(x, centerY - shaftThickness / 2,
                   shaftLength, shaftThickness,
                   SSD1306_WHITE);

  // Arrow head (triangle)
  display.fillTriangle(
    x + shaftLength, y,              // top back of head
    x + shaftLength, y + height,     // bottom back of head
    x + width, centerY,              // tip
    SSD1306_WHITE
  );
}

//Test function without animation
void updateUI(const short route[], short nodeIndex)
{
  buzz(route[nodeIndex+1]);
  display.setTextSize(7);
  drawArrow32x16(arrowX, arrowY);
  display.setCursor(4, 8);
  display.print(route[nodeIndex]);
  display.setCursor(86, 8);
  display.print(route[nodeIndex+1]);
  display.display();
  display.clearDisplay();
}

//Function to animate numbers on display
/*
void updateUI(const short route[],short node) {
  buzz(node);
  short scrollVertical = 8;
  while (scrollVertical >= -64)
  {
    display.setTextSize(7);
    drawArrow32x16(arrowX, arrowY);
    display.setCursor(4, scrollVertical);
    display.print(route[node]);
    display.setCursor(86, scrollVertical);
    display.print(route[node+1]);
    display.display();
    delay(5);
    scrollVertical -= 2;     
    display.clearDisplay();
  }
  scrollVertical = 64;
  while (scrollVertical >= 8)
  {
    drawArrow32x16(arrowX, arrowY);
    display.setCursor(4, scrollVertical);
    display.print(route[node+1]);
    display.setCursor(86, scrollVertical);
    display.print(route[node+2]);
    display.display();
    delay(5);
    scrollVertical -= 2;     
    display.clearDisplay();
  }
}
*/

//Function plays Jurrasic Park Theme on buzzer
void theme() {
  tone(buzzer, NOTE_C5);  //C
  delay(250);
  tone(buzzer, NOTE_B5);  //B
  delay(250);
  tone(buzzer, NOTE_C5);  //C
  delay(500);
  tone(buzzer, NOTE_G4);  //G
  delay(500);
  tone(buzzer, NOTE_F4);  //f
  delay(500);

  noTone(buzzer);
  delay(100);

  tone(buzzer, NOTE_C5);  //C
  delay(250);
  tone(buzzer, NOTE_B5);  //B
  delay(250);
  tone(buzzer, NOTE_C5);  //C
  delay(500);
  tone(buzzer, NOTE_G4);  //G
  delay(500);
  tone(buzzer, NOTE_F4);  //f
  delay(500);

  noTone(buzzer);
  delay(100);

  tone(buzzer, NOTE_C5);  //C
  delay(250);
  tone(buzzer, NOTE_B5);  //B
  delay(250);
  noTone(buzzer);
  delay(50);
  tone(buzzer, NOTE_B5);  //B
  delay(250);
  noTone(buzzer);
  delay(50);
  tone(buzzer, NOTE_C5);  //C
  delay(750);

  noTone(buzzer);
  tone(buzzer, NOTE_G4);  //G
  delay(500);
  tone(buzzer, NOTE_C4);  //low c
  delay(500);
  tone(buzzer, NOTE_AS4);  //Bflat
  delay(1250);
  noTone(buzzer);
  delay(500);

  noTone(buzzer);

}

//Function that buzzes for each node passed
void buzz(int nodeIndex) {

  for (int i=0; i<nodeIndex; i++)
  {
    delay(75);
    tone(buzzer, NOTE_A3);
    delay(75);
    noTone(buzzer);
  }
}

//Plays jingle to indicate starting movement
void buzzStart() {

  tone(buzzer, NOTE_D4);
  delay(100);
  tone(buzzer, NOTE_C4);
  delay(100);
  tone(buzzer, NOTE_A5);
  delay(100);
  noTone(buzzer);
}

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

//Attempt to connect to server
bool connectToServer() {
  Serial.print("Connecting to server: ");  
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
  Serial.begin(115200);

  // Start I2C on chosen pins
  Wire.begin(I2C_SDA, I2C_SCL);
  pinMode(button, INPUT_PULLDOWN);
  //attachInterrupt(button, handleButtonISR, FALLING);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 allocation failed");
    while (true);
  }
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 16);
  display.print("START");
  display.display();
  buzzStart();
}

void loop() {
  if (route[nodeIndex] == 5)
  {
    theme();
  }
  updateUI(route, nodeIndex);
}