// TCode-Class-H v1.0,
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

#ifndef TCODE_H
#define TCODE_H
#include "Arduino.h"
#include <EEPROM.h>
#include "TCodeAxis.h"

#define TCODE_CHANNEL_TYPES 4
#define TCODE_CHANNEL_COUNT 10
#define CURRENT_TCODE_VERSION "TCode v0.4"

#ifndef TCODE_EEPROM_MEMORY_OFFSET
#define TCODE_EEPROM_MEMORY_OFFSET 0
#endif

#define TCODE_EEPROM_MEMORY_ID "TCODE"
#define TCODE_EEPROM_MEMORY_ID_LENGTH 5
#define TCODE_EEPROM_SIZE TCODE_CHANNEL_COUNT*TCODE_CHANNEL_TYPES*8 + TCODE_EEPROM_MEMORY_ID_LENGTH


#ifndef TCODE_USE_EEPROM
#define TCODE_USE_EEPROM true
#endif



struct ChannelID{
  char type;
  int channel;
  bool valid;
};

typedef void (*TCODE_FUNCTION_PTR_T)(String input);

class TCode{
  public:
    TCode(String firmware); // Constructor for class using defined TCode Version number
    TCode(String firmware,String TCode_version); // Constructor for class using user defined TCode Version number
    static ChannelID getIDFromStr(String input); // Function to convert string ID to a channel ID Type and if it is valid
    void inputByte(byte input); // Function to read off individual byte as input to the command buffer
    void inputChar(char input); // Function to read off individual char as input to the command buffer
    void inputString(String input); // Function to take in a string as input to the command buffer
    int axisRead(String ID); // Function to read the current position of an axis
    void axisWrite(String ID,int magnitude,char ext, long extMagnitude); // Function to set an axis
    unsigned long axisLastT(String ID); // Function to query when an axis was last commanded
    void axisRegister(String ID,String Name); // Function to name and activate axis
	bool axisChanged(String ID); //Function to check if an axis has changed
	void axisEasingType(String ID, EasingType e); //Function to set the easing type of an axis;

	
    void stop(); //Function stops all outputs

    void setMessageCallback(TCODE_FUNCTION_PTR_T function); //Function to set the used message callback this can be used to change the method of message transmition (if NULL is passed to this function the default callback will be used)
    void sendMessage(String s); //Function which calls the callback (the default callback for TCode is Serial communication)

    void init(); //Initalizes the EEPROM and checks for the magic string
    
  private:
    String versionID;
    String firmwareID;
    String bufferString;
    TCodeAxis Linear[TCODE_CHANNEL_COUNT];
    TCodeAxis Rotation[TCODE_CHANNEL_COUNT];
    TCodeAxis Vibration[TCODE_CHANNEL_COUNT];
    TCodeAxis Auxiliary[TCODE_CHANNEL_COUNT];

    TCODE_FUNCTION_PTR_T message_callback;

    static void defaultCallback(String input); // Function which is the default callback for TCode this uses the serial communication if it is setup with Serial.begin() if it is not setup then nothing happens

    void axisRow(String id,String name); // Function to return the details of an axis stored in the EEPROM

    bool isnumber(char c); //Function which returns true if a char is a numerical
    String getNextIntStr(String input,int& index); //Function which goes through a string at an index and finds the first integer string returning it
    char getCurrentChar(String input,int index); //Function which gets a char at a location and returns '\0' if over/underflow
    bool extentionValid(char c); //Function checks if a char is a valid extention char

    void executeString(String input); // Function to divide up and execute input string
    void readCommand(String command); // Function to process the individual commands
    void axisCommand(String command); // Function to read and interpret axis commands
    void deviceCommand(String command); // Function to identify and execute device commands
    void setupCommand(String command); // Function to modify axis preference values

    bool checkMemoryKey(); //Function to check if a memory key "TCODE" has been placed in the EEPROM at the location TCODE_EEPROM_MEMORY_OFFSET
    void placeMemoryKey(); //Function to place the memory key at location TCODE_EEPROM_MEMORY_OFFSET
    void resetMemory(); //Function to reset the stored values area of the EEPROM 
    int getHeaderEnd(); //Function to get the memory location of the start of the stored values area
    int getMemoryLocation(String id); //Function to get the memory location of an ID
    void updateSavedMemory(String id,int low, int high); //Function to update the memory location of an id

    byte readEEPROM(int idx); //Function abstracts the EEPROM read command so that it can be redefined if need be for different board types
    void writeEEPROM(int idx,byte b); //Function abstracts the EEPROM write command so that it can be redefined if need be for different board types
    template< typename T > T &getEEPROM( int idx, T &t ); //Function abstracts the EEPROM get command so that it can be redefined if need be for different board types
    template< typename T > void putEEPROM( int idx, T t ); //Function abstracts the EEPROM put command so that it can be redefined if need be for different board types
};







#endif