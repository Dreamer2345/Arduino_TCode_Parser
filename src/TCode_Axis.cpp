// TCode-Axis-CPP v1.0,
// protocal by TempestMAx (https://www.patreon.com/tempestvr)
// implemented by Eve 05/02/2022
// usage of this class can be found at (https://github.com/Dreamer2345/Arduino_TCode_Parser)
// Please copy, share, learn, innovate, give attribution.
// Container for TCode Axis's 
// History:
// 

#ifndef TCODE_AXIS_CPP
#define TCODE_AXIS_CPP
#include "TCode_Axis.h"

// Constructor for Axis Class
TCodeAxis::TCodeAxis(){
    rampStartTime = 0;
    rampStart = TCODE_DEFAULT_AXIS_RETURN_VALUE;
    rampStopTime = rampStart;
    rampStop = rampStart;
    axisName = "";
    lastT = 0;
}

// Function to set the axis dynamic parameters
void TCodeAxis::set(int x, char ext, long y) {
  unsigned long t = millis(); // This is the time now
  x = constrain(x,0,9999);
  y = constrain(y,0,9999999);
  // Set ramp parameters, based on inputs
  switch(ext){
    case 'S':{
      rampStart = getPosition();  // Start from current position
      int d = x - rampStart;  // Distance to move
      if (d<0) { d = -d; }
      long dt = d;  // Time interval (time = dist/speed)
      dt *= 100;
      dt /= y; 
      rampStopTime = t + dt;  // Time to arrive at new position
    } break;
    case 'I':{
      rampStart = getPosition();  // Start from current position
      rampStopTime = t + y;  // Time to arrive at new position
    } break;
    default: if(y == 0){
      rampStopTime = t;
      rampStart = x;
    }
  }
  rampStartTime = t;
  rampStop = x;
  lastT = t;
}
  
// Function to return the current position of this axis
int TCodeAxis::getPosition() {
  int x; // This is the current axis position, 0-9999
  unsigned long t = millis(); 
  if (t > rampStopTime) {
    x = rampStop;
  } else if (t > rampStartTime) { 
    x = map(t,rampStartTime,rampStopTime,rampStart,rampStop);
  } else {
    x = rampStart;
  }
  x = constrain(x,0,9999);
  return x;
}

// Function to stop axis movement at current position
void TCodeAxis::stop() {
  unsigned long t = millis(); // This is the time now
  rampStart = getPosition();
  rampStartTime = t;
  rampStop = rampStart;
  rampStopTime = t;
}

bool TCodeAxis::changed() {
  if(lastPosition != getPosition()){
	lastPosition = getPosition();
	return true;
  }
  return false;
}




#endif
