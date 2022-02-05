#ifndef TCODE_AXIS_CPP
#define TCODE_AXIS_CPP
#include "TCode_Axis.h"

// Constructor for Axis Class
Axis::Axis(){
    rampStartTime = 0;
    rampStart = 5000;
    rampStopTime = rampStart;
    rampStop = rampStart;
    axisName = "";
    lastT = 0;
}

// Function to set the axis dynamic parameters
void Axis::Set(int x, char ext, long y) {
  unsigned long t = millis(); // This is the time now
  x = constrain(x,0,9999);
  y = constrain(y,0,9999999);
  // Set ramp parameters, based on inputs
  switch(ext){
    case 'S':{
      rampStart = GetPosition();  // Start from current position
      int d = x - rampStart;  // Distance to move
      if (d<0) { d = -d; }
        long dt = d;  // Time interval (time = dist/speed)
      dt *= 100;
      dt /= y; 
      rampStopTime = t + dt;  // Time to arrive at new position
    } break;
    case 'I':{
      rampStart = GetPosition();  // Start from current position
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
int Axis::GetPosition() {
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
void Axis::Stop() {
  unsigned long t = millis(); // This is the time now
  rampStart = GetPosition();
  rampStartTime = t;
  rampStop = rampStart;
  rampStopTime = t;
}

#endif
