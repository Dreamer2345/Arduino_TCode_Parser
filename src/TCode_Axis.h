#ifndef TCODE_AXIS_H
#define TCODE_AXIS_H
#include "Arduino.h"

#define TCODE_DEFAULT_AXIS_RETURN_VALUE 5000;

class Axis{
  public:
    Axis();
    void Set(int x, char ext, long y); // Function to set the axis dynamic parameters
    int GetPosition(); // Function to return the current position of this axis
    void Stop(); // Function to stop axis movement at current position
    String axisName;
    unsigned long lastT;
  private:
    int rampStart;
    unsigned long rampStartTime;
    int rampStop;
    unsigned long rampStopTime;
};


#endif
