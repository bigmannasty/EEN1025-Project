const short distAnalogPin = 16;
const short trigPin = 17;
short lineValue[5];
const short lineSensePin[5] = { 4, 5, 6, 7, 15 };
const short WHITE_THRESHOLD = 500;

void setup() {
  Serial.begin(9600);
  pinMode(distAnalogPin, INPUT);
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
}

void loop() {
  // trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long distanceValue = pulseIn(distAnalogPin, HIGH, 30000); // 30ms ~ 5m max
  Serial.println(distanceValue);
  delay(200);
  /*
  for (short i = 0; i < 5; i++) {
    lineValue[i] = analogRead(lineSensePin[i]);
    Serial.println(lineValue[i]);  // This prints the actual analog sensor reading
    delay(600);          // display new set of readings every 600mS
  }
  */
}
