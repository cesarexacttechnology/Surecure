//
//  SCEquipment.cpp
//  PlantManager
//
//  Created by Mark Davis on 5/18/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#include "SCEquipment.h"

namespace SCClasses {

bool SCEquipment::isInArtificialMode = false;
SCClasses::SCInputs* SCEquipment::plantInputs = nullptr;
SCClasses::Relays* SCEquipment::plantRelays = nullptr;
    
SCEquipment::SCEquipment(){
    isInitialized = false;                      // This sub-system has not been initialized yet.
    isOperating = false;                        // This sub-system is not operating yet.
    isOperatingEquipment = false;               // This sub-system's thread is not operating yet.
    isRequestedToSignOn = false;                // This sub-system has not been requested to sign on yet.
    timeOfLastFailure_ = chrono::system_clock::now() - chrono::seconds(1);// Best guess of when the last failure was.
    timeOfLastSuccess_ = chrono::system_clock::now();// Best guess of time of last successful communication.
    errorsBeforeRestart_ = 3;                   // The number of consecutive errors refore this sub-system is re-initialized.
    nConsecutiveErrors_ =0;                     // There haven't been any errors yet.
    timeInitialized_ = chrono::system_clock::now();// The time when this sub-system was successfully initialized.
    isConfigurationRead = false;                // The configuration has not been read yet.
    isOperationAllowed = true;                  // Operation is allowed until the Plant Manager is shut down.
    isARelayTesting = false;                    // No relays are being tested yet.
    testRelayDex_ = -1;                         // No relays are being tested yet.
    testRelayStartTime_ = chrono::system_clock::now();// Set to something even though testing hasn't started.
    errorMessage = "";                          // There have been no errors yet.
    isTemperatureMesurementComplete = false;    // Temperatures have not been measured yet.
    isRelaysAssigned = false;                   // Relays have not been assigned to this sub-system yet.
    shouldAlarmBeOn = false;                    // The alarm should not be on yet.
    doesAlarmWantOn = false;                    // The alarm should not be on yet.
}

SCEquipment::SCEquipment(SCSystem sys){
    isInitialized = false;                      // This sub-system has not been initialized yet.
    isOperating = false;                        // This sub-system is not operating yet.
    isOperatingEquipment = false;               // This sub-system's thread is not operating yet.
    isRequestedToSignOn = false;                // This sub-system has not been requested to sign on yet.
    timeOfLastFailure_ = chrono::system_clock::now() - chrono::seconds(1);// Best guess of when the last failure was.
    timeOfLastSuccess_ = chrono::system_clock::now();// Best guess of time of last successful communication.
    errorsBeforeRestart_ = 3;                   // The number of consecutive errors refore this sub-system is re-initialized.
    nConsecutiveErrors_ =0;                     // There haven't been any errors yet.
    timeInitialized_ = chrono::system_clock::now();// The time when this sub-system was successfully initialized.
    isConfigurationRead = false;                // The configuration has not been read yet.
    isOperationAllowed = true;                  // Operation is allowed until the Plant Manager is shut down.
    isARelayTesting = false;                    // No relays are being tested yet.
    testRelayDex_ = -1;                         // No relays are being tested yet.
    testRelayStartTime_ = chrono::system_clock::now();// Set to something even though testing hasn't started.
    errorMessage = "";                          // There have been no errors yet.
    subSystem = sys;                            // Copy the given system to the member subSystem.
    for (int nDex = 0; nDex < subSystem.getNumberOfInputs(); nDex++) {// Go through each input.
        temperature_.push_back(501.0f);         // Initialize its temperature.
    }                                           // Temperatures have been initialized.
    isTemperatureMesurementComplete = false;    // Temperatures have not been measured yet.
    isRelaysAssigned = false;                   // Relays have not been assigned to this sub-system yet.
    shouldAlarmBeOn = false;                    // The alarm should not be on yet.
    doesAlarmWantOn = false;                    // The alarm should not be on yet.
}

SCEquipment::~SCEquipment(){
    
}
  
void SCEquipment::setTestRelay(int relayDex) {
    testRelayDex_ = relayDex;                   // Set the relay being tested.
    testRelayStartTime_ = chrono::system_clock::now();// The relay test is just starting.
}
    
unsigned int SCEquipment::getTestRelay() {
    return testRelayDex_;                       // Provide the caller with the index of the relay being tested.
}

int SCEquipment::getErrorsBeforeRestart() {
    return errorsBeforeRestart_;                // Provide the caller with the number of errors before a restart.
}

void SCEquipment::setTimeInitialized() {
    timeInitialized_ = chrono::system_clock::now();
}

void SCEquipment::setTimeInitializedTo(chrono::time_point<chrono::system_clock> iniTime) {
    timeInitialized_ = iniTime;
}


chrono::time_point<chrono::system_clock> SCEquipment::getTimeInitialized() {
    return timeInitialized_;                    // Provide the caller with the time this hardware was initialized.
}
    
bool SCEquipment::setTemperature(unsigned int inDex, float temperature) {
    SCSystem subSys = this->getSubSystem();     // Get this equipment's sub-system.
    int nInputs = subSys.getNumberOfInputs();   // Get the number of inputs for this sub-system.
    if (inDex > nInputs) {                      // Is the input index valid for this sub-system?
        errorMessage = "Error setting the temperature: ";// No.
        errorMessage += "The given input index, " + to_string(inDex) + ", is not between 0 and " + to_string(nInputs) + ".";
        toLog(errorMessage);                    // Display the error.
        return false;                           // No. Return error value.
    }                                           // The input's index is valid.
    temperature_.at(inDex) = temperature;       // Set the desired temperature.
    return true;                                // The temperature was successfully set.
}
    
float SCEquipment::getTemperature(unsigned int nDex) {
    if (nDex >= temperature_.size()) {          // Does the given index point at a temperature?
        errorMessage = "Error retrieving the temperature: ";// No.
        errorMessage += "The given input index, " + to_string(nDex) + ", is not between 0 and " + to_string(nDex) + ".";
        toLog(errorMessage);                    // Display the error.
        return 501.0f;                          // No. Provide the caller with an unreal high tmeperature.
    } else {                                    // Yes.
        return temperature_[nDex];              // Provide the caller with the desired temperature.
    }                                           // The temperature has been provided.
}
    
void SCEquipment::incrementConsecutiveErrors() {
    time_t tTime = chrono::system_clock::to_time_t(timeOfLastSuccess_);// Create an error message.
    string ts = ctime(&tTime);                  // It includes the last time
    ts.resize(ts.size() - 1);                   // that communications with the hardware were
    errorMessage = "Communication, last success: " + ts + ". ";// successful.
    tTime = chrono::system_clock::to_time_t(timeOfLastFailure_);// It also includes
    ts = ctime(&tTime);                         // the last time when communications
    ts.resize(ts.size() - 1);                   // with the hardware
    errorMessage += "Last failure: " + ts + ". ";// failed. And finally the error count.
    errorMessage += "Error count: " + to_string(nConsecutiveErrors_) + " of " + to_string(errorsBeforeRestart_) + ".";
    toLog(errorMessage);                        // Display the error.
    if (timeOfLastFailure_ >= timeOfLastSuccess_) {// Was the last communication successful?
        nConsecutiveErrors_++;                  // No. This error follows another error and is thus a consecutive error.
    } else {                                    // The previous communication was successful.
        nConsecutiveErrors_ = 0;                // The number of consective errors is zero.
    }                                           // the consecutive error count has been set.
    timeOfLastFailure_ = chrono::system_clock::now();// This error just occurred.
    if (nConsecutiveErrors_ > errorsBeforeRestart_) {// Has the error count exceeded the amount tolerated?
        errorMessage = "Consecutive error count of " + to_string(nConsecutiveErrors_) + " exceeds ";// Yes, create an error message.
        errorMessage += to_string(errorsBeforeRestart_) + " consective errors allowed, Signon will be reattempted.";
        isRequestedToSignOn = true;             // Request a sign-on.
        nConsecutiveErrors_ = 0;                // Reset the error count since a sign-on was just requested.
    }                                           // A sign on has been requested if the consecutive error count exceeded the limit.
}

void SCEquipment::setSubSystem(SCSystem sys) {
    temperature_.clear();                       // Clear any temperatures.
    subSystem = sys;                            // Set the subsystem equal to the given SCSystem.
    for (int nDex = 0; nDex < subSystem.getNumberOfInputs(); nDex++) {// Go through each input.
        temperature_.push_back(501.0f);         // Initialize its temperature.
    }                                           // Temperatures have been initialized.
}
    
SCSystem SCEquipment::getSubSystem() {
    return subSystem;                           // Provide the user with the subsystem this equipment is a part of.
}
    
void SCEquipment::setFilePath(string filePath) {
    filePath_ = filePath;                       // Set the path to the equipment configuration files. No error checking.
}

void SCEquipment::toLog(string note) {
//    chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// Get the present time.
//    time_t tt = chrono::system_clock::to_time_t(now);// Convert the time to time_t format.
//    string time = ctime(&tt);                   // Convert the time from time_t format to string format.
//    time = time.substr(0, time.length() - 1);   // The conversion leaves a \n at the end of the string. Remove the \n.
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string time = "";
//    time += to_string(1900 + ltm->tm_year) + "/";
    int theMonth = ltm->tm_mon + 1;
    if(theMonth < 10) {
        time += "0";
    }
    time += to_string(theMonth) + "/";
    if(ltm->tm_mday < 10) {
        time += "0";
    }
    time += to_string(ltm->tm_mday) + "_";
    int theHour = ltm->tm_hour;
    if(theHour < 10) {
        time += "0";
    }
    time += to_string(theHour) + ":";
    int theMin = ltm->tm_min;
    if(theMin < 10) {
        time += "0";
    }
    time += to_string(theMin) + ":";
    int theSec = ltm->tm_sec;
    if(theSec < 10) {
        time += "0";
    }
    time += to_string(theSec);
    cout << time << " " << note << endl;       // Send the time and message to the console.
}

} // End of namespace SCClasses.
