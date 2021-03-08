#include "U8glib.h"
#include<EEPROM.h>

#define red     1
#define amber1  2
#define amber2  3
#define green1  12
#define green2  13 

void voltageFind();
void charging();
void discharge();
void background();
void value1();                                                        //Speed
void value2();                                                        //Odometer
void value3();                                                        //range

U8GLIB_KS0108_128 u8g(8, 9, 10, 11, 4, 5, 6, 7, 18, 14, 15, 17, 16);  // 8Bit Com: D0..D7: 8,9,10,11,4,5,6,7 en=18, cs1=14, cs2=15,di=17,rw=16

unsigned  long  currentMillis;   
unsigned  long  previousMillis = 0;
unsigned  long  previousMillis2 = 0;
const     int   interval = 500;
const     int   intervalSpeed = 100;

int   i,j,p,v,w;
int   sensorValueA5;                                                                    //Potentiometer
int   sensorValueA6;                                                                    //Voltage 60V
int   sensorValueA7;                                                                    //Speed
int   n               = 1000;                                                           //Sampling rate for battery
int   m               = 1000;                                                          //Sampling rate for speed
int   state;                                                                            //For charge and discharge
float voltage1;                                                                         //To store battery 0-5V range value
float voltage2;                                                                         //To store speed 0-5V range value
float sum;                                                                              //To store gross sum
float voltage_60V     = 0;                                                              //To store voltage Value
float average         = 0;                                                              //To store final average voltage
float multiplier;
float averageSpeed = 0;
int   speedValue;
int   num;
int   address = 0;
int   a=0, b=0, c=0, d=0, e=0, f=0;
float range = 0.0;
int zeroVolt = 54;
int fullVolt = 68;
int maxRange = 60;

float distance;
float dist;
float travelled = 0.0, prevtravelled = 0.0;
float travelled1, travelled2;
float lastdist, minus;
volatile bool flag  = HIGH;
volatile bool flag2 = HIGH;

float buf[] = {65.0, 62.8, 60.6, 58.4, 56.2, 54.0 };                                  //Battery edge values

void setup() {
      Serial.begin(115200);
    
      pinMode(red, OUTPUT);
      pinMode(amber1, OUTPUT);
      pinMode(amber2, OUTPUT);
      pinMode(green1, OUTPUT);
      pinMode(green2, OUTPUT);
    
      pinMode(0, OUTPUT);
      
      u8g.setRot180();
      
      EEPROM.get(0, lastdist);
      prevtravelled = lastdist;
      voltageFind();
      range = ((average - zeroVolt) / (fullVolt - zeroVolt)) *  maxRange;
  }

void loop() {
        currentMillis = millis();

        u8g.firstPage(); 
        do{
            background();
          } while( u8g.nextPage() );
        
        Speed();
        
        if (currentMillis - previousMillis >= interval || currentMillis < 100) {
            previousMillis = currentMillis;
            discharge();
          }
  }

void discharge() {
        if (average <= 54 && flag == HIGH) {
          a=b=c=d=f=0;
              if(f < 60) { p = 5; f++; }
              else { p = 4; flag = LOW; }
        }
        else if (flag == HIGH) {
            if(average <= 56.2 && average > 54)
            {
              a=b=c=d=f=0;
              if(e < 120) { p = 5; e++; }
              else { p = 4; }
            }
            else if(average <= 58.4 && average > 56.2)
            {
              a=b=c=e=f=0;
              if(d < 180) { p = 5; d++; }
              else { p = 4; }
            }
            else if(average <= 60.6 && average > 58.4)
            {
              a=b=d=e=f=0;
              if(c < 240) { p = 5; c++; }
              else { p = 4; }
            }
            else if(average <= 62.8 && average > 60.6)
            {
              a=c=d=e=f=0;
              if(b < 240) { p = 5; b++; }
              else { p = 4; }
            }
            else if(average <= 65 && average > 62.8)
            { 
              b=c=d=e=f=0;
              if(a < 300) { p = 6; a++; }
              else { p = 5; }
            }
            else if(average > 65)
            {
              a=b=c=d=e=f=0;
              p = 6;
            }
        }

        switch(p) {
          case 0 :  digitalWrite(red, LOW);
                    digitalWrite(amber1, LOW);
                    digitalWrite(amber2, LOW);
                    digitalWrite(green1, LOW);
                    digitalWrite(green2, LOW);
                    break;
          case 1 :  digitalWrite(amber1, LOW);
                    digitalWrite(amber2, LOW);
                    digitalWrite(green1, LOW);
                    digitalWrite(green2, LOW);
                    digitalWrite(red, HIGH);
                    delay(500);
                    digitalWrite(red, LOW);
                    delay(500);  
                    break;
          case 2 :  digitalWrite(red, HIGH);
                    digitalWrite(amber1, LOW);
                    digitalWrite(amber2, LOW);
                    digitalWrite(green1, LOW);
                    digitalWrite(green2, LOW);
                    break;
          case 3 :  digitalWrite(red, HIGH);
                    digitalWrite(amber1, HIGH);
                    digitalWrite(amber2, LOW);
                    digitalWrite(green1, LOW);
                    digitalWrite(green2, LOW);  
                    break;
          case 4 :  digitalWrite(red, HIGH);
                    digitalWrite(amber1, HIGH);
                    digitalWrite(amber2, HIGH);
                    digitalWrite(green1, LOW);
                    digitalWrite(green2, LOW); 
                    break;
          case 5 :  digitalWrite(red, HIGH);
                    digitalWrite(amber1, HIGH);
                    digitalWrite(amber2, HIGH);
                    digitalWrite(green1, HIGH);
                    digitalWrite(green2, LOW);  
                    break;
          case 6 :  digitalWrite(red, HIGH);
                    digitalWrite(amber1, HIGH);
                    digitalWrite(amber2, HIGH);
                    digitalWrite(green1, HIGH);
                    digitalWrite(green2, HIGH);
                    break;   
        }
}

void charging() {
      if (average < 56.2) {
              for(i = 10; i>5; i--) {
                digitalWrite(i, HIGH);
              }
              delay(200);
              for(i = 10; i>5; i--) {
                digitalWrite(i, LOW);
              }
              delay(200);
        }
        else if(average <= 58.4 && average >= 56.2)
        {
              for(i = 10; i>5; i--) {
                digitalWrite(i, HIGH);
                delay(500);
              }
              for(i = 10; i>5; i--) {
                digitalWrite(i, LOW);
              }
              delay(500);
        }
        else if(average <= 60.6 && average > 58.4)
        {
              digitalWrite(red, HIGH);
              for(i = 9; i>5; i--) {
                digitalWrite(i, HIGH);
                delay(500);
              }
              for(i = 9; i>5; i--) {
                digitalWrite(i, LOW);
              }
              delay(500);
        }
        else if(average <= 62.8 && average > 60.6)
        {
              digitalWrite(red, HIGH);
              digitalWrite(amber1, HIGH);
              for(i = 8; i>5; i--) {
                digitalWrite(i, HIGH);
                delay(500);
              }
              for(i = 8; i>5; i--) {
                digitalWrite(i, LOW);
              }
              delay(500);
        }
        else if(average <= 65 && average > 62.8)
        {
              digitalWrite(red, HIGH);
              digitalWrite(amber1, HIGH);
              digitalWrite(amber2, HIGH);
              for(i = 7; i>5; i--) {
                digitalWrite(i, HIGH);
                delay(500);
              }
              for(i = 7; i>5; i--) {
                digitalWrite(i, LOW);
              }
              delay(500);
        }
        else if(average < 67 && average > 65)
        {
              digitalWrite(red, HIGH);
              digitalWrite(amber1, HIGH);
              digitalWrite(amber2, HIGH);
              digitalWrite(green1, HIGH);
              digitalWrite(green2, HIGH);
              delay(500);
              digitalWrite(green2, LOW);
              delay(500);
        }
        else if(average >= 67)
        {
              digitalWrite(red, HIGH);
              digitalWrite(amber1, HIGH);
              digitalWrite(amber2, HIGH);
              digitalWrite(green1, HIGH);
              digitalWrite(green2, HIGH);
        }
}

void voltageFind() { 
        sum = 0;
        for(i=0; i<n; i++) {
          sensorValueA6 = analogRead(A6);                                                 //Read voltage value
          voltage1 = sensorValueA6 * (5.0 / 1023.0);                                      //Convert to 0-5V
          voltage_60V = 15.96 * voltage1;                                                 //Convert to 0-60V
          sum = sum + voltage_60V;                                                        //Gross value
        }
        average = sum/n;                                                                  //Final average value
}

void background() {

//  u8g.setFont(u8g_font_fub30n);
//  u8g.setFont(u8g_font_osb35n);
//  u8g.drawStr(50, 35,"00"); //speed

  u8g.setFont(u8g_font_fub30n);
  u8g.setPrintPos(70-v, 35); 
  u8g.print((int)averageSpeed);

//  u8g.setFont(u8g_font_fub11);
  u8g.setFont(u8g_font_profont15);
//  u8g.drawStr(54, 56,"00000.0"); //odometer
//  u8g.drawStr(5, 52,"000"); //Range
  u8g.drawStr(5, 18,"00:00");//Time

  u8g.drawLine(41,3,41,60);
  u8g.drawLine(5,32,41,32);
  u8g.drawLine(42,3,42,60);
  u8g.drawLine(41,40,120,40);

  value2();
  value3();

  u8g.setFont(u8g_font_chikita);
//  u8g.setFont(u8g_font_profont12);
  u8g.drawStr(97, 31,"km/h"); //speed
  u8g.drawStr(110, 56,"km"); //odometer
  u8g.drawStr(26, 52,"km"); //Range
  u8g.drawStr(25,25,"pm"); //Time
//  u8g.drawStr(16,15,"RANGE");
}

void Speed() {
  distance = 0;
  float sumSpeed = 0;
  speedValue = 0;
  unsigned long Start = millis();
  for(j=0; j<m; j++) {
      sensorValueA5 = analogRead(A5); //pot
      sensorValueA7 = analogRead(A7);
      multiplier = 0.0244 * sensorValueA5;
      voltage2 = sensorValueA7 * (5.0 / 1023.0);
      speedValue = multiplier * voltage2; 
      sumSpeed = sumSpeed + speedValue;
//      delay(1);
  }
  averageSpeed = sumSpeed/m;
  
  if(averageSpeed > 10) {digitalWrite(0, HIGH); }
  else { digitalWrite(0, LOW); }
  
  value1();
  unsigned long Stop = millis();
  int Time = Stop - Start;
  distance = (averageSpeed * Time * 5)/(1000 * 18);
  dist = dist + (distance/1000);  
  travelled = dist + lastdist;
//  minus = travelled - prevtravelled; 

  range = range - (distance/1000);
//  prevtravelled = travelled;
  
  if (range < 0) {range = 0;}
  if (travelled > 99999) { travelled = 0; }

  if(lastdist != travelled) {
    EEPROM.put(0, travelled);
  } 
}

void value1() {
  if (averageSpeed < 10) { v = 0; }
  else { v = 20; } 
}

void value2() {
  u8g.setFont(u8g_font_profont15);
  if (travelled < 10 ) {
    u8g.drawStr(54, 56,"0000"); //odometer
    u8g.setPrintPos(82, 56);
    u8g.print(travelled); 
  }
  else if(travelled < 100) { 
    u8g.drawStr(54, 56,"000"); //odometer
    u8g.setPrintPos(75, 56);
    u8g.print(travelled);
  }
  else if(travelled < 1000) {
    u8g.drawStr(54, 56,"00"); //odometer
    u8g.setPrintPos(68, 56);
    u8g.print(travelled);  
  }
  else if(travelled < 10000) { 
    u8g.drawStr(54, 56,"0"); //odometer
    u8g.setPrintPos(61, 56);
    u8g.print(travelled); 
  }
  else { 
    u8g.setPrintPos(54, 56);
    u8g.print(travelled); 
  }
}

void value3() {
  if(range < 10 && range >= 0) {
    u8g.drawStr(5, 52,"00"); //Range
    u8g.setPrintPos(19, 52);
    u8g.print((int)range); 
  }
  else if( range < 100) {
    u8g.drawStr(5, 52,"0"); //Range
    u8g.setPrintPos(12, 52);
    u8g.print((int)range); 
  }
  else if( range < 1000) {
    u8g.setPrintPos(5, 52);
    u8g.print((int)range); 
  }
  else {
    u8g.drawStr(5, 52,"000"); //Range
//    range = 0;
//    u8g.setPrintPos(19, 52);
//    u8g.print(range);
  }
}
