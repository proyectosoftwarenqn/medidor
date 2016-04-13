#include <SoftwareSerial.h>
#include "Server.h"
//#include "Emeter.h"
#include "EmonLib.h"  

#define DEBUG true
 
SoftwareSerial esp8266(2,3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                             // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                             // and the RX line from the esp to the Arduino's pin 3

EnergyMonitor emon1; 
double Irms;
int RawValue;
double Voltage;
double Amps;
void setup()
{
  Serial.begin(9600);
  esp8266.begin(115200); // your esp's baud rate might be different
  
  //Server server(2,3);
  //EMonitor emonitor(0,1,1480, 1,111.1,1);

  //ENERGY MONITOR EMONLIB -Cambio aca ahora llamamos a la funcion currentMulti
  //emon1.current(0, 55.0); 

  emon1.currentMulti(0,1,2,55.0);//donde las primeras 3 variables son las entradas analogicas
  
  pinMode(11,OUTPUT);
  digitalWrite(11,LOW);
  
  pinMode(12,OUTPUT);
  digitalWrite(12,LOW);
  
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  
  pinMode(10,OUTPUT);
  digitalWrite(10,LOW);
   
  //server.initAP("EMONITOR","0987654321");

    sendCommand("AT+RST\r\n",2000,false); // reset module
  sendCommand("AT+CWMODE=2\r\n",1000,false); // configure as access point
  sendCommand("AT+CWSAP=\"EMONITOR\",\"0987654321\",5,3\r\n",3000,false);
  sendCommand("AT+CIFSR\r\n",1000,false); // get ip address
  sendCommand("AT+CIPMUX=1\r\n",1000,false); // configure for multiple connections
  sendCommand("AT+CIPSERVER=1,80\r\n",1000,false); // turn on server on port 80     
  
  Serial.println("Server Ready");
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
 int dt = millis();
 //cambiamos aca tambien, ahora llamos a la function emon1.calcIrmsMulti la cual setea las variables de clase IrmsArray
  Irms = emon1.calcIrmsMulti(350);
  dt= millis()-dt;
  Serial.print(dt);
  Serial.print("ms Amp");
  Serial.print(" pahse1=");
  Serial.print(emon1.IrmsArray[0]);   
  Serial.print(" pahse2=");
  Serial.print(emon1.IrmsArray[1]);
  Serial.print(" pahse3=");
  Serial.print(emon1.IrmsArray[2]);
  Serial.println("");
    
 
  
  if(esp8266.available()) // check if the esp is sending a message 
  {
 
    
    if(esp8266.find("+IPD,"))
    {
     delay(1000); // wait for the serial buffer to fill up (read all the serial data)
     // get the connection id so that we can then disconnect
     int connectionId = esp8266.read()-48; // subtract 48 because the read() function returns 
     
                                           // the ASCII decimal value and 0 (the first decimal number) starts at 48
     String content = "";
     if(esp8266.find("/status"))
     {
      content+= Irms;
     
     }
     
     if(esp8266.find("led=")) // advance cursor to "pin="
     {     
     int ledStatus = (esp8266.read()-48); // get first number i.e. if the pin 13 then the 1st number is 1
     
     if(ledStatus==1)
     {
      digitalWrite(13,HIGH);
     }
     if(ledStatus==2)
     {
      digitalWrite(13,LOW);
     }
        
     // build string that is send back to device that is requesting pin toggle
     content = "Led ";
     content += " is ";
     
     if(digitalRead(13)==HIGH)
     {
       content += "ON";
     }
     else
     {
       content += "OFF";
     }
     
     }
     sendHTTPResponse(connectionId,content);
     
     // make close command
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     
     sendCommand(closeCommand,1000,DEBUG); // close connection
    }
  }
}
 
/*
* Name: sendData
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug)
{
    String response = "";
    
    int dataSize = command.length();
    char data[dataSize];
    command.toCharArray(data,dataSize);
           
    esp8266.write(data,dataSize); // send the read character to the esp8266
    if(debug)
    {
      Serial.println("\r\n====== HTTP Response From Arduino ======");
      Serial.write(data,dataSize);
      Serial.println("\r\n========================================");
    }
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}
 
/*
* Name: sendHTTPResponse
* Description: Function that sends HTTP 200, HTML UTF-8 response
*/
void sendHTTPResponse(int connectionId, String content)
{
     
     // build HTTP response
     String httpResponse;
     String httpHeader;
     // HTTP Header
     httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n"; 
     httpHeader += "Content-Length: ";
     httpHeader += content.length();
     httpHeader += "\r\n";
     httpHeader +="Connection: close\r\n\r\n";
     httpResponse = httpHeader + content + " "; // There is a bug in this code: the last character of "content" is not sent, I cheated by adding this extra space
     sendCIPData(connectionId,httpResponse);
}
 
/*
* Name: sendCIPDATA
* Description: sends a CIPSEND=<connectionId>,<data> command
*
*/
void sendCIPData(int connectionId, String data)
{
   String cipSend = "AT+CIPSEND=";
   cipSend += connectionId;
   cipSend += ",";
   cipSend +=data.length();
   cipSend +="\r\n";
   sendCommand(cipSend,1000,DEBUG);
   sendData(data,1000,DEBUG);
}
 
/*
* Name: sendCommand
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendCommand(String command, const int timeout, boolean debug)
{
    String response = "";
           
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
    
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}
