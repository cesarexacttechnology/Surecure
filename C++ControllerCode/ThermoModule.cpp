//
//  ThermoModule.cpp
//  PlantManager
//
//  Created by Mark Davis on 8/17/16.
//  Copyright © 2016 Products Engineering. All rights reserved.
//

#include "ThermoModule.hpp"
#include <sstream>
#include <iostream>

namespace SCClasses {

ThermoModule::ThermoModule() {
    address_ = 256;                         // Set the address to an invalid number.
    for (int z = 0; z < 8; z++) {           // Go through each possible channel on this thermocouple module.
        offsets_.push_back(0.0f);           // Set the offset to zero.
        temperatures_.push_back(500.0f);    // Set the temperatures to open.
    }                                       // All offsets and tempeartures have been initialized.
    pauseForRetry_ = 2;                     // The default time to wait before trying to communicate again is two minutes.
    type_ = "4018";                         // The default module type is "4018".
    nInputs_ = 6;                           // the default number of thermocouples connected to this modules is 6.
    isLastCommAttemptValid = true;          // Assume the equipment worked last time.
}

ThermoModule::~ThermoModule() {
    offsets_.clear();
    temperatures_.clear();
}

unsigned int ThermoModule::getAddress() {
    return address_;                        // Provide the caller with this thermocoule module's address.
}

void ThermoModule::setAddress(unsigned int value){
    if (value < 256) {                      // Is the value a valid address?
        address_ = value;                   // Yes. Update the address.
    } else {                                // No. The value is invalid.
        address_ = 256;                     // Set the addres to an invalid number.
    }
}
    
float ThermoModule::getCJTemperature() {
    return cjTemperature_;                  // Return this module's cold junction temperature.
}

float ThermoModule::getOffset(int value) {
    if (value < 0 || value >= offsets_.size()) {// Is the index value valid?
        return 9999.9f;                     // No, return an invalid offset.
    }                                       // The index value is valid.
    return offsets_.at(value);              // Return the offset.
}
    
void ThermoModule::setOffset(unsigned int index, float value) {
    if (index < 8) {                        // Is the offsets index valid?
        offsets_.at(index) = value;         // Yes. Set the desired offset.
    }                                       // The offset is set.
}
    
float ThermoModule::getTemperature(int value) {
    if (value < 0 || value > 7) {           // Is the index value valid?
        return 9999.9f;                     // No, return an invalid temperature.
    }                                       // The index value is valid.
    return temperatures_.at(value);         // Return the temperature of the indexed thermocouple.
}
    
void ThermoModule::setTemperature(unsigned int index, float value) {
    if (value < 0 || value > 7) {           // Is the index value valid?
        return;                             // No, return, there is nothing to set.
    }                                       // The index value is valid.
    temperatures_.at(index) = value;        // Set the indexed thermocouple's temperature.
}
    
int ThermoModule::getStartIndex() {
    return startIndex_;                     // Return this module's channel start index.
}
    
void ThermoModule::setStartIndex(int value) {
    startIndex_ = value;                    // Set this module's channel start index.
}
    
unsigned int ThermoModule::getPauseForRetry() {
    return pauseForRetry_;                  // Return the minutes between retries when a failure in communication occurs.
}
 
void ThermoModule::setPauseForRetry(int value) {
    pauseForRetry_ = value;                 // Set the desired minutes between retries when a failure in communication occurs.
}
    
string ThermoModule::getType() {
    return type_;                           // Provide the caller this module's type.
}
    
void ThermoModule::setType(string value) {
    if (value == "4018" || value == "6018" ) {// Is the value a valid module type?
        type_ = value;                      // Yes. Set the module's type.
    }
}
    
int ThermoModule::getNoInputs() {
    return nInputs_;                        // Provide the caller the number of inputs connected to this module.
}
    
void ThermoModule::setNoInputs(int value) {
    if (value < 0) {                        // Is the number of inputs negative.
        value = 0;                          // Yes. This is invalid, set the number to 0.
    }
    if (value > 8) {                        // Does tha caller want more inputs than are possible?
        value = 8;                          // Set the number of inputs to the maximum.
    }
    nInputs_ = value;                       // Set the number of inputs.
}
    
chrono::time_point<chrono::system_clock> ThermoModule::getTimeOfRetry() {
    return timeOfRetry_;                    // Return the date/time when communication with this module will be attempted again.
}
    
void ThermoModule::setTimeOfRetry(chrono::time_point<chrono::system_clock> value) {
    if (chrono::system_clock::now() + chrono::hours(24) < value) {// Is the retry time greater than a day away?
        timeOfRetry_ = chrono::system_clock::now() + chrono::hours(24);// Yes. Set the retry time to a day from now.
    } else {                                // No.
        timeOfRetry_ = value;               // Set the time to retry to the desired vlue.
    }
}

string ThermoModule::getTemperatureCommand() {
    string tCommand = "#";          // Create a string to hold the temperature command. The command starts with #.
    tCommand += addressToString();  // Next is the two character module address.
    if (this->type_ == "6018") {    // Is this a nudam 6018?
        tCommand += "A";            // Yes. "A" is the all tempertures command.
    }                               // The ADAM 4018 does not have the "A" in its command string.
    tCommand += (char)13;           // Commands end with a carraige return.
    return tCommand;                // Return the completed command.
}
    
string ThermoModule::getOpenThermoCommand() {
    string oCommand = "$";          // The get open thermocouples command starts with "$".
    oCommand += addressToString();  // Next is the two character module address.
    oCommand += "B";                // "B" is the open thermocouples command.
    if (this->type_ == "6018") {    // Is this a nudam 6018?
        oCommand += "A";            // Yes. "A" is also part of the open thermocouple command.
    }                               // The ADAM 4018 does not have the "A" in its command string.
    oCommand += (char)13;           // Commands end with a carraige return.
    return oCommand;                // Return the completed command.
}

string ThermoModule::addressToString() {
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
    
void ThermoModule::convertToTemperatures(string tString, string oString) {
    if (oString.length() < 3) {     // Is the oString too short?
        oString = "00";             // Yes. Assume all thermocouples are closed.
    } else {                        // oString is long enough. The last two characters are the flag.
        oString = oString.substr(oString.length() - 3, 2);// Strip the characters before the last two.
    }                               // oString is probably valid.
    unsigned int openFlags;         // Contains the open flags as an unsigned integer.
    std::stringstream ss;           // Is used to convert the hexadecimal string into an unsigned interger.
    ss << std::hex << oString;      // Move the string containing the open flags into the stringstream for conversion.
    ss >> openFlags;                // Convert the open flags string into an unsigned integer.
    unsigned int openPointer = 1;   // Contains the bite in the openFlags being analized.
    for (int n = 0; n < 8; n++) {   // For each of the thermocouples contained on this thermoocuple module.
        float temperature = 500.0f; // The default temperature is 500 which is open.
        if (tString != "") {        // Does the temperature string exist?
            string theTemperature;  // The temperature magnitude string for just this channel.
            float sign = 1.0f;      // The sign of this channel's temperature(positive by default).
            try {                   // Try to get the temperature magnitude string for just this channel.
                // Extract the portion of tString that contains the temperature's magnitude for this channel.
                theTemperature = tString.substr(2 + n * 7, 6);// The fisrt of the string contains '>' and then either '+' or '-'.
                if (tString.substr(1 + n * 7, 1) == "-") {// Is the sign(first character) negative?
                    sign = -1.0f;   // This tempearture is negative. Change the sign.
                }                   // The sign has been set.
            } catch (...) {         // There was a problem with the string containing all the temperatures for this module.
                theTemperature = "500.0";// Set the thermocouple to open.
            }                       // theTemperature contains a valid temperature string for this channel.
            temperature = strtof(theTemperature.c_str(), nullptr);// Convert the string to a numeric temperature.
            temperature *= sign;    // Set the temperatures sign.
            temperature = temperature * 1.8f + 32.0f;// Convert the temperature to Fahrenheit.
            temperature += this->offsets_.at(n);// Adjust the temperature by the thermocouple offset.
            if (temperature > 500.0f) {// Is the temperature above the 500 degree limit?
                temperature = 500.0f;// Yes. Set the temperature to 500 degrees.
            } else if (temperature < -200.0f) {// Is the temperature below the -200 degree limit?
                temperature = -200.0f;// Yes. Set the temperature to -200 degrees.
            }                       // The temperature is within limits.
            unsigned int result = openPointer & openFlags;// Adjust for an open thermocouple.
            if (result > 0) {       // Was this channels open flag set?
                temperature = 500.0f;// Yes. Set the thermocouple to 500 degrees.
            }                       // The temperature has been adjust if there was an open thermocouple.
            temperatures_.at(n) = temperature;// Store this channel's temperature in the array of temperatures for this module.
            openPointer = openPointer << 1;// Adjust the open channel pointer for the next channel.
        } else {                    // The tempearture string is not valid.
            temperatures_.at(n) = 500.0f;// Set the temperature to 500(open).
        }                           // This channel's temperature has been processed.
    }                               // All channel temperatures have been set.
}
    
} // End of namespace SCClasses.
