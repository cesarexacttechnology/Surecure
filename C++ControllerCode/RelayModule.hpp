//
//  RelayModule.hpp
//  PlantManager
//
//  Created by Mark Davis on 10/31/16.
//  Copyright © 2016 Products Engineering. All rights reserved.
//

#ifndef RelayModule_hpp
#define RelayModule_hpp

#include <stdio.h>
#include <vector>
#include <chrono>
#include <string>
#include "Relays.h"

using namespace std;

namespace SCClasses {

class RelayModule {
public:
    /*! The default constructor. */
    RelayModule();
    /*! The default destructor. */
    ~RelayModule();
    /*! \returns The address for this relay module. */
    unsigned int getAddress();
    /*! Sets the address for this relay module. Valid values are between 0 and 255.
     * \param value The desired address for this relay module. */
    void setAddress(unsigned int value);
    /*! \returns This module's first relay index with respect to the starting relay index(0) on this sub-system. */
    int getStartIndex();
    /*! Sets This modules's first relay index with respect to the starting relay index(0) on this sub-system.
     *  /param value The desired first relay index with respect to the starting relay index(0) on this sub-system. */
    void setStartIndex(int value);
    /*! \returns The number of minutes between retries when a failure in communication occurs. */
    unsigned int getPauseForRetry();
    /*! Sets the number of minutes between retries when a failure in communication occurs.
     *  \param value The desired number of minutes between retries when a failure in communication occurs. */
    void setPauseForRetry(int value);
    /*! \returns The date/time when communication with this module will be attempted again. */
    chrono::time_point<chrono::system_clock> getTimeOfRetry();
    /*! Sets the date/time when communication with this module will be attempted again.
     *  \param value The desired date/time when communication with this module will be attempted again. */
    void setTimeOfRetry(chrono::time_point<chrono::system_clock> value);
    /*! Indicates if the last attempt at communication with this module succeeded. */
    bool isLastCommAttemptValid;
    /*! The active flag for this relay module. */
    bool isActive;
    /*! \returns The this modules type of hardware (4050 or 6050).*/
    string getType();
    /*! Set the type of hardware used by this module.
     * \param value The input type. 4050 or 6050. */
    void setType(string value);
    /*! \returns The number of relays connected to this module.*/
    int getNoOutputs();
    /*! Set the number of relays connected to this module.
     * \param value The number of relays connected to this module. */
    void setNoOutputs(int value);
    /*! Contains the desired state of each relay on this module. */
    vector<bool> desiredRelayStates_;
    /*! \returns The string representation of this relay module's address. */
    string addressToString();
    /*! \returns A string representation of the command that sets this module's relays to the desired state. */
    string getRelaySetStateCommand();
    /*! Contains the relays on this module. */
    vector<Relay*>relaysList_;
    /*! Should be called when a communication attempt with this module fails. Sets the retry time & the failure flag. */
    void failedCommunication();

private:
    /*! Contains this relay module's address. */
    unsigned int address_;
    /*! Contains this module's first relay index with respect to the starting relay indexes on this sub-system.
     * Ex.: 0 would be the 1st output(relay) on this sub-system. */
    int startIndex_;
    /*! Contains the number of minutes between retries when a failure in communication occurs. */
    unsigned int pauseForRetry_;
    /*! Contains the number of relays connected to this relay module. */
    int nOutputs_;
    /*! Contains this relay module's type of hardware. */
    string type_;
   /*! The date/time when communication with this relay module will be attempted again. */
    chrono::time_point<chrono::system_clock> timeOfRetry_;
    };
    
} /* namespace SCClasses */

#endif /* RelayModule_hpp */
