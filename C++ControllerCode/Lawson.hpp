//
//  Lawson.hpp
//  PlantManager
//
//  Created by Mark Davis on 11/30/15.
//  Copyright © 2015 Products Engineering. All rights reserved.
//

#ifndef Lawson_hpp
#define Lawson_hpp

#include <stdio.h>
#include "SCEquipment.h"
#include "SerialPort.hpp"

using namespace std;
using namespace SCClasses;

class LawsonEquip : public SCEquipment  {
public:
    /*! The default constructor. */
    LawsonEquip();
    /*! A copy constructor. */
    LawsonEquip(SCClasses::SCSystem sys);
    /*! The default destructor. */
    ~LawsonEquip();
    /*! The main loop to operate lawson devices. */
    static void operateEquipment(LawsonEquip* lawSys);
    /*! \returns The one and only pulse count for this lawson equipment. */
    int getPulseCount();
    /*! Sets the one and only pulse count for this lawson equipment. 
     * \param value The desired pulse count for this lawson equipment. */
    void setPulseCount(int value);
    /*! \returns The current cycle number in the control loop. */
    unsigned int getCycleNum();
    /*! \returns The special voltage measurement. */
    float getVolts();
    /*! \returns The index of the input used to measure special voltages. */
    unsigned int getVoltChnlDex();
    /*! \returns The requied cycle length in mSecs. */
    unsigned int getRequiredCycleLength();
    /*! Sets the index of the input used to measure special voltages.
     * \param value The desired index of the input used to measure special voltages. */
    void setVoltChnlDex(unsigned int value);
    /*! Sets the special voltage read state.
     * \param value The desired value of the special voltage read state. */
    void setVoltRead(unsigned int value);
    /*! \returns The special voltage read state. */
    unsigned int getVoltRead();
    /*! Sets the required cycle length(milliseconds) to allow voltages to settle.
     * \param value The desired value of the required cycle length. */
    void setRequiredCycleLength(unsigned int value);
    /*! Initializes the lawson equipment using a socket.
     * \returns Sets "isInitialized" to true if the initialization was successful. */
    void initialize();
    /*! Initializes the lawson equipment using an RS-232 port.
     * \returns Sets "isInitialized" to true if the initialization was successful. */
    void initialize232();
    /*! \returns A pointer to the response obtained from this sub-system after issuing a command. */
    unsigned char* getCommandResponse();
    /*! \returns The length of the response obtained from this sub-system after issuing a command. */
    unsigned int getCommandResponseLength();
    int MillisecondOfDay();
    /*! 08/04/2020 change the initialization of a lawson system to use request a sign-on and thus use a lawson sub-system thread. */
    bool lawIniChange;

private:
    /*! Contains the most recent pulse count. */
    int pulseCount_;
    /*! Contains the most recent counter reading. This is the raw reading from the meter. It may wrap back to zero. */
    unsigned int pulseCountReading_;
    /*! Contains the modeRegHi for configuring the Model 201.
     *  Mode is normally 0, gain is normally 1, and normal operation, thus _ModeRegHi is 0. */
    unsigned char modeRegHi_;
    /*! Contains the modeRegMid for configuring the Model 201. */
    unsigned char modeRegMid_;
    /*! Contains the modeRegMid for configuring the Model 201. */
    unsigned char modeRegLo_;
    /*! The time when the watchdog state needs to change. */
    chrono::time_point<chrono::system_clock> watchDogTime_;
    /*! The amount of time the watch dog has been in the current state. */
    chrono::duration<float> watchDogTimeSpan_;
    /*! 0 -> WatchDog off (needs to turn on). 1 -> Wait a set of cycles. 2 -> Watchdog on (needs to off). */
    int watchDogState_;
    /*! True if the system needs to be recalibrated. */
    bool calFlag_;
    /*! Contains the index of the first relay in a set of relays that will have their state reinforced. */
    unsigned int checkRelayIndex_;
    /*! Contains the bit representation of the output relays on a micro system. 0xFF is all off. */
    unsigned char microRelayBuffer_;
    /*! Is true while this sub-system is being initialized. */
    bool initializing_;
    /*! A list of the cold junction temperatures for each amplifier. */
    vector<float> coldJunc_;
    /*! A list of the offset voltages for each amplifier. */
    vector<float> offset_;
    /*! The control loop's current cycle number. */
    unsigned int cycleNum_;
    /*! The special voltage measurement. */
    float volts_;
    /*! The index of the input used to measure the special voltage. */
    unsigned int voltChnlDex_;
    /*! The time the caller last asked for a special voltage. This is used to prevent the special voltage state from locking. */
    chrono::time_point<chrono::system_clock> voltageTime_;
    /*! The flag used to read special voltages.
     *  0 -> Not reading.
     *  1 -> Commence the special voltage reading process. Set by an outside application. A special voltage reading will
     *       start after the current cycle is complete. This flag is set to 2 when this condition is encountered.
     *  2 -> A normal measurement is read this cycle. Select the special voltage chnl after making the normal measurement.
     *       Also set this flag to 3 when this condition is encountered. But only after selecting the special voltage chnl.
     *  3 -> A special voltage will be read this cycle. Select the normal chnl after making the special voltage measurement.
     *       Set this flag to 4.
     *  4 -> A normal voltage will be read next cycle. This flag is set to 2 when this condition is encountered. The normal
     *       measurement will then take place.
     *  5 -> Stop reading special voltages. Set by an outside application. Select the normal chnl. Set this flag to zero.
     *  voltChnlDex -> The chnl to read the special voltage. */
    unsigned int voltageRead_;
    /*! The time the current cycle began. */
    chrono::time_point<chrono::system_clock> cycleStartTime_;
    /*! The number of milliseconds required for the voltage to settle. */
    unsigned int requiredCycleLength_;
    /*! The communication check sum between the PC and the Model 201. */
    unsigned char checkSum_;
    /*! The number of times to try waking the model 201. */
    unsigned int wakeUpAttempts_;
    /*! A list of the gain constants. */
    vector<float> gainK_;
    /*! A list of the cold junction offset constants. */
    vector<float> coldK_;
    /*! Contains the number of 35Bs. */
    unsigned int nThermAmps_;
    /*! The number of bytes that must be read from the lawson equipment. The word count is always 3 (3 * 8 = 24 bits).*/
    unsigned int wordCount_;
    /*! The time when the relay test is evaluated. */
    chrono::time_point<chrono::system_clock> timeRelayTested_;
    /*! The amount of time in seconds that a relay has been tested. */
    chrono::duration<float> relayTestSpan_;
    /*! Reads the constants for this sub-system stored in Law#.cfg. # is the sub-system number. 
     *  /returns True is the constants were successfully read. */
    bool readConstants();
    /*! Loads the constants of for this Lawson equipment. Uses 'filePath'.
     * \returns true if the configuration was successfully loaded. */
    bool readConstantsFromDB();
    /*! The command being issued to the this sub-system's Model 201. It must be less than 16 bytes. */
    unsigned char command_[16];
    /*! Receives the response from the model 201 for the last command sent.
     * \param lawSys The lawson system receiving the command.
     * \param respLength The number of unsigned characters received after the 'command_' is sent.
     * \param waitTime The time in milliseconds to wait for the reponse.
     * \returns true If the reponse was successfully received. 
     * The response is in the variable commandResponse_ and can be accessed using getCommandResponse(). */
    bool getResponse(LawsonEquip* lawSys, unsigned int respLength, unsigned int waitTime);
    /*! The result obtained from the Model 201 when a command was issued. */
    unsigned char commandResponse_[16];
    /*! The length of the response from the Model 201 when a command was issued. */
    unsigned int commandResponseLength_;
    /*! Send a two byte command packet to the model 201. 
     * \returns true When the two bytes were sent successfully.
     * \param lawSys The lawson system receiving the command.
     * \param outByte1 The first byte to send the Model 201.
     * \param outByte2 The second byte to send the Model 201. */
    static bool sendPacket(LawsonEquip* lawSys, unsigned char outByte1, unsigned char outByte2);
    /*! Send a two byte command packet to the model 201.
     * \returns true When the two bytes were sent successfully.
     * \param lawSys The lawson system receiving the command.
     * \param outByte1 The first byte to send the Model 201.
     * \param outByte2 The second byte to send the Model 201. */
    static bool sendPacket232(LawsonEquip* lawSys, unsigned char outByte1, unsigned char outByte2);
    /*! \returns True if the Model 201 calibrates correctly.
     *  \param lawSys The lawson system receiving the command. */
    bool calSys(LawsonEquip* lawSys);
    /*! \returns True if the Model 201 calibrates correctly.
     *  \param lawSys The lawson system receiving the command. */
    bool calSys232(LawsonEquip* lawSys);
    /*! Test the Plant Manager's checksum against the model 201's checksum. */
    bool checkSum();
    /*! Is set to true if commands are echoed. */
    bool echo_;
    /*! Uses this equipments serial port to signon to a model 201. */
    bool signon232();
    /*! The serial port used for communication to the model 201 */
    SerialPort thePort_;
    /*! Contains the name of the serial port used by this Lawson subSystem. */
    const char * thePortName_;
    /*! The serial port file descriptor. */
    int portDescriptor_;
    /*! Reads bytes from the serial port.
     * \returns True if at least the number of bytes given have been read within the given time period. Otherwise, returns false.
     * \param lawSys The lawson system being controlled.
     * \param bytesToRead The number of bytes to read from the serial port.
     * \param milliSecondsToRead The number of seconds to read the given number of bytes. */
    static bool getBytes(LawsonEquip* lawSys, unsigned int bytesToRead, int milliSecondsToRead);
    /*! Reads bytes from the serial port.
     * \returns True if at least the number of bytes given have been read within the given time period. Otherwise, returns false
     * \param lawSys The lawson system being controlled.
     * \param bytesToRead The number of bytes to read from the serial port.
     * \param milliSecondsToRead The number of seconds to read the given number of bytes. */
    static bool get232Bytes(LawsonEquip* lawSys, unsigned int bytesToRead, int milliSecondsToRead);
    /*! Empties the serial port's input buffer. 
     * \param lawSys The lawson system being controlled.
     * \returns true if there were no errors emptying the input buffer. */
    static bool emptyInputBuffer(LawsonEquip* lawSys);
    /*! Changes the address bus on the model 201 to select the given chnl.
     * \returns true if there were no errors emptying the input buffer. 
     * \param lawSys The lawson system being controlled.
     * \param chnlNum The number(0-255) of the chnl to select. */
    static bool selectChnl(LawsonEquip* lawSys, unsigned char chnlNum);
    /*! If isRequestedToSignOn is true, this function starts the sign-on process.
     * \param lawSys The lawson system receiving the command. */
    static void processSignOnRequest(LawsonEquip* lawSys);
    /*! If the voltage read flag is set, this function performs the steps to read a special voltage.
     * \param lawSys The lawson system receiving the command. */
    static void processVoltageRead(LawsonEquip* lawSys);
    /*! Processes the offset voltage for each Model 35B.
     * \param lawSys The lawson system receiving the command. */
    static void processOffsets(LawsonEquip* lawSys);
    /*! Processes the cold junction for each Model 35B.
     * \param lawSys The lawson system receiving the command. */
    static void processColdJunctions(LawsonEquip* lawSys);
    /*! Processes the cold junction for each Model 35B.
     * \param lawSys The lawson system receiving the command. */
    static void processTemperatures(LawsonEquip* lawSys);
    /*! Processes turning the relays on for this sub-system.
     * \param lawSys The lawson system receiving the command. */
    static void processTurningRelaysOn(LawsonEquip* lawSys);
    /*! Processes turning the relays off for this sub-system.
     * \param lawSys The lawson system receiving the command. */
    static void processTurningRelaysOff(LawsonEquip* lawSys);
    /*! Processes calibrating this sub-system.
     * \param lawSys The lawson system receiving the command. */
    static void processCalibration(LawsonEquip* lawSys);
    /*! Processes the pulses supplied to this sub-system by a pulse meter.
     * \param lawSys The lawson system receiving the command. */
    static void processPulseCounts(LawsonEquip* lawSys);
    /*! Toggles the watchdog relay on this sub-system.
     * \param lawSys The lawson system receiving the command. */
    static void toggleWatchdog(LawsonEquip* lawSys);
    /*! Sends the given relay command for the Model 188 on for this sub-system.
     * \param lawSys The lawson system receiving the command. 
     * \param command The relay command. 
     * \returns true If the command was sent successfully. */
    static bool sendStandardRelayCommand(LawsonEquip* lawSys, unsigned char command);
    /*! Sends the set of commands to turn on a standard system relay.
     * \param lawSys The lawson system receiving the command.
     * \returns true If the command was sent successfully. */
    static bool sendStandardRelayOnCommand(LawsonEquip* lawSys);
    /*! Sends the set of commands to turn off a standard system relay.
     * \param lawSys The lawson system receiving the command.
     * \returns true If the command was sent successfully. */
    static bool sendStandardRelayOffCommand(LawsonEquip* lawSys);
    /*! Turns off all the relays on this sub-system.
     * \param lawSys The lawson sub-system which will have all of its relays turned off.
     * \returns true If the all the relays were turned off. */
    static bool turnAllRelaysOff(LawsonEquip* lawSys);
    /*! Activates or deactivates the given relay. To activate the given relay, the model 188 must have previously
     * been sent the activate command. Likewise, to deactivate the given relay, the deactivate command must have
     * previously been sent.
     * \param lawSys* The lawson system receiving the command.
     * \param relay* The relay to select.
     * \param relayNo The relay to number is used if the relay pointer is nullptr.
     * \returns true If the select standard relay command was sent successfully. */
    static bool selectStandardRelay(LawsonEquip* lawSys, Relay* relay, unsigned char relayNo = 0);
    /*! Actually turns on or off the given relays in a Micro sub-sstem.
     * \param lawSys The lawson system receiving the command. */
    static void processMicroRelays(LawsonEquip* lawSys);
    /*! \returns The lawson chnl index to be selected next.
     * \param lawSys The lawson system being controlled. */
    static unsigned char nextChnl(LawsonEquip* lawSys);
    /*! \returns Converts the logical chnl# (0-83) into the code used by the 201 for the chnl.
     * \param lawSys The lawson system being controlled. 
     * \param chnl The logical chnl# (0-83). */
    static unsigned char ioChnl(LawsonEquip* lawSys, unsigned char chnl);
    /*! Retrieves the rawest form of data from the Lawson Labs equipment. The count is proportional to the voltage.
     * The channel the count is measured from is selected using selectChnl().
     * \returns The A/D converter's counts for the selected channel.
     * \param lawSys The lawson system being controlled. 
     * \throws A string exception when the counts cannot be obtained. */
    static unsigned int getCounts(LawsonEquip* lawSys);
    /*! \returns The voltage measured for the currently selected chnl.
     * \param lawSys The lawson system being controlled. */
    static float getVoltage(LawsonEquip* lawSys);
    /*! Sets the command to issued to this sub-system's Model 201.
     * \param lawSys The lawson system being controlled.
     * \param value The array of unsigned characters which make up the command. There is no checking for valid commands. */
    static void setCommand(LawsonEquip* lawSys, unsigned char* value);
    /*! Is true if this sub-system's Model 201 has a version 11 chip. */
    bool lawson11;

};

#endif /* Lawson_hpp */
