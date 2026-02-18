#include <BLEDevice.h>
#include <SonicSensor.h>


int DistanceValue = 0;
int lastDist = 0;

int detectPin = 46;

int opticalPin1 = 1;
int opticalPin2 = 2;

int opticalActivePin1 = 14;
int opticalActivePin2 = 13;

int opticalActive1 = 0;
int opticalActive2 = 0;

void setup() {
  Serial.begin(9600);

  pinSetup();

  pinMode(detectPin, OUTPUT);
  digitalWrite(detectPin, LOW);

  pinMode(opticalActivePin1, INPUT);
  pinMode(opticalActivePin2, INPUT);

  //pinMode(opticalPin1, INPUT);
  //pinMode(opticalPin2, INPUT);


}

void loop() {



  trigPulse();
  DistanceValue = distanceSense();
  opticalActive1 = digitalRead(opticalActivePin1);
  opticalActive2 = digitalRead(opticalActivePin2);
  
  /*
  Serial.println("Sonic Dis");
  Serial.println(DistanceValue);
  Serial.println("Optical 1 Dis");
  Serial.println(analogRead(opticalPin1));
  Serial.println("Optical 2 Dis");
  Serial.println(analogRead(opticalPin2));

  Serial.println("");
  Serial.println("OPACTIVE");
  Serial.println(opticalActive1);
  Serial.println(opticalActive2);
  Serial.println("");
  */
  
  if (DistanceValue <= 900 && DistanceValue > 300 && lastDist <= 900) {
    digitalWrite(detectPin, HIGH);
  }
  else if ((analogRead(opticalPin1) >= 2000 && opticalActive1 == 1) || (analogRead(opticalPin2) >= 2000 && opticalActive2 == 1)) { 
    digitalWrite(detectPin, HIGH);
  }
  else {digitalWrite(detectPin, LOW);}
  if (DistanceValue > 300) lastDist = DistanceValue;

  delay(40);

}
