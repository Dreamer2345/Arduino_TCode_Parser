//Example project to show how to use the TCode class with the ESP32
//This uses the onboard LED for 

#include "TCode.h"
#define FIRMWAREVERSION "TCode_Serial_Example"

static const int PWMFreq = 5000;
static const int PWMResolution = 10;
static const int MAX_DUTY_CYCLE = (int)(pow(2, PWMResolution) - 1);
static const int PWMCHANNEL = 0;
static const int PWMPIN = 2;

TCode tcode(FIRMWAREVERSION);
int PWMVar,lastPWMVar;

void setup() {
  Serial.begin(115200);
  ledcSetup(PWMCHANNEL, PWMFreq, PWMResolution); 
  ledcAttachPin(PWMPIN, PWMCHANNEL);   
  while(!Serial){}
  delay(500);
  tcode.Init();
  tcode.InputString("D0");
  tcode.InputString("D1");
  tcode.AxisRegister("A0", "PWM");
  tcode.AxisWrite("A0",5000,'I',3000);
  Serial.println("Ready!");
}

void loop() {
  while (Serial.available() > 0) {
    tcode.InputByte(Serial.read());
  }

  PWMVar = tcode.AxisRead("A0");
  if(PWMVar != lastPWMVar){
    lastPWMVar = PWMVar;
    int normalized = map(PWMVar,0,9999,0,MAX_DUTY_CYCLE);  
    ledcWrite(PWMCHANNEL, normalized);
  }
}
