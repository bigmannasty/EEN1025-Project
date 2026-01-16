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
