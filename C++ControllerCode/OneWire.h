//
//  OneWire.h
//  PlantManager
//
//  Created by Mark Davis on 5/5/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#ifndef __PlantManager__OneWire__
#define __PlantManager__OneWire__

#include <sys/socket.h>
#include "SCSockets.h"
#include "SCEquipment.h"
#include "SCXML.h"

using namespace std;
using namespace SCClasses;

class OneWireDevice {
public:
    /*! The potential types  of onw wire devices. */
    enum OW_Type {
        OW_TYPE_THERMOCOUPLE,
        OW_TYPE_RELAY,
        OW_TYPE_VOLTAGE,
        OW_TYPE_TEMPERATURE,
        OW_TYPE_NONE,
    };
    /*! The default constructor. */
    OneWireDevice();
    /*! The default destructor. */
    ~OneWireDevice();
    /*! Sets this devices type. 
     * \param type This device's type. */
    void setType(OW_Type value);
    /*! \returns This devices type. */
    OW_Type getType();
    /*! Sets this device's pin index. -1 implies the device is not using a pin and therefore not being used.
     * \param value The arduino pin used by this device. */
    void setPin(int value);
    /*! \returns The arduino pin this device uses. */
    int getPin();
    /*! Sets this device's address. ""(null string) implies the device is not using an address and therefore not being used.
     * \param value The address used by this device. */
    void setAddress(string value);
    /*! \returns The this device's address. */
    string getAddress();
    /*! Sets this device's index with respect to the sub-system it is a part.
     * \param value This device's index. Must be less than this sub-system's number of inputs if it is an input.
     * Must be less than this sub-system's number of outputs if it is an output. 0 <= value <= 4095.*/
    void setIndex(int value);
    /*! \returns This device's index with respect to the sub-system it is a part. */
    int getIndex();
    /*! Is true if this device is being used. */
    bool isActive;
    /*! Contains an error message which provides additional information if a function fails. */
    string errorMessage;
    /*! \returns This device's last valid temperature until the number of seconds since the last valid tmeperature
     * exceeds nSecsBeforeOpen. It then returns 503 */
    float getTemperature();
    /*! Sets this device's last valid temperature. Also, sets the last time a valid temperature was sampled to the present time.
     * \param value The last valid temperature. */
    void setLastValidTemperature(float value);
    /*! Sets the number of seconds before this devices shows an invalid high temperature.
     * \param value The number of seconds before this device shows an invalid high temperature. */
    void setNumOfSecsBeforeOpen(int value);
    
private:
    /*! This one wire device's type. */
    OW_Type type_;
    /*! The Pin number on the Arduino board that this One Wire Device is connected to. (0-49 are valid) */
    int pin_;
    /*! This device's address. The address must consist of 12 hex characters. */
    string address_;
    /*! The index within the sub-system containing this device.
     * For expample: If this is the second sub-system and this device measures temperature and the sub-system's first 
     * channel is 16, an index of 2 would refer to the system's 18th input. 0 <= index <= 4095.*/
    int index_;
    /*! The last time when a valid reading for this device's temperature was sampled. */
    chrono::time_point<chrono::system_clock> lastSampleTime_;
    /*! The last valid temperature. */
    float lastValidTemperature_;
    /*! The number of seconds before this device should show an open temperature. */
    int nSecsBeforeOpen_;
};

class Arduino {
public:
    /*! The default constructor. */
    Arduino();
    /*! The default destructor. */
    ~Arduino();
    /*! Sets this arduino's address(not ipAddress).
     * \param value The desired address for this arduino. */
    void setAddress(string value);
    /*! \returns This arduino's address(not ipAddress). */
    string getAddress();
    /*! Is true if the aruino is being used. */
    bool isActive;
    /*! Sets the number of minutes which must elapse before reattempting to communicate with a failed arduino.
     * \param value The number of minute which must elapse before reinitializing communications with this arduino. */
    void setRetryDuration(int value);
    /*! \returns The number of minute which must elapse before reinitializing communications with this arduino. */
    int getRetryDuration();
    /*! Sets the theoretical(approximate) number of seconds between sampling.
     * \param value The number of seconds which must elapse before resampling this arduino. */
    void setSamplePeriod(int value);
    /*! \returns The number of seconds which must pass before resampling this Arduino. */
    int getSamplePeriod();
    /*! Contains an error message which provides additional information if a function fails. */
    string errorMessage;
    /*! Contains the client socket used to communicate with this arduino. */
    SCClientSocket clientSocket;
    /*! Resets the time of day after which reconnection may be attempted. 
     * \param min The number of minutes from the present to reset the reconnection time.
     * Setting min to zero implies the recoonection will be attempted as soon as is reasonable.
     * Setting min to a negative number causes the reconnention time to be incremented by the default retry duration. 
     * The default setting for min is 0. */
    void resetReconnectTime(int min);
    /*! Get the current reconnect time. */
    chrono::time_point<chrono::system_clock> getReconnectTime();
    /*! \returns true If it has been at least sample period seconds since the last sample. */
    bool isOKToMeasureTemperatures();
    /*! Resets the sample time to rigth now. */
    void resetSampleTime();
    /*! The number of consecutive failed attempts to communicate. */
    int failureCount;
    /*! Adds a pin number to the list of pins. 
     * This method will not add a pin number that is already on the list.
     * \param pinNo The number of the pin to add to the list of pins. */
    void addPinNo(int pinNo);
    /*! \returns The number of pins used on this Aruino. */
    int getNumOfPins();
    /*! \returns The pin number with the given index.
     * \param nDex The index of the pin number being sought. */
    int getPinNo(int nDex);
    /*! Opens a client socket to this Arduino. Connects via a socket to this Arduino.
     * \returns true if the socket connection is successfully opened. */
    bool connect();
    /*! Sets the number of seconds before the devices connected to this Arduino show invalid high temperatures.
     * \param value The number of seconds before the devices shows invalid high temperatures. */
    void setNumOfSecsBeforeOpen(int value);
    /*! \returns The number of seconds before devices connected to this Arduino show open when there are communication errors. */
    int getNumOfSecsBeforeOpen();
    /*! Is true if this arduino is connected to a socket. */
    bool isConnected_;
    /*! Writes a message to the console and adds a time stamp.
     *  \param note The message to send to the console. */
    void toLog(string note);

private:
    /*! Contains the address for this Arduino.(not ipAddress) */
    string address_;
    /*! The time period in minutes between attempts to connect to this Arduino's socket (2 mins default). */
    int retryDuration_;
    /*! The time of day after which reconnection can be tried. */
    chrono::time_point<chrono::system_clock> reconnectTime_;
    /*! Contains the pins used on this arduino.
     * The smallest number a pin can have is one. */
    vector<int> pins_;
    /*! Contains the theoretical number of seconds between samples. 
     * The smallest allowed period is 2 seconds. The largest allowed period is 60 * 60 * 4 = 4 hours = 14,400 seconds. */
    int samplePeriod_;
    /*! The time when the temperatures were last sampled. */
    chrono::time_point<chrono::system_clock> sampleTime_;
    /*! The number of seconds before the devices connected to this Arduino should show an open temperature. */
    int nSecsBeforeOpen_;
};

class OneWireEquip : public SCEquipment  {
public:
    /*! The default constructor. */
    OneWireEquip();
    /*! The default destructor. */
    ~OneWireEquip();
    /*! If true, indicates that all the inputs have been measured at least once. 
     * Use this property to avoid setting the alarm too soon.*/
    bool areAllInputsMeasured;
    /*! Sets the port number used for arduino communicaton.
     * \param value A number between 65000 and 65500 inclusive. */
    void setPortNo(int value);
    /*! \returns The port number used for arduino communication. */
    int getPortNo();
    /*! Creates and adds a new arduino with the given address to the list of arduinos.
     * \returns A pointer to the arduino if successful. nullptr on failure.
     * \param ipAddress The IP address of the socket used by the new Arduino.
     * \param address The desired address of the new arduino.
     * \param isActivated true if the new Arduino will be active.
     * \param retryDuration The number of minutes to wait before attempting to communicate with this Arduino after a failure. 
     * \param samplePeriod The number of seconds which must elapse before a re-sampling the Arduino. */
    Arduino* addArduino(string ipAddress, string address, bool isActivated, int retryDuration, int samplePeriod);
    /*! \returns A pointer to the arduino with the given index.
     * \param index The index of the arduino to return. */
    Arduino* getArduino(int index);
    /*! \returns A pointer to the arduino with the given address.
     * \param address The address of the arduino to retrieve. */
    Arduino* getArduino(string address);
    /*! Removes the arduino with the given index.
     * \returns true if the arduino was successfully removed.
     * \param index The index of the arduino to remove. */
    bool removeArduino(int index);
    /*! Removes the arduino with the given address.
     * \returns true if the arduino was successfully removed.
     * \param address The address of the arduino to remove. */
    bool removeArduino(string address);
    /*! Creates and adds a new one wire device with the given parameters to the list of devices.
     * \returns A pointer to the device if successful. nullptr on failure.
     * \param type The type of one wire device.
     * \param address The desired address of the new one wire device. It must be unique.
     * \param index The unique index within this sub-system used to reference this device. */
    OneWireDevice* addDevice(OneWireDevice::OW_Type type, string address, int index);
    /*! \returns A pointer to the device with the given index.
     * \param index The index of the device to return. */
    OneWireDevice* getDevice(int index);
    /*! \returns A pointer to the device with the given address.
     * \param address The address of the device to retrieve. */
    OneWireDevice* getDevice(string address);
    /*! Removes the device with the given index.
     * \returns true if the device was successfully removed.
     * \param index The index of the device to remove. */
    bool removeDevice(int index);
    /*! Removes the device with the given address.
     * \returns true if the device was successfully removed.
     * \param address The address of the device to remove. */
    bool removeDevice(string address);
    /*! \returns The number of one wire devices connected to this arduino. */
    int getNumOfDevices();
    /*! Initializes the one wire equipment. */
    void initialize();
    /*! Saves the configuration of this one wire equipment. Uses 'filePath'.
     * \returns true if the configuration was successfully saved. */
    bool saveConfig();
    /*! Loads the configuration of this one wire equipment. Uses 'filePath'.
     * \returns true if the configuration was successfully loaded. */
    bool loadConfig();
    /*! The main loop to operate one wire devices. */
    static void operateEquipment(OneWireEquip* owSys);
    
private:
    /*! The port number used by any arduino run by this application. 65000 <= portNo_ <= 65500. */
    int portNo_;
    /*! A list of the Arduinos that make up this sub-system. */
    vector<Arduino*> arduinos_;
    /*! A list of the one wire devices which are connected to this sub-system. */
    vector<OneWireDevice*> devices_;
    /*! Attempts to connect to all sockets required to commuicate with all of this sub-system's Arduinos. */
    void connectToSockets();
    /*! Is used to get all the temperatures associated with this one wire equipment. */
    void getTemperatures();
    /*! \returns A two character string containing the hexidecimal representation of the given address.
     * \param address The numeric address to convert to a hexidecimal string. 
     * It must have a value between 0 and 255 to work. */
    string addressToString(int address);
    /*! Converts the given hexadecimal value into an integer.
     * \param hexChar The hexadeciaml character to convert to an integer.
     * \returns The integer value of the given hexadecimal character. If the character is not in the set of 
     * hexadecimal characters, -1 is returned. */
    int hexToInt(char hexChar);
};


#endif /* defined(__PlantManager__OneWire__) */
