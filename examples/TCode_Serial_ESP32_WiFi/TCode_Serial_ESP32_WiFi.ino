//Example project to show how to use the TCode class with the ESP32
//This uses the onboard LED for 
#include <TCode.h>
#include <WiFi.h>
#define FIRMWAREVERSION "TCode_Wifi_Example"

static const int PWMFreq = 5000;
static const int PWMResolution = 10;
static const int MAX_DUTY_CYCLE = (int)(pow(2, PWMResolution) - 1);
static const int PWMCHANNEL = 0;
static const int PWMPIN = 2;


const char *ssid = "***";
const char *password = "***";
const unsigned port = 8088;

WiFiServer server;
WiFiClient client;
void WifiClientCallback(const String& message){
  if(client){
    if(client.connected()){
      client.print(message);
    }
  }
}

TCode<1> tcode(FIRMWAREVERSION); //creates an instance of the TCode class
int PWMVar;


void updatePins(){
  if(tcode.axisChanged("V0")){ //make sure we are only changing the output if something has happened
    PWMVar = tcode.axisRead("V0"); //Reads back the position state of the axis which we registered
    int normalized = map(PWMVar,0,9999,0,MAX_DUTY_CYCLE);  
    ledcWrite(PWMCHANNEL, normalized); //Sets our pin to an analogue pwm output based on the position of the read axis
  }  
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
      delay(500);
      Serial.print(".");
  }
  Serial.println("Connected");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
  server.begin(port);
  
  ledcSetup(PWMCHANNEL, PWMFreq, PWMResolution); 
  ledcAttachPin(PWMPIN, PWMCHANNEL);   
  tcode.setMessageCallback(&WifiClientCallback);
  tcode.init(); //Initializes the EEPROM and checks for the magic key in memory
  tcode.axisRegister("V0", "PWM"); //Registers our axis Vibrate 0 as PWM
  tcode.axisWrite("V0",5000,'I',3000); //Sets that channel to aproach 5000 in 3 seconds
  Serial.println("Ready!");
}

void loop() {
  client = server.available();
  if(client){
    Serial.println(F("[Client connected]"));
    while (client.connected()) {
      if (client.available()) {
        tcode.inputByte(client.read());
      }
      
      updatePins();
    }
    client.stop();
    tcode.stop();
    updatePins();
    Serial.println(F("[Client disconnected]"));
  }
}
