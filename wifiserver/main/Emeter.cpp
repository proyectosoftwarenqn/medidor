#include "Emeter.h"
#include "Arduino.h"
EMonitor::EMonitor(int _inPinV,int _inPinI, int _numberOfSamples, double _VCAL, double _ICAL, double _PHASECAL)
{
 
    numberOfSamples = _numberOfSamples;
    inPinV = _inPinV;
    inPinI = _inPinI;
    VCAL = _VCAL;
    ICAL = _ICAL;
    PHASECAL = _PHASECAL;
}

void EMonitor::measure()
{
  
for (int n=0; n<numberOfSamples; n++)
{

   //Used for offset removal
   lastSampleV=sampleV;
   lastSampleI=sampleI;
   
   //Read in voltage and current samples.   
   sampleV = analogRead(inPinV);
   sampleI = analogRead(inPinI);
   
   //Used for offset removal
   lastFilteredV = filteredV;
   lastFilteredI = filteredI;
  
   //Digital high pass filters to remove 2.5V DC offset.
   filteredV = 0.996*(lastFilteredV+sampleV-lastSampleV);
   filteredI = 0.996*(lastFilteredI+sampleI-lastSampleI);
   
   //Phase calibration goes here.
   calibratedV = lastFilteredV + PHASECAL * (filteredV - lastFilteredV);
  
   //Root-mean-square method voltage
   //1) square voltage values
   
   sqV= calibratedV * calibratedV;
   
   //2) sum
   sumV += sqV;
   
   //Root-mean-square method current
   //1) square current values
   sqI = filteredI * filteredI;
   //2) sum 
   sumI += sqI;

   //Instantaneous Power
   instP = calibratedV * filteredI;
   //Sum
   sumP +=instP;
}

//Calculation of the root of the mean of the voltage and current squared (rms)
//Calibration coeficients applied. 
Vrms = VCAL*sqrt(sumV / numberOfSamples); 
Irms = ICAL*sqrt(sumI / numberOfSamples); 

//Calculation power values
realPower = VCAL*ICAL*sumP / numberOfSamples;
apparentPower = Vrms * Irms;
powerFactor = realPower / apparentPower;

//Reset accumulators
sumV = 0;
sumI = 0;
sumP = 0;
 
}
