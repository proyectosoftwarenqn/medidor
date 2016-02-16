/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef Server_h
#define Server_h

#include "Arduino.h"
#include <SoftwareSerial.h>

class Server
{
  public:
    Server(int RX, int TX);
    void setDebugMode(boolean debugMode);
    String sendATCommand(String command, const int timeout);
    void initAP(String ssid, String password);
    String requestCMD();
    bool checkUrl(String url);
    String getParam(String name);
    void sendHttpResponse(String content);
    
  private:
    SoftwareSerial* esp8266;
    boolean debug;
    int conectionId;
};

#endif