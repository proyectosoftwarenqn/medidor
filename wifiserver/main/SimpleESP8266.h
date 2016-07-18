/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef SimpleESP8266_h
#define SimpleESP8266_h

#include "Arduino.h"



class SimpleESP8266
{
  public:
    SimpleESP8266(int tx, int rx);
    void setSerials(Stream* espSerial, Stream* debugSerial);
    void restart(void);
    bool sendATCommand(String command, String response, const int timeout);
    void setCWMODE(uint8_t mode);
  private:
    void readATResponse(int timeout);
    bool readATResponseAndCheck(String expected, int timeout);
    Stream* esp8266;
    Stream* debug;
    String atResponse;
};

#endif
