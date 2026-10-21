//
//  SCChnls.h
//  PlantManager
//
//  Created by Mark Davis on 3/7/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#ifndef __PlantManager__SCChnls__
#define __PlantManager__SCChnls__

#include <vector>
#include <string>
#include "SCMaturity.h"
#include "SCProgram.h"

using namespace std;


#include <stdio.h>


namespace SCClasses {

    class SCChnls;
    
    class SCChnl {
    private:
        /*! This channel's relay index into the relay array. An index of -1 indicates this channel has no relay. */
        int relayIndex_;
        /*! This channel's input index into the input array. An index of -1 indicates this channel has no input. */
        int inputIndex_;
        /*! The maximum number of characters that a name can have. */
        unsigned int maxNameLength_;
        /*! This channel's name. The name cannot have more than 'maxNameLength' characters. */
        string name_;
        /*! This channel is not allowed to reach the set point in less than this time (units are seconds).
         * If the channel is too cold and continues at it's last rate of rise
         * ([current temperature - last temperature] / [time between samples])
         * and will exceed it's calculted setpoint at "approachLimit"
         * seconds from now, the channel is turned off, even though the channel's
         * temperature is below it's setpoint. Likewise, if the channel is too hot
         * and continues at it's last rate of desent and will fall below it's
         * calculted setpoint at "approachLimit" seconds from now, the channel is
         * turned on, even though the channel's temperature is above it's setpoint.
         * The "approachLimit" is only evaluated if minimum time-on and maximum
         * time-on for it's relay have been satisfied.
         * If the "approachLimit" is set to zero, this disables the "approachLimit". 
         * The units are seconds. */
        unsigned int approachLimit_;
        /*! The SCChnls to which this SCChnl belongs. It can be nullptr if this SCChnl does not belong to an array of SCChnls. */
        SCChnls* theSCChnls_;
    public:
        /*! The default constructor. */
        SCChnl();
        /*! The constructor which makes the channel belong to SCChnls. */
        SCChnl(SCChnls* scChnls);
        /*! The default destructor. */
        virtual ~SCChnl();
        /*! This channel's program. */
        SCProgram program;
        /*! \returns This channel's relay index into the relay array. An index of -1 indicates this channel has no relay. */
        int getRelayIndex();
        /*! Sets this channel's relay index into the relay array. An index of -1 or a value larger than the number of relays
         * indicates this channel does not have a relay associated with it.
         * \returns True if this relay could be assigned to this channel. A relay can only be assigned if it is not assigned to
         * another SCChnl(this only applies when this SCChnl is a part of an array of SCChnls, theSCChnls_ = nullptr).
         * \param value The desired value for the relay index. */
        bool setRelayIndex(int value);
        /*! \returns This channel's input index into the input array. An index of -1 indicates this channel has no input. */
        int getInputIndex();
        /*! Sets this channel's input index into the input array. An idex of -1 or a value larger than the number of inputs
         *  indicates this channel does not have an input associated with it. Multiple channels can have the same input.
         * \param value The desired value for the input index. */
        void setInputIndex(int value);
        /*! \returns This channel's name. */
        string getName();
        /*! Sets this channel's name.
         * \returns True if the name can be set to the desired value. A name can only be assigned if it is not assigned to
         * another SCChnl(this only applies when this SCChnl is a part of an array of SCChnls, theSCChnls_ = nullptr).
         * \param value The desired value for this channel's name. */
        bool setName(string value);
        /*! Gets this channel's approach limit(seconds). */
        unsigned int getApproachLimit();
        /*! Sets this channels approach limit(seconds). Zero indicates there is no approach limit. */
        void setApproachLimit(unsigned int value);
        /*! The number of degrees this channel is allowed to be away from it's setpoint without activating an alarm.
         *  A negative number means the deviation is not set, an alarm is never activated. 
         *  Setting the deviation to zero will cause the alarm to always be on. */
        float deviation;
        /*! Disables this channel's ability to activate an alarm based on a temperature deviation. */
        void disableDeviationAlarm();
        /*! Removes any input associated with this channel. */
        void removeInput();
        /*! Removes any relay associated with this channel. */
        void removeRelay();
        /*! \returns This channel's zero based index into SCChnls. Returns -1 if this SCChnl is not part of an array of SCChnls.*/
        int getIndex();
        /*! This chnl's maturity. */
        SCMaturity maturity;
    };
    
    class SCChnls: public std::vector<SCChnl*> {
    private:
        /*! The version of the file holding the SCChnl configurations. */
        unsigned int version_;
        /*! The location of the file holding the SCChnl configurations. */
        string filePath_;
        /*! The name of the file used to store the SCChnl configurations. */
        string fileName_;
        /*! Contains an error message which provides additional information if a function fails. */
        string errorMessage_;
        /*! The temperature(Fahenheit) below which all control ceases. */
        float lowerTemperatureLimit_;
        /*! The temperature(Fahenheit) above which all control ceases. */
        float upperTemperatureLimit_;
        /*! The largest valid relay index. */
        int maximumRelayIndex_;
        /*! The largest valid input index. */
        int maximumInputIndex_;
    public:
        /*! The default constructor. */
        SCChnls();
        /*! The default destructor. */
        virtual ~SCChnls();
        /*! \returns The version of the SCChnl configuration file. */
        unsigned int getVersion();
        /*! Sets the path to get the SCChnl configuration file.
         * \param value The desired path to get the SCChnl configuration file. */
        void setFilePath(string value);
        /*! \returns Additional error inforamtion for the last error which occured. */
        string getErrorMessage();
        /*! \returns The lower temperature limit in degrees Fahrenheit. */
        float getLowerTemperatureLimit();
        /*! Sets the lower temperature limit in degrees Fahrenheit. The limit can be between -200 and 400 Fahrenheit.
         * \param value The desired lower temperature limit. */
        void setLowerTemperatureLimit(float value);
        /*! \returns The upper temperature limit in degrees Fahrenheit. */
        float getUpperTemperatureLimit();
        /*! Sets the upper temperature limit in degrees Fahrenheit. The limit can be between -200 and 400 Fahrenheit.
         * \param value The desired upper temperature limit. */
        void setUpperTemperatureLimit(float value);
        /*! Loads the configurations of all SCChnls. Uses the 'filePath'. */
        bool loadFromDB();
        /*! Adds a default SCChnl at the end of the array. */
        void add();
        /*! Adds the SCChnl with the given paramters to the array at the desired index.
         * \param name The desired name for the SCChnl being added.
         * \param inputDex The index of the input to be associated with the SCChnl being added(-1 means there is no input).
         * \param relayDex The index of the relay to be associated with the SCChnl being added(-1 means there is no input).
         * \param dev The number of degrees(F) the SCChnls temperature must deviate from its set point before an alarm
         * is activated.
         * \param aLimit The minimum number of seconds this SCChnl must take before reaching its setpoint(future).
         * \param index The index where this new SCChnl should be added. -1 means put it at the end. A value larger
         * than the size of SCChnls also means put it at the end.
         * \returns True if a SCChnl with the given parameters is added to SCChnls. Use getErrorMessage()
         * to determine a reason for failure. */
        bool add(string name, int inputDex, int relayDex, float dev, unsigned int aLimit, int index);
        /*! Removes the SCChnl from the given position in the array. All channels following the removed channel are
         * moved up.
         * \returns True if the channel was removed. Sets the errorMessage_ if the SCChnl couldn't be removed.
         * \param index The location in the array to remove an SCChnl. */
        bool remove(unsigned int index);
        /*! \returns True if the given name is used by any one of the SCChnls.*/
        bool nameExists(string name);
        /*! \returns True if the relay index is used by any one of the SCChnls.*/
        bool relayIndexExists(int value);
        /*! Sets the largest valid relay index. */
        void setMaximumRelayIndex(int value);
        /*! Sets the largest valid input index. */
        void setMaximumInputIndex(int value);
        /*! The temperature used as a base line to calculate maturity in Fahrenheit. */
        double maturityBase;
        /*! Is true if the base line for the maturity is set to the program's start temperature. */
        bool isMaturityReferencedToProgram;
    };

}

#endif /* defined(__PlantManager__SCChnls__) */
