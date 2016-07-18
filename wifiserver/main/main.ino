#include <SoftwareSerial.h>
#include "SimpleESP8266.h"
//#include "Emeter.h"
#include "EmonLib.h"  
#include <ESP8266wifi.h>
#define DEBUG true
 
SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                           // and the RX line from the esp to the Arduino's pin 3
SimpleESP8266 emeter(1,2);

EnergyMonitor emon1; 
double Irms;
double IrmsSum;
int RawValue;
double Voltage;
double Amps;
void setup()
{
  Serial.begin(9600);
  esp8266.begin(9600); // your esp's baud rate might be different
  
 
  emon1.current(0, 55.0); 
  emeter.setSerials(&esp8266, &Serial);
  emeter.restart();
}
 
void loop()
{

  /*
  emonitor.mesure();
  Serial.print("RP");
  Serial.print(emonitor.realPower);         // Apparent power
  Serial.print("-AP");
  Serial.print(emonitor.apparentPower);         // Apparent power
  Serial.print("-PF");
  Serial.print(emonitor.powerFactor);         // Apparent power
  Serial.println("");
  */

 /* double Irms = emon1.calcIrms(1480);  // Calculate Irms only
  
  Serial.print(Irms*230.0);         // Apparent power
  Serial.print(" ");
  Serial.println(Irms);   
  */
 float dt = millis();
 //cambiamos aca tambien, ahora llamos a la function emon1.calcIrmsMulti la cual setea las variables de clase IrmsArray
  Irms = emon1.calcIrms(350);//esta funcion realiza el calculo de irms tomando 350 muestras
  //Irms *= 220.0; 
   dt= millis()-dt;
  Serial.print("time=");
  Serial.print(dt);
  Serial.print("ms Irms-pahse1=");
  Serial.print(Irms);
  Serial.print(" AnalogReadOnA0=");
  Serial.println(analogRead(0));

  
  IrmsSum += Irms;
    
  
  
}
 

