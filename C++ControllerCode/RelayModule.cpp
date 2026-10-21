//
//  RelayModule.cpp
//  PlantManager
//
//  Created by Mark Davis on 10/31/16.
//  Copyright © 2016 Products Engineering. All rights reserved.
//

#include "RelayModule.hpp"

namespace SCClasses {
    
RelayModule::RelayModule() {
    address_ = 256;                         // Set the address to an invalid number.
    pauseForRetry_ = 2;                     // The default time to wait before trying to communicate again is two minutes.
    for (int z = 0; z < 8; z++) {           // Go through each possible relay on this module.
        desiredRelayStates_.push_back(false);// Set the desired states to off.
    }                                       // All relays are set to off.
    isLastCommAttemptValid = true;          // Assume the equipment worked last time.
}
    
RelayModule::~RelayModule() {
}
    
unsigned int RelayModule::getAddress() {
    return address_;                        // Provide the caller with this relay module's address.
}
    
void RelayModule::setAddress(unsigned int value){
    if (value < 256) {                      // Is the value a valid address?
        address_ = value;                   // Yes. Update the address.
    } else {                                // No. The value is invalid.
        address_ = 256;                     // Set the addres to an invalid number.
    }
}
    
string RelayModule::addressToString() {
    string addressString = "";      // Contains the two chracter address string.
    unsigned int ones = 0;          // Contains the ones digit.
    unsigned int sixteens = 0;      // Contains the sixteens digit.
        
    ones = address_ % 16;           // Calculate the base sixteen ones portion of the address.
    sixteens = address_ / 16;       // Calculate the base sixteen sixteens portion of the address.
    sixteens = sixteens % 16;       // Just in case a bad value was passed, limit the sixteens value to a valid number.
    // Create the character for the ones place.
    char cOnes;                     // Contains the ones place character.
    if (ones < 10) {                // Is the value for the ones place less than 10?
        cOnes = (char)'0' + ones;   // The ones character is the letter '0'(zero) incremented by the ones value.
    } else {                        // The ones value is greater than or equal to ten.
        cOnes = (char)'A' + ones - 10;// The ones character is 'A' incremented by the amount of the ones value greater than ten.
    }                               // The ones place character is set.
    // Create the character for the sixteens place.
    char cSixteens;                 // Contains the sixteens place character.
    if (sixteens < 10) {            // Is the value for the ones place less than 10?
        cSixteens = (char)'0' + sixteens;// The sixteens character is the letter '0'(zero) incremented by the sixteens value.
    } else {                        // The sixteens value is greater than or equal to ten.
        cSixteens = (char)'A' + sixteens - 10;// The sixteens character is 'A' incremented by the sixteens value greater than ten.
    }                               // The sixteens character has been created.
    addressString = cSixteens;      // The sixteens character comes first in the address string.
    addressString += cOnes;         // It is followed by the ones character.
    return addressString;           // Provide the caller with the address string.
}
    
    
int RelayModule::getStartIndex() {
    return startIndex_;                     // Return this thermocouple module's channel start index.
}
    
void RelayModule::setStartIndex(int value) {
    startIndex_ = value;                    // Set this relay's channel start index.
}
    
unsigned int RelayModule::getPauseForRetry() {
    return pauseForRetry_;                  // Return the minutes between retries when a failure in communication occurs.
}
    
void RelayModule::setPauseForRetry(int value) {
    pauseForRetry_ = value;                 // Set the desired minutes between retries when a failure in communication occurs.
}

string RelayModule::getType() {
    return type_;                           // Provide the caller with this module's hardware type.
}
    
void RelayModule::setType(string value) {
    if (value == "4050" || value == "6050" ) {// Is the value a valid module type?
        type_ = value;                      // Yes. Set the module's type.
    }
}
    
int RelayModule::getNoOutputs() {
    return nOutputs_;                        // Provide the caller the number of relays connected to this module.
}
    
void RelayModule::setNoOutputs(int value) {
    if (value < 0) {                        // Is the number of outputs negative.
        value = 0;                          // Yes. This is invalid set the number to 0.
    }
    if (value > 8) {                        // Does tha caller want more outputs than are possible?
        value = 8;                          // Set the number of outputs to the maximum.
    }
    nOutputs_ = value;                      // Set the number of outputs.
}
    
chrono::time_point<chrono::system_clock> RelayModule::getTimeOfRetry() {
    return timeOfRetry_;                    // Return the date/time when communication with this module will be attempted again.
}
    
void RelayModule::setTimeOfRetry(chrono::time_point<chrono::system_clock> value) {
    if (chrono::system_clock::now() + chrono::hours(24) < value) {// Is the retry time greater than a day away?
        timeOfRetry_ = chrono::system_clock::now() + chrono::hours(24);// Yes. Set the retry time to a day from now.
    } else {                                // No.
        timeOfRetry_ = value;               // Set the time to retry to the desired vlue.
    }
}
    
string RelayModule::getRelaySetStateCommand() {
    // Both the 4050 and the 6050 use the same command.
    string command = "#";           // Contains the command to set this module to the desired state. Relay set commands start with #.
    command += addressToString();   // Add the two character address to the command.
    command += "00";                // The actual "set relay command" is "00".
    unsigned int relayOutput = 0;   // The output part of the command(binary flags, 1 indicates on 0 indicates off).
    unsigned int relayMask = 1;     // A mask used to step through the relays,
    for (unsigned int relayNumber = 0; relayNumber < nOutputs_; relayNumber++) {// Go through each of this moduless potential relays.
        if (desiredRelayStates_.at(relayNumber) == true) {// Does this relay want on?
            relayOutput = relayOutput | relayMask;// Adjust the output number to set this relay to on.
        }                           // The output number has been adjusted.
        relayMask = relayMask << 1; // Slide the mask bit.
        relayMask = relayMask & 255;// Make sure it if limited to the first eight bits.
    }                               // The output number(binary) has been set.
    // The relay output is sent as two characters (characters from 0-9 and A-F) to represent the relay state.
    unsigned int firstByte = relayOutput / 16;// The first byte is for relay 4-7 (zero based).
    unsigned int secondByte = relayOutput % 16;// The second byte is for relays 0-3 (zero based).
    char firstChar = ' ';           // Will be the character for relays 4-7 (zero based).
    char secondChar = ' ';          // Will be the character for relays 0-3 (zero based).
    if (firstByte < 10) {           // Is the value of the first byte less than 10 (means 0-9 for the character)?
        firstChar = (char)'0' + firstByte;// The ones character is the letter '0'(zero) incremented by the ones value.
    } else {                        // The value is between 10 and 15 (means A-F for the character).
        firstChar = (char)'A'  + firstByte % 10;// The ones character is the letter 'A' incremented by the amount > 10.
    }                               // The first character of the address is set.
    if (secondByte < 10) {          // Is the value of the second byte less than 10 (means 0-9 for the character)?
        secondChar = (char)'0' + secondByte;// The sixteens character is the letter '0'(zero) incremented by the sixteens value.
    } else {                        // The value is between 10 and 15 (means A-F for the character).
        secondChar = (char)'A'  + secondByte % 10;// The sixteens character is the letter 'A' incremented by the amount > 10.
    }                               // The second character of the address is set.
    command += firstChar;           // Put the high order byte of the address after the #.
    command += secondChar;          // Put the low order byte of the address after the high order character.
    command += (char)13;            // Commands end with a carrage return.
    return command;                 // Return the command to the caller.
}

void RelayModule::failedCommunication() {
    timeOfRetry_ = chrono::system_clock::now() + chrono::minutes(pauseForRetry_);// Set the time communications should be retired.
    isLastCommAttemptValid = false; // Set the failure flag.
}

    
} // End of namespace SCClasses.
