/*
  Emeter.h - Library for openenergymonitor
  Created by Gonzalo Valdez, Matias Barril 2016
  Private liscence - emeter.com.ar
*/

#ifndef Emeter_h
#define Emeter_h


class EMonitor
{
  public:
    EMonitor(int _inPinV,int _inPinI, int _numberOfSamples, double _VCAL, double _ICAL, double _PHASECAL);

    void measure();

    //Useful value variables
    double realPower,
           apparentPower,
           powerFactor,
           Vrms,
           Irms;

    
  private:
    //Setup variables
    int numberOfSamples;
    //Set Voltage and current input pins
    int inPinV;
    int inPinI;
    //Calibration coeficients
    //These need to be set in order to obtain accurate results
    double VCAL;
    double ICAL;
    double PHASECAL;

    //Sample variables
    int lastSampleV,lastSampleI,sampleV,sampleI;

    //Filter variables
    double lastFilteredV, lastFilteredI, filteredV, filteredI;
    double filterTemp;

    //Stores the phase calibrated instantaneous voltage.
    double calibratedV;

    //Power calculation variables
    double sqI,sqV,instP,sumI,sumV,sumP;

};

#endif