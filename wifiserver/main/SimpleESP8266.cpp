#include "Arduino.h"
#include "SimpleESP8266.h"

const char CWMODE_1[] = "AT+CWMODE=1\r\n";
const char CWMODE_2[] = "AT+CWMODE=2\r\n";
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

void SimpleESP8266::startStationMode(){
  wifiMode = 1;
  sendATCommand(CWMODE_1, "OK", 1000);
}

void SimpleESP8266::startAPMode(){
  wifiMode = 2;
  sendATCommand(CWMODE_2, "OK", 1000);
}

void SimpleESP8266::startAPSMode(){
  wifiMode = 3;
  sendATCommand(CWMODE_3, "OK", 1000);
}

void SimpleESP8266::setSecurity(String ssid, String password){
  String CWSAP = "AT+CWSAP=\""+ssid+"\",\""+password+"\",5,3\r\n";
  sendATCommand(CWSAP, "OK", 1000);
}

void SimpleESP8266::startServer(String port, String connections){
  String CIPMUX = "AT+CIPMUX="+connections+"\r\n";
  sendATCommand(CIPMUX, "OK", 1000);
  String CIPSERVER = "AT+CIPSERVER=1,"+port+"\r\n";
  sendATCommand(CIPSERVER, "OK", 1000);
}


bool SimpleESP8266::hasRequest(){
  if(esp8266->available())
  {      
        if(esp8266->find(IPD)){
        int timeout = 500;
        long int time = millis();
        String ipdMsg = "";
        char c;
        while( (time+timeout) > millis())
        {
          //TODO: get out of loop if the esp8266 has read all request
          while(esp8266->available()){
          c = esp8266->read();
          ipdMsg += c;
        }
       }
       int cnnDelimiter = ipdMsg.indexOf(",");
       int readQueryIndex = ipdMsg.indexOf(HTTP_READ);
       currentConnectionId = ipdMsg.substring(0,cnnDelimiter).toInt();

        if(readQueryIndex!=-1){
        requestedAction = ACTION_READ;  
        }else{
         //Auto close connection for unknown
        requestedAction = ACTION_UNKNOWN;
        closeConnection(currentConnectionId);
        }
        return true;
        }
  
  }

return false;
}

void SimpleESP8266::response(String content){
//build http response
 // build HTTP response
     String httpResponse;
     String httpHeader;
     // HTTP Header
     httpHeader = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=UTF-8\r\n"; 
     httpHeader += "Content-Length: ";
     httpHeader += content.length();
     httpHeader += "\r\n";
     httpHeader +="Connection: close\r\n\r\n";
     httpResponse = httpHeader + content + " ";
     sendCIPSEND(currentConnectionId, httpResponse, 300);
     closeConnection(currentConnectionId);
}

void SimpleESP8266::sendCIPSEND(int connectionId, String data, int timeout){
//wait till response or Time Out  
   String cipsend = "AT+CIPSEND=";
   cipsend += connectionId;
   cipsend += ",";
   cipsend +=data.length();
   cipsend +="\r\n";
   esp8266->print(cipsend);

   //wait for ">"
   long int time = millis();
   char c;
   while(time+timeout>millis())
   {
    if(esp8266->available()){
      c = esp8266->read();
      if(ESP8266_DEBUG){
        debug->print(c);
      }
      if(c == '>')
        break;
     } 
   }

   int dataSize = data.length();
   char charData[dataSize];
   data.toCharArray(charData,dataSize);
   esp8266->write(charData,dataSize);
   
   time=millis();
   while(time+timeout>millis())
   {
    if(esp8266->available()){
      c = esp8266->read();
      if(ESP8266_DEBUG){
        debug->print(c);
      }
      
     } 
   }
}

void SimpleESP8266::closeConnection(uint8_t connectionId){
     String closeCommand = "AT+CIPCLOSE="; 
     closeCommand+=connectionId; // append connection id
     closeCommand+="\r\n";
     sendATCommand(closeCommand,"OK",100);
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


