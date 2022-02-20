//Example project to show how to use the TCode class with arduino boards
//This uses the onboard LED for 
#include <TCode.h>

#define FIRMWAREVERSION "TCode_Serial_Example"

static const int ANALOGPIN = 2;

TCode<1> tcode(FIRMWAREVERSION); //creates an instance of the TCode class
int AnalogVar;

void setup() {
  Serial.begin(115200);
  pinMode(ANALOGPIN,OUTPUT);
  while(!Serial){}
  tcode.init(); //Initializes the EEPROM and checks for the magic key in memory
  tcode.axisRegister("V0", "PWM"); //Registers our axis Vibrate 0 as PWM
  tcode.axisWrite("V0",5000,'I',3000); //Sets that channel to aproach 5000 in 3 seconds
  Serial.println("Ready!");
}

void loop() {
  while (Serial.available() > 0) {
    tcode.inputByte(Serial.read()); //Reads a byte from the serial datastream and passes it to the TCode object
  }

   
  if(tcode.axisChanged("V0")){ //make sure we are only changing the output if something has happened
    AnalogVar = tcode.axisRead("V0"); //Reads back the position state of the axis which we registered
    int normalized = map(AnalogVar,0,9999,0,255);  
    analogWrite(ANALOGPIN, normalized); //Sets our pin to an analogue pwm output based on the position of the read axis
  }
}
