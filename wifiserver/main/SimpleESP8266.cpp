#include "Arduino.h"
#include "SimpleESP8266.h"

const char CWMODE_1[] = "AT+CWMODE=1";
const char CWMODE_3[] = "AT+CWMODE=3\r\n";
const char CWMODE_CHECK[] = "AT+CWMODE?";
const char CWMODE_OK[] = "+CWMODE:1";
const char ATRST[] = "AT\r\n";
char IPD[] = "+IPD,";
char HTTP_READ[] = "/read";
const char EOL[] = "\r\n";
const bool ESP8266_DEBUG = true;

const int ACTION_UNKNOWN = 0;
const int ACTION_READ = 1; 


SimpleESP8266::SimpleESP8266(int tx, int rx){

    
}

void SimpleESP8266::setSerials(Stream* espSerial, Stream* debugSerial){
    esp8266 = espSerial;
    debug = debugSerial;
    debug->print("Serial OK");
}

void SimpleESP8266::restart(void)
{
  debug->println("Restarting ESP8266");
  bool result = sendATCommand(ATRST, "OK", 1000);
  debug->println("Restart OK");
}

void SimpleESP8266::setCWMODE(uint8_t mode){

}

bool SimpleESP8266::sendATCommand(String command, String response, const int timeout){

  if(ESP8266_DEBUG)
  {
  debug->print("AT Sending -> ");
  debug->println(command);
  if(response!="")
    {
      debug->print("Expected ->");
      debug->println(response);
    }  
  }
  
  esp8266->print(command);
  delay(100);
  long int time = millis();
    
    if(response!=""){
      return readATResponseAndCheck(response, timeout);
    }else{
    readATResponse(timeout);
    }
    
return true;
}

void SimpleESP8266::readATResponse(int timeout){
    char c;
    long int time = millis();
    atResponse = "";
    
    while( (time+timeout) > millis())
    {
      if(esp8266->available()){
      c = esp8266->read();
      atResponse +=c;
      }
    }
    if(ESP8266_DEBUG){
        debug->print("AT Response ->");
        debug->println(atResponse); 
    }
}

bool SimpleESP8266::readATResponseAndCheck(String expected, int timeout){
    char c;
    long int time = millis();
    atResponse = "";
    int indexOfExpected=-1;    
    while( (time+timeout) > millis())
    {
      if(esp8266->available()){
      c = esp8266->read();
      atResponse +=c;
      }
      indexOfExpected = atResponse.indexOf(expected);
      if(indexOfExpected!=-1)
      {
       if(ESP8266_DEBUG)
       {
        debug->print("AT Response Checked ->");
        debug->println(atResponse); 
       }
       return true;
       break;
       }      
    }
       if(ESP8266_DEBUG)
       {
        debug->print("AT Response Fail ->");
        debug->print("Expected ->");
        debug->println(expected);
        debug->print("Received ->");
        debug->println(atResponse); 
       }
 return false;
}


