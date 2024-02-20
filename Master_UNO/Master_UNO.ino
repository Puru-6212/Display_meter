#include <Arduino.h>

const int dataIN = 2; //IR sensor INPUT

unsigned long prevmillis; // To store time
unsigned long duration; // To store time difference
unsigned long lcdrefresh; // To store time for lcd to refresh

int rpm; // RPM value

bool currentstate; // Current state of IR input scan
bool prevstate; // State of IR sensor in previous scan

char b[7];
//String c;
void setup()
{
  Serial.begin(9600);
  pinMode(dataIN,INPUT);   
  prevmillis = 0;
  prevstate = LOW;  
}

void loop()
{
 // RPM Measurement
  currentstate = digitalRead(dataIN); // Read IR sensor state
 if( prevstate != currentstate) // If there is change in input
   {
     if( currentstate == HIGH ) // If input only changes from LOW to HIGH
       {
         duration = ( micros() - prevmillis ); // Time difference between revolution in microsecond
         rpm = (60000000/duration); // rpm = (1/ time millis)*1000*1000*60;
         if(rpm<0){
          rpm = 0;
         }
         prevmillis = micros(); // store time for next revolution calculation
       }
   }
  prevstate = currentstate; // store this scan (prev scan) data for next scan
  
  // LCD Display
  if( ( millis()-lcdrefresh ) >= 100 )
    {
      Serial.println(rpm);     
      lcdrefresh = millis(); 

    }

}
