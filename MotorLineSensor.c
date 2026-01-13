int AnalogValue[5] = { 0, 0, 0, 0, 0 };
int AnalogPin[5] = { 4, 5, 6, 7, 15 };  // keep 8 free for tone O/P music

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {

  //Code will retrieve sensor values continuously
  int i;
  for (i = 0; i < 5; i++) 
  {
    AnalogValue[i] = analogRead(AnalogPin[i]);
    //FOR DEBUGGING
    /*
Serial.print(AnalogValue[i]); // This prints the actual analog sensor reading
Serial.print("\t"); //tab over on screen
 if(i==4)
  {
Serial.println(""); //carriage return
delay(600); // display new set of readings every 600mS
  }
*/
  }
  //If most left sensors are high , turn right
  //Code rn is incorrect! Just adding if statements
  //Speed change based on difference
  if ((AnalogValue[0] && AnalogValue[1]) == 0)
  {

  }
  else if ((AnalogValue[3] && AnalogValue[4]) == 0)
  {

  }
}

/*
TO DO:

1. Introduce speed change based on difference in sensor readings of left sensors and right sensors

*/
