//Initialisation of distance variable and analog pin to read data from
int Distance1Value = 0;
int AnalogPin1 = 16;

int Distance2Value = 0;
int AnalogPin2 = 17;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  //Sets DistanceValue to be the sensor reading
  Distance1Value = analogRead(AnalogPin1);
  Distance2Value = analogRead(AnalogPin2);

  //Prints reading in neat format in serial monitor
  Serial.print("Distance 1:");
  Serial.print(" ");
  Serial.print(Distance1Value);
  Serial.println("");
  delay(200);
  //Serial.println("--------------");
  /*
  Serial.print("Distance 2:");
  Serial.print(" ");
  
  Serial.print(Distance2Value);
  Serial.println("");
  
  */
}

/*
NOTES:

1. Distance sensor seems to range from roughly 55 to 3400, reading gets higher as object is closer to sensor
2. If object is too close, sensor malfunctions and spits out numbers from 50-70
3. When an object is detected and removed, sensor reading oscillates sometimes
*/