// TCode-Axis-H v1.0,
// protocal by TempestMAx (https://www.patreon.com/tempestvr)
// implemented by Eve 05/02/2022
// usage of this class can be found at (https://github.com/Dreamer2345/Arduino_TCode_Parser)
// Please copy, share, learn, innovate, give attribution.
// Container for TCode Axis's 
// History:
// 
#pragma once
#ifndef TCODE_AXIS_H
#define TCODE_AXIS_H
#include "Arduino.h"

#define TCODE_DEFAULT_AXIS_RETURN_VALUE 5000;
#define TCODE_MIN_AXIS_SMOOTH_INTERVAL 3     // Minimum auto-smooth ramp interval for live commands (ms)
#define TCODE_MAX_AXIS_SMOOTH_INTERVAL 100   // Maximum auto-smooth ramp interval for live commands (ms)


enum class EasingType{
	LINEAR,EASEIN,EASEOUT,EASEINOUT,
	NONE,
};

class TCodeAxis{
  public:
    TCodeAxis();
    void set(int x, char ext, long y); // Function to set the axis dynamic parameters
    int getPosition(); // Function to return the current position of this axis
    void stop(); // Function to stop axis movement at current position
	
	bool changed(); //Function to check if an axis has changed since last getPosition
	
	void setEasingType(EasingType e); //Function to set the easing type for get position
	
    String axisName;
    unsigned long lastT;
	EasingType easing;
  private:
	int lastPosition;
    int rampStart;
    unsigned long rampStartTime;
    int rampStop;
    unsigned long rampStopTime;
	int minInterval;
};


#endif
