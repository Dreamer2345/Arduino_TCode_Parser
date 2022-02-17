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
#include "TCodeAxis.h"



#ifdef TCODE_HAS_FPU  
float lerp(float start, float stop, float t){
	t = constrain(t,0.0f,1.0f);
	return (((1-t) * start)+(t * stop));
}

float easeIn(float t){
	t = constrain(t,0.0f,1.0f);
	return t * t;
}

float easeOut(float t){
	t = constrain(t,0.0f,1.0f);
	return 1.0 - ((1-t)*(1-t));
}



int mapEaseIn(int in, int inStart, int inEnd, int outStart, int outEnd){
	float t = in - inStart;
	t /= (inEnd - inStart);
	t = easeIn(t);
	t = constrain(t,0.0f,1.0f);
	t *= (outEnd - outStart);
	t += outStart;
	t += 0.5f;
	return (int)t;
}

int mapEaseOut(int in, int inStart, int inEnd, int outStart, int outEnd){
	float t = in - inStart;
	t /= (inEnd - inStart);
	t = easeOut(t);
	t = constrain(t,0.0f,1.0f);
	t *= (outEnd - outStart);
	t += outStart;
	t += 0.5f;
	return (int)t;
}

int mapEaseInOut(int in, int inStart, int inEnd, int outStart, int outEnd){
	float t = in - inStart;
	t /= (inEnd - inStart);
	t = lerp(easeIn(t),easeOut(t),t);
	t = constrain(t,0.0f,1.0f);
	t *= (outEnd - outStart);
	t += outStart;
	t += 0.5f;
	return (int)t;
}
#else
//This is for processors which lack an FPU
#include "TCodeFixed.h"
int32_t lerp(int32_t start, int32_t stop, int32_t t){
	t = constrainQ16(t,0,Q16fromInt(1));
	int32_t tn = subQ16(Q16fromInt(1),t);
	int32_t a = multQ16(tn,start);
	int32_t b = multQ16(t,start);
	return addQ16(a,b);
}

int32_t easeIn(int32_t t){
	t = constrain(t,0,Q16fromInt(1));
	t = multQ16(t,t);
	return t;
}

int32_t easeOut(int32_t t){
	t = constrain(t,0,Q16fromInt(1));
	t = subQ16(Q16fromInt(1),t);
	t = multQ16(t,t);
	return subQ16(Q16fromInt(1),t);
}

int mapEaseIn(int in, int inStart, int inEnd, int outStart, int outEnd){
	int32_t t = Q16fromInt(in - inStart);
	t = divQ16(t,Q16fromInt(inEnd - inStart));
	t = easeIn(t);
	t = constrainQ16(t,0,Q16fromInt(1));
	t = multQ16(t,Q16fromInt(outEnd - outStart));
	t = addQ16(t,Q16fromInt(outStart));
	t = addQ16(t,Q16fromFloat(0.5f));
	return IntfromQ16(t);
}

int mapEaseOut(int in, int inStart, int inEnd, int outStart, int outEnd){
	int32_t t = Q16fromInt(in - inStart);
	t = divQ16(t,Q16fromInt(inEnd - inStart));
	t = easeOut(t);
	t = constrainQ16(t,0,Q16fromInt(1));
	t = multQ16(Q16fromInt(outEnd - outStart),t);
	t = addQ16(t,Q16fromInt(outStart));
	t = addQ16(t,Q16fromFloat(0.5f));
	return IntfromQ16(t);
}

int mapEaseInOut(int in, int inStart, int inEnd, int outStart, int outEnd){
	int32_t t = Q16fromInt(in - inStart);
	t = divQ16(t,Q16fromInt(inEnd - inStart));
	t = lerp(easeIn(t),easeOut(t),t);
	t = constrainQ16(t,0,Q16fromInt(1));
	t = multQ16(Q16fromInt(outEnd - outStart),t);
	t = addQ16(t,Q16fromInt(outStart));
	t = addQ16(t,Q16fromFloat(0.5f));
	return IntfromQ16(t);
}
#endif




// Constructor for Axis Class
TCodeAxis::TCodeAxis(){
	easing = EasingType::LINEAR;
    rampStartTime = 0;
    rampStart = TCODE_DEFAULT_AXIS_RETURN_VALUE;
    rampStopTime = rampStart;
    rampStop = rampStart;
    axisName = "";
    lastT = 0;
}

void TCodeAxis::setEasingType(EasingType e){
	easing = e;
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
	switch(easing){
	  case EasingType::LINEAR: x = map(t,rampStartTime,rampStopTime,rampStart,rampStop); break;
	  case EasingType::EASEIN: x = mapEaseIn(t,rampStartTime,rampStopTime,rampStart,rampStop); break;
	  case EasingType::EASEOUT: x = mapEaseOut(t,rampStartTime,rampStopTime,rampStart,rampStop); break;
	  case EasingType::EASEINOUT: x = mapEaseInOut(t,rampStartTime,rampStopTime,rampStart,rampStop); break;
	  default: x = map(t,rampStartTime,rampStopTime,rampStart,rampStop);
	}
    
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
