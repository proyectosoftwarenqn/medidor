#include <SoftwareSerial.h>
#include "SimpleESP8266.h"
#include "EmonLib.h"  
#include <ESP8266wifi.h>
#define DEBUG true
const char EMETER_CORE_VERSION[] = "v0.0.1";
 
SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                           // and the RX line from the esp to the Arduino's pin 3
SimpleESP8266 emeterEsp(1,2);

EnergyMonitor emeterMonitor; 
double Irms;
double w;
double kw;
double kwh; //Counter->kwh
String readResponse;

void setup()
{
  Serial.begin(9600);
  esp8266.begin(9600); // your esp's baud rate might be different
  

  
  emeterMonitor.current(0, 55.0); 
  
  //eMeter wifiConf 
  emeterEsp.setSerials(&esp8266, &Serial);
  Serial.print("Starting eMeter core ");
  Serial.print(EMETER_CORE_VERSION);
  Serial.println();
  Serial.print("Authors: Gonzalo, Valdez & Matias, Barril.");
  Serial.println("==========================================");
  emeterEsp.startStationMode();
  emeterEsp.setSecurity("EMETER_001aaa", "0000453298");
  emeterEsp.startServer("80", "1");
  //-----------------------------------------------------------
}
 
void loop()
{


   float dt = millis();
 
  //cambiamos aca tambien, ahora llamos a la function emon1.calcIrmsMulti la cual setea las variables de clase IrmsArray
  Irms = emeterMonitor.calcIrms(350);//esta funcion realiza el calculo de irms tomando 350 muestras
  
  dt= millis()-dt;
  Serial.print("time=");
  Serial.print(dt);
  Serial.print("ms Irms-pahse1=");
  Serial.print(Irms);
  Serial.print(" AnalogReadOnA0=");
  Serial.println(analogRead(0));

  
  w = 0.95 * 220.0 * Irms;
  
  kw = (w/1000.0);

  //dt from ms -> min

  dt = dt/60000.0;

  //dt from min -> h

  dt = dt/60;
  
  //kwh counter since start
  //TODO: save counter in sdcard.
  kwh += kw/dt;
  

  if(emeterEsp.hasRequest()){
    if(emeterEsp.requestedAction==1){ //read action
    readResponse = "";
    readResponse += w;
    readResponse += ";";
    readResponse += kwh;
    emeterEsp.response(readResponse);
    }
  }
    
  
  
}
 

