//Initialisation of distance variable and analog pin to read data from
int DistanceValue = 0;
int AnalogPin = 16;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  //Sets DistanceValue to be the sensor reading
  DistanceValue = analogRead(AnalogPin);

  //Prints reading in neat format in serial monitor
  Serial.print("Distance:");
  Serial.print(" ");
  Serial.print(DistanceValue);
  Serial.println("");
  delay(600);
}

/*
NOTES:

1. Distance sensor seems to range from roughly 55 to 3400, reading gets higher as object is closer to sensor
2. If object is too close, sensor malfunctions and spits out numbers from 50-70
3. When an object is detected and removed, sensor reading oscillates sometimes
*/