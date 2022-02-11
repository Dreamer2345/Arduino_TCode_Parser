// TCode-Class-CPP v1.0,
// protocal by TempestMAx (https://www.patreon.com/tempestvr)
// implemented by Eve 05/02/2022
// usage of this class can be found at (https://github.com/Dreamer2345/Arduino_TCode_Parser)
// Please copy, share, learn, innovate, give attribution.
// Decodes T-code commands
// It can handle:
//   10x linear channels (L0, L1, L2... L9)
//   10x rotation channels (R0, R1, R2... L9) 
//   10x vibration channels (V0, V1, V2... V9)
//   10x auxilliary channels (A0, A1, A2... A9)
// History:
// 

#ifndef TCODE_CPP
#define TCODE_CPP
#include "TCode.h"




TCode::TCode(String firmware){
  bufferString = "";
  firmwareID = firmware;
  versionID = CURRENT_TCODE_VERSION;
  stop();
  setMessageCallback(NULL);
}

TCode::TCode(String firmware,String TCode_version){
  bufferString = "";
  firmwareID = firmware;
  versionID = TCode_version;
  stop();
  setMessageCallback(NULL);
}


bool TCode::axisChanged(String ID){
  ChannelID decoded_id = TCode::getIDFromStr(ID);
  if (decoded_id.valid) {
    switch(decoded_id.type) {
      // Axis commands
      case 'L': return Linear[decoded_id.channel].changed();
      case 'R': return Rotation[decoded_id.channel].changed();
      case 'V': return Vibration[decoded_id.channel].changed(); 
      case 'A': return Auxiliary[decoded_id.channel].changed(); 
    }
  }
  return false;
}

ChannelID TCode::getIDFromStr(String input){
  char type = input.charAt(0);
  int channel = input.charAt(1) - '0';
  bool valid = true;
  if((channel < 0)||(channel >= TCODE_CHANNEL_COUNT))
    valid = false;
  switch(type){
    case 'L':
    case 'R':
    case 'V':
    case 'A':break;
    default: valid = false;
  }
  return {type,channel,valid};
}


void TCode::axisEasingType(String ID, EasingType e){
  ChannelID decoded_id = TCode::getIDFromStr(ID);
  if(decoded_id.valid){
    switch(decoded_id.type) {
      // Axis commands
      case 'L': Linear[decoded_id.channel].setEasingType(e); break;
      case 'R': Rotation[decoded_id.channel].setEasingType(e); break;
      case 'V': Vibration[decoded_id.channel].setEasingType(e); break;
      case 'A': Auxiliary[decoded_id.channel].setEasingType(e); break;
    }
  }
}

void TCode::inputByte(byte input){
  inputChar(((char)input));
}

void TCode::inputChar(char input){
  bufferString += String((char)toupper(input));  // Add new character to string
  if (input == '\n') {  // Execute string on newline
    bufferString.trim();  // Remove spaces, etc, from buffer
    executeString(bufferString); // Execute string
    bufferString = ""; // Clear input string
  }
}

void TCode::inputString(String input){
  input.toUpperCase();
  bufferString = input + String('\n');
  bufferString.trim();  
  executeString(bufferString);
  bufferString = "";
}

int TCode::axisRead(String inputID){
  int x = TCODE_DEFAULT_AXIS_RETURN_VALUE; // This is the return variable
  ChannelID ID = TCode::getIDFromStr(inputID);
  if (ID.valid) {
    switch(ID.type) {
      // Axis commands
      case 'L': x = Linear[ID.channel].getPosition(); break;
      case 'R': x = Rotation[ID.channel].getPosition(); break;
      case 'V': x = Vibration[ID.channel].getPosition(); break;
      case 'A': x = Auxiliary[ID.channel].getPosition(); break;
    }
  }
  return x;
}

void TCode::axisWrite(String inputID, int magnitude, char extension, long extMagnitude){
  ChannelID ID = TCode::getIDFromStr(inputID);
  if (ID.valid) {
    switch(ID.type) {
      // Axis commands
      case 'L': Linear[ID.channel].set(magnitude,extension,extMagnitude); break;
      case 'R': Rotation[ID.channel].set(magnitude,extension,extMagnitude); break;
      case 'V': Vibration[ID.channel].set(magnitude,extension,extMagnitude); break;
      case 'A': Auxiliary[ID.channel].set(magnitude,extension,extMagnitude); break;
    }
  }
}

unsigned long TCode::axisLastT(String inputID){
  unsigned long t = 0; // Return time
  ChannelID ID = TCode::getIDFromStr(inputID);
  if(ID.valid){
    switch(ID.type) {
      // Axis commands
      case 'L': t = Linear[ID.channel].lastT; break;
      case 'R': t = Rotation[ID.channel].lastT; break;
      case 'V': t = Vibration[ID.channel].lastT; break;
      case 'A': t = Auxiliary[ID.channel].lastT; break;
    }
  }
  return t;
}

void TCode::axisRegister(String inputID, String axisName){
  ChannelID ID = TCode::getIDFromStr(inputID);
  if (ID.valid) {
    switch(ID.type) {
      // Axis commands
      case 'L': Linear[ID.channel].axisName = axisName; break;
      case 'R': Rotation[ID.channel].axisName = axisName; break;
      case 'V': Vibration[ID.channel].axisName = axisName; break;
      case 'A': Auxiliary[ID.channel].axisName = axisName; break;
    }
  }
}


// Function to divide up and execute input string
void TCode::executeString(String input){
  int index = input.indexOf(' ');  // Look for spaces in string
  while (index > 0) {
    readCommand(input.substring(0,index));  // Read off first command
    bufferString = input.substring(index+1);  // Remove first command from string
    index = input.indexOf(' ');  // Look for next space
  }
  readCommand(input);  // Read off last command
}

void TCode::readCommand(String command){
  command.toUpperCase();

  // Switch between command types
  switch( command.charAt(0) ) {
    // Axis commands
    case 'L':
    case 'R':
    case 'V':
    case 'A':
      axisCommand(command);
    break;

    // Device commands
    case 'D':
      deviceCommand(command);
    break;

    // Setup commands
    case '$':
      setupCommand(command);
    break; 
  }
}

bool TCode::isnumber(char c){
  switch(c){
    case '0': 
    case '1': 
    case '2': 
    case '3': 
    case '4': 
    case '5':
    case '6': 
    case '7': 
    case '8': 
    case '9': return true;
    default: return false;
  }
}

bool TCode::extentionValid(char c){
  switch(c){
    case 'I': 
    case 'S': return true;
    default: return false;
  }
}


String TCode::getNextIntStr(String input,int& index){
  String accum = "";
  while(isnumber(getCurrentChar(input,index))){
    accum += getCurrentChar(input,index++);
  }
  return accum;
}

char TCode::getCurrentChar(String input,int index){
  if(index >= input.length()||index < 0)
    return '\0';
  return input[index];
}

void TCode::axisCommand(String input){
  ChannelID ID = TCode::getIDFromStr(input);
  bool valid = ID.valid;
  int channel = ID.channel;
  char type = ID.type;
  int Index = 2;
  int magnitude = 0;
  long extMagnitude = 0;
  
  String magnitudeStr = getNextIntStr(input,Index);
  while(magnitudeStr.length() < 4){magnitudeStr += '0';}
  magnitude = magnitudeStr.toInt();
  magnitude = constrain(magnitude,0,9999);
  if (magnitude == 0 && magnitudeStr.charAt(magnitudeStr.length()-1) != '0') { valid = false; }
  
  char extention = getCurrentChar(input,Index++);
  if(extentionValid(extention)){
    String extMagnitudeStr = getNextIntStr(input,Index);    
    extMagnitude = extMagnitudeStr.toInt();
    extMagnitude = constrain(extMagnitude,0,9999999);
    if (extMagnitude == 0 && extMagnitudeStr.charAt(extMagnitudeStr.length()-1) != '0') { extention = ' ';  valid = false; }
  } else {
    extention = ' ';  
  }
  
  if (valid) {
    switch(type) {
      // Axis commands
      case 'L': Linear[channel].set(magnitude,extention,extMagnitude); break;
      case 'R': Rotation[channel].set(magnitude,extention,extMagnitude); break;
      case 'V': Vibration[channel].set(magnitude,extention,extMagnitude); break;
      case 'A': Auxiliary[channel].set(magnitude,extention,extMagnitude); break;
    }
  }
}

void TCode::stop(){
  for (int i = 0; i < 10; i++) { Linear[i].stop(); }
  for (int i = 0; i < 10; i++) { Rotation[i].stop(); }
  for (int i = 0; i < 10; i++) { Vibration[i].set(0,' ',0); }
  for (int i = 0; i < 10; i++) { Auxiliary[i].stop(); }  
}

void TCode::deviceCommand(String input){
  input = input.substring(1);
  switch(input.charAt(0)){
    case 'S': stop(); break; 
    case '0': sendMessage(firmwareID+'\n'); break;
    case '1': sendMessage(versionID+'\n'); break;
    case '2':
      for (int i = 0; i < 10; i++) { axisRow("L" + String(i), Linear[i].axisName); }
      for (int i = 0; i < 10; i++) { axisRow("R" + String(i), Rotation[i].axisName); }
      for (int i = 0; i < 10; i++) { axisRow("V" + String(i), Vibration[i].axisName); }
      for (int i = 0; i < 10; i++) { axisRow("A" + String(i), Auxiliary[i].axisName); }             
    break;
  }

}

int TCode::getHeaderEnd(){
  return TCODE_EEPROM_MEMORY_OFFSET + TCODE_EEPROM_MEMORY_ID_LENGTH;
}

int TCode::getMemoryLocation(String id){
  ChannelID decoded_id = getIDFromStr(id);
  int memloc = -1;
  if(decoded_id.valid){
    int typeoffset = -1;
    switch(decoded_id.type){
      case 'L': typeoffset = 0; break;
      case 'R': typeoffset = 1; break;
      case 'V': typeoffset = 2; break;
      case 'A': typeoffset = 3; break;
      default: return -2;
    }
    int typebyteoffset = (sizeof(int)*2)*TCODE_CHANNEL_COUNT*typeoffset;
    int entrybyteoffset = (sizeof(int)*2)*decoded_id.channel;
    memloc = typebyteoffset + entrybyteoffset + getHeaderEnd();
  }
  return memloc;
}

bool TCode::checkMemoryKey(){
  char b[TCODE_EEPROM_MEMORY_ID_LENGTH];
  for(int i = 0; i < TCODE_EEPROM_MEMORY_ID_LENGTH; i++)
    b[i] = (char)readEEPROM(TCODE_EEPROM_MEMORY_OFFSET+i);
  return(String(b) == String(TCODE_EEPROM_MEMORY_ID));
}

void TCode::placeMemoryKey(){
  putEEPROM(TCODE_EEPROM_MEMORY_OFFSET,TCODE_EEPROM_MEMORY_ID);
}

void TCode::resetMemory(){
  int headerEnd = getHeaderEnd();
  for(int j = 0; j < TCODE_CHANNEL_TYPES; j++){  
    for(int i = 0; i < TCODE_CHANNEL_COUNT; i++){
      int memloc = ((sizeof(int)*2)*TCODE_CHANNEL_COUNT*j)+((sizeof(int)*2)*i) + headerEnd;      
      putEEPROM(memloc,((int)0));
      putEEPROM(memloc+sizeof(int),((int)0));
    }
  }
}

void TCode::updateSavedMemory(String id,int low,int high){
  ChannelID decoded_id = getIDFromStr(id);
  if(decoded_id.valid){
    int memloc = getMemoryLocation(id);
    if(memloc >= 0){
      low = constrain(low,0,9999);
      high = constrain(high,0,9999); 
      putEEPROM(memloc,low); 
      memloc += sizeof(int); 
      putEEPROM(memloc,high);  
    }
  }
}

void TCode::axisRow(String id,String axisName){
  if(axisName != ""){
    int memloc = getMemoryLocation(id);
    if(memloc >= 0){
      int low, high;
      getEEPROM(memloc,low); 
      memloc += sizeof(int); 
      getEEPROM(memloc,high);
      low = constrain(low,0,9999);
      high = constrain(high,0,9999);
      sendMessage(id+" "+String(low)+" "+String(high)+" "+axisName+"\n");
    } 
  }
}


void TCode::setupCommand(String input){
  int index = 3;
  input = input.substring(1);
  ChannelID decoded_id = getIDFromStr(input);
  bool valid = decoded_id.valid;
  int low = 0;
  int high = 0;
  
  String lowStr = getNextIntStr(input,index);
  low = lowStr.toInt();
  low = constrain(low,0,9999);
  if (low == 0 && lowStr.charAt(lowStr.length()-1) != '0') { valid = false; }
  
  String highStr = getNextIntStr(input,index);
  high = highStr.toInt();
  high = constrain(high,0,9999);
  if (low == 0 && highStr.charAt(highStr.length()-1) != '0') { valid = false; }

  if(valid){
    if(TCODE_USE_EEPROM){
      updateSavedMemory(input,low,high);
      switch (decoded_id.type) {
        case 'L': axisRow("L" + String(decoded_id.channel), Linear[decoded_id.channel].axisName); break;
        case 'R': axisRow("R" + String(decoded_id.channel), Rotation[decoded_id.channel].axisName); break;
        case 'V': axisRow("V" + String(decoded_id.channel), Vibration[decoded_id.channel].axisName); break;
        case 'A': axisRow("A" + String(decoded_id.channel), Auxiliary[decoded_id.channel].axisName); break;             
      }
    }
    else{
      sendMessage("EEPROM NOT IN USE\n");
    }
  }
}

void TCode::defaultCallback(String input){
  if(Serial){
    Serial.print(input);
  }
}

void TCode::setMessageCallback(TCODE_FUNCTION_PTR_T f){
  if(f == NULL){
    message_callback = &defaultCallback;
  }
  else{
    message_callback = f;  
  }
}

void TCode::sendMessage(String s){
  if(message_callback != NULL)
    message_callback(s);
}

//PER BOARD CODE AREA
#ifdef ARDUINO_ESP32_DEV
  void TCode::init(){
    EEPROM.begin(TCODE_EEPROM_SIZE);
    if(!checkMemoryKey() && TCODE_USE_EEPROM){
      placeMemoryKey();
      resetMemory();  
    }
  }
  
  byte TCode::readEEPROM(int idx){
    return EEPROM.read(idx);
  }
  
  void TCode::writeEEPROM(int idx,byte b){
    EEPROM.write(idx,b); 
    EEPROM.commit();   
  }
  
  template< typename T > T &TCode::getEEPROM( int idx, T &t ){
    return EEPROM.get(idx,t);  
  }
  
  template< typename T > void TCode::putEEPROM( int idx, T t ){
    EEPROM.put(idx,t);
    EEPROM.commit();
  }
  
#else //Uses the default arduino methods for setting EEPROM
  void TCode::init(){
    if(!checkMemoryKey() && TCODE_USE_EEPROM){
      placeMemoryKey();
      resetMemory();  
    }
  }
  
  byte TCode::readEEPROM(int idx){
    return EEPROM.read(idx);
  }
  
  void TCode::writeEEPROM(int idx,byte b){
    EEPROM.write(idx,b);  
  }
  
  template< typename T > T &TCode::getEEPROM( int idx, T &t ){
    return EEPROM.get(idx,t);  
  }
  
  template< typename T > void TCode::putEEPROM( int idx, T t ){
    EEPROM.put(idx,t);
  }
#endif


#endif
