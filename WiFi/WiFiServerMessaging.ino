#include <WiFi.h> 

// wi-fi details 
char ssid[] = "***"; 
char password[] = "***"; 

WiFiClient client; 

void connectToWiFi() { 

  Serial.print("Connecting to network: "); 
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

void setup() { 
  Serial.begin(9600); 
  delay(1000); 
  connectToWiFi(); 
}
