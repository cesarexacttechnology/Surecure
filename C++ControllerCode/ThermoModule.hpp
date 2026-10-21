//
//  ThermoModule.hpp
//  PlantManager
//
//  Created by Mark Davis on 8/17/16.
//  Copyright © 2016 Products Engineering. All rights reserved.
//

#ifndef ThermoModule_hpp
#define ThermoModule_hpp

#include <stdio.h>
#include <vector>
#include <chrono>
#include <string>


using namespace std;

namespace SCClasses {

class ThermoModule {
public:
    /*! The default constructor. */
    ThermoModule();
    /*! The default destructor. */
    ~ThermoModule
    ();
    /*! \returns The address for this thermocouple module. */
    unsigned int getAddress();
    /*! Sets the address for this thermocouple module. Valid values are between 0 and 255.
     * \param value The desired address for this thermocouple module. */
    void setAddress(unsigned int value);
    /*! \returns This thermocouple module's cold junction temperature. */
    float getCJTemperature();
    /*! \returns The temperature offset of the give channel on this module(0-7 are valid).
     *  \param value The thermocouple's index on this module(0-7 are valid). */
    float getOffset(int value);
    /*! Adds an offset value to the list of input offsets.
     * \param index The index of the input to set its offset(0-7 are valid).
     * \param value The desired offset. */
    void setOffset(unsigned int index, float value);
    /*! \returns The temperature of the give channel on this module(0-7 are valid).
     *  \param value The thermocouple's index on this module(0-7 are valid). */
    float getTemperature(int value);
    /*! Set the temperature for the thermocouple with the given offset(0-7 are valid)
     *  \param index The thermocouple's index on this module(0-7 are valid).
     *  \param value The temperature of the indexed thermocouple. */
    void setTemperature(unsigned int index, float value);
    /*! \returns This module's channel index with respect to the starting channel on this sub-system. */
    int getStartIndex();
    /*! Sets This module's channel index with respect to the starting channel on this sub-system.
     *  /param value The desired first channel index with respect to the starting channel on this sub-system for this module. */
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
    /*! \returns True is the indexed thermocouple is open.
     *  \param index The thermocouple's index on this module(0-7 are valid). */
    bool isOpen(unsigned int index);
    /*! Sets the indexed thermocouple to open.
     *  \param index The thermocouple's index on this module(0-7 are valid). */
    void setOpenThermocouple(unsigned int index);
    /*! Sets the indexed thermocouple to closed.
     *  \param index The thermocouple's index on this module(0-7 are valid). */
    void resetOpenThermocouple(unsigned int index);
    /*! Sets this modules's open thermocouple flag.
     *  \param value The open thermocouple flag for this module. FF means all open. 00 means all closed.*/
    void setOpenThermcouples(unsigned int value);
    /*! Indicates if the last attempt at communication with this module succeeded. */
    bool isLastCommAttemptValid;
    /*! The active flag for this module. */
    bool isActive;
    /*! \returns The this modules type of hardware (4018 or 6018).*/
    string getType();
    /*! Set the type of hardware used by this module.
     * \param value The input type. 4018 or 6018. */
    void setType(string value);
    /*! \returns The number of thermocouples connected to this module.*/
    int getNoInputs();
    /*! Set the number of thermocouples connected to this module.
     * \param value The number of inputs. */
    void setNoInputs(int value);
    /*! \returns The command used to get the temperatures on this module. */
    string getTemperatureCommand();
    /*! \returns The command used to get the open thermocouples on this module. */
    string getOpenThermoCommand();
    /*! \returns The command used to get the open thermocouples on this module. */
    string openCommand();
    /*! \returns The string representation of this module's address. */
    string addressToString();
    /*! Updates the temperature list (temperatures_) based on tString & oString. 
     * \param tString The string containing the temperature data.
     * \param oString The string containing the open thermocouple data. */
    void convertToTemperatures(string tString, string oString);
    
private:
    /*! Contains the address for this module. */
    unsigned int address_;
    /*! Contains this module's cold junction temperature. */
    float cjTemperature_;
    /*! Contains a list of temperature offsets for each channel on this module. */
    vector<float> offsets_;
    /*! Contains the current temperature for each thermocouple on this module. */
    vector<float> temperatures_;
    /*! Contains this module's first input index with respect to the starting input indexes on this sub-system.
     * Ex.: 0 would be the 1st input on this subsystem. */
    int startIndex_;
    /*! Contains the number of minutes between retries when a failure in communication occurs. */
    unsigned int pauseForRetry_;
    /*! The date/time when communication with this module will be attempted again. */
    chrono::time_point<chrono::system_clock> timeOfRetry_;
    /*! Contains the open thermocouple flag for this thermocouple module. FF means all open. 00 means all closed. */
    unsigned int openThermcouples_;
    /*! Contains this thermocouple module's type of hardware. */
    string type_;
    /*! Contains the number of inputs connected to this thermocouple module. */
    int nInputs_;
};

} /* namespace SCClasses */

    
#endif /* ThermoModule_hpp */
