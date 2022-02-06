# Arduino_TCode_Parser
arduino T-Code parser

currently the supported boards are the DOIT ESP32 DEVKIT V1 other arduino boards may require editing to support the EEPROM due to how the ESP32 handles its EEPROM by emulating it with a section of flash memory

this is made to simplify the creation of projects using the TCode Protocol documentaion can be [found here](https://stpihkal.docs.buttplug.io/protocols/tcode.html#live-control)

protocol made by tempest [found here](https://www.patreon.com/tempestvr/posts?filters[tag]=Dev%20Blog)

---
## TCode Class 
### Constructors
>```cpp
>TCode(String firmware);
>```
>>This Constructor Sets the internal firmware string which is returned by passing the command `D0` to the class the TCode Version is set to the default value specified by `#define CURRENT_TCODE_VERSION` this value is returned by passing the command `D1`.

>```cpp
>TCode(String firmware,String TCode_version);
>```
>>This Constructor Sets the internal firmware string which is returned by passing the command `D0`. this constructor also allows for the setting of the TCode Version which is returned by passing the command `D1`.


### Methods
>```cpp
>static ChannelID getIDFromStr(String input);
>```
>>This Function is used to get the Channel type and Channel number from a string input where the Channel type can be anything from 'L':Linear, 'R':Rotation, 'V':Vibration, 'A':Auxiliary. and Channel number can be from 0-9 the ChannelID struct also contains a bool which this function sets wether or not the input string has a valid type and Channel number.

>```cpp
>void InputByte(byte input);
>```
>>This Function Appends a byte to the internal buffer this byte is casted to a char before being appended. if a `'\n'` is appended to the buffer then the commands within the buffer are read and executed.

>```cpp
>void InputChar(char input);
>```
>>This Function Appends a char to the internal buffer. if a `'\n'` is appended to the buffer then the commands within the buffer are read and executed.

>```cpp
>void InputString(String input);
>```
>>This Function Sets the internal buffer to the input string and executes that string immediately not waiting for a `'\n'`. commands given to this function do not require a '\n' to execute.

>```cpp
>int AxisRead(String ID);
>```
>>This Function reads the current value of an axis with the id pointed to by the inputted string form ID.

>```cpp
>void AxisWrite(String ID,int magnitude,char ext, long extMagnitude);
>```
>>This Function Sets the internal variables of an axis pointed to by the inputted String ID. magnitude is the strength of the axis this can be in the ranges 0-9999 whereas the extention magnitude can be in the ranges 0-9999999. the char ext changes how the axis interprets the extMagnitude either changing how long it will take to reach the specified value or changing the rate of change between two values at one second.

>```cpp
>unsigned long AxisLastT(String ID);
>```
>>>This Function returns the last time value of the string ID which is stored in the Axis. the last time is the time at which the AxisWrite function was used for that Axis. 

>```cpp
>void AxisRegister(String ID,String Name);
>```
>>This Function Registers a string name to a given axis via the given string ID. this is used when the `D2` command is used to return the saved min and max values for a given axis.

>```cpp
>void Stop();
>```
>>This Function stops all actions which are occuring currently and for the vibration channel sets the strength to 0 this does the same effect as passing the `DSTOP` command.

>```cpp
>void SetMessageCallback(TCODE_FUNCTION_PTR_T function);
>```
>>This function sets the send message callback this can be used to change the method of how the microcontroller talks to the outside world by default it uses Arduino Serial communication. if a null pointer is passed into this function then the default message callback is used.

>```cpp
>void SendMessage(String s);
>```
>>This function calls the send message callback by default this function uses `Serial.print()` to communicate so newlines will have to be added where needed

>```cpp
>void Init();
>```
>>This function checks the EEPROM for the magic key at the location defined by `#define TCODE_EEPROM_MEMORY_OFFSET` if it is not found then it is placed this only occurs if '#define TCODE_USE_EEPROM true' is true 
