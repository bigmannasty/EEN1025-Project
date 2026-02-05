int AnalogValue[5] = {0,0,0,0,0};
int AnalogPin[5] = {4,5,6,7,15}; // keep 8 free for tone O/P music

/*
BEFORE:
Brown   - pin 4   - row 1
Red     - pin 5   - row 2
Orange  - pin 6   - row 3
Yellow  - pin 7   - row 4
Green   - pin 15  - row 5
*/

void setup()
{
// put your setup code here, to run once:
Serial.begin(9600);
}

void loop() 
{
// put your main code here, to run repeatedly:
int i;
for (i=0;i<5;i++)
 {
AnalogValue[i]=analogRead(AnalogPin[i]);
   
Serial.print(AnalogValue[i]); // This prints the actual analog sensor reading
Serial.print("\t"); //tab over on screen
 if(i==4)
  {
Serial.println(""); //carriage return
delay(600); // display new set of readings every 600mS
  }
 }
}
