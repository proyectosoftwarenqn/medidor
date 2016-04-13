#include "Arduino.h"
#include "Server.h"

Server::Server(int RX, int TX){
    esp8266 = new SoftwareSerial(RX,TX);
    esp8266->begin(115200);
}

void Server::setDebugMode(boolean debugMode)
{
     debug = debugMode;
}
 
String Server::sendATCommand(String command, const int timeout)
{
    String response = "";
           
    esp8266->print(command); // send the read character to the esp8266
    long int time = millis();
    
    while((time+timeout) > millis())
    {
      while(esp8266->available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266->read(); // read the next character.
        
        response+=c;
      } 
       
    }
   
    return response;    
}

 void Server::initAP(String ssid, String password)
 {
  sendATCommand("AT+RST\r\n",2000); // reset module
  sendATCommand("AT+CWMODE=2\r\n",1000); // configure as access point
  sendATCommand("AT+CWSAP=\""+ssid+"\",\""+password+"\",5,3\r\n",3000);
  sendATCommand("AT+CIFSR\r\n",1000); // get ip address
  sendATCommand("AT+CIPMUX=1\r\n",1000); // configure for multiple connections
  sendATCommand("AT+CIPSERVER=1,8080\r\n",1000); // turn on server on port 80     
 }
 
 
boolean Server::checkUrl(String url)
{
    return true;
}

String Server::getParam(String name)
{
    String paramValue ="";
    return paramValue;
}

void Server::sendHttpResponse(String content)
{
    
}
