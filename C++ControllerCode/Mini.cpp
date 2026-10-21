//
//  Mini.cpp
//  PlantManager
//
//  Created by Mark Davis on 11/30/15.
//  Copyright © 2015 Products Engineering. All rights reserved.
//

#include "Mini.hpp"
#include "ServerSocket.h"
#include "Socket.h"
#include "SocketException.h"
#include <cstdlib>
#include <algorithm>

#pragma mark - MiniEquip class implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////
string MiniEquip::commandResponse = "";

MiniEquip::MiniEquip() {
    socketPort_ = 65001;                        // By default, the port the NetCOM device will try to connect over is 65001.
    command = "";                               // There is no command to the mini modules yet.
    readVoltageState = 0;                       // Special voltages are not being read.
    miniSocket = nullptr;                       // The pointer has not been assigned.
    connectedIPAddress = "";                    // There is no IP address yet.
}

MiniEquip::~MiniEquip() {
    int nMods = (int)thermoModules.size();      // Get the number of thermocouple modules.
    for (int dex = 0; dex < nMods; dex++) {     // Go through each module.
        if (thermoModules.at(dex) != nullptr) {  // Make sure there is memory to free.
            delete thermoModules.at(dex);        // Remove the memory allocated to this module.
        }
    }                                           // All memory has been de-allocated.
    thermoModules.clear();                       // Clear the array of modules.
}
                                        
void MiniEquip::initializeRelays(Relays *plantRelays) {
    if (isRelaysAssigned == false) {            // Have the relays been assigned to this sub-system's relay modules?
        SCSystem theSubSys = getSubSystem();    // No. Get access to the sub-system containing this mini equipment.
        for_each(plantRelays->begin(), plantRelays->end(), [&] (Relay* aRelay){// Go through each plant relay.
            int lastRelayDex = theSubSys.getFirstRelay() + theSubSys.getNumberOfRelays();// Calc this sub-sys last relay index.
            // Is this relay a part of this sub-system? (Its index must be between the first and last relay indexs for this sub-sys)
            if (aRelay->indexNo >= theSubSys.getFirstRelay() && aRelay->indexNo < lastRelayDex ) {
                // This relay is a part of this sub-system.
                unsigned int relaySubDex = aRelay->indexNo - theSubSys.getFirstRelay();// Calculate its relative sub-system index.
                int diff = 9999;                // Contains the difference between this relay's relative index
                                                // and the relative index of a module's first relay.
                unsigned int closestModDex = 9999;// Is the index of the module whith the smallest positive difference
                                                // to the relay being evaluated.
                for (unsigned int modDex = 0; modDex < relayModules.size(); modDex++) {// Go through each module.
                    RelayModule* rMod = relayModules.at(modDex);// Get access to this module.
                    if (relaySubDex >= rMod->getStartIndex()) {// Is this relay possibly on this module?
                        int d = relaySubDex - rMod->getStartIndex();// Yes. Get the difference from this module's start index.
                        if (d < diff) {         // Is this difference is smaller than the stored difference?
                            diff = d;           // Yes. Replace the differnce. The relay could be on this module.
                            closestModDex = modDex;// Store this module's index.
                        }                       // The two differences have been compared. the smallest has been stored.
                    }                           // If this relay was potentially a part of this module, it has been processed.
                }                               // Each module has been evaluated to find the one containing this relay.
                relayModules.at(closestModDex)->relaysList_.push_back(aRelay);// Store this relay in the relay module list.
            }                                   // This relay has been evaluated and potentially saved as a part of a relay module.
        });                                     // All the plant relays have been gone through.
        isRelaysAssigned = true;                // The relays are assigned to each relay module in this sub-system.
    }                                           // If the relays weren't assigned, they are now.
} // End of function initializeRelays().

string MiniEquip::getSerialServerName(string ipAddress) {
    FILE *fp;                               // A file pointer used to open a pipe stream to get the server name.
    int status;                             // A flag to determine if the pipe stream closed correctly.
    char buff[512];                         // The server name sent by the NetCom device.
    string command = "snmpwalk -c public -v1 ";// The start of the unix system command to get the server name.
    command += ipAddress;                   // The IP address already provided by the NetCom device.
    command += " SNMPv2-MIB::sysName.0";    // The end of the command to get the server name.
    fp = popen(command.c_str(), "r");       // Open the pipe stream and issue the command to get the server name.
    if (fp == NULL) {                       // Did the pipe stream fail to open?
        errorMessage = "Error issuing the command in MiniEquip::getSerialServerName()!";// Failure. Create an error.
        toLog(errorMessage);                // Display the error.
        return "";                          // Returning an empty string indicates failure.
    } // End of condition testing for an open pipe.
    if(fgets(buff, 512, fp) == NULL) {      // Was the server name obtained from the pipe stream?
        errorMessage = "Error receiving the response in MiniEquip::getSerialServerName()!";// No. Create an error.
        toLog(errorMessage);                // Display the error.
        pclose(fp);                         // Close the pipe stream.
        return "";                          // Returning an empty string indicates failure.
    } // End of the condition testing server name retrieval.
    status = pclose(fp);                    // Close the pipe stream.
    if (status == -1) {                     // Did the pipe stream close successfully?
        errorMessage = "Error closing the pipe in MiniEquip::getSerialServerName()!";// No. Create an error.
        toLog(errorMessage);                // Display the error.
        return "";                          // Returning an empty string indicates failure.
    } // End of the condition testing the pipe closure.
    string reply(buff);                     // Convert the charachters received from the pipe to a string.
    size_t found = reply.find_last_of(":"); // Find the last : in the string.
    if (found == string::npos) {            // Was a : found?
        errorMessage = "The server name was invalid in MiniEquip::getSerialServerName()!";// No. Create an error.
        toLog(errorMessage);                // Display the error.
        return "";                          // Returning an empty string indicates failure.
    } // End of the condition testing the pipe closure.
    string serverName = reply.substr(found + 2);// Parse the server name from the pipe reply.
    return serverName;                      // Return the server name to the caller.
} // End of function getSerialServerName().

bool MiniEquip::isSerialServerConnected() {
    SCSystem theSubSys = getSubSystem();    // Access the sub-system associated with this mini.
    string portName = theSubSys.getPortName();// portName is the server name from the DB for this sub-system.
    string serverName = getSerialServerName(connectedIPAddress);// Use the operating system to get the server name.
    if(serverName == "") {                  // Was a server name returned?
        return false;                       // No. The network connection is no longer good.
    } // End of condition checking for a server name.
    // There is a server name. Does it match the port name for this mini.
    string name = serverName.substr(0, portName.length());// Only compare the number of characters found in the port name.
    if(name == portName) {                  // Does server name match the port name?
        return true;                        // Yes. The network connection is still good.
    } // End of condition: IP Address has the same server name as the DB.
    return false;                           // This server name does not match the one for this mini.
}


string MiniEquip::getServerName() {
    return serverName_;                 // Provide the caller with the server name.
}

void MiniEquip::setCommand(string value) {
    command = value;
}

string MiniEquip::getCommandResponse() {
    return commandResponse;
}

void MiniEquip::resetCommandResponse() {
    commandResponse = "";
}

void MiniEquip::relayTestPeform() {
    // Calculate the offset into this sub-system for this relay.
    unsigned int relayOffset = getTestRelay() - getSubSystem().getFirstRelay();
    // Go through all the relay modules in this mini-system to set the desired relay states.
    for_each(relayModules.begin(), relayModules.end(), [&] (RelayModule* relayMod) {
        // Is the test relay contained on this relay module?
        if (relayOffset >= relayMod->getStartIndex() && relayOffset < relayMod->getStartIndex() + relayMod->getNoOutputs()) {
            // Yes. Calculate the test relay's offset into this module. (0 offset is the first relay on this module.)
            unsigned int relayOffsetInMod = relayOffset - relayMod->getStartIndex();
            for (unsigned int offset = 0; offset < relayMod->getNoOutputs(); offset++) {// Go through each relay on this module.
                if (offset == relayOffsetInMod) {// Is this the test relay.
                    relayMod->desiredRelayStates_.at(offset) = true;// Yes. The test relay wants to be on.
                    relayMod->relaysList_.at(offset)->requestToTurnOn();// Start the process of turning on the test relay.
                } else {                        // This is not the test relay.
                    relayMod->desiredRelayStates_.at(offset) = false;// A relay which is not being tested wants to be off.
                    relayMod->relaysList_.at(offset)->requestToTurnOff();// Start the process of turning off this relay.
                }                               // This relay has the correct state set for testing.
            }                                   // The states of all relays on this module have been set.
        } else {                                // The relay being tested is not on this module. Relays on this module should be off.
            for (unsigned int offset = 0; offset < relayMod->getNoOutputs(); offset++) {// Go through each relay on this module.
                relayMod->desiredRelayStates_.at(offset) = false;// A relay which is not being tested wants to be off.
                relayMod->relaysList_.at(offset)->requestToTurnOff();// Start the process of turning off this relay.
            }                                   // The states of all relays on this module have been set.
        }                                       // The states of the relays on this module have been set for testing.
    });                                         // The states of all the relays on this mini sub-system have been set for testing.
    // Go through all the relay modules in this mini-system again and set the desired relay state.
    for_each(relayModules.begin(), relayModules.end(), [&] (RelayModule* relayMod) {
        if (relayMod->isActive == true) {       // Is this module in active mode?
            // Yes. Is it OK to set the state of the relays on this module.
            if (relayMod->isLastCommAttemptValid == true || // The module must be communicating or ready for a retry.
                (relayMod->isLastCommAttemptValid == false && chrono::system_clock::now() > relayMod->getTimeOfRetry()) ||
                SCEquipment::isInArtificialMode == true) {  // The modules may be in artificial mode.
                setRelaysOnModule(relayMod);    // It is OK to set the state for testing.
            }                                   // This modules state has been processed.
        }                                       // Active modules have been processed.
    });                                         // All relay modules have been processed.
}// relayTestPerform() complete.

void MiniEquip::relaysAllOff() {
    // Go through all this mini-system's relay modules and set the desired relay states to off.
    for_each(relayModules.begin(), relayModules.end(), [&] (RelayModule* relayMod) {
        for (unsigned int offset = 0; offset < relayMod->getNoOutputs(); offset++) {// Go through each of this module's relays.
            relayMod->desiredRelayStates_.at(offset) = false;// Set the relay's desired state to off.
            relayMod->relaysList_.at(offset)->requestToTurnOff();// Start the process of turning off this relay.
        } // Done looping to set each of this module's relays to off.
    }); // The desired state of each relay on this mini sub-system is off.
    // Go through each of this mini-system's relay modules and actually turn the relays off.
    for_each(relayModules.begin(), relayModules.end(), [&] (RelayModule* relayMod) {
        if (relayMod->isActive == true) {       // Is this module in active mode?
            // Yes. Is it OK to set the state of the relays on this module.
            if (relayMod->isLastCommAttemptValid == true || // The module must be communicating or ready for a retry.
                (relayMod->isLastCommAttemptValid == false && chrono::system_clock::now() > relayMod->getTimeOfRetry()) ||
                SCEquipment::isInArtificialMode == true) {  // The modules may be in artificial mode.
                setRelaysOnModule(relayMod);    // It is OK to turn this module's relays off.
            } // Done determining this module's relays can be turned off.
        } // Done determining this relay module is active.
    }); //Done actually turning off all this mini sub-system's relays.
} // End of function realysAllOff.

void MiniEquip::operateEquipment(MiniEquip* miniSys, Relays* plantRelays) {
    miniSys->isOperating = true;                // This mini sub-system is now operating.
    string ssNo = to_string(miniSys->getSubSystem().getSubSystemNo());
    miniSys->toLog("Starting MiniEquip::operatingEquipment() sub-system " + ssNo + ".");
    try {                                       // Try operating the equipment associated with this Mini sub-system.
        while (miniSys->isOperationAllowed == true) {  // The endless loop to operate Mini equipment.
            if(miniSys->isRequestedToSignOn == true) {
                miniSys->toLog("Mini" + ssNo + " is requesting to sign-on.");
                this_thread::sleep_for(chrono::milliseconds(5000));// Wait 5 seconds before trying again.
                continue;
            }
            for(int nDex = 0; nDex < miniSys->thermoModules.size(); nDex++) {// Go through each thermocouple module.
                ThermoModule* pTMod = miniSys->thermoModules.at(nDex);// Access one of the thermocouple modules.
                if (pTMod->isActive == false) { // Is this module active?
                    continue;                   // No. Go on to the next one.
                }                               // This thermocoule module is active.
                string tCommand = pTMod->getTemperatureCommand();// Get the command to retrieve temperatures from this module.
                string oCommand = pTMod->getOpenThermoCommand();// Get the command to retrieve open thermocouples on this module.
                string tResponse = "";          // Contains the temperature response.
                string oResponse = "";          // Contains the open thermocouple response.
                if (SCEquipment::isInArtificialMode == true) {// Is the plant manager in artificial mode?
                    continue;                   // Yes. There is no need to actually try to measure the temperatures. Go to next mod.
                }                               // The plant manager is in real mode.
                if (pTMod->isActive == true) {  // Is this thermocoule module active?
                    // Yes. Attempt to get the temperatures of the thermocouples connected to it.
                    bool isOKToCommunicate = false;// By default it is not OK to communicate with this module.
                    if (pTMod->isLastCommAttemptValid == true) {// Was the last attempt to communicate with this module successful?
                        isOKToCommunicate = true;// Yes. It is OK to try to communicate again.
                    } else {                    // Last time communication failed.
                        if (chrono::system_clock::now() > pTMod->getTimeOfRetry()) {// Has enough time passed to try again?
                            isOKToCommunicate = true;// Yes.
                        }                       // The OK to communicate flag has been set.
                    }                           // The OK to communicate flag has been set.
                    if (isOKToCommunicate == true) {// Is it OK to communicate with this thermocouple module?
                        for (int dex = 0; dex < 3; dex++) {// Try up to three time to get temperatures.
                            miniSys->setCommand(tCommand);// Set the temperature command.
                            // Start here, this command's response should be 58 characters starting with >.
                            if (miniSys->sendCommand(miniSys, 58) == false) {// Did sending the temperature command work?
                                pTMod->isLastCommAttemptValid = false;// No. Set the last attempt at communication to failed.
                                string em = "Sub-sys"+ ssNo + ": Attempt " + to_string(dex + 1);// Create an error message.
                                em += " getting temperatures from module with address " + pTMod->addressToString();;
                                miniSys->errorMessage = em;// Set the error message.
                                miniSys->toLog(miniSys->errorMessage);// Display the error.
                                this_thread::sleep_for(chrono::milliseconds(500));// Wait .5 seconds before trying again.
                                continue;       // Try sending the command again if less than three attepts have been made.
                            } else {            // The temperature command worked.
                                tResponse = miniSys->getCommandResponse();// Get the reponse.
                                if (tResponse.size() < 58) {// Is the response size 58?
                                    miniSys->errorMessage = "Sub-sys"+ ssNo + ": Attempt " + to_string(dex + 1);// No. Create error.
                                    miniSys->errorMessage += ": Last temperature response: " + tResponse;
                                    miniSys->toLog(miniSys->errorMessage); // Display the error.
                                    pTMod->isLastCommAttemptValid = false;// No. This is not valid.
                                    this_thread::sleep_for(chrono::milliseconds(500));// Wait .5 seconds before trying again.
                                    continue;   // Move on to the next attempt.
                                }               // The response size was 58.
                                if (tResponse.at(0) != '>') {   // Was the response chacter correct?
                                    miniSys->errorMessage = "Sub-sys"+ ssNo + ": Attempt " + to_string(dex + 1);// No. Create error.
                                    miniSys->errorMessage += ": Return character: " + tResponse.substr(0, 1);
                                    miniSys->errorMessage += " should have been >.";
                                    miniSys->toLog(miniSys->errorMessage); // Display the error.
                                    pTMod->isLastCommAttemptValid = false;// No. This is not valid.
                                    this_thread::sleep_for(chrono::milliseconds(500));// Wait .5 seconds before trying again.
                                    continue;   // Move on to the next attempt.
                                }
                                pTMod->isLastCommAttemptValid = true;// The last attempt at communication worked.
                                break;          // There is no need to get the temperatures again.
                            }                   // This attempt at getting the temperatures is complete.
                        }                       // Done attempting to get the temperatures.
                        if (pTMod->isLastCommAttemptValid == false) {// Were the temperatures obtained?
                            // No. Set the retry time.
                            string em = "Sub-system "+ ssNo + ": Error getting temperatures from thermocouple module ";
                            em += pTMod->addressToString();;
                            miniSys->errorMessage = em;// Set the mini system error message.
                            miniSys->toLog(miniSys->errorMessage);// Display the error.
                            chrono::time_point<chrono::system_clock> retry = chrono::system_clock::now();// Start with now.
                            retry += chrono::minutes(pTMod->getPauseForRetry());// Add the preset number of minutes.
                            pTMod->setTimeOfRetry(retry);// Set the next retry time.
                            //miniSys->incrementConsecutiveErrors();// Increment the error count. (Used for restart)
                            oResponse = "FF";   // Since no temperatures were obtained, set all temperatures to open.
                            pTMod->convertToTemperatures(tResponse, oResponse);// Put temperatures in this modules temp array.
                        } else {                // Temperatures were aquired. Get the open thermocouples.
                            // Start here, check to see if a model 4118 uses this command.
                            miniSys->setCommand(oCommand);// Set the open thermocouple command.
                            for (int dex = 0; dex < 3; dex++) {// Try up to three times.
                                this_thread::sleep_for(chrono::milliseconds(60));
                                // Start here, this command should return 6 characters starting with !.
                                if (miniSys->sendCommand(miniSys, 6) == false) {// Did sending the open thermocouple command work?
                                    pTMod->isLastCommAttemptValid = false;// No. The last attempt at communication failed.
                                    continue;   // Try again unless more than three attempts have been made.
                                } else {        // Communications succeeded.
                                    oResponse = miniSys->getCommandResponse();// Get the open thermocouples response.
                                    if (oResponse.at(0) != '!') {
                                        miniSys->errorMessage = "Attempt " + to_string(dex);
                                        miniSys->errorMessage += ", sub-sys " + ssNo  + ", oResponse = " + oResponse;
                                        miniSys->toLog(miniSys->errorMessage);// Display the error.
                                        pTMod->isLastCommAttemptValid = false;// No. This is not valid.
                                        this_thread::sleep_for(chrono::milliseconds(100));// Wait .1 seconds before trying again.
                                        continue;   // Move on to the next attempt.
                                    }
                                    pTMod->isLastCommAttemptValid = true;// The last attempt at communication worked.
                                    break;      // There is no need to try  again.
                                }               // Need to try to get open thermocouples again.
                            }                   // Done attempting to get open thermocouples.
                            if (pTMod->isLastCommAttemptValid == false) {// Were open thermocouples obtained?
                                // No. Set the retry time starting with now.
                                string em = "Error getting open thermocouples from module " + pTMod->addressToString();;
                                miniSys->errorMessage = em;// Set the mini system error message.
                                miniSys->toLog(miniSys->errorMessage);// Display the error.
                                chrono::time_point<chrono::system_clock> retry = chrono::system_clock::now();
                                retry += chrono::minutes(pTMod->getPauseForRetry());// Add preset minutes to get new retry time.
                                pTMod->setTimeOfRetry(retry);// Set the new retry time.
                                // Create an error message.
                                miniSys->errorMessage = "Last open thermocouple response was: " + tResponse;
                                miniSys->toLog(miniSys->errorMessage);// Display the error.
                                //miniSys->incrementConsecutiveErrors();// Increment the error count. (Used for restart)
                                oResponse = "00";   // Since open thermocouples were not obtained, set all thermocouples to closed.
                            }                   // Done getting open thermocouples.
                            pTMod->convertToTemperatures(tResponse, oResponse);// Put temperatures in this modules temp array.
                        }                       // Done getting temperatures & open thermocouples.
                        for (unsigned int inDex = 0; inDex < pTMod->getNoInputs(); inDex++) {// Go through each input.
                            // Set the appropriate sub-system temperature for this input.
                            miniSys->setTemperature(pTMod->getStartIndex() + inDex, pTMod->getTemperature(inDex));
                        }                       // All sub-system temperatures have been set.
                    }                           // Done communicating.
                }// End of "pTMod->isActive == true".
            }                                   // All the thermocoule modules have been processed.
            miniSys->isTemperatureMesurementComplete = true;// All temperature have been measured.
            this_thread::sleep_for(chrono::milliseconds(500));
            if (miniSys->isARelayTesting == false) {// Is a relay being tested?
                // No. It is OK to change the states of the relays.
                unsigned int nRelays = (unsigned int)plantRelays->size();// Get the total number of relays for the entire system.
                for (unsigned int relayDex = 0; relayDex < nRelays; relayDex++) { // Go through all the relays in the entire system.
                    Relay* relay = plantRelays->at(relayDex);// Get the plant relay with this index.
                    SCSystem sys = miniSys->getSubSystem();// Get the subsystem this mini equipment is associated with.
                    int relayOffset = relay->indexNo - sys.getFirstRelay();// Calculate this relay's offset into this sub-system.
                    for_each(miniSys->relayModules.begin(), miniSys->relayModules.end(), [&] (RelayModule* rMod){
                        // Test each relay module to find if it is the one that contains this relay offset.
                        unsigned int finishDex =rMod->getStartIndex() + rMod->getNoOutputs();// Last index on this relay module.
                        if (relayOffset >= rMod->getStartIndex() && relayOffset < finishDex) {// Is the relay part of this module?
                            // Yes. Calculate the offset into this module for this relay. (0 offset is the first relay on this module)
                            int modOffset = relayOffset - rMod->getStartIndex();// This is the offset.
                            // Set the desired state for this relay which is a part of this module.
                            rMod->desiredRelayStates_.at(modOffset) = relay->isWantingOn;// Set desired state.
                            return;             // Found the correct module for this relay, don't need to search any more modules.
                        } // End of condition: This relay is a part of this module on this sub-system.
                    }); // End of looping through all the relay modules in this sub-systems.
                } // End of looping through all the system relays.
                bool isOKSent = false;// Indicates "Host is OK" command not sent, needs to be sent to prevent Watchdog(realys off).
                for (unsigned int rModDex = 0; rModDex < miniSys->getNoOfRelayMods(); rModDex++) {// Go through each relay module.
                    if (miniSys->isOperationAllowed == false) {// Is the system being shut down now?
                        break;                      // Don't process anymore relay modules.
                    }                               // This sub-system is still running.
                    RelayModule* rMod = miniSys->relayModules.at(rModDex);// Get access to a relay module for this sub-system.
                    if (rMod->isActive == true) { // Is this relay module active?
                        bool isOKToPlay = rMod->isLastCommAttemptValid;// Yes. It's OK to talk if the last attempt worked.
                        if (isOKToPlay == false) {  // Did the last attempt fail?
                            if (chrono::system_clock::now() > rMod->getTimeOfRetry()) {// Failed. Time to try again?
                                isOKToPlay = true;  // Yes it is time to try communicating again.
                            } else if (miniSys->isInArtificialMode == true) {// Is the plant manager in artificial mode?
                                isOKToPlay = true;  // Yes. This means it's OK to try because nothing will actually be sent.
                            }// The communication flag has been changed to true provided conditions wre right.
                        }                           // The communicatoin flag is set.
                        if (isOKToPlay == true) {   // Is it OK to attempt communication with this relay module?
                            if (isOKSent == false && rMod->getType() == "6050") {// Yes. Has the "Host is OK" command been sent?
                                string cmnd = "~**";// No. Set the first part of the command.
                                cmnd += (char)13;   // Add a carraige return to end the command.
                                miniSys->setCommand(cmnd);// Set the command.
                                // Start here, this command does not have a reponse.
                                if (miniSys->sendCommand(miniSys, 0) == false) {// Did sending the host is OK command work?
                                    rMod->isLastCommAttemptValid = false;// No. The last attempt at communication failed.
                                    // Set the retry time starting with now.
                                    //chrono::time_point<chrono::system_clock> retry = chrono::system_clock::now();
                                    //retry += chrono::minutes(rMod->getPauseForRetry());// Add preset minutes to get new retry time.
                                    //rMod->setTimeOfRetry(retry);// Set the new retry time.
                                    // Create an error message.
                                    string em = "Sub-sys"+ ssNo + ": Error sending \"Host Is OK\" to rMod address " + rMod->addressToString();
                                    miniSys->errorMessage = em;// Set the mini system error message.
                                    miniSys->toLog(miniSys->errorMessage);// Display the error.
                                }                   // Failed "Host Is OK" has been processed.
                                isOKSent = true;    // Only need to send the OK command once per cycle.
                            }                       // The "Host Is OK" command has been sent if required.
                            miniSys->setRelaysOnModule(rMod);// Set the relays on this module.
                        }// Done trying to communicate with this relay modules since it was OK to do so.
                    }// Done processing an active relay module.
                }// Done processing relays when no relays are being tested.
            } else {                                // A relay is being tested.
                if (chrono::system_clock::now() < miniSys->testRelayStartTime_) {// Safety check, has the test start time been set?
                    miniSys->testRelayStartTime_ = chrono::system_clock::now();// No. Set it to now.
                }                                   // The start time for the relay test is OK.
                if (chrono::system_clock::now() - miniSys->testRelayStartTime_ > chrono::minutes(3)) {
                    // Even if the relay being tested is not part of this minisys, this flag
                    miniSys->isARelayTesting = false;// must be set to false to allow relays on this minisys to come back on.
                    Relay* testRelay = plantRelays->at(miniSys->testRelayDex_);// Get the relay being tested.
                    testRelay->requestToTurnOff();  // 3 minutes have elapsed. tyr to turn this relay off.
                } else {                            // It is OK to perform the relay test.
                    miniSys->relayTestPeform();     // Perform the relay test on this mini sub-system.
                }// The relay test has been executed.
            }// Relay test execution complete if required.
            if ( miniSys->readVoltageState != 0) {
                ;// Start here, add code for reading special voltages.
            }// Read special voltage complete if required.
            if (miniSys->isCommunicationDownOnAllModules() == true) {// Has all communication failed?
                if(miniSys->isSerialServerConnected() == false) {// Is the network connection still open?
                    if(miniSys->miniSocket != NULL) {           // Does this sub-system have a valid socket?
                        miniSys->miniSocket->closeSocket();     // Yes. Close this end of the link between the PM and the sub-system.
                    } // End of condition: This mini sub-system has a valid socket.
                    miniSys->isRequestedToSignOn = true;// Try to restart the connection.
                    miniSys->errorMessage = "isRequestedToSignOn set to true on mini" + ssNo;
                    miniSys->errorMessage += " because comm down on all modules.";
                    miniSys->toLog(miniSys->errorMessage);// Display the signon status.
                } // End of condition checking for a failed connection.
            } // End of conditional check if all modules are not communicating.
            this_thread::sleep_for(chrono::milliseconds(1000));// Wait one second before repeating.
        } // The end of the mini sub-system operation loop.
    } // The end of the try block protecting the NetCOM serial socket.
    catch ( SocketException& e ) {              // Were there any exceptions in the operate equipment loop?
        int subSystemNo = miniSys->getSubSystem().getSubSystemNo();// Yes. Create an error message.
        miniSys->errorMessage = "Exception operating mini sub-system " + to_string(subSystemNo) + "!";
        miniSys->toLog(miniSys->errorMessage);  // Display the error.
    } // End of catching operateEquipment exceptions.
    miniSys->relaysAllOff();                    // Turn off any relays which may have been on.
    this_thread::sleep_for(chrono::milliseconds(200));// Wait 200 milli-seconds before closing the port to allow the relays to turn off.
    if(miniSys->miniSocket != NULL) {           // Does this sub-system have a valid socket?
        miniSys->miniSocket->closeSocket();     // Yes. Close this end of the communication link between the PM and the sub-system.
    } // End of condition: This mini sub-system has a valid socket.
    miniSys->isOperating = false;               // The equipment is no longer operating.
} // End of function operateEquipment.

std::mutex mtx;                                 // Prevents two threads from using a socket at the same time.

bool MiniEquip::sendCommand(MiniEquip* miniSys, int replyLength) {
    // Verify the socket is ready to send a command.
    if (SCEquipment::isInArtificialMode == true) {// Is the plant manager in artificial mode?
        return true;                            // Yes. There is nothing wrong. The command should not be sent. Don't alarm the caller.
    }                                           // The plant manager is in real mode.
    bool outcome = true;                        // Contains the result of the send operation.
    // Allow up to 5 seconds for the send operation to complete.
    chrono::time_point<chrono::system_clock> endTime = chrono::system_clock::now() + chrono::seconds(5);// Time sending is over.
    string ssNo = to_string(miniSys->getSubSystem().getSubSystemNo());// Create a string containing the sub-system number for error messages.
    mtx.lock();                                 // Lock communications with the mini hardware until the reply to this command is received.
    // No more than one thread (sub-system) can talk to the mini equipment at a time since any reply from any mini equipment
    // will be received at the same IP address and port. So if two commands were sent before a reply was received, the replies
    // might come back in any order. So the thread (sub-system) expecting a reply from its command might receive the reply
    // from the command sent by the other thread (sub-system). If the reply came from the wrong sub-system, the reply would have to be
    // transferred to the other thread (sub-system). At best this would be very tedious.
    while(chrono::system_clock::now() < endTime) {// While the end time has not been exceeded.
        int r = miniSys->miniSocket->pollSend(1000);// Query the socket for up to one second to see if it is ready to send.
        if (r > 0) {                            // 'r' is the number of sockets ready to send a command.
            break;                              // Done waiting for the socket to be ready.
        } // End of condition: The socket is ready to send.
    } // End of looping waiting to send the command.
    if (chrono::system_clock::now() >= endTime) {// Was the end time exceeded?
        miniSys->errorMessage = "Sub-sys" + ssNo + " socket was never ready to send!";// Yes.
        miniSys->toLog(miniSys->errorMessage);  // Display the error.
        mtx.unlock();                           // Communication failed. Allow others to send commands.
        return false;                           // Let the caller know the command could not be sent.
    } // End of condition: The time out value was exceeded.
    string cmd = miniSys->command;              // Copy the command for error messages.
    cmd = cmd.substr(0, cmd.size() - 1);        // Remove the last character which is a carriage return.
    try {                                       // Try to send the command.
        outcome = miniSys->miniSocket->send(miniSys->command);// Transmit the command.
    } catch (SocketException& e) {              // Catch any errors.
        miniSys->errorMessage = "Sub-sys" + ssNo + " error sending command, " + cmd + ": ";// Create an error message.
        miniSys->errorMessage +=  e.description();// Add the cause of the error.
        miniSys->toLog(miniSys->errorMessage);  // Display the error.
        mtx.unlock();                           // Mini equipment communication is complete for this command. Allow sending commands.
        return false;                           // Notify the caller that the command wasn't sent.
    } // End of catching unexpected results from sending a command.
    if(outcome == false) {                      // Did the send fail.
        miniSys->errorMessage = "Sub-sys" + ssNo + " failed to send command: " + cmd;// Create an error message.
        miniSys->toLog(miniSys->errorMessage);  // Display the error.
        mtx.unlock();                           // Mini equipment communication is complete for this command. Allow sending commands.
        return false;                           // Notify the caller that the command wasn't sent.
    } // End of condition: Failed to send the command.
    string hostOKCmnd = "~**";                  // Create the "Host is OK" command.
    hostOKCmnd += (char)13;                     // Add a carriage return.
    if (miniSys->command == hostOKCmnd) {       // Was the "Host is OK" command just sent?
        this_thread::sleep_for(chrono::milliseconds(50));// Yes. Wait 50 milliseconds. There is no reply.
        mtx.unlock();                           // Mini equipment communication is complete for this command. Allow sending commands.
        return true;                            // Let the caller know the "Host is OK" command sent.
    } // End of condition: The command sent was "Host is OK".
    this_thread::sleep_for(chrono::milliseconds(128));// Wait 128 milliseconds before trying to get a response.
    string data;                                // Contains the response (or partial response) from sending the command.
    commandResponse = "";                       // Clear the response before obtaining the new response.
    while(chrono::system_clock::now() < endTime) {// While the end time has not been exceeded.
        while (miniSys->miniSocket->poll(200) <= 0) {// Wait up to a 200 millisecond for a response.
            this_thread::sleep_for(chrono::milliseconds(50));// No response yet. Wait 50 milliseconds for the remaining response.
            if(chrono::system_clock::now() >= endTime) {// Has the end time been exceeded?
                break;                          // Yes. End the loop waiting for the remaining response.
            } // End of condition: The end time has been exceeded.
        } // There is at least a partial response.  End the loop waiting for a response.
        if(chrono::system_clock::now() >= endTime) {// Has the end time been exceeded?
            break;                          // Yes. End the loop waiting for the remaining response.
        } // End of condition: The end time has been exceeded.
        int result = miniSys->miniSocket->recv(data);// Get the response. This cannot be blocking. poll() indicates there is data.
        if (result == -1 || result == 0) {      // Was there an error?
            miniSys->errorMessage = "Sub-sys" + ssNo + " response error to command: " + cmd;
            miniSys->toLog(miniSys->errorMessage);// Display the error.
            mtx.unlock();                       // Mini equipment communication is complete for this command. Allow sending commands.
            return false;                       // Communications failed.
        } else if (result != 0) {               // There was something in the socket.
            commandResponse += data;            // Put the response where it can be accessed by the caller.
            if(commandResponse.length() >= replyLength) {// Has the entire response been received?
                break;                          // Yes. Exit the receiving loop.
            } // End of condition: The response is complete.
        } // End of condition: There was something in the socket to receive.
        this_thread::sleep_for(chrono::milliseconds(32));// The response is not complete. Wait 32 milliseconds for the remaining response.
    } // End of looping to get the entire response.
    if (chrono::system_clock::now() >= endTime) {// Was the end time exceeded?
        miniSys->errorMessage = "Sub-sys" + ssNo + " timed out receiving a response to: " + cmd ;// Yes.
        miniSys->toLog(miniSys->errorMessage);  // Display the error.
        miniSys->errorMessage = "Partial response: " + commandResponse ;// Yes.
        miniSys->toLog(miniSys->errorMessage);  // Display the error.
        mtx.unlock();                           // Communication failed. Allow others to send commands.
        return false;                           // Let the caller know the command could not be sent.
    } // End of condition: The time out value was exceeded.
    mtx.unlock();                               // Mini equipment communication is complete for this command. Allow sending commands.
    return true;                                // Let the caller know there is a response.
} // End of function sendCommand.

bool MiniEquip::loadFromDB() {
    int subSysNo = this->subSystem.getSubSystemNo();// Get the sub-system number.
    bool isActivated = false;               // By default any module is not activated.
    int address = 0;                        // Each module has an address.
    int pauseForRetry = 2;                  // Each module will wait 2 minutes before communication is re-attempted after a failure.
    int startIndex = 0;                     // Each module has a start index.
    string type = "none";                   // Each module has a type.
    int number = 0;                         // Each module has either a number of inputs or outputs.
    sqlite3 *db;                            // Create a location for the database.
    int  rc;                                // Create a return variable.
    string sql;                             // Create a string for SQL commands.
    string dbName = filePath_ + "SCConfig.db";// Set the configuration database name.
    rc = sqlite3_open(dbName.c_str(), &db); // Open the database named SC_Config.db. It has a table which contains the mini configurations.
    if( rc ){                               // Was the database opened?
        errorMessage = "Can't open SCConfig.db to load Mini configuration for sub-system " + to_string(subSysNo);// No. Create a message.
        errorMessage += ": " + string(sqlite3_errmsg(db));// Add the database error to the message.
        toLog(errorMessage);                // Display the error.
        return false;                       // This error prevents loading the mini configuration for this sub-system.
    }                                       // The database is open.
    string tableName = "minis";             // Create the table name for the Mini sub-system configurations.
    sql = "SELECT * from " + tableName + " WHERE systemNo=" + to_string(subSysNo);// Create an SQL statement to read the Minis which
                                            // are a part of this sub-system.
    sqlite3_stmt *stmt;                     // Will contain the results of the querry.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
    if (rc != SQLITE_OK) {                  // Was it successful?
                                            // No, there was a problem. Create an error message.
        errorMessage = "Failed to query 'minis' for sub-system " + to_string(subSysNo);
        errorMessage += ": " + string(sqlite3_errmsg(db));// Add the database error to the message.
        toLog(errorMessage);                // Display the error.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
        return false;                       // Let the caller know the mini configuration couldn't be retrieved from the database.
    }                                       // The configuration was retreived.
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {// Go through each row in the results.
        int id = sqlite3_column_int (stmt, 0);// The id is in the first column.
        int active = sqlite3_column_int(stmt, 1);// The active flag is in the table's first column.
        isActivated = false;                // The module is not active by default.
        if (active == 1) {                  // Is the active flag set to one?
            isActivated = true;             // Yes. Then this module is active.
        }
        address = sqlite3_column_int(stmt, 2);// The module's address is in the table's third column.
        startIndex = sqlite3_column_int(stmt, 3);// The module's start index is in the table's fourth column.
        // The start index is the index into all the system's inputs or relays that this module's
        // first input or relayis configured.
        pauseForRetry = sqlite3_column_int(stmt, 4);// The amount of time (minutes) the PM should wait before
        // attempting to communicate with this module after a communication failure.
        const unsigned char *modType = sqlite3_column_text(stmt, 5);// The module's type is in the table's sixth column.
        type = std::string(reinterpret_cast<const char*>(modType));// Convert the module's type to a string.
        number = sqlite3_column_int(stmt, 15);// The module's number of inputs or relays is in the table's sixteenth column.
        if(type == "6018" || type == "4018") {// Is this a thermocouple module?
            ThermoModule* theMod = addThermoMod(address,        // Try to
                                                isActivated,    // add this
                                                pauseForRetry,  // thermocouple
                                                startIndex,     // module to
                                                type,           // the list
                                                number);        // of thermocouple modules.
            if (theMod == nullptr) {        // Was the thermocouple module added?
                errorMessage = "Mini Module " + to_string(id) + " memory allocation failed.";// No. Create error.
                toLog(errorMessage);        // Display the error.
                sqlite3_finalize(stmt);     // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
                sqlite3_close(db);          // Close SCConfig.db. Since finalize was just called there should be no errors.
                return false;               // Let the caller know the mini configuration couldn't be retrieved from the database.
            }                               // The thermocoule module was added.
            for (int nDex = 0; nDex < number; nDex++) {// Load each inputs offset.
                double offset = sqlite3_column_double(stmt, 6 + nDex);// Get this input's offset from the DB.
                theMod->setOffset(nDex, offset);// Set this input's offset.
            }                               // All offsets have been set.
        }                                   // Done configuring a thermocouple module.
        if(type == "6050" || type == "4050") {// Is this a relay module?
            RelayModule* theRMod = addRelayMod(address, isActivated, pauseForRetry, startIndex, type, number);// Yes.
            if (theRMod == nullptr) {        // Was the relay module added?
                errorMessage = "Mini Module " + to_string(id) + " memory allocation failed.";// No. Create error.
                toLog(errorMessage);        // Display the error.
                sqlite3_finalize(stmt);     // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
                sqlite3_close(db);          // Close SCConfig.db. Since finalize was just called there should be no errors.
                return false;               // Let the caller know the mini configuration couldn't be retrieved from the database.
            }                               // The relay module was added.
        }                                   // Done configuring a relay module.
    }                                       // The first part of the given chnl's progrma has been read.
    if (rc != SQLITE_DONE) {                // Was there an error?
        errorMessage = "Error loading Mini configurations: " + string(sqlite3_errmsg(db));// No. Create error.
        toLog(errorMessage);                // Display the error.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
        return false;                       // Didn't get the correct return character.
    }                                       // The Mini module configurations have been read successfully.
    sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
    sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
    return true;                            // Let the user know the Mini module configuration were successfully read.
} // End of function loadFromDB().

int MiniEquip::getNoOfThermoMods() {
    return (int)thermoModules.size();
}

int MiniEquip::getNoOfRelayMods() {
    return (int)relayModules.size();
}

ThermoModule* MiniEquip::addThermoMod(unsigned int address,
                                      bool isActivated,
                                      unsigned int pauseForRetry,
                                      int startIndex,
                                      string type,
                                      int nInputs) {
    ThermoModule* pTMod = new ThermoModule();   // Create a new thermocouple module.
    if (pTMod != nullptr) {                     // Was the thermocouple module created?
        thermoModules.push_back(pTMod);         // Yes. Put it in the list of thermocouple modules.
    } else {                                    // No. It wasn't created.
        return nullptr;                         // Return a null pointer to indicate failure.
    }                                           // This thermocouple module was created.
    pTMod->setAddress(address);                 // Set its address.
    pTMod->isActive = isActivated;              // Set its active state.
    pTMod->setPauseForRetry(pauseForRetry);     // Set the number of minutes before trying to communicate after a failure.
    pTMod->setStartIndex(startIndex);           // Set this module's input start index.
    pTMod->setType(type);                       // Set this module's hardware type.
    pTMod->setNoInputs(nInputs);                // Set the number of inputs.
    return pTMod;                               // Return a pointer to the thermocouple module.
}

RelayModule* MiniEquip::addRelayMod(unsigned int address,
                                    bool isActivated,
                                    unsigned int pauseForRetry,
                                    int startIndex,
                                    string type,
                                    int nRelays) {
    RelayModule* pRMod = new RelayModule();
    if (pRMod != nullptr) {                     // Was the relay module created?
        relayModules.push_back(pRMod);          // Yes. Put it in the list of relay modules.
    } else {                                    // No. It wasn't created.
        return nullptr;                         // Return a null pointer to indicate failure.
    }                                           // This relay module was created.
    pRMod->setAddress(address);                 // Set its address.
    pRMod->isActive = isActivated;              // Set its active state.
    pRMod->setPauseForRetry(pauseForRetry);     // Set the number of minutes before trying to communicate after a failure.
    pRMod->setStartIndex(startIndex);           // Set this modules relay start index.
    pRMod->setType(type);                       // Set this module's hardware type.
    pRMod->setNoOutputs(nRelays);               // Set the number of outputs.
    return pRMod;                               // Return a pointer to this relay module.
}

void MiniEquip::setRelaysOnModule(RelayModule* rMod) {
    command = rMod->getRelaySetStateCommand();  // Set the relay command which sets the desired relay states for this relay module.
    bool communicationSucceededThisTime = false;// Relay command status.
    int communicationAttempt = 1;               // Number of attempets at communication.
    if (isInArtificialMode == true) {           // Is the plant manager in artificial mode?
        incrementRelayTimes(rMod);              // Increment the relay times.
        return;                                 // Simulation is complete.
    }// Done testing for artificial mode.
    for (; communicationAttempt < 5; communicationAttempt++) {// Try up to four times to communicate with this relay module.
        // Start here, this command's response is two characters. They should be ><CR>.
        if (sendCommand(this, 2) == true) {     // Was the command to set the relays successfully sent?
            if (commandResponse.at(0) != '>') { // Yes. Was the response valid?
                rMod->failedCommunication();  // No. Take communication failure action.
                errorMessage = "Invalid response from relay module:" + rMod->addressToString() + " to command ";
                errorMessage += command;
                errorMessage = errorMessage.substr(0, errorMessage.size() - 1);// Remove the carraige return character.
                errorMessage += ". Attempt: " + to_string(communicationAttempt) + ".";
                toLog(errorMessage);            // Display the error.
                this_thread::sleep_for(chrono::milliseconds(500));// Wait five tenths of a second to try again.
                continue;                       // Attempt sending the command again.
            }// Done with invalid command response.
            incrementRelayTimes(rMod);          // Yes. Increment the relay times.
            rMod->isLastCommAttemptValid = true;// Communication succeeded.
            communicationSucceededThisTime = true;// Communication succeeded.
            break;                              // Since the command was successfully sent, there is no need to try again.
        } else {                                // The command failed to send or respond to a sent command.
            rMod->failedCommunication();        // Take communication failure action.
            errorMessage = "Error communicating with relay module:" + rMod->addressToString() + ", attempt: ";
            errorMessage += to_string(communicationAttempt) + ".";
            toLog(errorMessage);                // Display the error.
            this_thread::sleep_for(chrono::milliseconds(500));// Wait five tenths of a second to try again.
            return;
        }// Done with failed to send or respond to a sent command.
    }// Done with the attempts to send the "set relays" command.
    if (communicationSucceededThisTime == false) {// Did communication fail?
        //incrementConsecutiveErrors();         // Increment the consecutive error count.
        errorMessage = "Communication failed after 4 attempts on relay module:" + rMod->addressToString() + "= " + commandResponse;
        toLog(errorMessage);                    // Display the error.
    } // Done dealing with communication failure.
} // End of function setRelaysOnModule.

void MiniEquip::incrementRelayTimes(RelayModule* rMod) {
    for_each(rMod->relaysList_.begin(), rMod->relaysList_.end(), [&] (Relay* relay){
        // Go through each relay on this module.
        if (relay->isOn == false) {                 // Was this relay off?
            if (relay->isWantingOn == true) {       // Yes. Does the relay want to turn on?
                relay->enteringOnState();           // Yes. Start the clocks and set the state.
            }// Done testing if the relay wants on.
        } else {                                    // This relay was on.
            if (relay->isWantingOn == true) {       // Does this relay want to stay on?
                relay->incrementTimeOn();           // Yes. Increment the total time it has been on.
            } else {                                // This relay wants to turn off.
                relay->enteringOffState();          // Stop the clocks and reset the state.
            }// Done testing if this relay, which was on, wants to stay on or be turned off.
        }// Done processing this relay regardless of its current state or desired state.
    });// Done going through all the relays on this module.
}

bool MiniEquip::isCommunicationDownOnAllModules() {
    for(int nDex = 0; nDex < thermoModules.size(); nDex++) {// Go through each thermocouple module.
        ThermoModule* pTMod = thermoModules.at(nDex);// Access one of the thermocouple modules.
        if (pTMod->isActive == false) { // Is this module active?
            continue;                   // No. Go on to the next one.
        }                               // This thermocoule module is active.
        if (pTMod->isLastCommAttemptValid == true) {// Was the last attempt at communication with this module successful?
            return false;               // Yes. At least one module is working. Therefore, they are not all down.
        }                               // The evaluation of this thermocouple module is done.
    }                                   // The evaluation of all the thermocouple modules are done.
    for(int nDex = 0; nDex < relayModules.size(); nDex++) {// Go through each relay module.
        RelayModule* pRMod = relayModules.at(nDex);// Access one of the relay modules.
        if (pRMod->isActive == false) { // Is this module active?
            continue;                   // No. Go on to the next one.
        }                               // This relay module is active.
        if (pRMod->isLastCommAttemptValid == true) {// Was the last attempt at communication with this module successful?
            return false;               // Yes. At least one module is working. Therefore, they are not all down.
        }                               // The evaluation of this relay module is done.
    }                                   // The evaluation of all the relay modules are done.
    return true;                        // At this point there are no modules running.
}

void MiniEquip::resetCommunicationsOnAllModules() {
    for(int nDex = 0; nDex < thermoModules.size(); nDex++) {// Go through each thermocouple module.
        ThermoModule* pTMod = thermoModules.at(nDex);// Access a thermocouple module.
        pTMod->isLastCommAttemptValid = true;// Set the last attempt at communication to true.
    } //End of looping through each thermocouple module to set its last communication attempt to true.
    for(int nDex = 0; nDex < relayModules.size(); nDex++) {// Go through each relay module.
        RelayModule* pRMod = relayModules.at(nDex);// Access a relay modules.
        pRMod->isLastCommAttemptValid = true;// Set the last attempt at communication to true.
    } //End of looping through each relay module to set its last communication attempt to true.
}
