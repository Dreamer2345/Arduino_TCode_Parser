//Example project to show how to use the TCode class with the ESP32
//This uses the onboard LED for output
#include <TCode.h>
#include <BluetoothSerial.h>
BluetoothSerial SerialBT;

#define FIRMWAREVERSION "TCode_Serial_Example"

static const int PWMFreq = 5000;
static const int PWMResolution = 10;
static const int MAX_DUTY_CYCLE = (int)(pow(2, PWMResolution) - 1);
static const int PWMCHANNEL = 0;
static const int PWMPIN = 2;

TCode<1> tcode(FIRMWAREVERSION); //creates an instance of the TCode class
int PWMVar;

void BluetoothCallback(String in){ //This overwrites the callback for message return
  SerialBT.print(in);
}

void setup() {
  Serial.begin(115200);
  SerialBT.enableSSP();
  SerialBT.begin("ESP32test");
  ledcSetup(PWMCHANNEL, PWMFreq, PWMResolution); 
  ledcAttachPin(PWMPIN, PWMCHANNEL);   
  while(!Serial){}
  tcode.setMessageCallback(BluetoothCallback); //This changes the callback to the one we defined
  tcode.init(); //Initializes the EEPROM and checks for the magic key in memory
  tcode.axisRegister("V0", "PWM"); //Registers our axis Vibrate 0 as PWM
  tcode.axisWrite("V0",5000,'I',3000); //Sets that channel to aproach 5000 in 3 seconds
  Serial.println("Ready!");
}

void loop() {
  while (SerialBT.available() > 0) {
    tcode.inputByte(SerialBT.read()); //Reads a byte from the serial datastream and passes it to the TCode object
  }

   
  if(tcode.axisChanged("V0")){ //make sure we are only changing the output if something has happened
    PWMVar = tcode.axisRead("V0"); //Reads back the position state of the axis which we registered
    int normalized = map(PWMVar,0,9999,0,MAX_DUTY_CYCLE);  
    ledcWrite(PWMCHANNEL, normalized); //Sets our pin to an analogue pwm output based on the position of the read axis
  }
}
