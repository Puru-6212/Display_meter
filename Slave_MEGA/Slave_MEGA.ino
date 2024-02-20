#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Arduino.h>
#include "HX711.h"

LiquidCrystal_I2C lcd1(0x25,20,4);
LiquidCrystal_I2C lcd2(0x27,20,4);

//load cell parameters
const int LOADCELL_DOUT_PIN = 9;
const int LOADCELL_SCK_PIN = 8;
HX711 scale;
float weight;

//variables for serial transfer of rpm 
char a[7]; 
int i,speed;
int rpmnew;


//voltage variables 
int value;
float voltage;
float R1 = 100000;
float R2 = 5000;

//current variables
float average1;
float average2;
float average3;
float current;

//power in variables
float Pin;

//power out variables
float pout, torque;
 
void setup() {
  Serial.begin(9600);
  lcd1.init();
  lcd1.backlight();
  lcd2.init();
  lcd2.backlight();
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(86);            // this value is obtained by calibrating the scale with known weights
  scale.tare();

}

void loop() {
    if(Serial.available()>0)
    {
    memset(a,'\0',6*sizeof(char));
    Serial.readBytes(a,7);
    speed = atoi(a);

    weight = scale.get_units();          //Fetching value of the weight
    if(weight<0){
      weight=0;
    }
    torque = weight * 9.81 * 0.001* 0.2;  //calculation of torque
    pout = 2* 3.14 * speed * torque / 60; //calculation of Output Power
    lcd();
  }

  currentsense();
  voltagesense();
  Pin = (voltage) * current;   //Calculation of Input Power
  inputpower();
}

void lcd(){
  //Parameter Name
  lcd2.setCursor(0,0);
  lcd2.print("Weight: ");
  lcd2.setCursor(0,1);
  lcd2.print("Torque");
  lcd2.setCursor(0,2);
  lcd2.print("RPM: ");

  //Parameter Unit
  lcd2.setCursor(19,0);
  lcd2.print("g");
  lcd2.setCursor(19,1);
  lcd2.print("N");
  lcd2.setCursor(19,3);
  lcd2.print("W");

  //Parameter Values
  lcd2.setCursor(10,0);
  lcd2.print(weight,1);
  lcd2.setCursor(10,1);
  lcd2.print(torque);

  lcd2.setCursor(7,2);
  if ( speed < 1000000 ) lcd2.print( ' ');
  if ( speed < 100000 ) lcd2.print( ' ');
  if ( speed < 10000 ) lcd2.print( ' ');
  if ( speed < 1000 ) lcd2.print( ' ');
  if ( speed < 100 )  lcd2.print( ' ');
  if ( speed < 10 )   lcd2.print( ' ');
  lcd2.print(speed);

  lcd2.setCursor(0,3);
  lcd2.print("P-out: ");
  lcd2.setCursor(10,3);
  lcd2.print(pout);
}

void currentsense(){
  //current sensor
  float average1 = 0;
  float average2 = 0;
  float average3 = 0;
  for(int i = 0; i < 1000; i++) 
  {
    average1 = average1 + (.044 * analogRead(A0) -3.78) / 1000;   //current value from sensor 1 (ACS712)
    average2 = average2 + (.044 * analogRead(A1) -3.78) / 1000;   //current value from sensor 2 (ACS712)
    average3 = average3 + (.044 * analogRead(A2) -3.78) / 1000;   //current value from sensor 3 (ACS712)
  }
  lcd1.setCursor(0,1);
  lcd1.print("Current: ");
  lcd1.setCursor(10,1);
  current = average1+average2+average3-56; //Total current = adding the values of all three sensors
  lcd1.print(current);
  lcd1.setCursor(19,1);
  lcd1.print("A");
}

void voltagesense(){
  //Voltage code
  value = analogRead(A7);  //Fetched through voltage divider circuit
  voltage = value * (5.0/1024) * ((R1+R2)/R2);  
  lcd1.setCursor(0,0);
  lcd1.print("Voltage: ");
  lcd1.setCursor(10,0);
  lcd1.print(voltage);   
  lcd1.setCursor(19,0);
  lcd1.print("V");

}

void inputpower(){
  // Displaying Power Input 
  lcd1.setCursor(0, 2);
  lcd1.print("P-in: ");
  lcd1.setCursor(12, 2);
  lcd1.print(Pin);
  lcd1.setCursor(19, 2);
  lcd1.print("W");
}