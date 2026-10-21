//
//  CountModule.cpp
//  PlantManager
//
//  Created by Mark Davis on 12/2/15.
//  Copyright © 2015 Products Engineering. All rights reserved.
//

#include "CountModule.hpp"


CountModule::CountModule() {
    address_ = 256;                         // The default address is invalid.
    counter_ = 0;                           // There should be no counts until they are read from the counter module.
}

CountModule::~CountModule() {
    
}

int CountModule::getAddress() {
    return address_;                        // Provide the caller with this count module's address.
}

void CountModule::setAddress(unsigned int value){
    if (value < 256) {                      // Is the value a valid address?
        address_ = value;                   // Yes. Update the address.
    } else {                                // No. The value is invalid.
        address_ = 256;                     // Set the addres to an invalid number.
    }
}

int CountModule::getCounter(){
    return counter_;                        // Provide the caller with this 6080's counter value.
}

void CountModule::resetCounter() {
    counter_ = 0;                           // Set this 6080's counter to zero.
}

void CountModule::incrementCounter() {
    counter_++;                             // Increment this 6080's counter. (used for simulation)
}
