//
//  SCPulseMeters.h
//  PlantManager
//
//  Created by Mark Davis on 10/24/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#ifndef PlantManager_SCPulseMeters_h
#define PlantManager_SCPulseMeters_h

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sqlite3.h>

using namespace std;

namespace SCClasses {
    
    class SCDemandLimit {
    private:
        /*! The electrical demand limit(watts). */
        unsigned int limit_;
        /*! The minute of the day when this limit goes into effect. 
         * 1440 implies this limit is never used. */
        unsigned int startMinute_;
        
    public:
        /*! The default constructor. */
        SCDemandLimit();
        /*! \returns The electrical demand limit. */
        unsigned int getLimit();
        /*! Sets the electrical demand limit. 
         * /param value The desired electrical demand limit. */
        void setLimit(unsigned int value);
        /*! \returns The minute of the day when this electrical demand limit begins. 
         * 1440 implies this limit is never used. */
        unsigned int getStartMinute();
        /*! Sets the minute of the day when this electrical demand limit takes effect.
         * /param value The desire minute of the day when this electrical demand limit take effect.
         * If the value is greater than 1440, it will be set to 1440(indicates limit is not used). */
        void setStartMinute(int value);
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class SCDemandSample {
    private:
        /*! This sample's number of pulses. */
        unsigned int nPulses_;
        /*! This sample's number of calculated watt-hours used by elctric heating circuits associated with this meter. */
        unsigned int usage_;
        /*! This sample's time in seconds of the day. */
        unsigned int time_;
        
    public:
        /*! The default constructor. */
        SCDemandSample();
        /*! \returns This sample's number of pulses. */
        unsigned int getPulses();
        /*! Sets this ample's number of pulses.
         * /param value This sample's number of pulses. */
        void setPulses(unsigned int value);
        /*! \returns This sample's number of watt-hours used by electric heating circuits associated with this meter. */
        unsigned int getUsage();
        /*! Sets this sample's number of watt-hours used by electric heating circuits associated with this meter.
         * \param value This sample's number of watt-hours. */
        void setUsage(unsigned int value);
        /*! This sample's time in seconds of the day. */
        unsigned int getTime();
        /*! Set this sample's time in seconds of the day.
         * \returns true If the sample's time was a legal second of the day.
         * \param value The time in seconds of the day when this sample occurred. */
        bool setTime(unsigned int value);
    };

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class SCPulseMeter {
    private:
        /*! Contains the number of pulses registered by this meter as of the last time demand data was recorded. */
        unsigned int nPulses_;
        /*! Contains the electrical usage(watt-hours) for controlled loads(heaters) on this meter. */
        unsigned int usageHeat_;
        /*! Contains a list of the demand limits associated with this meter. */
        vector<SCDemandLimit*> demandLimits_;
        /*! Contains the period(in seconds) of time demand is calculated over in seconds. (typically 900 seconds, 15 minutes). */
        unsigned int demandPeriod_;
        /*! Contains the zero based index of this pulse meter. */
        unsigned int index_;
        /*! Contains this meter's pulse constant in watt-hours per pulse. */
        unsigned int pulseConstant_;
        /*! Contains the desired number of recent demand data samples stored. */
        unsigned int nDemandSamples_;
        /*! Contains demand samples. Used for storing recent demand data.          
         * This vector is used in a circular fashion. Once the vector is full, the next sample is
         * written to position zero, then position one, etc.. */
        vector<SCDemandSample*> demandSamples_;
        /*! Contains the current sample in the demandLimits_ array of samples. */
        unsigned int currentDemandSampleNo_;
        /*! Used to initialize the demand samples when the Plant Manager is started. */
        unsigned int estimatedDemandSamplePeriod_;
        /*! For now, it is 60 seconds. This is the time period over which demand control decisions are estimated to be made. */
        unsigned int estimatedDemandCtrlPeriod_;
        /*! Contains the sub-system index this pulse meter is associated with. */
        unsigned int subSystemIndex_;
        /*! Contains the address of the pulse counter processed by this pulse meter. */
        unsigned int pulseCounterAddress_;
        /*! \returns The demand sample index immediately prior to the given index.
         * \param value The demand sample index for which the previous index is desired. */
        unsigned int prevoiusIndex(unsigned int value);
        
    public:
        /*! The default constructor. */
        SCPulseMeter();
        /*! The default destructor. */
        virtual ~SCPulseMeter();
        /*! \returns The number of pulses registered by this meter as of the last time demand data was recorded. */
        unsigned int getPulses();
        /*! Sets the number of pulses to be registered by this meter.
         * \param value The number of pulses  registered. */
        void setPulses(unsigned int value);
        /*! \returns The total electrical usage for controlled loads(heat) associated with this meter. */
        unsigned int getUsageHeat();
        /*! Sets the electrical usage for controlled loads(heat) associated with this meter.
         * /param value The desired electrical usage for controlled loads(heat) associated with this meter.. */
        void setUsageHeat(unsigned int value);
        /*! Adds a new demand limit to the list of demand limits which apply to this meter.
         * \returns true If the demand limit was added.
         * \param limit The electrical demand limit in watts. 
         * \param startMinute The minute of the day when this limit takes effect. 
         * 1440 implies it never takes effect. */
        bool addDemandLimit(unsigned int limit, unsigned int startMinute);
        /*! Sets or modifies an existing demand limit.
         * \param index The index of the demand limit in the demand limit list.
         * \param limit The electrical demand limit in watts.
         * \param startMinute The minute of the day when this limit takes effect.
         * \returns true If the index is for an existing limit.
         * 1440 implies it never takes effect. */
        bool setDemandLimit(unsigned int index, unsigned int limit, unsigned int startMinute);
        /*! Removes an existing demand limit from the list of electrical demand limits.
         * \param index The index of the demand limit in the demand limit list. 
         * \returns true If the index is for an existing limit. */
        bool removeDemandLimit(unsigned int index);
        /*! \returns The desired demand limit from the list of limits. */
        SCDemandLimit* getDemandLimit(unsigned int index);
        /*! \returns The current demand limit in watts. */
        unsigned int getDemandLimit();
        /*! Is used by getDemandLimit() to compare the start minute of two demand limits. 
         * \returns true if the first limit's start time is before the seond's. */
        static bool compareLimitTimes(SCDemandLimit* limit1, SCDemandLimit* limit2);
        /*! \returns The period of time demand is calculated over in seconds. (typically 900 seconds, 15 minutes). */
        unsigned int getDemandPeriod();
        /*! Sets the period of time demand is calculated over in seconds. (typically 900 seconds, 15 minutes)
         * \param value The desired period(in seconds) to use for calculating electrical demand. */
        void setDemandPeriod(unsigned int value);
        /*! \returns The zero based index for this pulse meter. */
        unsigned int getIndex();
        /*! Sets this meter's index.
         * \param value The desired value for this meter's index. */
        void setIndex(unsigned int value);
        /*! \returns This meter's pulse constant in watt-hours per pulse. */
        unsigned int getPulseConstant();
        /*! Sets this meter's pulse constant in watt-hours per pulse. 
         * \param value The desired value of this meter's pulse constant in watt-hours per pulse. */
        void setPulseConstant(unsigned int value);
        /*! Initializes the array of demand samples when the pulse meter is created. Exists for readability. */
        void configureDemandSamples();
        /*! \returns The number of demand data samples containg recent demand data. */
        unsigned int getNumberOfDemandSamples();
        /*! Sets the number of recent demand samples kept in memory.
         * \param value The desired number of recent demand sample's to keep. */
        void setNumberOfDemandSamples(unsigned int value);
        /*! \returns The Demand data for the given sample number.
         * \param sampleNo The sample number. Must be between 0 and nDemandSamples_ - 1. */
        SCDemandSample* getDemandSample(unsigned int sampleNo);
        /*! Sets the demand data for the given sample.
         * \returns true If the demand sample could be set.
         * \param index The sample's index. Must be between 0 and nDemandSamples_ - 1.
         * \param nPulses The number of pulses since the last sample.
         * \param usage The controlled loads(heat) usage since the last sample.
         * \param time This sample's second of the day. */
        bool setDemandSample(unsigned int index, unsigned int nPulses, unsigned int usage, unsigned int time);
        /*! \returns The current demand sample number. */
        unsigned int getCurrentDemandSampleNo();
        /*! Increments the current demand sample number.
         * If after incrementing the number it is equal to nDemandSamples_, the number is set to zero. 
         * /returns The new current demand sample number. */
        unsigned int incrementCurrentDemandSampleNo();
        /*! \returns The demand sample's second of the day for the given index.
         * \param index The sample's index. Must be between 0 and nDemandSamples_ - 1. */
        unsigned int getDemandSampleTime(unsigned int index);
        /*! \returns The demand sample's pulses for the given index.
         * \param index The sample's index. Must be between 0 and nDemandSamples_ - 1. */
        unsigned int getDemandSamplePulses(unsigned int index);
        /*! \returns The demand sample's calculated heat usage for the given index.
         * \param index The sample's index. Must be between 0 and nDemandSamples_ - 1. */
        unsigned int getDemandSampleUsage(unsigned int index);
        /*! \returns The sub-system index this pulse meter is associated with. */
        unsigned int getSubSystemIndex();
        /*! Sets the sub-system index this pulse meter is associated with.
         * \param value The index of the desired sub-system to associate this sub-system. */
        void setSubSystemIndex(unsigned int value);
        /*! \returns The address of the pulse counter processed by this pulse meter. */
        unsigned int getPulseCounterAddress();
        /*! Sets the address of the pulse counter processed by this pulse meter. 
         * \param value The pulse counter's address to be procedded by this pulse meter. */
        void setPulseCounterAddress(unsigned int value);
        /*! \returns The electrical demand(watts) for this meter. This is the total electric demand on this meter.
         * It is calculated over the demand period(demandPeriod_). */
        unsigned int getDemand();
        /*! \returns The electrical demand(watts) for only the electric heating circuits connected to this meter. 
         * This demand is calculated over the demand period(demandPeriod_). */
        unsigned int getDemandHeat();
        /*! \returns The instantaneous electrical demand(watt) for the electric heating cicuits on this meter.
         * This demand is calculated over the last sample period. */
        unsigned int getDemandHeatInstantaneous();
        /*! \returns The instantaneous electrical demand(watt) for evrything on this meter.
         * This demand is calculated over the last sample period. */
        unsigned int getDemandInstantaneous();
        /*! Initializes this pulse meter. */
        void initialize();
        /*! \returns The index of the last stored demand sample.
         * The current index is where the next demand sample will be stored. */
        unsigned int getLastSampleIndex();
        /*! \returns The heat usage in the last given number of seconds.
         * Since the usage might not be stored exactly on the given number of seconds,
         * this function also adjusts the given seconds to equal the span actually analyzed. 
         * \param seconds A pointer to the number of seconds to calculate the usage since. */
        unsigned int getHeatUsageInTheLast(unsigned int* seconds);
        /*! \returns The number of pulses recorded in the last given number of seconds. 
         * Since the pulses might not be stored exactly on the given number of seconds,
         * this function also adjusts the given seconds to equal the span actually analyzed.
         * \param seconds A pointer to the number of seconds to calculate the pulses since. */
        unsigned int getPulsesInTheLast(unsigned int* seconds);
        /*! Sets this meter's estimated sample period. 
         * \param value The desired value for the estimated sample period. */
        void setEstimatedSamplePeriod(unsigned int value);
        /*! Calculates how much electric heat can be turned on and still maintain the demand limit.
         * \returns The permitted amount of demand(watts). Loads totalling this demand can be activated without 
         * exceeding the demand limit. */
        unsigned int allowedDemand();
        /*! Contains an error message which provides additional information if a function fails. */
        string errorMessage;
    };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class SCPulseMeters: public std::vector<SCPulseMeter*> {
    private:
        /*! The version of the file holding the pulse meter's configuration. */
        unsigned int version_;
        /*! The version of the file holding the demand history. */
        unsigned int versionDemandHistory_;
        /*! The location of the file holding the pulse meter's configuration. */
        string filePath_;
        /*! Contains an error message which provides additional information if a function fails. */
        string errorMessage_;
        /*! The name of the file used to store this meter's data. */
        string fileName_;
        /*! The stream to the demand history file. */
        /*! Contains The storage interval in minutes for the Demand Data.
         * The interval must go evenly into 60. (Ex: 1, 2, 3, 4, 5, 6, 10). */
        unsigned int demandHistoryStorageInterval_;
        
    public:
        /*! The default constructor. */
        SCPulseMeters();
        /*! The default destructor. */
        virtual ~SCPulseMeters();
        /*! True if this pulse meter's data has been loaded. */
        bool loaded;
        /*! \returns This pulse meter's version. */
        int getVersion();
        /*! The name of the object that created this pulse meter. */
        string owner;
        /*! \returns The path to the file containing this pulse meter's data. */
        string getFilePath(void);
        /*! Sets the path to the file containing this pulse meter's data.
         * \param strPath The path to this pulse meter's data file. */
        void setFilePath(string strPath);
        /*! \returns An error message which provides additional information for a failing function. */
        string getErrorMessage();
        /*! \returns The storage interval in minutes for the Demand. */
        unsigned int getDemandHistoryStorageInterval();
        /*! Used to set storage interval in minutes for the Demand Data.
         * The interval must go evenly into 60. (Ex: 1, 2, 3, 4, 5, 6, 10).
         * \param value The number of minutes used for the deamnd data storage interval. */
        void setDemandHistoryStorageInterval(unsigned int value);
        /*! Load all pulse meter configurations from SCConfig.db.
         * \returns True if each pulse meter's configuration was loaded successfully. */
        bool loadFromDB();
        /*! Stores the demand data for each meter at the present time to SC_dmd.db.
         * \returns True If there was no problem transfering data to SC_dmd.db. */
        bool storeDemandToDB();
        /*! Adds the given pulse meter to the vector of pulse meter's.
         * \param meter* A pointer to the pulse meter to be added. */
        void addMeter(SCClasses::SCPulseMeter* meter);
        /*! Removes the given pulse meter from the vector of pulse meters.
         * \returns true If the meter was successfully removed.
         * \param nDex The number of the pulse meter to remove(zero based).*/
        bool removeMeter(unsigned int nDex);
    };

}

#endif
