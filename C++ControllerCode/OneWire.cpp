//
//  OneWire.cpp
//  PlantManager
//
//  Created by Mark Davis on 5/5/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#include "OneWire.h"

OneWireDevice::OneWireDevice() {
    type_ = OW_TYPE_TEMPERATURE;                // The default type is a temperature sensor.
    isActive = false;                           // This device is inactive until the user activates it.
    pin_ = -1;                                  // The pin is invalid until the user sets it.
    errorMessage = "";                          // There is no error message until there is an error.
    address_ = "0000000000000000";              // The default address if 0.
    index_ = -1;                                // The index is illegal until it is set.
    lastSampleTime_ = chrono::system_clock::now() - chrono::minutes(60);//Set the last sample time back an hour.
    // Temperatures should show open immediately.
    nSecsBeforeOpen_ = 60;                      // The default is to give the device 60 seconds to get a valid temperature.
    lastValidTemperature_ = 503;                // There is no valid temperature, thus the default is an invalid high temperature.
}

OneWireDevice::~OneWireDevice() {
    
}

void OneWireDevice::setType(OW_Type value) {
    type_ = value;                               // Provide the caller with this device's type.
}

OneWireDevice::OW_Type OneWireDevice::getType() {
    return type_;                               // Set this device's type.
}

void OneWireDevice::setNumOfSecsBeforeOpen(int value) {
    if (value < 0) {                            // Is the desired duration greater than zero?
        nSecsBeforeOpen_ = 0;                   // No. Set the time to wait before showing an open temperature to zero.
    } else {                                    // Yes.
        nSecsBeforeOpen_ = value;               // Set the time to wait before showing an open temperature to the desired value.
    }
}

void OneWireDevice::setLastValidTemperature(float value) {
    lastValidTemperature_ = value;              // Set the last valid temperature.
    lastSampleTime_ = chrono::system_clock::now();// Set the time this valid temperature was obtained.
}

float OneWireDevice::getTemperature() {
    if (chrono::system_clock::now() < lastSampleTime_ + chrono::seconds(nSecsBeforeOpen_)) {// Time for open?
        return lastValidTemperature_;           // No. Return the last valid temperature.
    } else {                                    // Yes. This one hasn't received a valid reading in a long time.
        return 503;                             // Return an invalid high temperature to signify an open sensor.
    }
}


void OneWireDevice::setPin(int value) {
    if (value >= 0 && value <=49) {             // Is the given value legal for a pin?
        pin_ = value;                           // Yes. Set the pin.
    } else {                                    // No.
        pin_ = -1;                              // Set the pin to an illegal value.
        isActive = false;                       // This device cannot be used without a pin.
    }                                           // The pin is set.
}

int OneWireDevice::getPin() {
    return pin_;                                // Provide the caller with pin index.
}

void OneWireDevice::setAddress(string value) {
    string test = "0123456789ABCDEF";           // This string contains the valid Hex characters.
    errorMessage = "";                          // Reset the error message.
    if (value.length() != 16) {                 // Does the address have exactly 16 characters?
        // No. The address must have 16 characters to be legal.
        errorMessage = "The OneWire Address does not contain the correct number of hex characters.";// Set error.
        return;                                 // Done, cannot set the address.
    }                                           // The value has the correct length.
    for (int nDex = 0; nDex < 16; nDex++) {     // Go through each hex character in the address string.
        int testDex = 0;                        // The index into the string containing valid Hex characters.
        for (; testDex < 16; testDex++) {       // Go through the 16 valid Hex characters.
            if (value[nDex] == test[testDex]) { // Is this a valid Hex character?
                break;                          // Yes. No need to search farther.
            }                                   // No. Try the next Hex character.
        }                                       // The nDex character of the value string has been tested.
        if (testDex == 16) {                    // Did the testDex reach 16? (No valid Hex character was found)
            errorMessage = "OneWire Address: " + value + " is not legal. Character '" + value[nDex] + "' is not legal.";//Set error.
            return;                             // The value was not a valid Hex address.
        }                                       // This character was a valid Hex character.
    }                                           // value was a valid Hex address.
    address_ = value;                           // Set the address to the value passed.
}

string OneWireDevice::getAddress() {
    return address_;                            // Provide the caller with this device's address.
}

void OneWireDevice::setIndex(int value) {
    if(value < 0 || value > 4095) {             // Is the index positive and below 4096?
        errorMessage = "The OneWire Index must be beteen 0 and 4095."; // No. This is an illegal index.
        index_ = -1;                            // Set the index to an illegal value.
    }                                           // The value is a legall index.
    index_ = value;                             // Set the index to the desired value.
}

int OneWireDevice::getIndex() {
    return index_;                              // Provide the caller with this device's index.
}



#pragma mark - Arduino class implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////

Arduino::Arduino() {
    address_ = "0000";                          // Set this arduino's default address.
    isActive = false;                           // This arduino is not active until it is set to active.
    retryDuration_ = 2;                         // The default time to wait after a communication failure is two minutes.
    errorMessage = "";                          // No error has ocurred yet.
    reconnectTime_ = chrono::system_clock::now();// It is OK to try to reconnect now.
    failureCount = 0;                           // There have been no failures yet.
    samplePeriod_ = 15;                         // The default sample period is 15 seconds.
    sampleTime_ = chrono::system_clock::now() - chrono::seconds(samplePeriod_);// Allow temperature sampling immediately.
    nSecsBeforeOpen_ = 60;                      // The default is to give the device 60 seconds to get a valid temperature.
    isConnected_ = false;                       // This arduino is not yet connected to a socket.
}

Arduino::~Arduino() {
    pins_.clear();                              // Clear the array of pins.
}

void Arduino::setAddress(string value) {
    string test = "0123456789ABCDEF";           // This string contains the valid Hex characters.
    errorMessage = "";                          // Reset the error message.
    if (value.length() != 4) {                  // Does the address have exactly 4 characters?
        // No. The address must have 4 characters to be legal.
        errorMessage = "The Arduino Address does not contain the correct number of hex characters.";// Set error.
        return;                                 // Done, cannot set the address.
    }                                           // The value has the correct length.
    for (int nDex = 0; nDex < 4; nDex++) {      // Go through each hex character in the address string.
        int testDex = 0;                        // The index into the string containing valid Hex characters.
        for (; testDex < 16; testDex++) {       // Go through the 16 valid Hex characters.
            if (value[nDex] == test[testDex]) { // Is this a valid Hex character?
                break;                          // Yes. No need to search farther.
            }                                   // No. Try the next Hex character.
        }                                       // The nDex character of the value string has been tested.
        if (testDex == 16) {                    // Did the testDex reach 16? (No valid Hex character was found)
            errorMessage = "Arduino Address: " + value + " is not legal. Character '" + value[nDex] + "' is not legal.";//Set error.
            return;                             // The value was not a valid Hex address.
        }                                       // This character was a valid Hex character.
    }                                           // value was a valid Hex address.
    address_ = value;                           // Set the address to the value passed.
}

string Arduino::getAddress() {
    return address_;                            // Provide the caller with this arduino's address
}

void Arduino::setRetryDuration(int value) {
    if (value < 0) {                            // Is the desired delay at least zero?
        retryDuration_ = 0;                     // No, set the duration to allow for the most retries.
    } else {                                    // Yes.
        retryDuration_ = value;                 // Set the amount of time to wait aftetr a communication failure.
    }                                           // the retry duration has been set.
}

int Arduino::getRetryDuration() {
    return retryDuration_;                      // Provide the caller with the retry duration.
}

void Arduino::setSamplePeriod(int value) {
    if (value < 2) {                            // Is the value less than an Arduino can handle?
        value = 2;                              // Yes. Set it to the shortest period an Arduino can handle.
    }                                           // The value is not too small.
    if (value > 14400) {                        // Is the value greater than four hours?
        value = 14400;                          // Yes. Set the period to four hours.
    }                                           // The value is within bounds.
    samplePeriod_ = value;                      // Set the sample period.
}

int Arduino::getSamplePeriod() {
    return samplePeriod_;                       // Provide the caller with the sample period.
}

void Arduino::setNumOfSecsBeforeOpen(int value) {
    if (value < 0) {                            // Is the desired duration greater than zero?
        nSecsBeforeOpen_ = 0;                   // No. Set the time to wait before showing an open temperature to zero.
    } else {                                    // Yes.
        nSecsBeforeOpen_ = value;               // Set the time to wait before showing an open temperature to the desired value.
    }
}

int Arduino::getNumOfSecsBeforeOpen() {
    return nSecsBeforeOpen_;                    // Provide the caller with the number of seconds before devices readings go open.
}

void Arduino::resetReconnectTime(int min = 0) {
    if (min < 0) {                              // Was the desired adjustment less than zero?
        min = retryDuration_;                   // Yes. Increment the next retry time by the default retry duration.
    }                                           // The minutes to adjust the retry time by are set.
    chrono::minutes m(min);                     // Create a chrono version of the amount to increment the reconnect time.
    reconnectTime_ = chrono::system_clock::now() + m;// Add the increment amonut to the present time to get a new reconnect time.
}

void Arduino::resetSampleTime() {
    sampleTime_ = chrono::system_clock::now();  // Set the samplle time to now.
}

bool Arduino::isOKToMeasureTemperatures() {
    chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// now is the present time.
    chrono::duration<double> timePast;          // The amount of time which has passed since the last sample.
    double	seconds;                            // Will contain the number of seconds since the last sample.
    if (now < sampleTime_) {                    // Past sample time?
        sampleTime_ = now;                      // Reset the sample time so this doesn't happen again.
        return false;                           // No, not OK to sample.
    }                                           // now is after the sample time as expected.
    // Calculate elapsed time.
    timePast = now - sampleTime_;               // Iime past is the difference between now and the last sample.
    seconds = timePast.count();                 // Convert to seconds.
    if (seconds > samplePeriod_) {              // Has enough time passed to allow sampling the temperatures again?
        toLog("Seconds past = " + to_string(seconds));
        if (seconds > 89.5) {
            toLog("Seconds past 89.5");
        }
        return true;                            // Yes. Let the caller know.
    }                                           // No.
    return false;                               // Let the caller know that they need to wait before sampling.
}

chrono::time_point<chrono::system_clock> Arduino::getReconnectTime() {
    return reconnectTime_;                      // Provide the caller the time after which it is OK to try to reconnect.
}

void Arduino::addPinNo(int pinNo) {
    bool isAlreadyOnList = false;               // The pin starts out not being on the list.
    for (std::vector<int>::iterator pinIt = pins_.begin(); pinIt != pins_.end(); ++pinIt) {// Go through each listed pin.
        if ((*pinIt) == pinNo) {                // Is this listed pin the same as this device's pin?
            isAlreadyOnList = true;             // Yes. This pin is already on the list.
            break;                              // There is no need to finish going through the list.
        }                                       // This listed pin does not match any pins on the list.
    }                                           // The entire list of pins has been evaluated.
    if (isAlreadyOnList == false) {             // Is this pin missing from the list.
        pins_.push_back(pinNo);                 // Yes. Add it to the list of pins being used.
    }                                           // This pin is now on the pin list.
}

int Arduino::getNumOfPins() {
    return (int)pins_.size();                   // Provide the caller with the number of pins used by this Arduino.
}

int Arduino::getPinNo(int nDex) {
    if (nDex >= getNumOfPins()) {               // Is the index valid?
        return -1;                              // The index is not valid. return an invalid pin number.
    }                                           // The index is valid.
    return pins_.at(nDex);                      // Provide the caller with the desired pin number.
}

bool Arduino::connect() {
    errorMessage = "";                          // Reset the error message.
    if(clientSocket.connectToServer() == false) {// Connect to the Arduino?
        errorMessage = "Connection to Arduino " + getAddress()+  " failed. Error: " + clientSocket.errorMessage;// No.
        resetReconnectTime(-1);                 // Since connection failed, increment the reconnection time by the default.
        failureCount++;                         // Increment the failure count.
        return false;                           // Let the caller know the connection was not established.
    } else {                                    // Yes.
        string data = clientSocket.receiveData(1024);// Get the connection response.
        if (data == "") {                       // Was there a response?
            errorMessage = "Arduino " + getAddress() +  " had a bad connection response.";// No.
            resetReconnectTime(-1);             // Since connection failed, increment the reconnection time by the default.
            failureCount++;                     // Increment the failure count.
            return false;                           // Let the caller know the connection was not established.
        }                                       // The connection response was good.
        failureCount = 0;                       // The connection was successful.
        resetReconnectTime();                   // Allow reconnections to happen.
    }                                           // This Arduino is connected.
    return true;                                // Let the caller know the connection to this Arduino was successful.
}

void Arduino::toLog(string note) {
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




#pragma mark - OneWireEquip class implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////

OneWireEquip::OneWireEquip() {
    areAllInputsMeasured = false;               // The inputs have not been measured yet.
    portNo_ = -1;                               // There is no port number until it has been set.
}

OneWireEquip::~OneWireEquip() {
    if (arduinos_.size() > 0) {                 // Are there any arduinos?
        for(unsigned int i = 0; i < arduinos_.size(); i++){	// Yes. Go through each arduino.
            Arduino* ard = arduinos_.at(i);     // Get this Arduino's address.
            ard->clientSocket.close();          // Close its socket.
            delete ard;                         // Free the memory allocated to it.
        }                                       // The arduino memory has been freed
        arduinos_.clear();						// Clear all the array of arduinos.
    }                                           // All arduinos have been removed.
    if (devices_.size() > 0) {                  // Are there any devices?
        for(unsigned int i = 0; i < devices_.size(); i++){	// Yes. Go through each device.
            delete devices_.at(i);              // Free the memory allocated to it.
        }                                       // The device memory has been freed
        devices_.clear();						// Clear the array of devices.
    }                                           // All devices have been removed.
}

void OneWireEquip::setPortNo(int value) {
    errorMessage = "";                          // Reset the error message.
    if (value < 2000 || value > 65500) {        // Is the desired port number valid?
        errorMessage = "The given port number, " + to_string(value) + ", is not between 65000 and 65500.";// No.
        toLog(errorMessage);                    // Display the error.
        return;                                 // No.
    } else {                                    // Yes.
        portNo_ = value;                        // Set the port number.
        for (std::vector<Arduino*>::iterator it = arduinos_.begin() ; it != arduinos_.end(); ++it){// Go through each arduino.
            (*it)->clientSocket.setPortNo(portNo_);// Set this arduino's port number.
        }                                       // The port on each arduino has been set
    }                                           // The port number has been set.
}

int OneWireEquip::getPortNo() {
    return portNo_;                             // Provide the caller with the communications port number.
}

Arduino* OneWireEquip::addArduino(string ipAddress, string address, bool isActivated, int retryDuration, int samplePeriod) {
    string test = "0123456789ABCDEF";           // This string contains the valid Hex characters.
    errorMessage = "";                          // Reset the error message.
    struct sockaddr_in testAdd;                 // Create an address to test.
    if (inet_pton(AF_INET, ipAddress.c_str(), &(testAdd.sin_addr)) == -1) {// Could the string convert to a network address?
        errorMessage = "Cannot add an Arduino with an illegal IP address.";// No. Set error.
        toLog(errorMessage);                    // Display the error.
        return nullptr;                         // Done, cannot set the address.
    }                                           // The given string could be converted to a network address.
    if (address.length() != 4) {                // Does the address have exactly 4 characters?
        // No. The address must have 4 characters to be legal.
        errorMessage = "Cannot add an Arduino when the address does not contain the correct number of hex characters.";// Set error.
        toLog(errorMessage);                    // Display the error.
        return nullptr;                         // Done, cannot set the address.
    }                                           // The address has the correct length.
    for (int nDex = 0; nDex < 4; nDex++) {      // Go through each hex character in the address string.
        int testDex = 0;                        // The index into the string containing valid Hex characters.
        for (; testDex < 16; testDex++) {       // Go through the 16 valid Hex characters.
            if (address[nDex] == test[testDex]) { // Is this a valid Hex character?
                break;                          // Yes. No need to search farther.
            }                                   // No. Try the next Hex character.
        }                                       // The nDex character of the address string has been tested.
        if (testDex == 16) {                    // Did the testDex reach 16? (No valid Hex character was found)
            // No. Set error.
            errorMessage = "Cannot add an Arduino with address: " + address + ". Character '" + address[nDex] + "' is not legal.";
            toLog(errorMessage);                // Display the error.
            return nullptr;                     // The address was not a valid Hex address.
        }                                       // This character was a valid Hex character.
    }                                           // address was a valid Hex address.
    if (portNo_ < 1) {                          // Has the port number been set?
        errorMessage = "Cannot add an Arduino until the port number is set.";// No. Set error.
        toLog(errorMessage);                    // Display the error.
        return nullptr;                         // An arduino cannot be configured without a port.
    }                                           // The port has been set.
    if (retryDuration < 0) {                    // Is the retry duration legal?
        errorMessage = "Cannot add an Arduino with a negative retry duration.";// No. Set error.
        toLog(errorMessage);                    // Display the error.
        return nullptr;                         // An arduino cannot be configured without a valid retry duration.
    }                                           // The retry duration is legal.
    if (samplePeriod < 2) {                     // Is the sample period legal?
        errorMessage = "Cannot add an Arduino with a sample period less than 2.";// No. Set error.
        toLog(errorMessage);                    // Display the error.
        return nullptr;                         // An arduino cannot be configured without a valid sample period.
    }                                           // The sample period is legal.
    Arduino* a = new Arduino();                 // Create a new arduino.
    a->clientSocket.setServerIPAddress(ipAddress);// Set the IP address of the new Arduino's client socket.
    a->setAddress(address);                     // Set its address to the desired value.
    a->clientSocket.setPortNo(portNo_);         // Set the port of the new Arduino's client socket.
    a->isActive = isActivated;                  // Set the active state of the new Arduino.
    a->setRetryDuration(retryDuration);         // Set the new Arduino's retry duration.
    a->setSamplePeriod(samplePeriod);           // Set the new Arduino's sample period.
    arduinos_.push_back(a);                     // Add this new arduino to the list of arduinos.
    return a;                                   // Return a pointer to the new arduino.
}

Arduino* OneWireEquip::getArduino(int index) {
    errorMessage = "";                          // Reset the error message.
    if (index < 0 || index >= arduinos_.size()) {// Does the index fall within the arduino list?
        errorMessage = "Cannot access arduino with illegal index, " + to_string(index) + ".";// No. Set error.
        toLog(errorMessage);                    // Display the error.
        return nullptr;                         // Return null pointer. There is no arduino with the given index.
    }                                           // The index is valid.
    return arduinos_.at(index);                 // Return the access to the arduino at the given index.
}

Arduino* OneWireEquip::getArduino(string address) {
    errorMessage = "";                          // Reset the error message.
    for (std::vector<Arduino*>::iterator it = arduinos_.begin() ; it != arduinos_.end(); ++it){// Go through each arduino.
        if ((*it)->getAddress() == address) {   // Is the given address the same as this arduino's address?
            return *it;                         // Yes. Provide the caller a pointer to this arduino.
        }                                       // No. This wasn't the arduino.
    }                                           // An arduino with the given address was not found.
    errorMessage = "Cannot access an arduino with address, " + address + ".";// No. Set error.
    toLog(errorMessage);                        // Display the error.
    return nullptr;                             // Return null pointer. There is no arduino with the given index.
}

bool OneWireEquip::removeArduino(int index) {
    errorMessage = "";                          // Reset the error message.
    if (index < 0 || index >= arduinos_.size()) {// Is the index too large?
        errorMessage = "Cannot remove arduino with index " + to_string(index);// Yes.
        errorMessage += ". There are only " + to_string(arduinos_.size()) + " arduinos.";// Set error.
        toLog(errorMessage);                    // Display the error.
        return false;                           // The index doesn't reference anything that can be removed.
    }                                           // The index is valid.
    delete arduinos_.at(index);                 // Free the memory allocated to this arduino.
    arduinos_.erase(arduinos_.begin() + index); // Remove the arduino at the given index from the list.
    return true;                                // Let the caller know the arduino was removed.
}

bool OneWireEquip::removeArduino(string address) {
    errorMessage = "";                          // Reset the error message.
    for (int index = 0; index < arduinos_.size(); index++){// Go through each arduino.
        Arduino* a = arduinos_.at(index);       // Get access to this arduino.
        if (a->getAddress() == address) {       // Is the given address the same as this arduino's address?
            delete arduinos_.at(index);         // Yes. Free the memory allocated to this arduino.
            arduinos_.erase(arduinos_.begin() + index);// Remove the arduino at the given index from the list.
            return true;                        // Let the caller know the arduino was removed.
        }                                       // No. This wasn't the arduino.
    }                                           // An arduino with the given address was not found.
    errorMessage = "Cannot access an arduino with address, " + address + ".";// Set the error.
    toLog(errorMessage);                        // Display the error.
    return false;                               // There isn't an arduino with the given address to remove.
}

OneWireDevice* OneWireEquip::addDevice(OneWireDevice::OW_Type type, string address, int index) {
    string test = "0123456789ABCDEF";           // This string contains the valid Hex characters.
    errorMessage = "";                          // Reset the error message.
    if (address.length() != 16) {               // Does the address have exactly 16 characters?
        // No. The address must have 16 characters to be legal.
        errorMessage = "Cannot add this device. The address does not contain the correct number of hex characters.";// Set error.
        toLog(errorMessage);                    // Display the error.
        return nullptr;                         // Done, cannot set the address and therefore cannot add the device.
    }                                           // The value has the correct length.
    for (int nDex = 0; nDex < 16; nDex++) {     // Go through each hex character in the address string.
        int testDex = 0;                        // The index into the string containing valid Hex characters.
        for (; testDex < 16; testDex++) {       // Go through the 16 valid Hex characters.
            if (address[nDex] == test[testDex]) { // Is this a valid Hex character?
                break;                          // Yes. No need to search farther.
            }                                   // No. Try the next Hex character.
        }                                       // The nDex character of the value string has been tested.
        if (testDex == 16) {                    // Did the testDex reach 16? (No valid Hex character was found)
            char errChar = address[nDex];       // no. Get the errant character.
            string errStr = string(&errChar);   // Convert it to a string.
            errorMessage = "Cannot add device. Address: " + address;// Set error
            errorMessage += " is not legal. Character '" + errStr + "' is not a Hex character.";// message.
            toLog(errorMessage);                // Display the error.
            return nullptr;                     // The value was not a valid Hex address.
        }                                       // This character was a valid Hex character.
    }                                           // 'address' was a valid Hex address.
    // Check to see if there is already a device with this address.
    for (std::vector<OneWireDevice*>::iterator it = devices_.begin() ; it != devices_.end(); ++it){// Go through each device.
        if ((*it)->getAddress() == address) {   // Is the desired address the same as this device's address?
            errorMessage = "The address " + address + " has already been used.";// Yes. Set error message.
            toLog(errorMessage);                // Display the error.
            return nullptr;                     // The value was not valid for a new address.
        }                                       // No. This wasn't the device.
    }                                           // A device with the given address was not found.
    if (index< 0) {                             // Is the given value legal for the index?
        errorMessage = "Cannot add device. index: " + to_string(index);// No.
        errorMessage += " is not legal. The index must be greater than or equal to 0.";//Set error.
        toLog(errorMessage);                    // Display the error.
        return nullptr;                         // Done, cannot set the index and therefore cannot add the device.
    }                                           // The index is legal.
    for (std::vector<OneWireDevice*>::iterator it = devices_.begin() ; it != devices_.end(); ++it){// Go through each device.
        if ((*it)->getIndex() == index) {       // Is the desired index the same as this device's index?
            errorMessage = "The index " + to_string(index) + " has already been used.";// Yes. Set error message.
            toLog(errorMessage);                // Display the error.
            return nullptr;                     // The value was not valid as a new index since it has already been used.
        }                                       // No. This wasn't the device.
    }                                           // A device with the given index was not found.
    OneWireDevice* ow = new OneWireDevice();    // Create a new arduino.
    if (ow == nullptr) {                        // Was memory allocated for the OneWireDevice?
        errorMessage = "Cannot add device. The OneWireDevice couln't be allocated";// No.
        toLog(errorMessage);                    // Display the error.
        return nullptr;                         // Done, cannot set the address and therefore cannot add the device.
    }                                           // Memory is allocated for the OneWireDevice.
    ow->setAddress(address);                    // Set the new device's address.
    ow->setIndex(index);                        // Set the new device's index.
    ow->setType(type);                          // Set the type of OneWireDevice.
    devices_.push_back(ow);                     // Add this new device to the list of devices connect to this arduino.
    return ow;                                  // Return a pointer to the new device.
}

OneWireDevice* OneWireEquip::getDevice(int index) {
    errorMessage = "";                          // Reset the error message.
    if (index < 0 || index >= devices_.size()) { // Is the index zero or positive and less than the number of devices?
        errorMessage = "Cannot access device with illegal index, " + to_string(index) + ".";// No. Set error.
        toLog(errorMessage);                    // Display the error.
        return nullptr;                         // Return null pointer. There is no device with the given index.
    }                                           // The index is legal.
    return devices_.at(index);                  // Provide the caller with access to the device at the given index.
}

OneWireDevice* OneWireEquip::getDevice(string address) {
    errorMessage = "";                          // Reset the error message.
    for (std::vector<OneWireDevice*>::iterator it = devices_.begin() ; it != devices_.end(); ++it){// Go through each device.
        if ((*it)->getAddress() == address) {   // Is the given address the same as this device's address?
            return *it;                         // Yes. Provide the caller a pointer to this device.
        }                                       // No. This wasn't the device.
    }                                           // A device with the given address was not found.
    errorMessage = "Cannot access a device with address, " + address + ".";// No. Set error.
    toLog(errorMessage);                        // Display the error.
    return nullptr;                             // Let the caller know there was no device with that address.
}

bool OneWireEquip::removeDevice(int index) {
    errorMessage = "";                          // Reset the error message.
    if (index < 0 || index >= devices_.size()) {// Is the index too large?
        errorMessage = "Cannot remove device with index " + to_string(index);// Yes.
        errorMessage += ". There are only " + to_string(devices_.size()) + " devices.";// Set error.
        toLog(errorMessage);                    // Display the error.
        return false;                           // The index doesn't reference anything that can be removed.
    }                                           // The index is valid.
    delete devices_.at(index);                  // Free the memory allocated to this device.
    devices_.erase(devices_.begin() + index);   // Remove the device at the given index from the list.
    return true;                                // Let the caller know the device was removed.
}

bool OneWireEquip::removeDevice(string address) {
    errorMessage = "";                          // Reset the error message.
    for (int index = 0; index < devices_.size(); index++){// Go through each device.
        OneWireDevice* owd = devices_.at(index);// Get access to this device.
        if (owd->getAddress() == address) {     // Is the given address the same as this device's address?
            delete devices_.at(index);          // Yes. Free the memory allocated to this device.
            devices_.erase(devices_.begin() + index);// Remove the device at the given index from the list.
            return true;                        // Let the caller know the device was removed.
        }                                       // No. This wasn't the device.
    }                                           // A device with the given address was not found.
    errorMessage = "Cannot access a device with address, " + address + ".";// Set the error.
    toLog(errorMessage);                        // Display the error.
    return false;                               // There isn't an arduino with the given address to remove.
}

int OneWireEquip::getNumOfDevices() {
    return (int)devices_.size();                // Provide the number of devices potentially connected to this sub-system.
}

void OneWireEquip::initialize() {
    timeInitialized_ = chrono::system_clock::now();// The initialization is occurring now.
    // Is used to prevent initialization from ocuring too often.
    if (isConfigurationRead == false) {         // Has the one wire equipment configuration been loaded?
        if (loadConfig() == false) {            // No. Was it successfully loaded?
            return;                             // No. An error message is saved by loadConfig(). Nothing left to do.
        }                                       // The one wire equipment configuration was loaded.
    }                                           // The configuration is loaded.
    if (this->subSystem.isOnLine == false) {    // Is the sub-system containing this one wire equipment on-line?
        errorMessage = "One Wire sub-system " + to_string(subSystem.getSubSystemNo()) + " is off-line.";// No. Create error.
        toLog(errorMessage);                    // Display the error.
        return;                                 // Initialization can go no farther.
    }                                           // The sub-system containing this one wire equipment is on-line.
    connectToSockets();                         // Connect all arduinos that make up this sub-system to sockets.
}

void OneWireEquip::connectToSockets() {
    // For this equipment, go through each Arduino in order to establish socket connections.
    // In other words, establish socket connections with all the Arduinos which make up this one wire equipment.
    for (std::vector<Arduino*>::iterator it = arduinos_.begin(); it != arduinos_.end(); ++it) {// Interate through the Arduinos.
        // There may be another Arduino that use the same socket as this one. Don't open the same socket twice.
        // Go back and check the previous Arduinos to be sure a socket connection hasn't already been established.
        bool match = false;                     // Is set to flase when there is another Arduino with the same socket.
        for (std::vector<Arduino*>::iterator it2 = arduinos_.begin(); it2 != it; ++it2) {// Interate through the prvious Arduinos.
            // Does this Arduino's socket(address) match this previously opened Arduino's socket(address)?
            if ((*it)->clientSocket.getServerIPAddress().sin_addr.s_addr == (*it2)->clientSocket.getServerIPAddress().sin_addr.s_addr) {
                match = true;                   // Yes. There is a match.
                break;                          // There is no need to see if another match exists.
            }                                   // This one didn't match this previous one.
        }                                       // match is true if this Arduino's socket(address) matches another already opened.
        if (match == true) {                    // Is there a match?
            continue;                           // Yes. There is a match. We don;t want to open the same socket twice so
        }                                       // we will go on to evaluate the next Arduino.
        // This Arduino's socket has not been opened already. Its socket needs to be opened.
        Arduino* ard = *it;                     // ard represents the Arduino to establish a connection with.
        if (ard->connect() == false) {          // Was a connection established to the Arduino?
            ard->isConnected_ = false;          // This arduino is not connected to a socket.
            errorMessage = ard->errorMessage;   // No. Get the error.
            toLog(errorMessage);                // Display the error.
        } else {                                // The connection attempt succeeded.
            ard->isConnected_ = true;           // This arduino is connected to a socket.
        }                                       // The attempt to connect this arduino to a socket is complete.
    }                                           // All arduino have been connected.(This doesn't mean each connection succeeded.)
}


bool OneWireEquip::saveConfig() {
    SCXML xmlOWConfig;                          // Create an XML object to hold the one wire eqipment configuration.
    xmlOWConfig.root.name = "OneWireConfig";    // Give it a name to reflect one wire configuration.
    xmlOWConfig.root.data = "";                 // There is no data in the root.
    XMLNode* parent = &xmlOWConfig.root;        // Contains the address of the root node.
    XMLNode* xmlVersion = new XMLNode(parent, "Version", to_string(1));// Create a node for the version.
    xmlOWConfig.root.subNodes.push_back(xmlVersion);// Add this node to the array of subnodes.
    XMLNode* xmlCount = new XMLNode(parent, "NumberOfArduinos", to_string(arduinos_.size()));// Create node for number of Arduinos.
    xmlOWConfig.root.subNodes.push_back(xmlCount);// Add this node to the array of subnodes.
    XMLNode* xmlPort = new XMLNode(parent, "Port", to_string(portNo_));// Create a node for the port number.
    xmlOWConfig.root.subNodes.push_back(xmlPort);// Add this node to the array of subnodes.
    for (unsigned int ardDex = 0; ardDex < arduinos_.size(); ardDex++) {// Go through each Arduino.
        XMLNode* xmlArd = new XMLNode(parent, "Arduino_" + to_string(ardDex), "");// Create a node for this Arduino.
        string ipAdd = arduinos_.at(ardDex)->clientSocket.getIPAddressString();// Get the IP address used by this Arduino.
        XMLNode* xmlIPAdd = new XMLNode(xmlArd, "IP_Address", ipAdd);// Create a node containing this Arduino's IP Address.
        xmlArd->subNodes.push_back(xmlIPAdd);   // Add this node to the array of subnodes.
        XMLNode* xmlAdd = new XMLNode(xmlArd, "Address", arduinos_.at(ardDex)->getAddress());// Create node containing the Address.
        xmlArd->subNodes.push_back(xmlAdd);     // Add this node to the array of subnodes.
        string isActiveStr = "false";           // By default, the Arduino is not active.
        if (arduinos_.at(ardDex)->isActive == true) {// Is this Arduino active?
            isActiveStr = "true";               // Yes. Set isActive to true.
        }                                       // IsActive has been set.
        XMLNode* xmlActive = new XMLNode(xmlArd, "IsActive", isActiveStr);// Set the active status of this Arduino.
        xmlArd->subNodes.push_back(xmlActive);  // Add this node to the array of subnodes.
        string retryPeriod = to_string(arduinos_.at(ardDex)->getRetryDuration());// Create a string representing the retry duration.
        XMLNode* xmlRetry = new XMLNode(xmlArd, "PauseForRetry", retryPeriod);// Set the time to wait for a retry for this Arduino.
        xmlArd->subNodes.push_back(xmlRetry);   // Add this node to the array of subnodes.
        string sPeriod = to_string(arduinos_.at(ardDex)->getSamplePeriod());// Create a string representing the sample period.
        XMLNode* xmlSPeriod = new XMLNode(xmlArd, "SamplePeriod", sPeriod);// Set minimum seconds between samples on this Arduino.
        xmlArd->subNodes.push_back(xmlSPeriod); // Add this node to the array of subnodes.
        int nPins = arduinos_.at(ardDex)->getNumOfPins();// Get the number of pins being used by this Arduino.
        XMLNode* xmlNPins = new XMLNode(xmlArd, "NumberOfPins", to_string(nPins));// Create node containing # of pins.
        xmlArd->subNodes.push_back(xmlNPins);   // Add this node to the array of subnodes.
        for (unsigned int pinDex = 0; pinDex < nPins; pinDex++) {
            int thePin = arduinos_.at(ardDex)->getPinNo(pinDex);// Get access to pin being used on this Arduino.
            XMLNode* xmlPin = new XMLNode(xmlArd, "Pin_" + to_string(pinDex), to_string(thePin));// Create a node for this pin.
            xmlArd->subNodes.push_back(xmlPin); // Add this node to the array of subnodes.
        }                                       // All the pins being used by this Arduino have been added.
        xmlOWConfig.root.subNodes.push_back(xmlArd);// Add this Arduino to the vector containing xml data.
    }                                           // Each Arduino's data has been put in xml format and stored.
    XMLNode* xmlDevCount = new XMLNode(parent, "NumberOfDevices", to_string(devices_.size()));// Create node for number of Devices.
    xmlOWConfig.root.subNodes.push_back(xmlDevCount);// Add this node to the array of subnodes.
    for (unsigned int devDex = 0; devDex < devices_.size(); devDex++) {// Go through each Device.
        XMLNode* xmlDev = new XMLNode(parent, "Device_" + to_string(devDex), "");// Create a node for this Device.
        OneWireDevice* owDev = devices_.at(devDex);// Get this device from the list of devices.
        XMLNode* xmlType = new XMLNode(xmlDev, "Type", to_string(owDev->getType()));// Create node with the device's type.
        xmlDev->subNodes.push_back(xmlType);    // Add this node to the array of subnodes.
        string isActiveStr = "false";           // By default, the Device is not active.
        if (owDev->isActive == true) {          // Is this Device active?
            isActiveStr = "true";               // Yes. Set isActive to true.
        }                                       // IsActive has been set.
        XMLNode* xmlActive = new XMLNode(xmlDev, "IsActive", isActiveStr);// Set the active status of this Device.
        xmlDev->subNodes.push_back(xmlActive);  // Add this node to the array of subnodes.
        XMLNode* xmlAdd = new XMLNode(xmlDev, "Address", owDev->getAddress());// Create node containing the Address.
        xmlDev->subNodes.push_back(xmlAdd);     // Add this node to the array of subnodes.
        XMLNode* xmlDex = new XMLNode(xmlDev, "Index", to_string(owDev->getIndex()));// Create node containing the device's index.
        xmlDev->subNodes.push_back(xmlDex);     // Add this node to the array of subnodes.
        xmlOWConfig.root.subNodes.push_back(xmlDev);// Add this Device to the vector containing xml data.
    }                                           // All Device nodes have been added.
    xmlOWConfig.createLines();                  // Put the XML nodes in line format.
    xmlOWConfig.setFilePath(filePath_);         // Set the path to the XML file.
    string fileName = "OneWire";                // Start the file name.
    fileName += to_string(this->subSystem.getSubSystemNo());// Add the subsystem number to the file name.
    fileName += ".xml";                         // Complete the name with an xml extension.
    xmlOWConfig.fileName = fileName;            // Set the name of the XML file.
    if (xmlOWConfig.writeXMLFile() == false) {  // Write the program using the preset filePath and fileName.
        errorMessage = "There was an error writing to " + filePath_ + fileName + ".";// Create an error message.
        toLog(errorMessage);                    // Display the error.
        return false;                           // Let the caller know there was a problem.
    }                                           // The program was successfully stored.
    // All the memory allocated to the subNodes of xmlOWConfig is freed when it goes out of scope. Its detructor is called.
    return true;
}

bool OneWireEquip::loadConfig() {
    isConfigurationRead = false;                // The configuration has not successfully been loaded yet.
    SCXML xmlOWConfig;                          // Create an XML object to hold the one wire eqipment configuration.
    xmlOWConfig.setFilePath(filePath_);         // Set the path to the XML file.
    string fileName = "OneWire";                // Start the file name.
    fileName += to_string(this->subSystem.getSubSystemNo());// Add the subsystem number to the file name.
    fileName += ".xml";                         // Complete the name with an xml extension.
    xmlOWConfig.fileName = fileName;            // Set the name of the XML file.
    if (xmlOWConfig.readXMLFile() == false) {   // Could the XML file be read?
        errorMessage = "The one wire equipment configuration file " + filePath_ + fileName + " couldn't be loaded.";// No.
        toLog(errorMessage);                    // Display the error.
        return false;                           // No, let the caller know there was a problem.
    }                                           // The XML file containg the one wire equipment configuration was read.
    string data = xmlOWConfig.getDataFrom("Version");// Get the config file version.
    if (data == "") {                           // Was data retrieved for the version?
        errorMessage = "There wasn't a version found in " + filePath_ + fileName + ".";// No. Create error.
        toLog(errorMessage);                    // Display the error.
        return false;                           // Let the caller know there was an error.
    }                                           // Something was read for the version.
    int version = atoi(data.c_str());           // Set the one wire equipment config file version.
    if (version > 1) {                          // Is this an advanced version?
        ;                                       // Yes. For the future.
    }                                           //
    data = xmlOWConfig.getDataFrom("Port");     // Get the port used by all the Arduinos.
    if (data == "") {                           // Was data retrieved for the port?
        errorMessage = "The port wasn't found in " + filePath_ + fileName + ".";// No. Create error.
        toLog(errorMessage);                    // Display the error.
        return false;                           // Let the caller know there was an error.
    }                                           // Something was read for the the port.
    int port = (int)strtol(data.c_str(), nullptr, 10);// Convert the number of Arduinos string to an integer.
    setPortNo(port);                            // Set this equipment's port.
    if (errorMessage != "") {                   // Was there an error?
        errorMessage = "The port couldn't be set.";// No. Create error.
        toLog(errorMessage);                    // Display the error.
        return false;                           // Let the caller know there was an error.
    }                                           // The port number has been set.
    data = xmlOWConfig.getDataFrom("NumberOfArduinos");// Get the number of Arduinos.
    if (data == "") {                           // Was data retrieved for the number of Arduinos?
        errorMessage = "The number of Arduinos wasn't found in " + filePath_ + fileName + ".";// No. Create error.
        toLog(errorMessage);                    // Display the error.
        return false;                           // Let the caller know there was an error.
    }                                           // Something was read for the the number of Arduinos.
    int nArduinos = (int)strtol(data.c_str(), nullptr, 10);// Convert the number of Arduinos string to an integer.
    data = xmlOWConfig.getDataFrom("NumberOfDevices");// Get the number of Devices.
    if (data == "") {                           // Was data retrieved for the number of Devices?
        errorMessage = "The number of Devices wasn't found in " + filePath_ + fileName + ".";// No. Create error.
        toLog(errorMessage);                    // Display the error.
        return false;                           // Let the caller know there was an error.
    }                                           // Something was read for the the number of Devices.
    int nDevices = (int)strtol(data.c_str(), nullptr, 10);// Convert the number of Devices string to an integer.
    for (int ardDex = 0; ardDex < nArduinos; ardDex++) {// Read data for each Arduino.
        string ardNo = to_string(ardDex);       // Create a string version of the Arduino index.
        XMLNode* xmlArd = xmlOWConfig.findNode(&xmlOWConfig.root, "Arduino_" + ardNo);//Get the node containing this Arduino.
        if (xmlArd == nullptr) {                // Was the node retrieved?
            errorMessage = "Arduino_" + ardNo + " wasn't found in " + filePath_ + fileName + ".";// No. Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // An Arduino exists.
        XMLNode* xmlIPAddress = xmlArd->findSubNode("IP_Address");// Get the node containing this Arduino's IP address.
        if (xmlIPAddress == nullptr) {          // Was the node retrieved?
            errorMessage = "Arduino_" + ardNo + " IP address wasn't found in " + filePath_ + fileName + ".";// No. Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The IP address exists.
        string ipAddress = xmlIPAddress->data;  // Get the IP address from the node's data.
        XMLNode* xmlAddress = xmlArd->findSubNode("Address");// Get the node containing this Arduino's address.
        if (xmlAddress == nullptr) {            // Was the node retrieved?
            errorMessage = "Arduino_" + ardNo + " address wasn't found in " + filePath_ + fileName + ".";// No. Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The address exists.
        string address = xmlAddress->data;      // Get the address from the node's data.
        XMLNode* xmlIsActive = xmlArd->findSubNode("IsActive");// Get the node containing this Arduino's active state.
        if (xmlIsActive == nullptr) {          // Was the node retrieved?
            errorMessage = "Arduino_" + ardNo + " active state wasn't found in " + filePath_ + fileName + ".";// No. Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The active state exists.
        bool boolActive = false;                // The default active state is false.
        if (xmlIsActive->data == "true") {      // Is this Arduino active?
            boolActive = true;                  // Yes. Set the active state to true.
        }                                       // The active state has been set.
        XMLNode* xmlRetryPeriod = xmlArd->findSubNode("PauseForRetry");// Get the node containing this Arduino's retry period.
        if (xmlRetryPeriod == nullptr) {        // Was the node retrieved?
            errorMessage = "Arduino_" + ardNo + " retry period wasn't found in " + filePath_ + fileName + ".";// Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The duration to pause before reconnecting exists.
        int retryPeriod = (int)strtol(xmlRetryPeriod->data.c_str(), nullptr, 10);// Get the retry period from the node's data.
        XMLNode* xmlSPeriod = xmlArd->findSubNode("SamplePeriod");// Get the node containing this Arduino's sample period.
        if (xmlSPeriod == nullptr) {            // Was the node retrieved?
            errorMessage = "Arduino_" + ardNo + " sample period wasn't found in " + filePath_ + fileName + ".";// Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The sample period exists.
        int samplePeriod = (int)strtol(xmlSPeriod->data.c_str(), nullptr, 10);// Get the seconds between samples from node's data.
        XMLNode* xmlNPins = xmlArd->findSubNode("NumberOfPins");// Get the node containing this Arduino's number of pins used.
        if (xmlNPins == nullptr) {              // Was the node retrieved?
            errorMessage = "Arduino_" + ardNo + " number of pins wasn't found in " + filePath_ + fileName + ".";// Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The number of pins exist.
        int nPins = (int)strtol(xmlNPins->data.c_str(), nullptr, 10);// Get the number of pins used from node's data.
        Arduino* ptrArduino = addArduino(ipAddress, address, boolActive, retryPeriod, samplePeriod);// Create this Arduino.
        if (ptrArduino == nullptr) {            // Was the Arduino created?
            errorMessage = "Arduino_" + ardNo + " couldn't be created.";// No. Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }
        ptrArduino->clientSocket.setPortNo(port);// Set this arduino's socket's port.
        for (int pinDex = 0; pinDex < nPins; pinDex++) {// Read each pin number.
            string pinNo = to_string(pinDex);       // Create a string version of the Arduino index.
            XMLNode* xmlPin = xmlOWConfig.findNode(&xmlOWConfig.root, "Pin_" + pinNo);//Get the node containing this pin.
            if (xmlPin == nullptr) {                // Was the node retrieved?
                errorMessage = "Pin_" + pinNo + " wasn't found for Arduino_" +  ardNo + ".";// No. Create error.
                toLog(errorMessage);                // Display the error.
                return false;                       // Let the caller know there was an error.
            }                                       // The pin exists.
            int pin = (int)strtol(xmlPin->data.c_str(), nullptr, 10);// Get the pin number from node's data.
            ptrArduino->addPinNo(pin);              // Add this pin to the list of pins used by this Arduino.
        }                                           // All the pins have been added.
    }                                               // All Arduinos have been added to this equipment(sub-system).
    for (int devDex = 0; devDex < nDevices; devDex++) {
        string devNo = to_string(devDex);       // Create a string version of the Device's index.
        XMLNode* xmlDev = xmlOWConfig.findNode(&xmlOWConfig.root, "Device_" + devNo);//Get the node containing this Device.
        if (xmlDev == nullptr) {                // Was the node retrieved?
            errorMessage = "Device_" + devNo + " wasn't found in " + filePath_ + fileName + ".";// No. Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // A Device exists.
        XMLNode* xmlType = xmlDev->findSubNode("Type");// Get the node containing this device's type.
        if (xmlType == nullptr) {               // Was the node retrieved?
            errorMessage = "Device_" + devNo + " type wasn't found in " + filePath_ + fileName + ".";// No. Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The device's type exists.
        int t = (int)strtol(xmlType->data.c_str(), nullptr, 10);//Convert the string containing the type to a number.
        XMLNode* xmlActDev = xmlDev->findSubNode("IsActive");// Get the node containing this device's type.
        if (xmlActDev == nullptr) {              // Was the node retrieved?
            errorMessage = "Device_" + devNo + " active status wasn't found in " + filePath_ + fileName + ".";// Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The device's active status exists.
        bool boolActDev = false;                // By default, the device is not active.
        if (xmlActDev->data == "true") {        // Is this device active?
            boolActDev = true;                  // Yes. Set its active flag to true.
        }                                       // The active flag has been set.
        XMLNode* xmlAdd = xmlDev->findSubNode("Address");// Get the node containing this device's address.
        if (xmlAdd == nullptr) {                // Was the node retrieved?
            errorMessage = "Device_" + devNo + " address wasn't found in " + filePath_ + fileName + ".";// Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The device's address exists.
        string add = xmlAdd->data;              // Remeber the device's address.
        XMLNode* xmlDex = xmlDev->findSubNode("Index");// Get the node containing this device's index.
        if (xmlDex == nullptr) {                // Was the node retrieved?
            errorMessage = "Device_" + devNo + " index wasn't found in " + filePath_ + fileName + ".";// Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The device's index exists.
        int id = (int)strtol(xmlDex->data.c_str(), nullptr, 10);//Convert the string containing the index to a number.
        // Get the node containing this device's number of seconds before its reading switches
        XMLNode* xmlSecsBeforeOpen = xmlDev->findSubNode("SecondsBeforeOpen");// to open.
        int secsBeforeOpen = 30;                // This is the default number of seconds before a device shows open.
        if (xmlSecsBeforeOpen != nullptr) {     // Was the node retrieved?
                                                // Yes. Convert the string containing the number of seconds before open
            secsBeforeOpen = (int)strtol(xmlSecsBeforeOpen->data.c_str(), nullptr, 10);// to a number.
        }                                       // The device's seconds before open have been determined.
        // Add a device with the paramters just loaded to this arduino's list of connected devices.
        OneWireDevice* ptrDevice = addDevice((OneWireDevice::OW_Type)t, add, id);
        if (ptrDevice == nullptr) {             // Could the device be added to this arduino's list of devices.
            errorMessage = "Device_" + devNo + " couldn't be added.";// No. Create error.
            toLog(errorMessage);                // Display the error.
            return false;                       // Let the caller know there was an error.
        }                                       // The device was added to this arduino's list of devices.
        ptrDevice->isActive = boolActDev;       // Set the active status of this device.
        ptrDevice->setNumOfSecsBeforeOpen(secsBeforeOpen);// Set the number of seconds before this device goes open(up scale).
    }                                           // All devices have been added to this arduino.
    isConfigurationRead = true;                 // Remember the configuration has been loaded.
    return true;                                // Let the caller know everything was loaded and added.
}

void OneWireEquip::operateEquipment(OneWireEquip* owSys) {
    while (owSys->isOperationAllowed == true) {     // The endless loop to operate one wire equipment.
        this_thread::sleep_for(chrono::milliseconds(100));// Pause .1 seconds before getting temperatures again.
        owSys->getTemperatures();                   // Get the temperatures.
        owSys->isOperating = true;                  // The system is operating.
    }                                               // The end of the one wire operation loop.
    for (std::vector<Arduino*>::iterator it = owSys->arduinos_.begin(); it != owSys->arduinos_.end(); ++it) {// Go thru all Arduinos
        (*it)->clientSocket.close();                // and close their sockets.
    }                                               // All sockets are closed.
    owSys->isOperating = false;                     // The equipment is no longer operating.
    owSys->errorMessage = "Exiting One-Wire equipment thread " + to_string(owSys->subSystem.getSubSystemNo()) + "!";
    owSys->toLog(owSys->errorMessage);              // Display the error.
}

void OneWireEquip::getTemperatures() {
    if (SCEquipment::isInArtificialMode == true) {  // Is the Plant Manager running in artificial mode?
        for (int nDex = 0; nDex < getNumOfDevices(); nDex++) {//No. Go through each device connected to this sub-system.
            OneWireDevice* owd = getDevice(nDex);   // Get this device.
            temperature_.at(owd->getIndex()) = 60 + nDex / 10;// Set its temperature to a readable value.
        }                                           // All devices have readable temperatures.
        return;                                     // Nothing else to do when we are in Artificial mode.
    }                                               // The Plant Manager is not running in artificial mode.
    if (subSystem.isOnLine == false) {              // Is this sub-system operating?
        for (int nDex = 0; nDex < getNumOfDevices(); nDex++) {//No. Go through each device connected to this sub-system.
            OneWireDevice* owd = getDevice(nDex);   // Get this device.
            temperature_.at(owd->getIndex()) = 502; // Set its temperature to an upscale value.
        }                                           // All devices have high temperatures.
        return;                                     // There are no temperatures to get.
    }                                               // This sub-system is online.
    string data;                                    // Contains the data returned after issuing an Arduino a command.
    for (std::vector<Arduino*>::iterator it = arduinos_.begin(); it != arduinos_.end(); ++it) {// Go through each Arduino.
        Arduino* ard = *it;                         // Get a variable to hold the arduino.
        if (ard->isActive == false) {               // Is this Arduino active?
            continue;                               // No. Go on to the next Arduino.
        }                                           // This Arduino is active.
        if (ard->isOKToMeasureTemperatures() == false) {// Is it OK to measure temperatures on this Arduino?
            continue;                               // No. Go on to the next one.
        }                                           // It is OK to measure temperatures.
        string temperatureCmnd = ard->getAddress(); // The temperature command starts with the Arduino address.
        if (ard->isConnected_ == false) {           // Is this Arduino connected to a socket?
            if (ard->getReconnectTime() < chrono::system_clock::now()) {// No, has the reconnection time limit passed?
                if(ard->connect() == false) {       // Yes, try to connect. Did the Arduino connect to a socket?
                    ard->isConnected_ = false;      // This arduino is not connected to a socket.
                    errorMessage = "Aruino " + ard->getAddress() + " couldn't connect to its socket.";// No. Create error.
                    toLog(errorMessage);            // Display the error.
                    ard->resetReconnectTime(-1);    // Don't allow reconnection with this Arduino until after the retry duration.
                    continue;                       // Go on to the next Arduino.
                } else {                            // This Arduino connected to its socket.
                    ard->isConnected_ = true;       // This arduino is connected to a socket.
                }                                   // The attempt to connect this arduino to a socket is complete.
            } else {                                // The reconnection time has not occurred yet.
                continue;                           // Go on to the next Arduino.
            }                                       // This Arduino was reconnected.
        }                                           // This Arduino is connected.
        for (int nDex = 0; nDex < ard->getNumOfPins(); nDex++) {// Go thru all the pins used by this arduino.
            int pin = ard->getPinNo(nDex);          // Get this pin number.
            temperatureCmnd += addressToString(pin);// Append the pin number to the temperature command.
            temperatureCmnd += "33";                // Add the get temperatures code to the get temperatures command.
            chrono::time_point<chrono::system_clock> startCmndTime = chrono::system_clock::now();
            bool wasResponseValid = false;          // The response is not valid until tested.
            while (chrono::system_clock::now() - startCmndTime < chrono::seconds(8)) {
                if (ard->clientSocket.sendData(temperatureCmnd) == false) {// Was there a problem sending?
                    errorMessage = "There was an error sending data to Aruino " + ard->getAddress() + ".";
                    toLog(errorMessage);            // Display the error.
                    continue;
                }
                this_thread::sleep_for(chrono::seconds(2));// The Arduino needs two seconds to provide a response.
                data = ard->clientSocket.receiveData(4048);// Get the Arduino's response.
                if (data.find("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF") == data.length() - 36) {
                    wasResponseValid = true;
                    break;
                }
            }
            if (wasResponseValid == false) {
                errorMessage = "Failed to receive a valid response from Aruino " + ard->getAddress() + ", timeout.";// No.
                toLog(errorMessage);            // Display the error.
                ard->clientSocket.close();      // Close this Arduino's socket.
                ard->resetReconnectTime(-1);    // Reset the reconnect time to now plus the retry duration.
                ard->isConnected_ = false;      // This arduino is no longer connected to a socket.
                break;                          // Nothing else can be read from this Arduino.
            }
            if (data.find("^?") == 0) {
                errorMessage = "Received ^? from Aruino " + ard->getAddress() + ".";// No.
                toLog(errorMessage);            // Display the error.
                ard->clientSocket.close();      // Close this Arduino's socket.
                ard->resetReconnectTime(-1);    // Reset the reconnect time to now plus the retry duration.
                ard->isConnected_ = false;      // This arduino is no longer connected to a socket.
                break;                          // Nothing else can be read from this Arduino.
                // Start here, test the code in the wasResponseValid condition.
            }
            ard->resetSampleTime();             // Temperatures were just measured, reset the sample time.
            // Parse the response to get more meaningful data.
            int tempsReceived = (int)data.length() / 36 - 1;// Each device sends a line with 36 bytes.
            // The last line contains thirty-four "F"s plus carriage return line feed.
            for (int thisTemp = 0; thisTemp < tempsReceived; thisTemp++) { // Go through each temperature recieved and
                // convert the data in its response line into a temperature.
                // First determine which temperature is being converted.
                string owAddress = data.substr(thisTemp * 36, 16);// The line's first 16 bytes contain the one wire address.
                // Start here, verify the address is correct.
                int owOffset = -1;              // Contains the offset into this sub-system to store this temperature.
                // Start here, should one wire devices be independent of Arduinos? Answer is yes.
                for (int nDex = 0; nDex < getNumOfDevices(); nDex++) {// Go through all the devices on this sub-system.
                    OneWireDevice* owDevice = devices_.at(nDex);// Get this device.
                    if (owDevice->getAddress() == owAddress) {  // Is this the one wire device that has the response address?
                        if (owDevice->isActive == false) {      // Yes. Is this one wire device active?
                            toLog("One wire device " + owAddress + " is inactive and being ignored.");
                            owOffset = -1;                  // Ignore its response by setting the offset to -1.
                            break;                          // This one wire device is inactive. No need to look farther.
                        }                                       // This one wire device is active.
                        owOffset = owDevice->getIndex();        // Retrieve the offset to store this temperature.
                        break;                                  // Only one, one wire device can have this address, no
                    }                                           // need to go through more.
                }                                               // Done going through the one wire devices on this sub-system.
                if (owOffset < 0) {                             // Was an address match found?
                    toLog("There wasn't an address match.");
                    continue;                                   // There is no place to store this temperature.
                }                                               // Go onto the next response.
                // The temperature part of the data is returned is the 17th, 18th, 19th, and 20th bytes of the response line.
                // The 19th byte contains the sign. It is either 0 for positive numbers or F for negative numbers.
                // The 20th byte contains part of the sign and 2^4 through 2^6 bits. (Sign, 2^6, 2^5, 2^4)
                // The 17th byte contains 2^0 through 2^3 bits. (2^3, 2^2, 2^1, 2^0)
                // The 18th byte contains 2^-4 through 2^-1 bits. (2^-1, 2^-2, 2^-3, 2^-4)
                string tempData = data.substr(thisTemp * 36 + 16, 4); // Get the four bytes containing the temperature.
                float temp = (float)hexToInt(tempData[0]);      // Contains the temperature which is being converted.
                float signOfTemp = 1;                           // Start by assuming the temperature is positive.
                if (temp < 0)                                   // Did the Hexadecimal character convert to a number?
                {                                               // No.
                    temperature_.at(owOffset) = 502;            // Store this device's temperature as upscale.
                    continue;                                   // No need to continue, the
                }                                               // temperature will be left as open.
                float hiTemp = hexToInt(tempData[3]);           // Convert the high order temperature byte.
                if (hiTemp < 0)                                 // Did the Hexadecimal character convert to a number?
                {                                               // No.
                    temperature_.at(owOffset) = 502;            // Store this device's temperature as upscale.
                    continue;                                   // No need to continue, the
                }                                               // temprature will be left as open.
                if (hiTemp > 7)                                 // Is this byte greater than 7?
                {                                               // Yes, this implies a negative number.
                    hiTemp -= 8;                                // Remove the sign bit.
                    signOfTemp *= -1;                           // Set the sign of the temperature to negative.
                }                                       // Done processing a high order byte that represents a negative temeprature.
                hiTemp *= 16;                                   // Shift the number by four bits (2^4).
                temp += hiTemp;                                 // Combine the high and low order parts of the temperature.
                float lowTemp = hexToInt(tempData[1]);          // Convert the fraction temeprature byte.
                if (lowTemp < 0)                                // Did the Hexadecimal character convert to a number?
                {                                               // No.
                    temperature_.at(owOffset) = 502;            // Store this device's temperature as upscale.
                    continue;                                   // No need to continue, the
                }                                               // temperature will be left as open.
                lowTemp *= 0.0625f;                             // Shift the number down by four bits (2^4).
                temp += lowTemp;                                // Combine the temperature with its low order component.
                temp *= signOfTemp;                             // Set the sign of the temperature.
                temp = temp * 1.8f + 32.0f;                     // Standard equation for converting from degres C to degrees F.
                toLog("Temperature = " + to_string(temp) + ".");
                // All temperature processing including storage and control occurs using Fahrenheit.
                temperature_.at(owOffset) = temp;               // Store this device's temperature.
            }                                       // Done going through each temperature received from this pin on this Arduino.
        }
        // Start here, still waiting to reconnect. Might fill in best guess temperature data.
    }
    areAllInputsMeasured = true;                    // All temperatures have been measured at least once.
}

string OneWireEquip::addressToString(int address) {
    string addressString = "";          // Contains the two chracter address string.
    int ones = 0;                       // Contains the ones digit.
    int sixteens = 0;                   // Contains the sixteens digit.
    ones = address % 16;                // Calculate the base sixteen ones portion of the address.
    sixteens = address / 16;            // Calculate the base sixteen sixteens portion of the address.
    sixteens = sixteens % 16;           // Just in case a bad value was passed, limit the sixteens value to a valid number.
    char cOnes;                         // Create the character for the ones place.
    if (ones < 10) {                    // Is the value for the ones place less than 10?
        cOnes = '0';                    // Start with the character which represents zero.
        cOnes += ones;                  // Replace the zero character with one which is the given # of characters past it.
    } else {                            // The ones value is greater than or equal to ten.
        cOnes = 'A' + ones - 10;        // The ones character is 'A' incremented by the amount of the ones value greater than ten.
    }                                   // The ones character has been created.
    char cSixteens;                     // Create the character for the sixteens place.
    if (sixteens < 10) {                // Is the value for the sixteens place less than 10?
        cSixteens = '0';                // Start with the character which represents zero.
        cSixteens += sixteens;          // Replace the zero character with one which is the given # of characters past it.
    } else {                            // The sixteens value is greater than or equal to ten.
        cSixteens = 'A' + ones - 10;    // The ones character is 'A' incremented by the amount of the ones value greater than ten.
    }                                   // Both hexidecimal characters have been determined.
    addressString = cSixteens;          // The fist character in the hexidcimal string is the sixteens character.
    addressString.append(&cOnes);       // Append the one character to the end of the string.
    return addressString;               // Provide the caller with the heximdecimal string.
}

int OneWireEquip::hexToInt(char hexChar) {
    string test = "0123456789ABCDEF";   // This string contains the valid Hex characters.
    int dex = 0;                        // The index to step through the list of hex characters.
    for (; dex < 16; dex++)             // Go through each of the
    {                                   // legal hex characters.
        if (hexChar == test[dex])       // Is this the character?
        {                               // Yes.
            return dex;                 // Return its value.
        }                               // Done processing a character match.
    }                                   // Done going through the legal hex characters.
    return -1;                          // Return an illegal value.
}

