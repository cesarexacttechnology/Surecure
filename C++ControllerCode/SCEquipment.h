//
//  SCEquipment.h
//  PlantManager
//
//  Created by Mark Davis on 5/18/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#ifndef __PlantManager__SCEquipment__
#define __PlantManager__SCEquipment__

#include <stdio.h>
#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <thread>
#include "SCSystems.h"

using namespace std;

// Might want to create a Temperature class.
// The class would have:
// an index
// sensor type
// storage interval?
// last valid temperature
// time of last valid temperature.
// ID
// temperatures for averaging
// a storage function.
// maturity function?
// temperature retreival function.


namespace SCClasses {

class SCEquipment {
public:
    /*! The default constructor. */
    SCEquipment();
    /*! The constructor that takes an SCSystem as a parameter. */
    SCEquipment(SCSystem sys);
    /*! The default destructor. */
    ~SCEquipment();
    /*! Is true as long as this sub-system initialized correctly. */
    bool isInitialized;
    /*! Is true as long as this sub-system's hardware is operating. */
    bool isOperating;
    /*! Is true as long as operatingEquipment function is running. */
    bool isOperatingEquipment;
    /*! When set to true, this sub-system is to shut-down and restart. */
    bool isRequestedToSignOn;
    /*! Indicates the configuration has successfully been read. */
    bool isConfigurationRead;
    /*! As long as it is true, the OperateEquipment funtion runs. */
    bool isOperationAllowed;
    /*! Indicates a relay test has been in progress and the relays have been switched to test. */
    bool isARelayTesting;
    /*! Sets the relay to test.
     * \param relayDex The index of the relay with respect to the whole system. 0 is the first relay.*/
    void setTestRelay(int relayDex);
    /*! \returns The index of the relay being tested. */
    unsigned int getTestRelay();
    /*! Sets the subsystem this equipment is connected to.
     * \param sys The subsystem this equipment is connected to. */
    void setSubSystem(SCSystem sys);
    /*! \returns The subsystem associated to this equipment. */
    SCSystem getSubSystem();
    /*! \returns The number of consecutive errors before this subsystem is shut down and restarted. */
    int getErrorsBeforeRestart();
    /*! \returns The time when the susbsystem containing this hardware was initialized. */
    chrono::time_point<chrono::system_clock> getTimeInitialized();
    /*! Sets the time initialized to the current time. */
    void setTimeInitialized();
    /*! Sets the time this sub-system was last initialized.
     *  \param iniTime The time when the system was last initialized. */
    void setTimeInitializedTo(chrono::time_point<chrono::system_clock> iniTime);
    /*! Sets the temperature at the given index for the master array of temepratures.
     * \returns true if the inDex was valid for this sub-system.
     * \param inDex The zero based sub-system index of the channel whose temperature is being set.
     * \param temperature The value of the temperature being set. */
    bool setTemperature(unsigned int inDex, float temperature);
    /*! \returns The temperature for the given input.
     * \param The desired input's index(referenced to the its subsystem, zero based). */
    float getTemperature(unsigned int nDex);
    /*! Contains an error message which provides additional information if a function fails. */
    string errorMessage;
    /*! Set the path to the equipment configurations files. */
    void setFilePath(string filePath);
    /*! Is true if the plant is running in artificial mode. */
    static bool isInArtificialMode;
    /*! Increments the consecutive errors. If this was not a consecutive error, the consecutive error count is reset.
     *  As of 6/14/2017 this does not seem like a good idea. A socket seems to work or either not work. */
    void incrementConsecutiveErrors();
    /*! Is true if temperatures have been measured at least one time. */
    bool isTemperatureMesurementComplete;
    /*! Indicates that the list of relays for this sub-system has successfully been created. */
    bool isRelaysAssigned;
    /*! true indicates the alarm relay should be turned on. */
    bool shouldAlarmBeOn;
    /*! Is used by callers as a temporary value during alarm calculations. */
    bool doesAlarmWantOn;
    /*! Contains a pointer to the plant's inputs. */
    static SCInputs* plantInputs;
    /*! Contains a pointer to the plant's relays. */
    static Relays* plantRelays;
    /*! Writes a message to the console and adds a time stamp.
     *  \param note The message to send to the console. */
    void toLog(string note);

protected:
    /*! Contains the time when the PC last unsuccessfully communicated with the IO hardware on this sub-system. */
    chrono::time_point<chrono::system_clock> timeOfLastFailure_;
    /*! Contains the time when the PC last successfully communicated with the IO hardware on this sub-system. */
    chrono::time_point<chrono::system_clock> timeOfLastSuccess_;
    /*! The number of consecutive errors that must occur before this susb-system is reinitialized. */
    int errorsBeforeRestart_;
    /*! The number of consecutive errors that have occurred. */
    int nConsecutiveErrors_;
    /*! The time when this sub-system was attempted to be initialized. */
    chrono::time_point<chrono::system_clock> timeInitialized_;
    /*! The relay to test. -1 indicates no relay's are being tested. 0 is the first relay of the whole system.
     * The relays on this sub-system must be turned off when a relay on another sub-system is being tested. */
    unsigned int testRelayDex_;
    /*! Contains the date/time when relay testing began. */
    chrono::time_point<chrono::system_clock> testRelayStartTime_;
    /*! The current temperature for each thermal input. */
    vector<float> temperature_;
    /*! The sub-system configuration for this hardware. */
    SCSystem subSystem;
    /*! The location of the files holding the equipment configurations. */
    string filePath_;
};
    
}
#endif /* defined(__PlantManager__SCEquipment__) */
