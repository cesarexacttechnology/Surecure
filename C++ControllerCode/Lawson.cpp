//
//  Lawson.cpp
//  PlantManager
//
//  Created by Mark Davis on 11/30/15.
//  Copyright © 2015 Products Engineering. All rights reserved.
//

#include "Lawson.hpp"


#pragma mark - LawsonEquip class implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////

LawsonEquip::LawsonEquip() {
    pulseCount_ = 0;                    // There are no pulses until the meter pulses.
    pulseCountReading_ = 99999999;      // There is no way of know what might be in the counters hardware at the start.
                                        // 99999999 indicates the meter has not been read.
    modeRegHi_ = 0;                     // The modeRegHi_ is always set to zero. (It might be for future use)
    modeRegMid_ = 0;
    modeRegLo_ = 0;
    watchDogTime_ = chrono::system_clock::now();// The Watchdog should change as soon as possible.
    watchDogTimeSpan_ = chrono::seconds(40);// Assume the Watchdog has been in its current state for 40 seconds.
    watchDogState_ = 0;                 // Assume the Watchdog is off and needs to turn on.
    calFlag_ = false;                   // The system doesn't need to be recalibrated yet.
    checkRelayIndex_ = 0;               // Start by reinforcing the first group of relays starting with the first relay.
    microRelayBuffer_ = 0xFF;           // Start a Micro Controller with all the relays off.
    initializing_ = false;              // This sub-system is not initializing yet.
    for (int dex = 0; dex < 6; dex++) { // Go through every possible amplifier,
        coldJunc_.push_back(0.0);       // and set its cold junction temperature to 0.
    }                                   // All possible cold junction temperatures have been initialized.
    for (int dex = 0; dex < 6; dex++) { // Go through every possible amplifier,
        offset_.push_back(0.0);         // and set its offset voltage to 0.
    }                                   // All possible offset voltages have been initialized.
    cycleNum_ = 0;                      // The control loop starts with the zero cycle number.
    volts_ = 0;                         // The special voltage has not been measured. Set it to zero to start.
    voltChnlDex_ = 0;                   // Although 0 is probably not going to be the index of the voltage chnl, it must be set.
    voltageTime_ = chrono::system_clock::now();// The voltage time must be set. So, it will be set to the present time.
    voltageRead_ = 0;                   // Special voltages are not being read.
    cycleStartTime_ = chrono::system_clock::now();// Set the current cycle start time to now.
    requiredCycleLength_ = 290;         // The default required cycle length is 290 milliseconds.
    checkSum_ = 0;                      // the checksum start at 0.
    wakeUpAttempts_ = 20;               // There are up to 20 attempts to wake the Model 201.
    for (int dex = 0; dex < 6; dex++) { // Go through every possible amplifier,
        gainK_.push_back(0.0);          // and set its gain constant to 0.
        coldK_.push_back(0.0);          // Set its cold junction offset constant to 0.
    }                                   // All possible gain & cold junction offset constants have been initialized.
    nThermAmps_ = 0;                    // There are no thermocouple amplifiers until they have been read from the config file.
    wordCount_ = 3;                     // By default, the word count is always 3 (3 * 8 = 24 bits).
    shouldAlarmBeOn = false;            // The alarm should not be turned on until something triggers an alarm.
    timeRelayTested_ = chrono::system_clock::now();// No relay could have been tested before now.
    relayTestSpan_ = chrono::seconds(0);// No time has been spent testing a relay yet.
    echo_ = false;                      // If true, commands are echoed from the model 201.
    portDescriptor_ = -2;               // The port descriptor is set to an illegal value.
    lawson11 = true;                    // Is true when this sub-system has a version 11 chip.
    lawIniChange = true;              // Is true to perform sign-on inside of the thread established for this sub-system
}

LawsonEquip::LawsonEquip(SCClasses::SCSystem sys) {
    pulseCount_ = 0;                    // There are no pulses until the meter pulses.
    pulseCountReading_ = 0;             // There is no way of know what might be in the counters hardware at the start.
    modeRegHi_ = 0;                     // The modeRegHi_ is always set to zero. (It might be for future use)
    modeRegMid_ = 0;
    modeRegLo_ = 0;
    watchDogTime_ = chrono::system_clock::now();// The Watchdog should change as soon as possible.
    watchDogTimeSpan_ = chrono::seconds(40);// Assume the Watchdog has been in its current state for 40 seconds.
    watchDogState_ = 0;                 // Assume the Watchdog is off and needs to turn on.
    calFlag_ = false;                   // The system doesn't need to be recalibrated yet.
    checkRelayIndex_ = 0;               // Start by reinforcing the first group of relays starting with the first relay.
    microRelayBuffer_ = 0xFF;           // Start a Micro Controller with all the relays off.
    initializing_ = false;              // This sub-system is not initializing yet.
    for (int dex = 0; dex < 6; dex++) { // Go through every possible amplifier,
        coldJunc_.push_back(0.0);       // and set its cold junction temperature to 0.
    }                                   // All possible cold junction temperatures have been initialized.
    for (int dex = 0; dex < 6; dex++) { // Go through every possible amplifier,
        offset_.push_back(0.0);         // and set its offset voltage to 0.
    }                                   // All possible offset voltages have been initialized.
    cycleNum_ = 0;                      // The control loop starts with the zero cycle number.
    volts_ = 0;                         // The special voltage has not been measured. Set it to zero to start.
    voltChnlDex_ = 0;                   // Although 0 is probably not going to be the index of the voltage chnl, it must be set.
    voltageTime_ = chrono::system_clock::now();// The voltage time must be set. So, it will be set to the present time.
    voltageRead_ = 0;                   // Special voltages are not being read.
    cycleStartTime_ = chrono::system_clock::now();// Set the current cycle start time to now.
    requiredCycleLength_ = 290;         // The default required cycle length is 290 milliseconds.
    checkSum_ = 0;                      // the checksum start at 0.
    wakeUpAttempts_ = 24;               // There are up to 24 attempts to wake the Model 201.
    for (int dex = 0; dex < 6; dex++) { // Go through every possible amplifier,
        gainK_.push_back(0.0);          // and set its gain constant to 0.
        coldK_.push_back(0.0);          // Set its cold junction offset constant to 0.
    }                                   // All possible gain & cold junction offset constants have been initialized.
    nThermAmps_ = 0;                    // There are no thermocouple amplifiers until they have been read from the config file.
    wordCount_ = 3;                     // By default, the word count is always 3 (3 * 8 = 24 bits).
    shouldAlarmBeOn = false;            // The alarm should not be turned on until something triggers an alarm.
    timeRelayTested_ = chrono::system_clock::now();// No relay could have been tested before now.
    relayTestSpan_ = chrono::seconds(0);// No time has been spent testing a relay yet.
    subSystem = sys;                    // Assign the sub-system assciated to this lawson equipment.
    echo_ = false;                      // If true, commands are echoed from the model 201.
    portDescriptor_ = -2;               // The port descriptor is set to an illegal value.
    lawson11 = true;                    // Is true when this sub-system has a version 11 chip.
    lawIniChange = true;              // Is true to perform sign-on inside of the thread established for this sub-system
}

LawsonEquip::~LawsonEquip() {
}

void LawsonEquip::operateEquipment(LawsonEquip* lawSys) {
    SCSystem sys = lawSys->getSubSystem();  // Get the sub-system which this Model 201 belongs.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    lawSys->toLog("Starting LawsonEquip::operatingEquipment().");
    lawSys->isOperatingEquipment = true;    // The lawson operatingEquipment function is starting.
    try {                                   // Try operating the equipment associated with this lawson sub-system.
        if (lawSys->isInitialized == true) {// Did this sub-system initialize correctly?
            lawSys->isOperating = true;     // Yes. The system is now operating.
        }                                   // The isOperating flag has been set if the system initialized.
        int temperatureCycles = 2 * lawSys->nThermAmps_ + sys.getNumberOfInputs(); // Calculate
                                            // the number of cycles required to process tempeartures.
        while (lawSys->isOperationAllowed == true) {// As long as isOperationAllowed is true this loop runs.
                                            // isOperationAllowed is changed by an outside source when it is
                                            // time to stop this sub-system.
            chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// No. Contains the current date/time.
            chrono::duration<double> timeIntoCycle = now - lawSys->cycleStartTime_;// The time into the control cycle.
            if (timeIntoCycle.count() > (double)lawSys->requiredCycleLength_ / 1000) {
                if (lawSys->isRequestedToSignOn == true) {// Is sign-on being requested?
                    processSignOnRequest(lawSys);// Yes. Try signing on.
                    continue;
                } else if (lawSys->voltageRead_ > 0) {// Should a special voltage be read?
                    processVoltageRead(lawSys);// Yes.
                    // Has a special voltage just been read and now a normal voltage needs to be read?
                    if (lawSys->voltageRead_ == 4 || lawSys->voltageRead_ == 0) {
                        this_thread::sleep_for(chrono::milliseconds(50));// Yes. Wait 50 mSecs for some of the setteling time required.
                        continue;           // The next voltage read will be special. The normal voltage should not be read.
                    }                       // A normal voltage should be read.
                }
                if (lawSys->nThermAmps_ == 0) {//Are there thermocouple amps?
                    continue;               // No. This shouldn't ever be the case.
                } else if (lawSys->cycleNum_ < lawSys->nThermAmps_) {// Is this a cycle to get a Model 35B offset?
                    // The offset is a function of how the amplifier veries from zero.
                    lawSys->processOffsets(lawSys);// Yes. Get the offset for the Model 35B associated with this cycle.
                } else if (lawSys->cycleNum_ < 2 * lawSys->nThermAmps_) {// Is this a cycle to get a Model 35B cold junction?
                    lawSys->processColdJunctions(lawSys);// Yes. Get the cold junction for the Model 35B associated with this cycle.
                } else if (lawSys->cycleNum_ < temperatureCycles) {// Is this a cycle to get a temperature?
                    lawSys->processTemperatures(lawSys);// Yes. Get the temperature associated with this cycle.
                } else if (lawSys->cycleNum_ == temperatureCycles) {// Is this the cycle to turn on relays?
                    lawSys->isTemperatureMesurementComplete = true;// Yes. All the temperatures have been measured at least once.
                    if(sys.getType() == SCSystem::SYSTYPE_STANDARD) {// Is this a standard lawson sub-system?
                        lawSys->processTurningRelaysOn(lawSys);// Yes. Turn on relays which need to be on.
                    } else {            // This must be a lawson Micro.
                        lawSys->processMicroRelays(lawSys);// Turn on or off the relays on a lawson Micro.
                    }                   // Lawson standard relays are on if they should be all lawson micro relays are in the correct state.
                } else if (lawSys->cycleNum_ == temperatureCycles + 1) {// Is this the cycle to turn off relays?
                    if(sys.getType() == SCSystem::SYSTYPE_STANDARD) {// Is this a standard lawson sub-system?
                        lawSys->processTurningRelaysOff(lawSys);// Turn off relays which need to be off.
                    }                   // Lawson standard relays are off if they should be.
                } else if (lawSys->cycleNum_ == temperatureCycles + 2) {// Is this the cycle to calibrate the sub-system?
                    lawSys->processCalibration(lawSys);// Yes. Calibrate the sub-system.
                } else if (lawSys->cycleNum_ == temperatureCycles + 3) {// Is this the cycle to toggle the watchdog and count pulses?
                    lawSys->toggleWatchdog(lawSys);// Yes. Toggle the watchdog.
                    lawSys->processPulseCounts(lawSys);// Update the pulse count reading.
                } // All cycles have been processed.
                lawSys->cycleNum_++;        // Move onto the next cycle.
                if (lawSys->cycleNum_ >= temperatureCycles + 4) {// Did the cycle number exceed the nuber of cycles required?
                    lawSys->cycleNum_ = 0;  // Yes. Reset the cycle number to zero.
                }                           // The cycle number has been set.
                if (lawSys->voltageRead_ == 2) {// Was a normal measurement made and a special voltage needs to be read next cycle?
                    lawSys->voltageRead_ = 3;   // The next cycle should read a special voltage.
                    if(lawSys->selectChnl(lawSys, lawSys->voltChnlDex_)) {// Select the special voltage chnl.
                        lawSys->errorMessage = "Sub-system " + subSysNo + " error: Failed selecting the special voltage channel.";
                        lawSys->toLog(lawSys->errorMessage);// Display the error.
                    }                           // The voltage chnl has been selected.
                } else if (lawSys->voltageRead_ == 0) {// Is the next cycle a normal cycle?
                    if (lawSys->selectChnl(lawSys, lawSys->nextChnl(lawSys)) == false) {// Yes. Could the next chnl be selected?
                        lawSys->errorMessage = "Error selcting the next channel after offset on sub-system " + subSysNo;// No. Create error.
                        lawSys->toLog(lawSys->errorMessage);// Display the error.
                    }                           // The next normal chnl is selected.
                }                               // The next chnl is selected.
                lawSys->cycleStartTime_ = chrono::system_clock::now();// Set the cycle's start time to know when the chnl settles.
                this_thread::sleep_for(chrono::milliseconds(50));// Wait 50 mSecs to allow part of the settling time to elapse.
            }                               // Sufficient time has passed to read the normal voltage.
            this_thread::sleep_for(chrono::milliseconds(50));// Wait 50 mSecs to prevent a loop race.
        }                                   // This is the end of the operation loop.
    }                                       // This is the end of the encasing try statement.
    catch ( ... ) {                         // Were there any errors?
        int subSystemNo = lawSys->getSubSystem().getSubSystemNo();// Yes. Create an error message.
        lawSys->errorMessage = "Exception while operating on lawson sub-system " + to_string(subSystemNo) + "!";
        lawSys->toLog(lawSys->errorMessage);// Display the error.
    }                                       // If there were errors, they have been processed.
    lawSys->turnAllRelaysOff(lawSys);       // Turn off all the relays. It doesn't matter if it fails. The PM is shutting down.
    this_thread::sleep_for(chrono::milliseconds(20));// Wait 20 mSecs before disconnecting the serial port.
    if (lawSys->subSystem.connectionType == SCSystem::CONN_SERIAL) {// Is the communications port a serial port?
        if(SCEquipment::isInArtificialMode == false) {// Is this sub-system in real mode?
            lawSys->thePort_.disconnect();  // Yes. Close the serial port.
        } // End of condition: The PM is in real mode.
    } // End of condition: This sub-system uses a serial connection.
    lawSys->isOperating = false;            // The equipment is no longer operating.
    lawSys->isOperatingEquipment = false;   // The lawson operatingEquipment function is terminating.
}

int LawsonEquip::getPulseCount() {
    return pulseCount_;                 // Provide the caller with the pulse count.
}
    
void LawsonEquip::setPulseCount(int value) {
    if (value >= 0) {                   // Is the desired value not less than zero?
        pulseCount_ = value;            // Yes. Set the pulse count.
    }
}

unsigned int LawsonEquip::getCycleNum() {
    return cycleNum_;                   // Provide the caller with the cycle number.
}

float LawsonEquip::getVolts() {
    return volts_;                      // Provide the caller with the special voltage measurement.
}

void LawsonEquip::setVoltChnlDex(unsigned int value) {
    // Start here, if possible check to be sure this is not set to an index beyond the number of inputs.
    voltChnlDex_ = value;               // Set the index of input used to measure the special voltage.
}

unsigned int LawsonEquip::getVoltChnlDex() {
    return voltChnlDex_;                // Provide the caller with the index of the input used to measure the special voltage.
}

unsigned int LawsonEquip::getRequiredCycleLength() {
    return requiredCycleLength_;
}

void LawsonEquip::setVoltRead(unsigned int value) {
    if (value < 5) {                    // Is the desired voltage read state valid?
        voltageRead_ = value;           // Yes. Set the voltage read state.
    }
}

unsigned int LawsonEquip::getVoltRead() {
    return voltageRead_;                // Provide the caller with the special voltage read state.
}

void LawsonEquip::setRequiredCycleLength(unsigned int value) {
    requiredCycleLength_ = value;       // Set the desired cycle length in milliseconds.
}

void LawsonEquip::initialize() {
    if (this->subSystem.isOnLine == false) {// Is the sub-system containing this mini equipment on-line?
        errorMessage = "Lawson sub-system " + to_string(subSystem.getSubSystemNo()) + " is off-line.";// No. Create error.
        toLog(errorMessage);            // Display the error.
        return;                         // Initialization can go no farther.
    }                                   // The sub-system containing this mini equipment is on-line.
    if(SCEquipment::isInArtificialMode == true) {// Is the plant manager in artificial mode?)
        isInitialized = true;           // For artificial mode, this sub-system is done being initialized.
        return;                         // Yes. There is no initialization to do.
    } // End of condition: PM is in artificial mode.
    if (this->subSystem.connectionType == SCSystem::CONN_SERIAL) {// Does this equipment use RS-232?
        return initialize232();         // Yes. Use initialize232() to start the sub-system.
    }                                   // The equipment uses a socket.
}

void LawsonEquip::initialize232() {
    if (this->subSystem.isOnLine == false) {// Is the sub-system containing this mini equipment on-line?
        errorMessage = "Lawson sub-system " + to_string(subSystem.getSubSystemNo()) + " is off-line.";// No. Create error.
        toLog(errorMessage);            // Display the error.
        return;                         // Initialization can go no farther.
    }                                   // The sub-system containing this mini equipment is on-line.
    isRequestedToSignOn = false;        // Initialization should not be attempted again unless this attempt fails.
    isOperating = false;                // Since this sub-system is being initialized, it is not currently operating.
    isInitialized = false;              // The lawson equipment is not initialized yet.
    timeInitialized_ = chrono::system_clock::now();// The initialization is occurring now.
    initializing_ = true;               // The system is being initialized.
    thePort_.setName(this->subSystem.getPortName());// Set the serial port used by this lawson sub-system.
    // Lawson11: If the system is wireless, set thePort_'s baud rate to 9600. Otherwise set thePort_'s baud rate to 300.
    if(lawson11 == true) {              // Does this sub-system potentially have a version 11 chip?
        if(this->subSystem.isWireless == true) {// Yes. Is this sub-system set to wireless?
            thePort_.setBaudRate(9600); // This implies this sub-system has a version 12 chip.
        } else {                        // The sub-system has a version 11 chip.
            thePort_.setBaudRate(300);  // Set the ports baud rate to 300 to sign-on.
            toLog("Setting the baud rate to 300 to wake.");// Display progress initializing.
        } // End of condition: What baud rate should the port start at?
    } // End of condition: Are version 11 chips possible?
//****************Start here, changed code **********************************
//    portDescriptor_ = thePort_.connect();// Connect to the serial port.
    portDescriptor_ = thePort_.connect4();// Connect to the serial port.
//****************Start here, changed code **********************************
    if (portDescriptor_ == -1) {   // Was a connection to the Ethernet to serial converter established?
        initializing_ = false;          // No. Done initializing. Create an error message.
        errorMessage = "Error trying to connect to the the serial port for Lawson sub-system ";
        errorMessage += to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        isRequestedToSignOn = true;     // Initialization should be attempted again.
        return;                         // Failure.
    } else if ( portDescriptor_ == -2) { // Nothing happened.
        initializing_ = false;          // No. Done initializing. Create an error message.
        errorMessage = "Serial port connection function failed. The descriptor is still -2. Lawson sub-system ";
        errorMessage += to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        isRequestedToSignOn = true;     // Initialization should be attempted again.
        return;
    }// Successfully connected to the model 201.
    if(thePort_.enableDTR() == false) {
        toLog("DTR was not enabled.");
    }
    errorMessage = "Serial port worked correctly. Lawson sub-system ";
    errorMessage += to_string(subSystem.getSubSystemNo()) + ".";
    toLog(errorMessage);            // Display the error.
    if (signon232() == false) {         // Was sign-on successful?
        initializing_ = false;          // No. Done initializing.
        thePort_.disconnect();          // Close the serial port.
        errorMessage = "Sign-on failed on Lawson sub-system ";// Create an error message.
        errorMessage += to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        isRequestedToSignOn = true;     // Sign-on must be attempted again.
        return;                         // Initialization is over because of this faiure.
    }                                   // Sign-on was successful.
    if (calSys232(this) == false) {         // Was calibration successful?
        initializing_ = false;          // No. Done initializing.
        thePort_.disconnect();          // Close the serial port.
        errorMessage = "Sign-on failed on Lawson sub-system ";// Create an error message.
        errorMessage += to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        isRequestedToSignOn = true;     // Sign-on must be attempted again.
        return;                         // Initialization is over because of this faiure.
    }
    if (checkSum() == false) {          // Was checkSum successful?
        initializing_ = false;          // No. Done initializing.
        thePort_.disconnect();          // Close the serial port.
        errorMessage = "Checksum failed on Lawson sub-system ";// Create an error message.
        errorMessage += to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        isRequestedToSignOn = true;     // Sign-on must be attempted again.
        return;                         // Initialization is over because of this faiure.
    }
    if (selectChnl(this, 0) == false) { // Was chnl 0 selected?
        errorMessage = "Failed to select channel 0 on sub-system "  + to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
    }
    cycleStartTime_ = chrono::system_clock::now();// Set the time chnl 0 was selected.
    if (subSystem.getType() == SCSystem::SYSTYPE_MICRO) {// Does this sub-system operate a Micro-system?
        if (sendPacket232(this, 0x02, 0xFF) == false) {// Yes. Turn off the relays on this micro system.
            errorMessage = "Failed to turn off the relays on sub-system "  + to_string(subSystem.getSubSystemNo()) + ".";
            toLog(errorMessage);            // Display the error.
        }
    }
    initializing_ = false;              // Done initializing.
    isInitialized = true;               // Initialization comleted successfully.
    cycleNum_ = 0;                      // Set the cycle number to zero to start operation.
    toLog("Initialize232() finished successfully!!!");// Display initializtion comletion.
    return;
}

bool LawsonEquip::signon232() {
    // F is the parameter that Lawson labs uses to set modeRegHi_, modeRegMid_, and modeRegLo_.
    unsigned int F = 19531 / subSystem.getDataRate();
    // Shifting F right eight bits puts bits 9, 10, and 11 in the first three places. The highest order bit(the word length) is alway set (24 bit word).
    modeRegMid_ = (F >> 8) + 128;
    modeRegLo_ = F & 255;               // The 8 low order bits of F are modeRegLo_.
    requiredCycleLength_ = subSystem.getCycleLength();// Set the time required for the voltage to settle.
    if (isConfigurationRead == false) { // Has this lawson equipment's configuration file been read?
        if (readConstants() == false) { // No. Was the configuration file read successfully?
            return false;               // No. This sub-system cannot be initialized.
        }                               // The constants were read.
    }                                   // The constants have been read.
    this_thread::sleep_for(chrono::milliseconds(200));// Wait 1000 mSecs before sending the wake up command again.
    // It is assumed the Model 201 has the chip to communicate at 9600 baud.
    // The socket is open for sign-on.
    int count = 0;                      // Will contain the number of tries used to wake the model 201.
    // Will contains the baud code returned by the 201 once the baud rate is set. hex FF is illegal.
    unsigned char returnedBaudCode = 0xFF;
    do {                                // Try up to ten times to wake the Model 201.
        if(count++ > 10) {              // Have 10 attempts to wake the Model 201 been made?
            errorMessage = "Error trying to wake the Model 201 in sub-system " + to_string(subSystem.getSubSystemNo()) + ".";
            toLog(errorMessage);        // Display the error.
            return false;               // Failed to wake the Model 201.
        }                               // There have been less than 11 attempts.
        unsigned int wakeUpCalls = 0;   // Keeps track of how many wake-up calls have been sent.
        for (; wakeUpCalls < wakeUpAttempts_; wakeUpCalls++) {// For up to the alloted number of wakeup attempts.
            command_[0] = 0x00;            // Zero is the wake up command.
            toLog("Setting the wake-up command for attempt " + to_string(wakeUpCalls) + ".");
            thePort_.clear();
            if(thePort_.sendArray(command_, 1) == 1) {// Was the wake command successfully sent?
                this_thread::sleep_for(chrono::milliseconds(200));// Wait 1000 mSecs before getting the wake up response.
                if (get232Bytes(this, 1, 400) == false) {// Wait for up to 400 mSecs for a response.
                    toLog("No wakeup response attempt " + to_string(wakeUpCalls) + ".");
                }
                if (commandResponse_[0] == 0x03 || commandResponse_[0] == 0x80) {
                    errorMessage = "The model 201 on sub-system " + to_string(subSystem.getSubSystemNo());
                    errorMessage += " responded to the wake-up command.";
                    toLog(errorMessage);// Display the progress.
                    break;              // The response was valid. Move on to sign-on.
                }                       // The reponse was not valid.
            }                           // There was no valid response.
            toLog("Failed waking the Model 201. Attemmpt: " + to_string(wakeUpCalls));
            this_thread::sleep_for(chrono::milliseconds(200));// Wait 200 mSecs before sending the wake up command again.
        }                               // Done trying to wake up the model 201.
        if (wakeUpCalls >= wakeUpAttempts_) {// Did the Model 201 wake up?
            errorMessage = "Error trying to wake the Model 201 in sub-system " + to_string(subSystem.getSubSystemNo()) + ".";
            toLog(errorMessage);        // Display the error.
            return false;               // Failed to wake the Model 201.
        }                               // The model 201 responded to wake up.
        toLog("Initial Sign-on successful");// Display signon porgress.
        checkSum_ = 0;                  // Reset the checksum, the next command resets it on the 201.
        this_thread::sleep_for(chrono::milliseconds(200));// Wait 200 mSecs before sending the sign-on command.
        command_[0] = 0x88;             // hex 88 is the sign-on command.
        if(thePort_.sendArray(command_, 1) != 1) {// Was the sign-on command successfully sent?
            errorMessage = "Sign-on command failed on the Model 201 in sub-system " + to_string(subSystem.getSubSystemNo()) + ".";
            toLog(errorMessage);        // Display the error.
            return false;               // Failed to wake the Model 201.
        }                               // The sign-on command was sent
        this_thread::sleep_for(chrono::milliseconds(100));// Wait 100 mSecs before sending the baud rate command.
        command_[0] = (unsigned char)subSystem.getBaudCode();
        toLog("Sending the baud rate code: " + to_string(command_[0]) + ".");// Display signon progress.
        if(thePort_.sendArray(command_, 1) != 1) {// Was the baud code command successfully sent?
            errorMessage = "Baud-code command failed on the Model 201 in sub-system " + to_string(subSystem.getSubSystemNo()) + ".";
            toLog(errorMessage);        // Display the error.
            return false;               // Failed to wake the Model 201.
        }                               // The baud-code command was sent.
        if (get232Bytes(this, 1, 201) == false) {// Wait 201 mSecs to get the baud rate response. Was there a response?
            errorMessage = "There was an error getting the reply to the baud code on sub-system ";// No.
            errorMessage += to_string(subSystem.getSubSystemNo()) + ".";
            toLog(errorMessage);        // Display the error.
            return false;               // Failed to wake the Model 201.
        }
        returnedBaudCode = commandResponse_[0];
        toLog("The returned baud rate code is " + to_string(returnedBaudCode) + ".");// Display signon progress.
    } while (subSystem.getBaudCode() != (int)returnedBaudCode);
    // The Model 201 is connected.
    errorMessage = "The Model 201 on sub-system " + to_string(subSystem.getSubSystemNo()) + " responded to the baud rate command.";
    toLog(errorMessage);                // Display signon progress.
    // Lawson11: If the sub-system is not wireless,
    if(lawson11 == true) {              // Does this sub-system potentially have a version 11 chip?
        if(subSystem.isWireless == false) {// Yes. Is this sub-system set to wireless?
            // Lawson11: Close the serial port.
            toLog("The serial port is being closed so that it can be opened at the operating baud rate.");
            thePort_.disconnect();          // Close the serial port that was open at 300 baud.
            // Lawson11: Set the desired operating baud rate for thePort_.
            thePort_.setBaudRate(getSubSystem().getBaudRate());
            portDescriptor_ = thePort_.connect4();// Connect to the serial port.
            if (portDescriptor_ == -1) {    // Was a connection to the Ethernet to serial converter established?
                initializing_ = false;      // No. Done initializing. Create an error message.
                errorMessage = "Serial port error -1 connecting at operating baud rate, Lawson sub-system ";
                errorMessage += to_string(subSystem.getSubSystemNo()) + ".";
                toLog(errorMessage);        // Display the error.
                isRequestedToSignOn = true; // Initialization should be attempted again.
                return false;               // Failure.
            } else if ( portDescriptor_ == -2) { // Nothing happened.
                initializing_ = false;      // No. Done initializing. Create an error message.
                errorMessage = "Serial port error -2 connecting at operating baud rate, Lawson sub-system ";
                errorMessage += to_string(subSystem.getSubSystemNo()) + ".";
                toLog(errorMessage);        // Display the error.
                isRequestedToSignOn = true; // Initialization should be attempted again.
                return false;               // Failure
            }// Successfully connected to the model 201.
            // Lawson11: End of sub-system which is not wireless.
        } // End of condition: This sub-system uses a version 11 chip and needs to have its serial port reopened at operating baud.
    } // End of condition: This sub-system could have a version 11 chip.
    this_thread::sleep_for(chrono::milliseconds(100));// Wait 100 mSecs before sending the echo off command.
    command_[0] = 0x00;                 // hex 00 turns off the echo.
    if(thePort_.sendArray(command_, 1) != 1) {// Was the echo successfully turned off?
        errorMessage = "Echo off command failed on the Model 201 in sub-system " + to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        return false;                   // Failed to to send the echo off command.
    }                                   // The echo off command was sent.
    thePort_.clear();                   // Clear anything that might be in the serial port buffers.
    this_thread::sleep_for(chrono::milliseconds(100));// Wait 100 mSecs before sending the modReg variables.
    if(sendPacket232(this, modeRegHi_, modeRegMid_) == false) {// Send the first two registers. There is no expected response.
        errorMessage = "modeRegHi_ and modeRegMid_ could not be set in sub-system " + to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        return false;                   // Failed to sign-on to the Model 201.
    }                                   // modeRegHi_ & modeRegMid_ were sent.
    this_thread::sleep_for(chrono::milliseconds(10));// Wait 10 mSecs before sending the next mode reg settings.
    if(sendPacket232(this, modeRegLo_, modeRegMid_) == false) {// Send the last two registers. There is no expected response.
        errorMessage = "modeRegLo_ and modeRegMid_ could not be set in sub-system " + to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        return false;                   // Failed to wake the Model 201.
    }                                   // modeRegLo_ and modeRegMid_ were set.
    this_thread::sleep_for(chrono::milliseconds(10));// Wait 10 mSecs before sending the polled settings.
    unsigned char samplesCode = (unsigned char)subSystem.getAverageCode();// Get the samples code.
    unsigned char filter = (unsigned char)subSystem.getFilter();// Get the filters code.
    if(sendPacket232(this, samplesCode, filter) == false) {// Send the samples code and the filter code. There is no expected response.
        errorMessage = "Samples code and filter could not be set in sub-system " + to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        return false;                   // Failed to sebd the samples code and the filter code.
    }                                   // The samples to average code and filter code were sent.
    this_thread::sleep_for(chrono::milliseconds(10));// Wait 10 mSecs before sending the polled settings.
    unsigned char polled = 1;           // Set the Model 201 to use polled mode.
    unsigned char reserved = 0;         // This is not used.
    if(sendPacket232(this, reserved, polled) == false) {// Send the polled mode and reserved. There is no expected response.
        errorMessage = "Polled mode could not be set in sub-system " + to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        return false;                   // Failed to send the polled mode.
    }                                   // The polled mode setting was sent.
    this_thread::sleep_for(chrono::milliseconds(250));// Wait 250 mSecs before getting the reponse, the 201 needs time.
    if (get232Bytes(this, 3, 50) == false) { // Spend up to 600 mSecs getting the response to the mode settings.
        errorMessage = "No response setting the modes in Lawson sub-system " + to_string(subSystem.getSubSystemNo()) + ".";
        toLog(errorMessage);            // Display the error.
        return false;                   // Failed to set the modes.
    }                                   // There was a response to setting the modes.
    if ((commandResponse_[0] & 0x1F) != (modeRegHi_ & 0x1F)) {// Was modeRegHi_ set?
        errorMessage = "ModeRegHi response was invalid on sub-system " + to_string(subSystem.getSubSystemNo()) + ".";// No.
        toLog(errorMessage);            // Display the error.
        return false;                   // Failed to set modeRegHi_.
    }                                   // modeRegHi_ was set.
    if (commandResponse_[1] != modeRegMid_) {// Was modeRegMid_ set?
        errorMessage = "ModeRegMid response was invalid on sub-system " + to_string(subSystem.getSubSystemNo()) + ".";// No.
        toLog(errorMessage);            // Display the error.
        return false;                   // Failed to set modeRegMid_.
    }                                   // modeRegMid_ was set.
    if (commandResponse_[2] != modeRegLo_) {// Was modeRegLo_ set?
        errorMessage = "ModeRegLo response was invalid on sub-system " + to_string(subSystem.getSubSystemNo()) + ".";//No.
        toLog(errorMessage);            // Display the error.
        return false;                   // Failed to set modeRegLo_.
    }                                   // modeRegLo was echoed back correctly.
    thePort_.clear();                   // Clear anything that might be in the serial port buffers.
    toLog("Sign-on using a RS-232 port was successful.");
    return true;                        // Sign-on using a RS-232 port was successful.
}

bool LawsonEquip::getBytes(LawsonEquip *lawSys, unsigned int bytesToRead, int milliSecondsToRead) {
    if (lawSys->subSystem.connectionType == SCSystem::CONN_SERIAL) {// Does this equipment use RS-232?
        return get232Bytes(lawSys, bytesToRead, milliSecondsToRead);// Yes. Use send232Bytes() to send the command packet.
    }                                       // The equipment uses a socket.
    // Future for socket communications.
    return true;
}

bool LawsonEquip::get232Bytes(LawsonEquip* lawSys, unsigned int bytesToRead, int milliSecondsToRead) {
    int subSystemNo = lawSys->getSubSystem().getSubSystemNo();// Get the sub-system number for error messages.
    if (lawSys->subSystem.isOnLine == false) {// Is this sub-system online?
        return true;                        // No. There is no need to send to the Model 201. It is offline. But no failure.
    }                                       // This sub-system is online.
    if (SCEquipment::isInArtificialMode == true) {// Is the Plant Manager in artufical mode?
        return true;                        // Yes. There is no need to send to the Model 201. But no failure.
    }                                       // The Plant Manager is not in artificial mode.
    if (lawSys->initializing_ == false) {   // Is this equipment being initialized?
        if (lawSys->isOperating == false) { // No. Is this equipment operating?
            return true;                    // No. Nothing can be sent until the equipment is operating.
        }                                   // This equipment is operating.
    }                                       // This equipment is either operating or being initialized. OK to send packet.
    unsigned int bytesRead = 0;             // Contains the total number of bytes read.
    lawSys->commandResponseLength_ = 0;     // No bytes have been read yet.
    unsigned char resp[16];                 // Contains the bytes read on a single call.
    for(int dex = 0; dex < 16; dex++) {
        lawSys->commandResponse_[dex] = 0x00;
    }
    chrono::time_point<chrono::system_clock> getBytesStartTime_ = chrono::system_clock::now();
    while (bytesRead < bytesToRead) {       // Have all the bytes been read?
        chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// No. Contains the current date/time.
        chrono::duration<double> secondsRead = now - getBytesStartTime_;// Number of seconds since the last state change.
        int bytesInBuffer = lawSys->thePort_.bytesToRead();// Get the number of bytes ready to be read.
        unsigned int noBytesToReadNow = bytesInBuffer;
        if(bytesRead + bytesInBuffer > bytesToRead) {
            noBytesToReadNow = bytesToRead - bytesRead;
        }
        if (bytesInBuffer > 0) {            // Are there any bytes ready to be read?
            int numberRead = lawSys->thePort_.getArray(resp, noBytesToReadNow);// Yes. Read them.
            if (numberRead == -1) {         // Was there a read error?
                lawSys->initializing_ = false;      // Yes. The system needs to be restarted. Create an error message.
                lawSys->errorMessage = "Error getting data from the model 201 on Lawson sub-system ";
                lawSys->errorMessage += to_string(subSystemNo) + ".";
                lawSys->toLog(lawSys->errorMessage);// Display the error.
                lawSys->isRequestedToSignOn = true; // Initialization should be attempted again.
            }                               // the bytes in the buffer have been read.
            for (int dex = 0; dex < numberRead; dex++) {// Tranfer the bytes to the reponse
                lawSys->commandResponse_[bytesRead + dex] = resp[dex];// one byte at a time.
                lawSys->checkSum_ = (lawSys->checkSum_ + resp[dex]) % 256;// Increment the checksum.
            }                               // The bytes in the buffer have been transfered.
            bytesRead += numberRead;        // Increment the number of bytes read.
        }                                   // The serial ports buffer has been processed.
        if (secondsRead.count() > (double)milliSecondsToRead / 1000) {// Has the alloted read time elapsed?
            lawSys->timeOfLastFailure_ = now;       // Yes. A time-out is a failure.
            lawSys->commandResponseLength_ = bytesRead;// Make the bytes read available.
            lawSys->toLog("total bytes received = " + to_string(bytesRead));
            lawSys->isRequestedToSignOn = true;// Increment the error count.
            return false;                   // Yes. Failed to read all the bytes.
        }                                   // The alloted read time has not elapsed.
        this_thread::sleep_for(chrono::milliseconds(20));// Wait 20 mSecs before trying to get bytes again.
    }                                       // The response is complete.
    lawSys->timeOfLastSuccess_ = chrono::system_clock::now();// This was a success.
    lawSys->commandResponseLength_ = bytesRead;// Make the bytes read available.
    return true;                            // Let the caller know the correct number of bytes was received.
}

bool LawsonEquip::readConstants() {
    return readConstantsFromDB();
}

bool LawsonEquip::readConstantsFromDB() {
    int subSysNo = this->subSystem.getSubSystemNo();// Get the sub-system number.
    sqlite3 *db;                        // Create a location for the database.
    int  rc;                            // Create a return variable.
    string sql;                         // Create a string for SQL commands.
    string dbName = filePath_ + "SCConfig.db";// Set the configuration database name.
    rc = sqlite3_open(dbName.c_str(), &db);// Open the database named SC_Config.db. It has a table which contains the lawson constants.
    if( rc ){                           // Was the database opened?
        errorMessage = "Can't open SCConfig.db to load lawson constants for sub-system " + to_string(subSysNo);// No. Create a message.
        errorMessage += ": " + string(sqlite3_errmsg(db));// Add the database error to the message.
        toLog(errorMessage);            // Display the error.
        return false;                   // This error prevents loading the lawson sub-system.
    }                                   // The database is open.
    string tableName = "lawConstants";  // Create the table name for the lawson constants.
    sql = "SELECT * from " + tableName + " WHERE id=" + to_string(subSysNo);// Create an SQL statement to read the Minis which
                                        // are a part of this sub-system.
    sqlite3_stmt *stmt;                 // Will contain the results of the querry.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
    if (rc != SQLITE_OK) {              // Was it successful?
                                        // No, there was a problem. Create an error message.
        errorMessage = "Failed to prepare query for 'lawson constants' on sub-system " + to_string(subSysNo);
        errorMessage += ": " + string(sqlite3_errmsg(db));// Add the database error to the message.
        toLog(errorMessage);            // Display the error.
        sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        // In this case sqlite3_finalize will return an error because the previous statement caused a database error.
        sqlite3_close(db);              // Close the database. There is no need to check for errors because finalize was just called.
        return false;                   // Let the caller know the lawson constants couldn't be retrieved from the database.
    }                                   // The query to retrieve constants was prepared.
    rc = sqlite3_step(stmt);            // Get the results of the query for the constants.
    if (rc != SQLITE_ROW && rc != SQLITE_OK && rc != SQLITE_DONE) {// Were the results obtained?
        errorMessage = "Failed to  get query results for lawson constants on sub-system " + to_string(subSysNo);// No, create error message.
        errorMessage += ": " + string(sqlite3_errmsg(db));// Add the database error to the message.
        toLog(errorMessage);            // Display the error.
        sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        // In this case sqlite3_finalize will return an error because the previous statement caused a database error.
        sqlite3_close(db);              // Close the database. There is no need to check for errors because finalize was just called.
        return false;                   // Let the caller know the lawson constants couldn't be retrieved from the database.
    }                                   // The plant manager's reply has been sent to the databaseis.
    //int id = sqlite3_column_int (stmt, 0);// The id is in the first column.
    //int version = sqlite3_column_int(stmt, 1);// The version is in the table's second column.
    nThermAmps_ = sqlite3_column_int(stmt, 2);// The number of thermocouple amplifiers is in the table's third column.
    for (int nDex = 0; nDex < 6; nDex++) {// Load each potential thermocouple amplifier's constants.
        coldK_.at(nDex) = sqlite3_column_double(stmt, 3 + nDex);// Get this amplifier's cold junction constant from the DB.
        gainK_.at(nDex) = sqlite3_column_double(stmt, 4 + nDex);// Get this amplifier's gain constant from the DB.
    }                                   // All of the potential constants for this sub-system have been loaded.
    sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
    sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
    return true;                        // Let the user know the Mini module configuration were successfully read.
}

void LawsonEquip::setCommand(LawsonEquip* lawSys, unsigned char* value) {
    for (unsigned int dex = 0; dex < 16; dex++) {// Transfer 16 characters to the command. It may be that
        lawSys->command_[dex] = *(value + dex);         // not all of the characters are part of the command.
    }
}

bool LawsonEquip::getResponse(LawsonEquip* lawSys, unsigned int respLength, unsigned int waitTime) {
    for (int n = 0; n < 16; n++) {
        commandResponse_[n] = 0x00;
    }
    if (this->subSystem.connectionType == SCSystem::CONN_SERIAL) {// Does this equipment use a serial port?
        return get232Bytes(this, respLength, waitTime);// Yes. Use get232Bytes() to get the response.
    }                                           // This equipment uses a socket.
    return true;
}

unsigned char* LawsonEquip::getCommandResponse() {
    return commandResponse_;
}

unsigned int LawsonEquip::getCommandResponseLength() {
    return commandResponseLength_;
}

bool LawsonEquip::sendPacket(LawsonEquip* lawSys, unsigned char outByte1, unsigned char outByte2) {
    if (lawSys->subSystem.connectionType == SCSystem::CONN_SERIAL) {// Does this equipment use RS-232?
        return sendPacket232(lawSys, outByte1, outByte2);// Yes. Use sendPacket232() to send the command packet.
    }                                       // The equipment uses a socket.
    return true;                            // Let the caller know the packet was sent.
}

bool LawsonEquip::sendPacket232(LawsonEquip* lawSys, unsigned char outByte1, unsigned char outByte2) {
    if (lawSys->subSystem.isOnLine == false) {// Is this sub-system online?
        return true;                        // No. There is no need to send to the Model 201. It is offline. But no failure.
    }                                       // This sub-system is online.
    if (SCEquipment::isInArtificialMode == true) {// Is the Plant Manager in artufical mode?
        return true;                        // Yes. There is no need to send to the Model 201. But no failure.
    }                                       // The Plant Manager is not in artificial mode.
    if (lawSys->initializing_ == false) {   // Is this equipment being initialized?
        if (lawSys->isOperating == false) { // No. Is this equipment operating?
            return true;                    // No. Nothing can be sent until the equipment is operating.
        }                                   // This equipment is operating.
    }                                       // This equipment is either operating or being initialized. OK to send packet.
    unsigned char checkSum = (outByte1 + outByte2) & 255; // Set the check sum.
    unsigned char* outBuffer = new unsigned char[16];// Create the unsigned character data array.
    outBuffer[0] = outByte1;                // Set the first byte in the output buffer.
    outBuffer[1] = outByte2;                // Set the second byte in the output buffer.
    outBuffer[2] = checkSum;                // Set the third byte in the output buffer to the check sum.
    if (lawSys->thePort_.sendArray(outBuffer, 3) == false) {// Send the packet. Was the packet sent successfully?
        int subSystemNo = lawSys->getSubSystem().getSubSystemNo();// Yes. Create an error message.
        lawSys->errorMessage = "Error writing the packet in SendPacket232() on sub-system " + to_string(subSystemNo) + "!";
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;                       // No. Let the caller know the packet was not sent.
    }                                       // The packet was sent.
    delete[] outBuffer;                        // Free the memory allocated to the output buffer.
    return true;                            // Let the caller know the packet was sent.
}

int LawsonEquip::MillisecondOfDay() {
    timeval curTime;
    gettimeofday(&curTime, NULL);
    int milli = curTime.tv_usec / 1000;
    return milli;
}

bool LawsonEquip::calSys(LawsonEquip* lawSys) {
    if (lawSys->subSystem.connectionType == SCSystem::CONN_SERIAL) {// Does this equipment use RS-232?
        return calSys232(lawSys);           // Yes. Use sendPacket232() to send the command packet.
    }                                       // The equipment uses a socket.
    int subSystemNo = lawSys->getSubSystem().getSubSystemNo();// Get the sub-system number for error messages.
    if (subSystem.isOnLine == false) {      // Is this sub-system online?
        return true;                        // No. There is no need to send to the Model 201. It is offline. But no failure.
    }                                       // This sub-system is online.
    if (SCEquipment::isInArtificialMode == true) {// Is the Plant Manager in artufical mode?
        return true;                        // Yes. There is no need to send to the Model 201. But no failure.
    }                                       // The Plant Manager is not in artificial mode.
    if (initializing_ == false) {           // Is this equipment being initialized?
        if (isOperating == false) {         // No. Is this equipment operating?
            return true;                    // No. Nothing can be sent until the equipment is operating.
        }                                   // This equipment is operating.
    }                                       // This equipment is either operating or being initialized. OK to send packet.
    if (sendPacket(lawSys, 0x82, 0x70) == false) {   // Send the offset calibrate command. (Resets the zero.)
        lawSys->errorMessage = "Error sending the calibration offset command on sub-system " + to_string(subSystemNo) + "!";
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;                       // No. Let the caller know the calibration failed.
    }                                       // The offset was re-calibrated.
    //    this_thread::sleep_for(chrono::milliseconds(2000));// Wait for 10 milliseconds for the Model 201 to ready.
    if (getResponse(lawSys, wordCount_ + 1, 1000) == false) {
        lawSys->errorMessage = "Error receiving the calibration offset response on sub-system " + to_string(subSystemNo) + "!";
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;                       // No. Let the caller know the calibration failed.
    }                                       // The model 201 responded to the offset calibration.
   this_thread::sleep_for(chrono::milliseconds(100));
    if (sendPacket(lawSys, 0x83, 0x60) == false) {// Send the full scale calibrate command.
        lawSys->errorMessage = "Error sending the full scale calibration command on sub-system " + to_string(subSystemNo) + "!";
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;                       // No. Let the caller know the calibration failed.
    }                                       // The full scale was re-calibrated.
    if (getResponse(lawSys, wordCount_ + 1, 1000) == false) {
        lawSys->errorMessage = "Error receiving the full scale calibration response on sub-system " + to_string(subSystemNo) + "!";
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;                       // No. Let the caller know the calibration failed.
    }                                       // The model 201 responded to the offset calibration.
    return true;                            // Calibration completed successfully.
}

bool LawsonEquip::calSys232(LawsonEquip *lawSys) {
    int subSystemNo = getSubSystem().getSubSystemNo();// Get the sub-system number for error messages.
    if (subSystem.isOnLine == false) {      // Is this sub-system online?
        return true;                        // No. There is no need to send to the Model 201. It is offline. But no failure.
    }                                       // This sub-system is online.
    if (SCEquipment::isInArtificialMode == true) {// Is the Plant Manager in artufical mode?
        return true;                        // Yes. There is no need to send to the Model 201. But no failure.
    }                                       // The Plant Manager is not in artificial mode.
    if (initializing_ == false) {           // Is this equipment being initialized?
        if (isOperating == false) {         // No. Is this equipment operating?
            return true;                    // No. Nothing can be sent until the equipment is operating.
        }                                   // This equipment is operating.
    }                                       // This equipment is either operating or being initialized. OK to send packet.
    if (sendPacket232(this, 0x82, 0x70) == false) {// Send the offset calibrate command. (Resets the zero.) Did it send?
        errorMessage = "Error sending the calibration offset command on sub-system " + to_string(subSystemNo) + "!";// No.
        toLog(errorMessage);// Display the error.
        return false;                       // No. Let the caller know the offset calibration failed.
    }                                       // The offset calibrated was sent.
    if (get232Bytes(this, wordCount_ + 1, 2000) == false) {// Was there a reponse to offset calibartion?
        toLog("The bytes received in response to offset calibration was " + to_string(commandResponseLength_));
        errorMessage = "Error receiving the calibration offset response on sub-system " + to_string(subSystemNo) + "!";// No.
        toLog(errorMessage);                // Display the error.
        return false;                       // No. Let the caller know the offset calibration failed.
    }                                       // The model 201 responded to the offset calibration.
    if (sendPacket232(this, 0x83, 0x60) == false) {// Send the full scale calibrate command. Did it send?
        errorMessage = "Error sending the full scale calibration command on sub-system " + to_string(subSystemNo) + "!";// No.
        toLog(errorMessage);                // Display the error.
        return false;                       // No. Let the caller know the full scale calibration failed.
    }                                       // The full scale calibrated was sent.
    if (get232Bytes(this, wordCount_ + 1, 2000) == false) {// Was there a reponse to full scale calibartion?
        toLog("The bytes received in response to full scale calibration was " + to_string(commandResponseLength_));
        errorMessage = "Error receiving the full scale calibration response on sub-system " + to_string(subSystemNo) + "!";// No.
        toLog(errorMessage);                // Display the error.
        return false;                       // No. Let the caller know the full scale calibration failed.
    }                                       // The model 201 responded to the full scale calibration.
    return true;                            // System calibration succeeded.
}

bool LawsonEquip::checkSum() {
    int subSystemNo = this->getSubSystem().getSubSystemNo();// Get the sub-system number for error messages.
    if (subSystem.isOnLine == false) {      // Is this sub-system online?
        return true;                        // No. There is no need to send to the Model 201. It is offline. But no failure.
    }                                       // This sub-system is online.
    if (SCEquipment::isInArtificialMode == true) {// Is the Plant Manager in artifical mode?
        return true;                        // Yes. There is no need to send to the Model 201. But no failure.
    }                                       // The Plant Manager is not in artificial mode.
    if (initializing_ == false) {           // Is this equipment being initialized?
        if (isOperating == false) {         // No. Is this equipment operating?
            return true;                    // No. Nothing can be sent until the equipment is operating.
        }                                   // This equipment is operating.
    }                                       // This equipment is either operating or being initialized. OK to send packet.
    if (sendPacket(this, 0x87, 0x00) == false) {// Send the check sum command.
        errorMessage = "Error sending the check sum command on sub-system " + to_string(subSystemNo) + "!";
        toLog(errorMessage);                // Display the error.
        return false;                       // No. Let the caller know the check sum failed.
    }                                       // The offset was re-calibrated.
    if (getResponse(this, 2, 3000) == false) {
        errorMessage = "Error receiving the check sum response on sub-system " + to_string(subSystemNo) + "!";
        toLog(errorMessage);                // Display the error.
        return false;                       // No. Let the caller know the check sum failed.
    }                                       // The model 201 responded to the check sum.
    if (commandResponse_[0] != 0x87) {      // Was the check sum response valid?
                                            // No. Create an error message.
        errorMessage = "Invalid response to the check sum command on sub-system " + to_string(subSystemNo) + "!";
        toLog(errorMessage);                // Display the error.
        return false;                       // No. Let the caller know the check sum failed.
    }                                       // The reponse was valid.
    int ourIntCheckSum = (256 + checkSum_ - commandResponse_[1]) % 256;// Create check sum intger version to avoid minus values.
    unsigned char ourCheckSum = (unsigned char)ourIntCheckSum;// Convert back to unsigned char version.
    if (ourCheckSum != commandResponse_[1]) {// Does our checksum match the lawson checksum?
        errorMessage = "Error, Lawson checksum = " + to_string(commandResponse_[1]) + ", ";// No.
        errorMessage += "Plant Manager checksum = " + to_string(ourCheckSum) + ", ";// Create an error message.
        errorMessage += "on sub-system " + to_string(subSystemNo) + "!";
        toLog(errorMessage);                // Display the error.
        return false;                       // No. Let the caller know the checksum failed.
    }                                       // Passed the checksum test. Communications is OK.
    checkSum_ = 0;                          // The checksum is reset after this command.
    return true;                            // Let the caller know the checksum test was successful.
}

bool LawsonEquip::emptyInputBuffer(LawsonEquip* lawSys) {
    int bytesToRead = lawSys->thePort_.bytesToRead();// Get the number of bytes in the input buffer.
    int countDown = 16;                     // This prevents and endless loop. After reading the input buffer 16 times it gives up.
    while (bytesToRead > 0 && countDown-- > 0) {// Are there any bytes in the input buffer?
        if (bytesToRead > 16) {             // Yes. The software buffer is only 16 bytes. Are there more than 16 bytes?
            bytesToRead = 16;               // Set the number of bytes to read to 16.
        }                                   // Read to read at least part of the input buffer.
        if (getBytes(lawSys, bytesToRead, 100) == false) {// Read the bytes. Was there a problem?
            return false;                   // Yes. let the caller know.
        }                                   // The Input buffer was cleared of bytesToRead bytes.
        bytesToRead = lawSys->thePort_.bytesToRead();//Get the number of bytes left to read.
    }                                       // Done trying to empty the input buffer.
    if (countDown < 1) {                    // Where 256 bytes read?
        return false;                       // Yes. The input buffer could not be emptied.
    }                                       // The input buffer is empty.
    return true;                            // Success. Let the caller know.
}

bool LawsonEquip::selectChnl(LawsonEquip* lawSys, unsigned char chnlNum) {
    int subSystemNo = lawSys->getSubSystem().getSubSystemNo();// Get the sub-system number for error messages.
    if (lawSys->subSystem.isOnLine == false) {// Is this sub-system online?
        return true;                        // No. There is no need to send to the Model 201. It is offline. But no failure.
    }                                       // This sub-system is online.
    if (SCEquipment::isInArtificialMode == true) {// Is the Plant Manager in artifical mode?
        return true;                        // Yes. There is no need to send to the Model 201. But no failure.
    }                                       // The Plant Manager is not in artificial mode.
    if (lawSys->initializing_ == false) {   // Is this equipment being initialized?
        if (lawSys->isOperating == false) { // No. Is this equipment operating?
            return true;                    // No. Nothing can be sent until the equipment is operating.
        }                                   // This equipment is operating.
    }                                       // This equipment is either operating or being initialized. OK to send packet.
    if (emptyInputBuffer(lawSys) == false) {// Make sure the input buffer is empty. Is it empty?
        lawSys->errorMessage = "Error emptying the input buffer ";
        lawSys->errorMessage += to_string(chnlNum) + "on Lawson sub-system " + to_string(subSystemNo) + ".";
        return false;                       // No. We can't select the chnl.
    }                                       // The input buffer is ready to receive the response.
    if (lawSys->sendPacket(lawSys, 0x01, chnlNum) == false) {// Was the select chnl command sent?
        lawSys->errorMessage = "Error selecting channel " + to_string(chnlNum);
        lawSys->errorMessage += " on Lawson sub-system " + to_string(subSystemNo) + ".";
        return false;                       // No. We can't select the chnl.
    }
    this_thread::sleep_for(chrono::milliseconds(10));// Wait for 10 milliseconds for the Model 201 to ready.
    if (lawSys->thePort_.bytesToRead() > 0) {
        if (lawSys->emptyInputBuffer(lawSys) == false) {      // Make sure the input buffer is empty. Is it empty?
            lawSys->errorMessage = "Error emptying the input buffer after selecting channel ";
            lawSys->errorMessage += to_string(chnlNum) + " on Lawson sub-system " + to_string(subSystemNo) + ".";
            return false;                       // No. We can't select the chnl.
        }                                       // The input buffer is ready to receive the response.
    }
   return true;
}

void LawsonEquip::processSignOnRequest(LawsonEquip* lawSys) {
    chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// No. Contains the current date/time.
    chrono::duration<double> timeSinceInitialization = now - lawSys->getTimeInitialized();// The time since initialization.
    SCSystem sys = lawSys->getSubSystem();  // Get the sub-system which this Model 201 belongs.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    if (timeSinceInitialization.count() > 90) {
        lawSys->errorMessage = "Reinitialized sub-system " + subSysNo + ". ";
        time_t tTime = chrono::system_clock::to_time_t(lawSys->timeOfLastSuccess_);// Create a
        string ts = ctime(&tTime);              // string to the last communication
        ts.resize(ts.size() - 1);               // success
        lawSys->errorMessage += "Last comm success: " + ts;// and
        tTime = chrono::system_clock::to_time_t(lawSys->timeOfLastFailure_);// the last communication
        ts = ctime(&tTime);                     // failure, and
        ts.resize(ts.size() - 1);               // the number of consecutive errors.
        lawSys->errorMessage += " Last failure: " + ts + " Consecutive Count: " + to_string(lawSys->nConsecutiveErrors_);
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        lawSys->isOperating = false;        // The system is no longer operating.
        if (lawSys->subSystem.connectionType == SCSystem::CONN_SERIAL) {// Does this equipment use RS-232?
            lawSys->initialize232();        // Initialize using a serial port.
            if (lawSys->isInitialized == true) {// Was the initialization successful?
                lawSys->isOperating = true; // Yes. The equioment is operating again.
                lawSys->isRequestedToSignOn = false;// There is no need to sign=on again.
            } else {                        // Sign-on failed.
                lawSys->isOperating = false;// This sub-system is not operating.
                lawSys->isRequestedToSignOn = true;// Sign-on will have to be attempted again.
                lawSys->errorMessage = "Failed to reinitialize sub-system " + subSysNo;// Create error message.
                lawSys->errorMessage += " which was having continuous communication errors.";
                lawSys->toLog(lawSys->errorMessage);// Display the error.
            }                               // The attempt at re-initializing the model 201 using a serial port is complete.
        } else {                            // The equipment uses a socket.
            lawSys->initialize();           // Initialize using a socket.
            if (lawSys->isInitialized == true) {// Was the initialization successful?
                lawSys->isOperating = true; // Yes. The equioment is operating again.
                lawSys->isRequestedToSignOn = false;// There is no need to sign=on again.
            } else {                        // Sign-on failed.
                lawSys->isOperating = false;// This sub-system is not operating.
                lawSys->isRequestedToSignOn = true;// Sign-on will have to be attempted again.
                lawSys->errorMessage = "Failed to reinitialize sub-system " + subSysNo;// Create error message.
                lawSys->errorMessage += " which was having continuous communication errors.";
                lawSys->toLog(lawSys->errorMessage);// Display the error.
            }                               // The attempt at re-initializing the model 201 using a socket is complete.
        }
    }                                       // The attempted re-initialization is complete.
}

void LawsonEquip::processVoltageRead(LawsonEquip *lawSys) {
    SCSystem sys = lawSys->getSubSystem();  // Get the sub-system which this Model 201 belongs.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    if (lawSys->voltageRead_ == 1) {        // Did an outside application just request a special voltage reading?
        lawSys->volts_ = 0;                 // Yes. Set the voltage read to zero for simulation. This does nothing for real mode.
        lawSys->voltageRead_ = 2;           // First finish the current normal reading.
        return;                             // Return to make the normal measurement.
    }                                       // The special volatge read has been going.
    if (lawSys->voltageRead_ == 3) {        // Is it time to measue a special voltage?
                                            // Yes.
        if (lawSys->isInArtificialMode == true) {// Is this sub-system in artificial mode.
            lawSys->cycleStartTime_ = chrono::system_clock::now();// Reset the cycle start time.
            lawSys->volts_ += .01f;         // Increment the voltage for simulation.
            lawSys->voltageRead_ = 4;       // Set the flag to 4 so that next time through the normal measurement will be made.
            return;                         // For artificial mode the voltage has been set.
        }                                   // This is a real system.
        this_thread::sleep_for(chrono::milliseconds(500));// Wait for 500 milliseconds for the special voltage to really settle.
        try {                               // Try to measure
            lawSys->volts_ = getVoltage(lawSys);// the special voltage,
        } catch (string error) {            // Was there a problem?
            lawSys->volts_ = -9.0f;         // Yes. Set the volts to a bad value.
            lawSys->errorMessage = "Sub-system " + subSysNo + " error:" + error;// Create an error message.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
        }                                   // The voltage has been obtained.
        if(selectChnl(lawSys, nextChnl(lawSys)) == false) {// Select the next normal chnl to read.
            lawSys->errorMessage = "Sub-system " + subSysNo + " error: Failed selecting the next channel after reading a voltage ";// Error.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
        }                                   // The next chnl may or may not be selected.
        if (chrono::system_clock::now() < lawSys->voltageTime_) {// Is the clock running backwards like daylight savings time?
            lawSys->voltageTime_ = chrono::system_clock::now();// Yes. Reset the start time.
        }                                   // The clock is running forward.
        if (chrono::system_clock::now() - lawSys->voltageTime_ > chrono::minutes(2)) {// Has it been on this cycle for more than 2 min?
            lawSys->voltageRead_ = 0;       // Time out reading special voltages. Go back to normal mode.
        } else {                            // Special voltages are still being read.
            lawSys->voltageRead_ = 4;       // Set the flag to 4 so that next time through the normal measurement will be made.
        }                                   // The voltage flag has been set.
        lawSys->cycleStartTime_ = chrono::system_clock::now();// Reset the cycle start time.
        return;                             // Done with this part of the voltage read cycle.
    }                                       // done reading a special voltage.
    if (lawSys->voltageRead_ == 4) {        // Should a normal measurement be made this time?
        lawSys->voltageRead_ = 2;           // Set the flag to make a normal measurement this cycle.
        return;                             // Return to make the normal measurement.
    }
    // An outside application requested the special voltage read terminate.
    lawSys->voltageRead_ = 0;               // Set the flag to turn off special voltage reading.
    if(selectChnl(lawSys, nextChnl(lawSys)) == false) {// Select the next normal chnl to read.
        lawSys->errorMessage = "Sub-system " + subSysNo + " error: Failed selecting the next channel after reading a voltage ";// Error.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
    }                                       // The next chnl may or may not be selected.
    lawSys->cycleStartTime_ = chrono::system_clock::now();// Reset the cycle start time.
    return;                                 // Return with no special voltage measurements.
}

void LawsonEquip::processOffsets(LawsonEquip *lawSys) {
    SCSystem sys = lawSys->getSubSystem();  // Get the sub-system which this Model 201 belongs.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    float oldOffset = lawSys->offset_.at(lawSys->cycleNum_);// Contains the previous offset for this amp.
    float offset = 0;                       // Contains the measured offset voltage.
    try {                                   // Get the offset for the thermo amp corresponding to the cycle number.
        offset = lawSys->getVoltage(lawSys);// Get the offset voltage.
    } catch (string error) {                // Was there an error getting the voltage?
        lawSys->errorMessage = "Error getting the offset voltage on sub-system " + subSysNo + ", amplifier " + to_string(lawSys->cycleNum_);
        lawSys->errorMessage += "! Error:" + error;// Create an error message.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        offset = oldOffset;                 // Use the old offset, since there was an error.
    }                                       // Done dealing with offset errors.
    lawSys->offset_.at(lawSys->cycleNum_) = offset;// A valid offset was retreived, store it for temperature calculation.
}

void LawsonEquip::processColdJunctions(LawsonEquip *lawSys) {
    SCSystem sys = lawSys->getSubSystem();  // Get the sub-system which this Model 201 belongs.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    int thermAmp = lawSys->cycleNum_ - lawSys->nThermAmps_;// Calculate the amp to get the cold junction from.
    bool coldJuncValid = true;          // A flag that indicates if the cold junction was read successfully.
    float coldJunc = 0;                 // The measured cold junction voltage.
    try {                               // Get the cold junction voltage for the thermo amp corresponding to the cycle number.
        coldJunc = lawSys->getVoltage(lawSys);// Get the cold junction voltage.
    } catch (string error) {                // Was there an error getting the voltage?
        lawSys->errorMessage = "Error getting the cold junction on sub-system " + subSysNo + ", amplifier " + to_string(thermAmp);// Yes.
        lawSys->errorMessage += "! Error:" + error;// Create an error message.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        coldJuncValid = false;              // Use the old cold junction temperature, there was an error.
    }                                       // Done dealing with cold junction errors.
    if (coldJuncValid == true) {            // Was the cold junction read successfully?
        lawSys->coldJunc_.at(thermAmp) = coldJunc;// Calculate the
        lawSys->coldJunc_.at(thermAmp) -= lawSys->offset_.at(thermAmp);// new
        lawSys->coldJunc_.at(thermAmp) *= 1000 / lawSys->coldK_.at(thermAmp);// cold junction
        lawSys->coldJunc_.at(thermAmp) -= 273;// temperature.
    }                                       // Done calculating the cold junction temperature.
}

void LawsonEquip::processTemperatures(LawsonEquip *lawSys) {
    SCSystem sys = lawSys->getSubSystem();  // Get the sub-system which this Model 201 belongs.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    unsigned int chnl = lawSys->cycleNum_ - 2 * lawSys->nThermAmps_; // Calculate the chnl to measure.
    unsigned char thermoAmp = chnl / 14;    // Determine the thermocouple amp fo rthe given chnl.
    int inputIndex = sys.getFirstInput() + chnl; // Calculate the input index.
    float volts = 0;                        // Contains the measured voltage for the given chnl.
    float measurement = 0;                  // Contains the temperature.
    try {                               // Get the cold junction voltage for the thermo amp corresponding to the cycle number.
        volts = lawSys->getVoltage(lawSys);// Get the cold junction voltage.
    } catch (string error) {                // Was there an error getting the voltage?
        lawSys->errorMessage = "Error measuring temperature on sub-system " + subSysNo + ", channel " + to_string(chnl);//Yes.
        lawSys->errorMessage += "! Error:" + error;// Create an error message.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        lawSys->temperature_.at(chnl) = 500;// Set the temperature upscale.
        return;                             // Done processing the temeprature.
    }                                       // Done getting the chnl voltage.
    volts -= lawSys->offset_.at(thermoAmp); // Correct the voltage by the offset.
    SCClasses::PressureInput* pInput = nullptr;// The compiler has problems with pointers in a case statement.
    SCClasses::SCInput* input = SCEquipment::plantInputs->at(inputIndex);
    if (input->sensorType == SCInput::SENSOR_PRESSURE_TRANSDUCER) {
        pInput = (PressureInput*)input;
    }
    switch (input->sensorType)
    {
        case SCClasses::SCInput::SENSOR_THERMOCOUPLE_T:// This input was a type T thermocouple.
            measurement = volts * 1000 * lawSys->gainK_.at(thermoAmp); // Adjust the voltage for by the gain on the 35B.
            if (measurement < -136.1)
                measurement = .271 * measurement + 1.883;
            else if (measurement >= -136.1 && measurement < -98.3)
                measurement = .26455 * measurement + 1.005;
            else if (measurement >= -98.3 && measurement < -59.5)
                measurement = .2577 * measurement + .33505;
            else if (measurement >= -59.5 && measurement < -20.1)
                measurement = .2538 * measurement + .1015;
            else if (measurement >= -20.1 && measurement < 20.2)
                measurement = .2481 * measurement;
            else if (measurement >= 20.2 && measurement < 61.3)
                measurement = .2433 * measurement + .08516;
            else if (measurement >= 61.3 && measurement < 103.3)
                measurement = .2381 * measurement + .40476;
            else if (measurement >= 103.3 && measurement < 146.1)
                measurement = .2336 * measurement + .8645;
            else if (measurement >= 146.1 && measurement < 189.7)
                measurement = .2294 * measurement + 1.4908;
            else if (measurement >= 189.7 && measurement < 234.2)
                measurement = .2247 * measurement + 2.3708;
            else if (measurement >= 234.2 && measurement < 279.5)
                measurement = .22075 * measurement + 3.3002;
            else if (measurement >= 279.5 && measurement < 325.6)
                measurement = .2169 * measurement + 4.3709;
            else if (measurement >= 325.6)
                measurement = .2141 * measurement + 5.2784;
            measurement += lawSys->coldJunc_.at(thermoAmp);// Adjust for the cold junction temperature.
            if(SCEquipment::isInArtificialMode == true) {// Is the system in artificial mode?
                measurement = 25 - chnl / 10;           // Yes. Create an artificial temperature.
            }                                           // Done with artificial mode.
            if(sys.isOnLine == false) {                 // Is this sub-system offline?
                measurement = 500;                      // Yes. Create an upscale temperature.
            }                                           // Done with an offline subsystem.
            measurement = measurement * 1.8 + 32;       // Standard equation for converting from degres C to degrees F.
            // All temperature processing including storage and control ocurres using Fahrenheit.
            if(measurement > 500) {                     // Is the measurement greater than the limit?
                measurement = 500;                      // Yes. Adjust to the limit.
            }                                           // Done with a high measurement.
            if(measurement < -200) {                    // Is the measurement less than the limit?
                measurement = 503.1;                    // Yes. Adjust above the limit upscale.
            }                                           // Done with a low measurement.
            lawSys->temperature_.at(chnl) = measurement;// Put the measurement in this subsystem's temperature list.
            break;
        case SCClasses::SCInput::SENSOR_THERMOCOUPLE_K:// This chnl has a type K thermocouple.
            measurement = volts * 1000 * lawSys->gainK_.at(thermoAmp);// Adjust the voltage for by the gain on the 35B.
            if (measurement < -138.4)
                measurement = .26178 * measurement + 1.23037;
            else if (measurement >= -138.4 && measurement < -99.5)
                measurement = .25707 * measurement + .57841;
            else if (measurement >= -99.5 && measurement < -60.0)
                measurement = .25316 * measurement + .18987;
            else if (measurement >= -60.0 && measurement < -20.1)
                measurement = .25063 * measurement + .03759;
            else if (measurement >= -20.1 && measurement < 20.2)
                measurement = .24814 * measurement;
            else if (measurement >= 20.2 && measurement < 60.9)
                measurement = .2457 * measurement + .03686;
            else if (measurement >= 60.9 && measurement < 101.9)
                measurement = .2439 * measurement + .14634;
            else if (measurement >= 101.9 && measurement < 143.2)
                measurement = .24213 * measurement + .32688;
            else if (measurement >= 143.2 && measurement < 184.6)
                measurement = .24155 * measurement + .41063;
            else if (measurement >= 184.6 && measurement < 226.1)
                measurement = .24096 * measurement + .51807;
            else if (measurement >= 226.1 && measurement < 267.6)
                measurement = .24096 * measurement + .51807;
            else if (measurement >= 267.6 && measurement < 309.1)
                measurement = .24096 * measurement + .51807;
            else if (measurement >= 309.1)
                measurement = .24155 * measurement + .33816;
            measurement += lawSys->coldJunc_.at(thermoAmp);// Adjust for the cold junction temperature.
            if(SCEquipment::isInArtificialMode == true) {// Is the system in artificial mode?
                measurement = 25 - chnl / 10;           // Yes. Create an artificial temperature.
            }                                           // Done with artificial mode.
            if(sys.isOnLine == false) {                 // Is this sub-system offline?
                measurement = 500;                      // Yes. Create an upscale temperature.
            }                                           // Done with an offline subsystem.
            measurement = measurement * 1.8 + 32;       // Standard equation for converting from degres C to degrees F.
            // All temperature processing including storage and control ocurres using Fahrenheit.
            if(measurement > 500) {                     // Is the measurement greater than the limit?
                measurement = 500;                      // Yes. Adjust to the limit.
            }                                           // Done with a high measurement.
            if(measurement < -200) {                    // Is the measurement less than the limit?
                measurement = 503.1;                    // Yes. Adjust above the limit upscale.
            }                                           // Done with a low measurement.
            lawSys->temperature_.at(chnl) = measurement;// Put the measurement in this subsystem's temperature list.
            break;
        case SCClasses::SCInput::SENSOR_PRESSURE_TRANSDUCER:// This chnl has a pressure transducer.
            // Use the input's calibration constants to determine the pressure.
            measurement = pInput->k1 + pInput->k2 * volts * lawSys->gainK_.at(thermoAmp) / 100;
            pInput->setMeasuredValue(measurement);      // Set the input's measured value.
            break;                                      // Done with pressure transducers.
        case SCClasses::SCInput::SENSOR_CURRENT_TRANSFORMER:
            break;                                      // Nothing to do.
        case SCClasses::SCInput::SENSOR_VOLTAGE_TRANSDUCER:
            break;                                      // Nothing to do.
        default:
            lawSys->errorMessage = "Channel " + to_string(chnl) + "has an illegal sensor type";//Yes.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
            break;
    }                                           // End of the Sensor Type switch statement.
    return;                                     // The temperature(or voltage) has been set.
}

void LawsonEquip::processTurningRelaysOn(LawsonEquip *lawSys) {
    // This should only be called by a lawson SYSTYPE_STANDARD.
    SCSystem sys = lawSys->getSubSystem();// Get the sub-system which this Model 201 belongs.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    if (sys.getType() != SCClasses::SCSystem::SYSTYPE_STANDARD) {// Is this a standard lawson system?
        lawSys->errorMessage = "LawsonEquip::processTurningRelaysOn() on sub-system " + subSysNo;// No.
        lawSys->errorMessage += " called by non-SYSTYPE_STANDARD sub-system.";// Create an error message.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return;                         // There is no point in continueing.
    } // This is a standard lawson system.
    if (sendStandardRelayOnCommand(lawSys) == false) {// Did sending the standard system's relay on command succeed?
        return;                         // No. Done trying to turn relays on.
    }                                   // Done sending the relay on command.
    for(unsigned char relayDex = 0; relayDex < sys.getNumberOfRelays(); relayDex++) {// Go through each relay on this sub-system.
        Relay* relay = plantRelays->at(sys.getFirstRelay() + relayDex);// Get access to this relay which is a part of this sub-system.
        if (lawSys->isARelayTesting == true && relay->indexNo != lawSys->testRelayDex_) {// Is there a test relay but not this relay?
            continue;                   // Yes. This relay shouldn't be turned on.
        }                               // A relay test is not in progress or this is the relay being tested.
        // Does this relay want to be turned on?
        if (relay->isWantingOn == true || (lawSys->isARelayTesting == true && relay->indexNo == lawSys-> testRelayDex_)) {
            if (relay->isOn == false) { // Yes. Is this relay off?
                if (lawSys->selectStandardRelay(lawSys, relay) == false) {// Was the standard relay selected?
                    lawSys->errorMessage = "Failed to select relay " + to_string(relay->indexNo) + " on sub-system " + subSysNo + ".";// No
                    lawSys->toLog(lawSys->errorMessage);// Display the error.
                }                       // The standard relay was selected.
                relay->enteringOnState();//Complete the details of turning on a relay which was off.
            } else {                    // This relay was on and wants to stay on.
                relay->incrementTimeOn();// Increment this relay's time-on.
                unsigned int fRelay = sys.getFirstRelay();// Get this sub-system's first relay for readability.
                if (relay->indexNo >= lawSys->checkRelayIndex_ + fRelay &&
                    relay->indexNo < lawSys->checkRelayIndex_ + fRelay + sys.getNumberOfRelays()) {// Should this relay be reset?
                    if (selectStandardRelay(lawSys, relay) == false) {
                        lawSys->errorMessage = "Failed to reselect relay " + to_string(relay->indexNo) + " on sub-system " + subSysNo + ".";// No
                        lawSys->toLog(lawSys->errorMessage);// Display the error.
                    }                   // The standard relay was selected.
                }                       // The relay was reselected if required.
            }                           // Done turning or keeping on the relay regardless of its current state.
        }                               // The relay is on or doesn't want to come on.
    }                                   // End of the loop going through each relay in this sub-system.
    // Is this a system that should have an alarm on and is not doing a relay test?
    if (lawSys->doesAlarmWantOn == true && lawSys->isARelayTesting == false) {
        if (lawSys->selectStandardRelay(lawSys, nullptr, 86) == false) {// Yes. Was the standard relay alarm selected?
            lawSys->errorMessage = "Failed to select the alarm relay on sub-system " + subSysNo + ".";// No
            lawSys->toLog(lawSys->errorMessage);// Display the error.
        }                               // The standard alarm relay was selected.
    }                                   // The standard alarm relay was selected if it was supposed to be.
    if (lawSys->isARelayTesting == true) {// Is a relay being tested?
        lawSys->relayTestSpan_ = chrono::system_clock::now() - lawSys->testRelayStartTime_;
        if (lawSys->relayTestSpan_ > chrono::minutes(3)) {// Has the test been running for more than 3 minutes without an update?
            lawSys->isARelayTesting = false;// Yes. Reset the test flag to end the relay test. Time-out condition.
            // Start here, verify the relay is on this sub-system and then request that it be turned off.
            unsigned int fRelay = sys.getFirstRelay();// Get the first relay's system wide index.
            unsigned int tRelay = lawSys->getTestRelay();// Get the test relay's system wide index.
            if (tRelay >= fRelay && tRelay < fRelay + sys.getNumberOfRelays()) {// Is the test relay part of this sub-system?
                Relay* testRelay = plantRelays->at(lawSys->getTestRelay());// Yes. Get access to the test relay.
                testRelay->requestToTurnOff();// Request that the test relay be turned off.
            }                           // The test relay, if it was a part of this system has been requested to turn off.
        }                               // The relay test has been tested for a time-out condition.
    }                                   // A relay test, if there was one, has been tested for time-out.
    // Try sending the relay off command. This is to help prevent a relay from turning on from a spike.
    // It leaves the model 188 in the state of turning off relays rather than turning on relays.
    this_thread::sleep_for(chrono::milliseconds(50));// Wait for the model 201.
    if (lawSys->sendStandardRelayOffCommand(lawSys) == false) {// Did the model 188 go to the off relay off mode?
        lawSys->errorMessage = "Error putting the model 188 in off mode on sub-system " + subSysNo + ".";
        lawSys->toLog(lawSys->errorMessage);// Display the error.
    }                                   // The model 188 is in the relay off mode.
}                                       // The standard system has those relays which should be on activated.

void LawsonEquip::processTurningRelaysOff(LawsonEquip *lawSys) {
    // This should only be called by a lawson SYSTYPE_STANDARD.
    SCSystem sys = lawSys->getSubSystem();// Get the sub-system which this Model 201 belongs.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    if (sys.getType() != SCClasses::SCSystem::SYSTYPE_STANDARD) {// Is this a standard lawson system?
        lawSys->errorMessage = "LawsonEquip::processTurningRelaysOff() on sub-system " + subSysNo;// No.
        lawSys->errorMessage += " called by non-SYSTYPE_STANDARD sub-system.";// Create an error message.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return;                         // There is no point in continueing.
    } // This is a standard lawson system.
    // It is not necessary to send the relay off command since the model 188 was left with that command.
    for(unsigned char relayDex = 0; relayDex < sys.getNumberOfRelays(); relayDex++) {// Go through each relay on this sub-system.
        Relay* relay = plantRelays->at(sys.getFirstRelay() + relayDex);// Get access to this relay which is a part of this sub-system.
        // Is there a relay test and this is the relay?
        if (lawSys->isARelayTesting == true && relay->indexNo == lawSys->testRelayDex_) {
            continue;                   // Yes. This relay shouldn't be turned off.
        }                               // A relay test is not in progress or this is the relay being tested.
        if (relay->isWantingOn == false || lawSys->isARelayTesting == true) {// Does this relay want to be turned off?
            if (relay->isOn == true) {  // Yes. Is this relay already on?
                relay->isWantingOn = false;// Set this to false just in case it is a relay test.
                if (lawSys->selectStandardRelay(lawSys, relay) == false) {// Was the standard relay selected?
                    lawSys->errorMessage = "Failed to select relay " + to_string(relay->indexNo) + " on sub-system " + subSysNo + ".";// No
                    lawSys->toLog(lawSys->errorMessage);// Display the error.
                }                       // The standard relay was selected.
                relay->enteringOffState();//Complete the details of turning off a relay which was on.
            } else {                    // This relay was off and wants to stay off.
                unsigned int fRelay = sys.getFirstRelay();// Get this sub-system's first relay for readability.
                if (relay->indexNo >= lawSys->checkRelayIndex_ + fRelay &&
                    relay->indexNo < lawSys->checkRelayIndex_ + fRelay + sys.getNumberOfRelays()) {// Should this relay be reset?
                    if (selectStandardRelay(lawSys, relay) == false) {// Yes. Select this relay to turn it off.
                        // No.
                        lawSys->errorMessage = "Failed to reselect relay " + to_string(relay->indexNo) + " on sub-system " + subSysNo + ".";
                        lawSys->toLog(lawSys->errorMessage);// Display the error.
                    }                       // The standard relay was selected.
                }                           // The relay was reselected if required.
            }                               // Done turning or keeping on the relay regardless of its current state.
        }                                   // The relay is on or doesn't want to come on.
    }                                       // End of the loop going through each relay in this sub-system.
    // Is this a standard system that should have an alarm off?
    if (lawSys->doesAlarmWantOn == false && sys.getType() == SCSystem::SYSTYPE_STANDARD) {
        if (lawSys->selectStandardRelay(lawSys, nullptr, 86) == false) {// Yes. Was the standard relay alarm selected?
            lawSys->errorMessage = "Failed to select the alarm relay on sub-system " + subSysNo + ".";// No
            lawSys->toLog(lawSys->errorMessage);// Display the error.
        }                                   // The standard alarm relay was selected.
    }                                       // The standard alarm relay was selected if it was supposed to be.
}

void LawsonEquip::processCalibration(LawsonEquip *lawSys) {
    if (lawSys->voltageRead_ == 0) {        // Are special voltages being measured?
        SCSystem sys = lawSys->getSubSystem();// Get the sub-system which this Model 201 belongs.
        string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
        chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// Get the current time.
        time_t tt = chrono::system_clock::to_time_t(now);// Convert to get the minute of the day.
        struct tm* tmNow;                   // Will contain a more usable form of the current time.
        tmNow = localtime(&tt);             // Convert the current time into a more usable format.
        unsigned int minuteOfTheDay = tmNow->tm_hour * 60 + tmNow->tm_min;// The minute of the day determines when data is stored.
        if (minuteOfTheDay % 6 == 1) {      // The system is recalibrated once every six minutes. Is it time?
            if (lawSys->calFlag_ == true) { // Yes. Is it OK to calibrate? Has calibration happened this minute?
                if (lawSys->calSys(lawSys) == false) {// No. Was the system recalibrated?
                    lawSys->errorMessage = "System calibration failed on sub-system " + subSysNo + ".";// No.
                    lawSys->toLog(lawSys->errorMessage);// Display the error.
                    return;                 // Calibration is over.
                }                           // The system was recalibrated.
                lawSys->calFlag_ = false;   // Don't allow calibration to happen again this minute.
                if (lawSys->selectChnl(lawSys, 0) == false) {// Was channel 0 selected?
                    lawSys->errorMessage = "Couldn't select channel 0 after calibration on sub-system " + subSysNo + ".";// No.
                    lawSys->toLog(lawSys->errorMessage);// Display the error.
                    return;                 // there is nothing left to do.
                }                           // Channel 0 was selected.
            }                               // Done recalibrating.
        }                                   // Done testing for recalibration time.
        else {                              // It is not a calibration minute.
            lawSys->calFlag_ = true;        // But, it is OK to recalibrate again on the next calibration minute.
        }                                   // Done setting the calibration flag.
    }                                       // Done running a system calibration since voltages were not being measured.
}

void LawsonEquip::processPulseCounts(LawsonEquip *lawSys) {
    SCSystem sys = lawSys->getSubSystem();  // Get the sub-system which this Model 201 belongs.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    if (sys.getType() == SCSystem::SYSTYPE_MICRO) {// Is this a micro sub-system?
        return;                             // Yes. There is no pulse counter.
    }                                       // This is a Standard sub-system. There is a pulse counter.
    if (sys.isOnLine == false) {            // Is this sub-system off-line?
        return;                             // Yes. There is no connection to a Model 201 to get a count.
    }                                       // This sub-system is on-line.
    if (SCEquipment::isInArtificialMode == true) {// Is the Plant Manager in artifical mode?
        lawSys->pulseCount_++;              // Yes. Simulate the pulse count increasing.
        return;                             // Yes. Done with a simulated pulse counter.
    }                                       // The Plant Manager is not in artificial mode.
    if (lawSys->initializing_ == false) {   // Is this equipment being initialized?
        if (lawSys->isOperating == false) { // No. Is this equipment operating?
            return;                         // No. Nothing can be sent until the equipment is operating.
        }                                   // This equipment is operating.
    }                                       // This equipment is either operating or being initialized. OK to send packet.
    if (sendPacket(lawSys, 0x80, 0x4C) == false) {// Did sending the get pulse count command work?
        lawSys->errorMessage = "Error sending the get pulse count command on sub-system " + subSysNo + "!";// No create an error message.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return;                             // The pulse count cannot be obtained this time.
    }                                       // The pulse count command was sent.
    if (lawSys->getBytes(lawSys, 2, 2000) == false) {// Was there a reponse to get pulse counts?
        lawSys->errorMessage = "Error receiving the counts on sub-system " + subSysNo + "!";// No.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return;                             // The pulse count cannot be obtained this time.
    }                                       // The model 201 responded to get pulse counts.
    if (lawSys->pulseCountReading_ == 99999999) {// Has the pulse count been read yet?
        lawSys->pulseCountReading_ = lawSys->commandResponse_[1];// No. et the pulse count to the current value.
    }                                       // Done saving the pulse count reading.
    int changeInCounts = 0;                 // Contains the number of counts since the last time the counter was read.
    if (lawSys->commandResponse_[1] >= lawSys->pulseCountReading_) {// Is the new counter value greater than the stored value?
        changeInCounts = lawSys->commandResponse_[1] - lawSys->pulseCountReading_;// Yes. Calculate the change in counts.
    } else {                                // The pulse counter wrapped.
        changeInCounts = 256 + lawSys->commandResponse_[1] - lawSys->pulseCountReading_;// Adjust the change in counts for the wrap.
    }                                       // The change in pulse counts has been calculated.
    lawSys->pulseCount_ += changeInCounts;  // Update the total pulse count.
    if (lawSys->pulseCount_ > 9999999) {    // Is the pulse count too large? Could it cause an overflow?
        lawSys->pulseCount_ = changeInCounts;// Reset the pulse count.
    }                                       // Done preventing pulse count overflow.
    lawSys->pulseCountReading_ = lawSys->commandResponse_[1];// Remeber the last counter reading.
}

void LawsonEquip::toggleWatchdog(LawsonEquip *lawSys) {
    SCSystem sys = lawSys->getSubSystem();  // Get the sub-system which this Model 201 belongs.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    if (sys.getType() != SCSystem::SYSTYPE_STANDARD) {// Is this a standard sub-system?
        return;                             // There is no watchdog on a micro sub-system.
    }                                       // This is a standard sub-system.
    chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// Get the current time.
    lawSys->watchDogTimeSpan_ = now - lawSys->watchDogTime_;// Update the watchdog's time in its current state.
    if (lawSys->watchDogTimeSpan_ < chrono::seconds(40)) {// Has the watch dog been in its current state for less than 40 seconds?
        return;                             // Yes. It is too soon to change the watchdog state.
    }                                       // 40 seconds have gone by.
    if (lawSys->watchDogState_ == 0) {      // Is the watchdog relay off?
        if (lawSys->sendStandardRelayOnCommand(lawSys) == false) {// Yes. Send the relay on command. Did it send?
            lawSys->errorMessage = "Couldn't send the watchdog relay on command on sub-system " + subSysNo + ".";// No.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
            return;                         // The watchdog cannot be toggled this time.
        }                                   // The relay on command was sent.
        if (lawSys->selectStandardRelay(lawSys, nullptr, 87)) {
            lawSys->errorMessage = "Couldn't select the watchdog relay to activate it on sub-system " + subSysNo + ".";// No.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
            return;                         // The watchdog cannot be toggled this time.
        }                                   // The watchdog relay was selected.
        lawSys->watchDogState_ = 1;         // Remember the watchdog relay is on.
    } else if (lawSys->watchDogState_ == 1) {// Is this a wait cycle?
        lawSys->watchDogState_ = 2;         // Yes. Signal the relay is on and needs to be turned off.
        return;                             // That is all for this time.
    } else {                                // Turn the watchdog relay off.
        if (lawSys->selectStandardRelay(lawSys, nullptr, 87)) {
            lawSys->errorMessage = "Couldn't select the watchdog relay to deactivate it on sub-system " + subSysNo + ".";// No.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
            return;                         // The watchdog cannot be toggled this time.
        }                                   // The watchdog relay was selected.
    }                                       // The watchdog relay should be off.
    // Always send the relay off command last just in case of noise.
    if (lawSys->sendStandardRelayOffCommand(lawSys) == false) {// Send the relay off command. Did it send?
        lawSys->errorMessage = "Couldn't send the watchdog relay off command on sub-system " + subSysNo + ".";// No.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
    }                                       // The relay on command was sent.
}

bool LawsonEquip::sendStandardRelayCommand(LawsonEquip *lawSys, unsigned char command) {
    SCSystem sys = lawSys->subSystem;       // Get this equipments sub-system.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    if (sys.isOnLine == false) {            // Is this sub-system online?
        return true;                        // No. There is no need to send to the Model 201. It is offline. But no failure.
    }                                       // This sub-system is online.
    if (SCEquipment::isInArtificialMode == true) {// Is the Plant Manager in artufical mode?
        return true;                        // Yes. There is no need to send to the Model 201. But no failure.
    }                                       // The Plant Manager is not in artificial mode.
    if (lawSys->initializing_ == false) {   // Is this equipment being initialized?
        if (lawSys->isOperating == false) { // No. Is this equipment operating?
            return true;                    // No. Nothing can be sent until the equipment is operating.
        }                                   // This equipment is operating.
    }                                       // This equipment is either operating or being initialized. OK to send packet.
    if (sendPacket(lawSys, 0x02, command) == false) {
        lawSys->errorMessage = "Error sending relay command " + to_string(command) + " on sub-system " + subSysNo + ".";
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;                       // Let the caller know the relay command could not be sent.
    }
    chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
    while (lawSys->thePort_.bytesToSend() > 0 && chrono::system_clock::now() - start > chrono::milliseconds(200)) {
        this_thread::sleep_for(chrono::milliseconds(10));// Wait for 10 milliseconds for the Model 201 to be ready.
    }                                       // The send succeeded or timed out.
    if (chrono::system_clock::now() - start > chrono::milliseconds(200)) {
        lawSys->errorMessage = "Timeout waiting to send relay command on sub-system " + subSysNo + ".";
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;                       // Let the caller know the relay command could not be sent.
    }                                       // The relay command was sent.
    return true;                            // The relay command was sent.
}

bool LawsonEquip::sendStandardRelayOnCommand(LawsonEquip *lawSys) {
    if(sendStandardRelayCommand(lawSys, 112) == true) {// Did the turn relay on command succeed?
        if(sendStandardRelayCommand(lawSys, 240) == true) {//Yes. Did the stobe high command succeed?
            this_thread::sleep_for(chrono::milliseconds(1));// Yes. Pause.
            if (sendStandardRelayCommand(lawSys, 112) == false) {// Did the stobe low command succeed?
                lawSys->errorMessage = "Failed to issue the relay on command.";// No. Create an error message.
                lawSys->toLog(lawSys->errorMessage);// Display the error.
                return false;               // Let the caller know the relay on command failed.
            }                               // Done with failed relay on command.
        } else {                            // Did the relay on command fail?
            lawSys->errorMessage = "Failed to set strobe high while issuing relay on command.";// Yes. Create an error message.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
            return false;               // Let the caller know the relay on command failed.
        }                               // Done with failed relay on command.
    } else {                            // Failed to send the relay on command?
        lawSys->errorMessage = "Failed to set strobe low while issuing relay on command.";// Yes. Create an error message.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;                   // Let the caller know the relay on command failed.
    }                                   // Done dealing with a failed relay on command.
    this_thread::sleep_for(chrono::milliseconds(2));// Pause for lawson equipment.
    return true;                        // Let the caller know the relay on command succeeded.
}

bool LawsonEquip::sendStandardRelayOffCommand(LawsonEquip *lawSys) {
    if(sendStandardRelayCommand(lawSys, 120) == true) {// Did the turn relay off command succeed?
        if(sendStandardRelayCommand(lawSys, 248) == true) {//Yes. Did the stobe high command succeed?
            this_thread::sleep_for(chrono::milliseconds(1));// Yes. Pause.
            if (sendStandardRelayCommand(lawSys, 120) == false) {// Did the stobe low command succeed?
                lawSys->errorMessage = "Failed to issue the relay off command.";// No. Create an error message.
                lawSys->toLog(lawSys->errorMessage);// Display the error.
                return false;               // Let the caller know the relay off command failed.
            }                               // Done with failed relay on command.
        } else {                            // Did the relay off command fail?
            lawSys->errorMessage = "Failed to set strobe high while issuing relay off command.";// Yes. Create an error message.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
            return false;               // Let the caller know the relay off command failed.
        }                               // Done with failed relay off command.
    } else {                            // Failed to send the relay off command?
        lawSys->errorMessage = "Failed to set strobe low while issuing relay off command.";// Yes. Create an error message.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;                   // Let the caller know the relay off command failed.
    }                                   // Done dealing with a failed relay off command.
    return true;                        // Let the caller know the relay off command succeeded.
}

bool LawsonEquip::turnAllRelaysOff(LawsonEquip *lawSys) {
    SCSystem sys = lawSys->subSystem;   // Get this equipments sub-system.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    for (unsigned int relayDex = 0; relayDex < sys.getNumberOfRelays(); relayDex++) {
        SCClasses::Relay* relay = plantRelays->at(sys.getFirstRelay() + relayDex);// Get access to a relay on this sub-system.
        if (relay->isOn == true) {      // Is this relay on?
            relay->enteringOffState();  // Yes. Turn this relay off.
        }                               // This relay is either off or going to be off.
    }
    if (sys.getType() == SCSystem::SYSTYPE_MICRO) {// Is this a micro sub-system?
        if (sendPacket(lawSys, 0x02, 0xFF) == false) {// Yes. Send the command to turn off all the relays. Did it work?
            lawSys->errorMessage = "Failed to turn off all relays on micro sub-system " + subSysNo + "!";// No. Error.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
            return false;               // Let the caller know all the relay were not turned off.
        }                               // The relays on the micro sub-system were turned off.
    } else {                            // This is a standard Lawson sub-system.
        if (sendStandardRelayCommand(lawSys, 104) == true) {// Did the all relays off command succeed?
            if (sendStandardRelayCommand(lawSys, 232) == true) {// Yes. Did the strobe high command succeed?
                if (sendStandardRelayCommand(lawSys, 104) == false) {// Yes. Did the strobe low command succeed?
                    lawSys->errorMessage = "Failed to issue the strobe low in all relays off, sub-system " + subSysNo + "!";// No.
                    lawSys->toLog(lawSys->errorMessage);// Display the error.
                    return false;       // Let the caller know all the relays were not turned off.
                }                       // The strobe low was successfull.
            } else {                    // The strobe high command failed.
                lawSys->errorMessage = "Failed to issue the strobe high in all relays off, sub-system " + subSysNo + "!";// Error message.
                lawSys->toLog(lawSys->errorMessage);// Display the error.
                return false;       // Let the caller know all the relays were not turned off.
            }
        } else {                    // The all relays off command failed.
            lawSys->errorMessage = "Failed to issue all relays off in all relays off, sub-system " + subSysNo + "!";// Error message.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
            return false;           // Let the caller know all the relays were not turned off.
        }                           // All realys off was successfully isseud for a standard lawson sub-system.
    }                               // All realys off was successfully issued for either a micro or standard lawson sub-system.
    return true;                    // All of this sub-system's relays were turned off.
}

bool LawsonEquip::selectStandardRelay(LawsonEquip *lawSys, Relay *relay, unsigned char relayNo /* = 0x00*/) {
    SCSystem sys = lawSys->subSystem;// Get this equipment's sub-system.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    unsigned int relaySubDex = 0;   // Set the relay index with respect this sub-system.
    if (relay == nullptr) {         // Did the caller provide a relay?
        relaySubDex = relayNo;      // No. Set the relay index with respect this sub-system.
    } else {                        // The caller provided a relay.
        if (relay->indexNo < sys.getFirstRelay() || relay->indexNo >= sys.getFirstRelay() + sys.getNumberOfRelays()) {
            lawSys->errorMessage = "Error selecting standard relay. ";// No. Create
            lawSys->errorMessage += "Relay index " + to_string(relay->indexNo) + " is not a part of sub-system " + subSysNo + ".";// error.
            lawSys->toLog(lawSys->errorMessage);// Display the error.
            return false;           // Let the caller know the relay select command failed.
        }                           // The relay is a part of this sub-system.
        relaySubDex = relay->indexNo - sys.getFirstRelay();// Convert the relay's system wide index to a sub-system index.
    }                               // The relay's index with respect to this sub-system is set.
    if(sendStandardRelayCommand(lawSys, relaySubDex) == false) {// Was the relay selected?
        lawSys->errorMessage = "Error selecting relay " + to_string(relaySubDex) + " on sub-system " + subSysNo + ".";// No. Create a message.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;               // Let the caller know the relay select command failed.
    }                               // Done sending and checking the select relay command.
    this_thread::sleep_for(chrono::milliseconds(2));// Wait for the model 201.
    if(sendStandardRelayCommand(lawSys, relaySubDex + 128) == false) {// Was the relay strobe set high?
        lawSys->errorMessage = "Error strobing relay " + to_string(relaySubDex) + " high on sub-system " + subSysNo + ".";// No. Error.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;               // Let the caller know the relay select command failed.
    }                               // Done sending and checking the strobe relay high command.
    this_thread::sleep_for(chrono::milliseconds(2));// Wait for the model 201.
    if(sendStandardRelayCommand(lawSys, relaySubDex) == false) {// Was the relay selected?
        lawSys->errorMessage = "Error strobing relay " + to_string(relaySubDex) + " low on sub-system " + subSysNo + ".";  // No. Error.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
        return false;               // Let the caller know the relay select command failed.
    }                               // Done sending and checking the select relay command.
    this_thread::sleep_for(chrono::milliseconds(2));// Wait for the model 201.
    return true;                    // Let the caller know the relay was select, turned on or off.
}

void LawsonEquip::processMicroRelays(LawsonEquip *lawSys) {
    SCSystem sys = lawSys->subSystem;// Get this equipment's sub-system. This should only be called if the sub-system is a Micro.
    string subSysNo = to_string(sys.getSubSystemNo());// Get the sub-system number.
    for(unsigned char relayDex = 0; relayDex < sys.getNumberOfRelays(); relayDex++) {// Go through each relay on this Micro sub-system.
        Relay* relay = plantRelays->at(sys.getFirstRelay() + relayDex);// Get access to this relay which is a part of this sub-system.
        if (lawSys->isARelayTesting == true && relay->indexNo != lawSys->testRelayDex_) {// Is there a test relay but not this relay?
            continue;                   // Yes. This relay shouldn't be changed.
        }                               // A relay test is not in progress or this is the relay being tested.
        if (SCEquipment::isInArtificialMode == true) {// Is the Plant Manager in artufical mode?
            if (relay->isWantingOn == true) {// Yes. Does this relay want on?
                if (relay->isOn == false) {// Yes. Is this relay currently off?
                    relay->enteringOnState();// Yes. This relay needs to turn on to complete the simulation.
                }                       // The relay is either in or just went into the on state.
            } else {                    // The relay wants to be off.
                if (relay->isOn == true) {// Is this relay currently on?
                    relay->enteringOffState();// Yes. This relay needs to turn off to complete the simulation.
                }                       // The relay is either in or just went into the off state.
            }                           // The relay is in the correct state.
            continue;                   // This relay has been comletely handled for artificial mode, go onto the next relay.
        }                               // The Plant Manager is not in artificial mode.
        unsigned char flag = 1;         // Contains the bit for this relay.
        flag <<= relay->indexNo;        // Shift the bit to the right location for this relay. There are at most eight relays.
        // Does this relay want on or is this relay being tested?
        if (relay->isWantingOn == true || (lawSys->isARelayTesting == true && relay->indexNo == lawSys->testRelayDex_)) {
            flag = flag ^ 0xFF;         // Yes. XOR the flag. So, only the flag position will be zero. The rest will be one.
            lawSys->microRelayBuffer_ &= flag;// Turn on this relay in the relay buffer. The relay buffer uses reverse logic.
            if (relay->isOn == false) { // Is relay off and now turning on?
                relay->enteringOnState();// Yes. Complete the details of turning on a relay which was off.
            }                           // The relay is in the on state.
        } else {                        // This relay wants to be in the off state.
            lawSys->microRelayBuffer_ |= flag;// OR the flag. So, only the flag position will be set to one. The rest will be the same.
            if (relay->isOn == true) {  // Is relay on and now turning off?
                relay->enteringOffState();// Yes. Complete the details of turning off a relay which was on.
            }                           // The relay is in the off state.
        }                               // The state of this micro relay has been set.
    } // The state of all the Micro relays has been set.
    if (sendStandardRelayCommand(lawSys, lawSys->microRelayBuffer_) == false) {// Yes. Set the relay states.
        // Did setting the relay states for this micro system fail?
        lawSys->errorMessage = "Failed to set the relay states on micro sub-system " + subSysNo + ".";// Yes. Create an error.
        lawSys->toLog(lawSys->errorMessage);// Display the error.
    }                                   // Done actually turning on and off relays for this Micro sub-system.
} // End of processMicroRelays()

unsigned char LawsonEquip::nextChnl(LawsonEquip *lawSys) {
    unsigned char nextChnl = 0;                 // The next chnl to select based on the cylcle number and the number of thermocouple
                                            // amplifiers.
    int temperatureCycles = 2 * lawSys->nThermAmps_ + lawSys->subSystem.getNumberOfInputs();// Calculate the number of cycles
                                            // required to process the number of inputs on this sub-system.
    if (lawSys->cycleNum_ < lawSys->nThermAmps_) {// Are we measuring amplifier offsets?
        nextChnl = lawSys->cycleNum_ * 16;  // Yes. Set the chnl to the next 35B's offset chnl.
    }                                       // Done selecting the offset chnl.
    else if (lawSys->cycleNum_ < 2 * lawSys->nThermAmps_) {// Are we measuring cold junctions?
        unsigned char thrmAmp = lawSys->cycleNum_ - lawSys->nThermAmps_;// Yes. Claculate the 35B to look at.
        nextChnl = thrmAmp * 16 + 3;        // Return the chnl for the cold junction.
    }                                       // Done selecting cold junction chnl.
    else if (lawSys->cycleNum_ < temperatureCycles) {// Are we measuring chnl temepratures?
        unsigned char chnl = lawSys->cycleNum_ - 2 * lawSys->nThermAmps_;// Yes. 2 * _NumOfThermAmps measure offsets
                                            // and cold junstions. Since the cycleNum_ is greater than Thus, 2 * _NumOfThermAmps,
                                            // a thermocouple(voltage) is being measured.
        nextChnl = ioChnl(lawSys, chnl);    // Convert the logical chnl into the actual 35B extension(0-95).
    }                                       // Done selecting the next chnl for measuring temperatures.
    return nextChnl;                        // Return the next 35B extension to the caller.
}

unsigned char LawsonEquip::ioChnl(LawsonEquip *lawSys, unsigned char chnl) {
    unsigned char ioChnl = 0;               // The physical input index for the given logical chnl index.
    unsigned char thrmAmp = chnl / 14;      // The 35B index. There are 14 inputs on a 35B.
    unsigned char extension = chnl % 14;    // The extension for the chnl on this 35B.
    if (extension > 1) {                    // Is the extension greater than the extension for the cold junction?
        extension++;                        // Adjust for the cold junction extension.
    }                                       // The cold junction has been dealt with.
    ioChnl = thrmAmp * 16 + extension + 1;  // The first input on a 35B is to measure the offset(zero). Thus, the logical
                                            // input(extension) must be incremented by one. There are 16 actual inputs on
                                            // a model 35B. There are only 14 inputs for thermocouples(voltages).
    return ioChnl;                          // The ioChnl has been adjusted by the offset chnl(s) and the cold junction chnl(s).
}

unsigned int LawsonEquip::getCounts(LawsonEquip *lawSys) {
    unsigned int counts = 0;
    int subSystemNo = lawSys->getSubSystem().getSubSystemNo();// Get the sub-system number for error messages.
    if (lawSys->subSystem.isOnLine == false) {// Is this sub-system online?
        return 0;                           // No. There is no need to send to the Model 201. It is offline. But no failure.
    }                                       // This sub-system is online.
    if (SCEquipment::isInArtificialMode == true) {// Is the Plant Manager in artifical mode?
        return 0;                           // Yes. There is no need to send to the Model 201. But no failure.
    }                                       // The Plant Manager is not in artificial mode.
    if (lawSys->initializing_ == false) {   // Is this equipment being initialized?
        if (lawSys->isOperating == false) { // No. Is this equipment operating?
            return 0;                       // No. Nothing can be sent until the equipment is operating.
        }                                   // This equipment is operating.
    }                                       // This equipment is either operating or being initialized. OK to send packet.
    if (emptyInputBuffer(lawSys) == false) {// Make sure the input buffer is empty. Is it empty?
        string note = "Error emptying the input buffer to read the counts ";// No.
        note += "on Lawson sub-system " + to_string(subSystemNo) + ".";
        throw note;                         // No. We can't empty the serial input buffer.
    }                                       // The input buffer is ready to receive the response.
    if (sendPacket(lawSys, 0x81, 0x00) == false) {// Send the offset calibrate command. (Resets the zero.)
        string note = "Error sending the getCounts command on sub-system " + to_string(subSystemNo) + "!";
        throw note;                         // No. Let the caller know getting counts failed.
    }                                       // The counts were obtained.
    if (lawSys->getBytes(lawSys, lawSys->wordCount_ + 1, 2000) == false) {// Was there a reponse to get counts?
        string note = "Error receiving the counts on sub-system " + to_string(subSystemNo) + "!";// No.
        throw note;                         // No. Let the caller know the get counts failed.
    }                                       // The model 201 responded to get counts.
    if (lawSys->commandResponse_[0] != 0x81) {// Was the get counts command echoed?
        string note = "Failed to receive 0x81 from the get counts command!";// No.
        throw note;                         // Let the caller know getting counts failed.
    }                                       // Getting counts succeeded.
    counts = lawSys->commandResponse_[1];   // Start with the least significat bits.
    counts += lawSys->commandResponse_[2] * 256;// Add the next most significant bits.
    counts += lawSys->commandResponse_[3] * 65536; // Add the most significant bits.
    if (lawSys->checkSum() == false) {      // Is the checksum OK?
        string note = "Bad checksum getting counts on sub-system " + to_string(subSystemNo) + "!";// No.
        throw note;                         // Let the caller know getting counts failed.
    }                                       // The checksum was good.
    return counts;                          // Provide the caller with the counts.
}

float LawsonEquip::getVoltage(LawsonEquip *lawSys) {
    double voltage = 0.0;                   // Will contain the measured voltage.
    try {                                   // Try to get the voltage proportional counts.
        voltage = getCounts(lawSys) * .0000002980232;// Start to convert the counts into volts.
        voltage = 2 * voltage - 5;          // Finsh with the conversion.
    } catch (string error) {                // Was there an error?
        string note = "Error in getVoltage():" + error;// Yes.
        throw note;                        // Pass the error on.
    }                                       // There wasn't an error. The voltage has been obtained.
    return (float)voltage;                  // Pass the voltage to the caller.
}

