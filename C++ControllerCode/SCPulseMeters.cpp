//
//  SCPulseMeters.cpp
//  PlantManager
//
//  Created by Mark Davis on 10/24/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#include "SCPulseMeters.h"
#include <algorithm>

namespace SCClasses {

    SCDemandLimit::SCDemandLimit() {
        limit_ = 9999000;       // The default demand limit is so high it effectively does not exist.
        startMinute_ = 1440;    // The default start time is set to disable this limit.
    }

    unsigned int SCDemandLimit::getLimit() {
        return limit_;          // Provide the caller with the limit.
    }
    
    void SCDemandLimit::setLimit(unsigned int value) {
        limit_ = value;         // There are no restraints on the limit. Just set the limit to the value.
    }
    
    unsigned int SCDemandLimit::getStartMinute() {
        return startMinute_;    // Provide the caller with this limits start minute of the day.
    }
    
    void SCDemandLimit::setStartMinute(int value) {
        if (value > 1440 || value < 0) {     // Is the desired start minute legal(0-1440)?
            value = 1440;       // No. Set the start minute to a value that disables this limit.
        }                       // The desired start value is legal.
        startMinute_ = value;   // Set the start minute.
    }

#pragma mark - SCDemandLimit class implementation
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SCDemandSample::SCDemandSample() {
        nPulses_ = 0;           // The default sample has no pulses,
        usage_ = 0;             // no uage,
        time_ = 0;              // and a sample time of midnight.
    }

    unsigned int SCDemandSample::getPulses() {
        return nPulses_;            // Provide this sample's number of pulses.
    }
    
    void SCDemandSample::setPulses(unsigned int value) {
        nPulses_ = value;           // There is no limit to the number of pulses.
    }

    unsigned int SCDemandSample::getUsage() {
        return usage_;              // Provide this sample's usage in watt-hours.
    }
    
    void SCDemandSample::setUsage(unsigned int value) {
        usage_ = value;             // There are no limits to the usage.
    }

    unsigned int SCDemandSample::getTime() {
        return time_;               // Provide this sample's time(second of the day).
    }

    bool SCDemandSample::setTime(unsigned int value) {
        // There are 60 * 60 * 24 = 86,400 seconds in a day.
        // The first second of the day is 0.
        // The last second of the day is 86,399.
        if (value > 86399) {        // Is this an illegal time?
            time_ = 86399;          // Yes. Set the desired value to the last second of the day.
            return false;           // Teh desired time was illegal.
        }                           // The desired time is valid.
        time_ = value;              // Set this sample's time.
        return true;                // Let the caller know the time was legal.
    }



#pragma mark - SCDemandSample class implementation
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
    SCPulseMeter::SCPulseMeter() {
        nPulses_ = 0;               // The previous hardware reading will need to be set later.
        usageHeat_ = 0;             // The total usage of all relays on this meter will need to be set later.
//        nDemandLimits_ = 0;         // There are no demand limit periods until they are set up.
        demandPeriod_ = 900;        // The normal demand period is 15 minutes(900 seconds).
        index_ = 0;                 // This needs to be set after the pulse meter is instantiated.
        pulseConstant_ = 10;        // One is a very small value(basically there is no meter). This should be read from config.
                                    // DO NOT!!!! set the pulse constant to zero.
        nDemandSamples_ = 128;      // The logic of 128 is: the fastest Plant Manager can read pulses is 4 times a minute.
                                    // Up to thirty minutes of data might be required, therefore 4 * 30 = 120 which is almost 128.
        currentDemandSampleNo_ = 0; // There are no samples yet. The first one goes at index zero.
        estimatedDemandSamplePeriod_ = 15;// The estimated seconds between times when the demand is evaluated.
                                    // This period cannot be larger than 600 
        subSystemIndex_ = 0;        // Assumes this meter is on the first sub-system. This should be read from config.
        pulseCounterAddress_ = 9999;// This is an invalid address. This should be read from config.
        configureDemandSamples();   // Setup the array of demand samples. Assume the worst case previous power usage.
        estimatedDemandCtrlPeriod_ = 60;// The esitimated period over which demand control decisions are made.
        errorMessage = "";          // No error message until there is an error.
    }
    
    SCPulseMeter::~SCPulseMeter() {
        for(unsigned int n = 0; n < demandSamples_.size() ; n++){	// Go through each demand sample.
            delete demandSamples_.at(n);                            // Free the memory allocated to it.
        }                                                           // Memory allocated to demand samples has been freed.
        demandSamples_.clear();                                     // Clear the demand sample list of all samples.
        for(unsigned int n = 0; n < demandLimits_.size() ; n++){	// Go through each demand limit.
            delete demandLimits_.at(n);                             // Free the memory allocated to it.
        }                                                           // Memory allocated to demand limits has been freed.
        demandLimits_.clear();                                     // Clear the demand limit list of all limits.
    }
    
    void SCPulseMeter::initialize() {
/*
        if (demandLimits_.size() == 0) {                    // Has the demand limit list been created yet?
            for (int n = 0; n < nDemandLimits_; n++) {      // No. For each demand limit.
                SCDemandLimit* dl = new SCDemandLimit();    // Allocate space for a demand limit.
                demandLimits_.push_back(dl);                // Add the limit to the list.
            }                                               // There is a place for each limit on the list.
        }                                                   // The demand limit list exists.
 */
        configureDemandSamples();                           // Create the demand samples and initialize them for worst case outage.
    }
    
    unsigned int SCPulseMeter::getPulses() {
        return nPulses_;                        // Provide the caller with the number of pulses.
    }

    void SCPulseMeter::setPulses(unsigned int value) {
        nPulses_ = value;                       // Set the number of pulses. All values are legal.
    }
    
    unsigned int SCPulseMeter::getUsageHeat() {
        return usageHeat_;                      // Provide the caller with the total usage of all controlled loads.
    }
    
    void SCPulseMeter::setUsageHeat(unsigned int value) {
        usageHeat_ = value;                     // Set the usage for the electric heat. All values are legal.
    }
    
    bool SCPulseMeter::addDemandLimit(unsigned int limit, unsigned int startMinute) {
        SCDemandLimit* dLimit = new(std::nothrow) SCDemandLimit();// Create a demand limit.
        if(dLimit == nullptr) {                 // Was memory allocated for a new demand limit?
            return false;                       // No. Let the caller know the demand limit cannot be added.
        }                                       // The new demand limit was created.
        dLimit->setLimit(limit);                // Set the limit in watts.
        dLimit->setStartMinute(startMinute);    // Set the time when this limit begins.
        demandLimits_.push_back(dLimit);        // Add the limit to the list of limits.
        return true;
    }

    bool SCPulseMeter::setDemandLimit(unsigned int index, unsigned int limit, unsigned int startMinute) {
        if (index >= demandLimits_.size()) {    // Is the index that of an existing demand limit?
            return false;                       // No, let the caller know.
        }                                       // The index is legal.
        demandLimits_.at(index)->setLimit(limit);               // Set the new limit.
        demandLimits_.at(index)->setStartMinute(startMinute);   // Set the new start minute.
        return true;                            // The index is legal. Something was set. Return success.
    }
    
    bool SCPulseMeter::removeDemandLimit(unsigned int index) {
        if (index >= demandLimits_.size()) {            // Is the index that of an existing demand limit?
            return false;                               // No, let the caller know.
        }                                               // The index is legal.
        delete getDemandLimit(index);                   // Free the memory allocated to this demand limit.
        demandLimits_.erase(demandLimits_.begin() + index);// Remove the limit from the vector of limits.
        return true;                                    // The demand limit was removed.
    }
    
    SCDemandLimit* SCPulseMeter::getDemandLimit(unsigned int index) {
        if (index >= demandLimits_.size()) {            // Is the index that of an existing demand limit?
            return nullptr;                     // No, let the caller know.
        }                                       // The index is legal.
        return demandLimits_.at(index);
    }

    unsigned int SCPulseMeter::getDemandLimit() {
        if (demandLimits_.size() == 0) {                    // Are there any demand limits?
            return 9888000;                                 // Set the limit to 9,888 kW (effectively no limit).
        }                                                   // There is at least one demand limit.
        if (demandLimits_.size() == 1) {                    // Is there only a single limit?
            return demandLimits_.at(0)->getLimit();         // Yes. Provide the caller the limit which aplies all day.
        }                                                   // There are at least two limits.
        vector<SCDemandLimit*> demandLimitsSorted = demandLimits_;// Prepare to sort the demand limits by start time.
        std::sort(demandLimitsSorted.begin(), demandLimitsSorted.end(), compareLimitTimes);// Sort the demand limits by time.
        // Set the starting demad limit to the last one in the list. This is the limit we start a day with.
        unsigned int demandLimitNow = demandLimitsSorted.at(demandLimitsSorted.size() - 1)->getLimit();
        time_t tNow;                                        // Will contain the time now.
        tm* tInfo;                                          // Will contain details about the time now.
        time(&tNow);                                        // Get the current time.
        tInfo = localtime(&tNow);                           // Convert the time now to get information about it.
        unsigned int minuteNow = tInfo->tm_min + tInfo->tm_hour * 60;// Calculate the current minute of the day.
        for (unsigned int index = 0; index < demandLimitsSorted.size(); index++) {// Go through each limit.
            if (minuteNow > demandLimitsSorted.at(index)->getStartMinute()) {// Is the time now after this limit began?
                                                            // Yes. Update the limit now to show we have gone past
                demandLimitNow = demandLimitsSorted.at(index)->getLimit();// its start minute.
            }                                               // The limit has been updated.
            else {                                          // This limit's start minute has not been exceeded.
                break;                                      // No need to change the demand limit any more.
            }                                               // Done looking for the current demand limit.
        }                                                   // The correct demand limit has been set.
        return demandLimitNow;                              // Provide the caller with the demand limit in watts for right now.
    }
    
    bool SCPulseMeter::compareLimitTimes(SCDemandLimit* limit1, SCDemandLimit* limit2) {
        if (limit1->getStartMinute() < limit2->getStartMinute()) {// Is the first start minute bofre the second?
            return true;                                    // Yes.
        }                                                   //
        return false;                                       // The second start minute was before the first.
    }

    unsigned int SCPulseMeter::getDemandPeriod() {
        return  demandPeriod_;                              // Provide the caller with the demand period in seconds.
    }

    void SCPulseMeter::setDemandPeriod(unsigned int value) {
        if (value > 43200) {                                // Is the caller trying to set the demand periond to over 12 hours?
            value = 43200;                                  // Yes. Limit the demand period to 12 hours.
        }                                                   // The demand period is not more than 12 hours.
        demandPeriod_ = value;                              // Set the demand period (in seconds).
    }

    unsigned int SCPulseMeter::getIndex() {
        return index_;                                      // Provide the caller with this meter's index.
    }
    
    void SCPulseMeter::setIndex(unsigned int value) {
        index_ = value;                                     // There is no error checking.
    }

    unsigned int SCPulseMeter::getPulseConstant() {
        return pulseConstant_;                              // Provide the caller with this meter's pulse constant(watt-hrs/pulse).
    }

    void SCPulseMeter::setPulseConstant(unsigned int value) {
        pulseConstant_ = value;                             // Set the pulse constant in watt-hrs per pulse (no limits).
    }

    void SCPulseMeter::configureDemandSamples() {
        if (demandSamples_.size() > 0) {                    // Have the demand samples already been configured?
            for(unsigned int n = 0; n < demandSamples_.size() ; n++){	// Yes. Go through each demand sample.
                delete demandSamples_.at(n);                // Free the memory allocated to it.
            }                                               // Memory allocated to demand samples has been freed.
            demandSamples_.clear();                         // Clear the demand sample list of all samples.
        }                                                   // The demand sample array can now be re-initialized.
        time_t tNow;                                        // Will contain the time now.
        tm* tInfo;                                          // Will contain details about the time now.
        time(&tNow);                                        // Get the current time.
        tInfo = localtime(&tNow);                           // Convert the time now to get information about it.
        unsigned int secondNow = tInfo->tm_sec + tInfo->tm_min * 60 + tInfo->tm_hour * 3600;// Calculate the second of the day.
        unsigned int largestLimit = 0;                      // Will contain the largest limit, but starts as zero.
        if (demandLimits_.size() > 0) {                     // Does the array of limits exist?
            for (int i = 0; i < demandLimits_.size(); i++) {// Yes. Go through each limit.
                if (demandLimits_.at(i)->getLimit() > largestLimit) {// Is this limit larger than any of the previous limits.
                    largestLimit = demandLimits_.at(i)->getLimit();// Save it as the largest limit.
                }                                           // The largest limit has been updated if necessary.
            }                                               // The limit array has been gone through.
        }                                                   // The largest limit has been determined fromthe demand limit array.
        else {                                              // There was no demand limit array.
            largestLimit = 999000;                          // Set the largest demand limit to 999KW.
        }                                                   // The largest demand limit has been set.
        // Calculate the pulses per period as largestLimit in watt-hrs / 60 min / 4 samples per min / 10 watt-hrs per pulse.
        unsigned int pulsesPerPeriod = largestLimit / 60 / 4 / pulseConstant_;// Wosrt case number of pulses in a period.
        unsigned int usage = pulseConstant_ * pulsesPerPeriod;// Used as a worst case electric usage for a sample.
        for(unsigned int n = 0; n < nDemandSamples_ ; n++){	// Go through each demand sample.
            SCDemandSample* aSample = new SCDemandSample(); // Create a new default demand sample.
            aSample->setPulses(pulsesPerPeriod);            // Set the worst case number of pulses for this sample.
            aSample->setUsage(usage);                       // Set the worst case electric heat usage for this sample.
            // Calculate the second of the day for each sample.
            // The first sample is the oldest.
            // The first sample should be the current second of the day - 128 * 15sec back.
            // While calculating, one must check for going back to the previous day.
            // The second of the day cannot be negative. In order to prevent this, two days worth of seconds are added to
            // the calculated second. Also, the second is defined as an (int) rather than an (unsigned int). Later the
            // calculated second will be converted to the second of the day by taking its modulus 86,400 (86,400 is the number
            // of seconds in a day).
            int sec = secondNow + (n - 127) * estimatedDemandSamplePeriod_ + 86400 * 2;
            if (sec < 0) {                  // Is the second positive? Prevent something really wild from crashing the function.
                if (n == 0) {                               // Is this the first demand sample?
                    sec = 0;                                // Yes. Set the sample time to the start of the day.
                } else {                                    // Wasn't the first sample.
                    sec += demandSamples_.at(n - 1)->getTime() + 45;// Set the sample time to 45 sesconds after the previous time.
                }                                           // The sample second is set.
            }                                               // The calculated sample second is positive.
            unsigned int secondAtSample;                    // Will contain the second at the sample.
            secondAtSample = sec % 86400;                   // Convert to the second of the day the sample was taken.
            aSample->setTime(secondAtSample);               // Set sample time. Previous line makes it impossible to have an error.
            demandSamples_.push_back(aSample);              // Put the sample in the list of samples.
        }                                                   // The demand sample list has been built.
        currentDemandSampleNo_ = nDemandSamples_ - 1;       // The current demand sample is the last one.
    }
    
    unsigned int SCPulseMeter::getNumberOfDemandSamples() {
        return nDemandSamples_;                             // Provide the caller with the number of demand samples.
    }

    void SCPulseMeter::setNumberOfDemandSamples(unsigned int value) {
        nDemandSamples_ = value;                            // Set the potential number of demand sasmples (no limit).
    }

    SCDemandSample* SCPulseMeter::getDemandSample(unsigned int sampleNo) {
        if (sampleNo >= nDemandSamples_) {                  // Is the index legal?
            return nullptr;                                 // There is no demand sample to return.
        }                                                   // The sample number is legal.
        return demandSamples_.at(sampleNo);                 // Provide the caller with the demand sample.
    }
    
    bool SCPulseMeter::setDemandSample(unsigned int index, unsigned int nPulses, unsigned int usage, unsigned int time) {
        if (index >= nDemandSamples_) {                     // Is the index legal?
            return false;                                   // No, let the caller know the demand sample could not be set.
        }                                                   // The sample number is legal.
        demandSamples_.at(index)->setPulses(nPulses);       // Set the number of pulses.
        demandSamples_.at(index)->setUsage(usage);          // Set this sample's usage.
        if(demandSamples_.at(index)->setTime(time) == false) {// Set this sample's time.
            return false;                                   // Let the caller know the demand sample could not be set.
        }                                                   // The demand sample was set.
        return true;                                        // Let the caller know the demand sample was set.
    }

    unsigned int SCPulseMeter::getCurrentDemandSampleNo() {
        return currentDemandSampleNo_;                      // Provide the caller with the current demand sample number.
    }

    unsigned int SCPulseMeter::incrementCurrentDemandSampleNo() {
        currentDemandSampleNo_++;                       // Increment the current demand sample number.
        if (currentDemandSampleNo_ == nDemandSamples_) {// Did the new current demand sample number exceed the number of samples?
            currentDemandSampleNo_ = 0;                 // Set the current demand sample number back to zero.
        }                                               // The demand sample array acts as a circular buffer.
        return currentDemandSampleNo_;                  // Provide the caller with the new current demand sample number.
    }

    unsigned int SCPulseMeter::getDemandSampleTime(unsigned int index) {
        if (index >= nDemandSamples_) {                     // Is the index legal?
            return 86400;                                   // This is an illegal time.
        }                                                   // The sample number is legal.
        return demandSamples_.at(index)->getTime();         // Provide the caller the time for the given demand sample.
    }
    
    unsigned int SCPulseMeter::getDemandSamplePulses(unsigned int index) {
        if (index >= nDemandSamples_) {                     // Is the index legal?
            return 99999;                                   // This is an unrealistically high number of pulses.
        }                                                   // The sample number is legal.
        return demandSamples_.at(index)->getPulses();       // Provide the caller the number of pulses for the given demand sample.
    }
    
    unsigned int SCPulseMeter::getDemandSampleUsage(unsigned int index) {
        if (index >= nDemandSamples_) {                     // Is the index legal?
            return 99999;                                   // This is an unrealistically high number of watt-hrs to use.
        }                                                   // The sample number is legal.
        return demandSamples_.at(index)->getUsage();        // Provide the caller the usage for the given demand sample.
    }
 
    unsigned int SCPulseMeter::getSubSystemIndex() {
        return subSystemIndex_;                         // Provide the caller with the index of the sub-system associated with it.
    }
    
    void SCPulseMeter::setSubSystemIndex(unsigned int value) {
        subSystemIndex_ = value;                        // Sets this meter's sub-system index. Don't know how to check bounds
    }                                                   // on the sub-system index.
    
    unsigned int SCPulseMeter::getPulseCounterAddress() {
        return pulseCounterAddress_;                    // Provide the caller with the pulse counter address.
    }
    
    void SCPulseMeter::setPulseCounterAddress(unsigned int value) {
        pulseCounterAddress_ = value;                   // Sets this meter's pulse counter address. Don't know how to check
    }                                                   // the validity of value.

    unsigned int SCPulseMeter::prevoiusIndex(unsigned int value) {
        unsigned int index = value;                     // Will contain the previous index. The given value is off by one
                                                        // but might have to be wrapped.
        if (value >= nDemandSamples_) {                 // Is the given index illegal?
            return 0;                                   // Yes. Retrun the first index.
        }                                               // The given index is legal.
        if (index == 0) {                               // Adjust for wrapping?
            index = nDemandSamples_;                    // Yes. The last index is the largest allowed.
        }                                               // The index is set such that one can be subtracted from it.
        index--;                                        // The index is zero based.
        return index;                                   // Provide the caller with the idex of the last stored demand sample.
        
    }

    unsigned int SCPulseMeter::getLastSampleIndex() {
        return prevoiusIndex(currentDemandSampleNo_);   // The current demand sample number is not the last demand sample
                                                        // index but is only off by one or a wrap.
    }
    
    unsigned int SCPulseMeter::getPulsesInTheLast(unsigned int* seconds) {
        unsigned int pulseCount = 0;                    // Contains the calculated pulse count for the given period.
        unsigned int lastPeriodInSeconds = 0;           // Contains the number of seconds in the last sample period.
        unsigned int lastPulseCount = 0;                // Contains the number of pulses in the last sample period.
        unsigned int actualPeriodLength = 0;            // Contains the actual # of seconds over which the pulse count occurred.
        unsigned int startSampleIndex = getLastSampleIndex();// Contains the sample index of the most recently stored data.
        unsigned int index = startSampleIndex;          // The index of the sample currently being evaluated.
        unsigned int lastTime = demandSamples_.at(index)->getTime();// Contains the time of the last sample
                                                        // (second of the day, 86400 -> none).
        unsigned int prevTime = 86400;              // Contains the time of the previous sample(second of the day, 86400 -> none).
        unsigned int nSamples = 0;                      // The number of samples used to accumulate the pulse count.
        // A demand sample contains the current time, the number of pulses that occurred in the last sample period and
        // the amount of energy used by electric heat in the lasat sample period. In order to know the period's time
        // length, we must take the difference in time between this sample and the previous sample.
        do {    // Loop through the demand sample array to get the number of pulses which occurred over the given time period.
            lastPulseCount = demandSamples_.at(index)->getPulses();// Get this sample's pulse count.
            prevTime = demandSamples_.at(prevoiusIndex(index))->getTime();// Get the time the previous sample was taken.
            // Done with the index for this sample. Decrement the index so the next sample can be processed.
            if (index == 0) {                           // Is the index greater than zero? (Can it be decremented?)
                index = nDemandSamples_;                // No. The index needs to be wrapped.
            }                                           // The index can now be decremented.
            index--;                                    // Go to the previous demand sample.
            nSamples++;                                 // Increment the number of samples processed.
            if (lastTime == 86400 || prevTime == 86400) {// Has one of the times not been set?
                lastTime = prevTime;                    // Yes. We cannot process this sample. Save the next last time.
                continue;                               // Go to the next sample.
            }                                           // The last time and the previous time are both valid.
            if(lastTime > prevTime) {                   // Is the clock running forwards?
                lastPeriodInSeconds = lastTime - prevTime;// Yes. Calculate the number of seconds the last sample was taken over.
            }                                           // The sample duration has been calculated for a normal clock.
            else {                                      // The clock ran backwards.
                                                        // Use the estimated sample period
                lastPeriodInSeconds = estimatedDemandSamplePeriod_;// since the clock went backwards.
            }                                           // The sample duration has been calculated.
            actualPeriodLength += lastPeriodInSeconds;  // Increment the time the pulses were accumulated over.
            pulseCount += lastPulseCount;               // Increment the number of pulses over the desired period.
            lastTime = prevTime;                        // As we process back, the last time becomes the previous time.
        }                                               // A temporary value for the pulse count and time span are determined.
        while (actualPeriodLength < *seconds && nSamples < nDemandSamples_);// Loop until the the entire time span is processed.
                                                        // However, don't double process samples.
        if (actualPeriodLength > *seconds && nSamples > 1) {// Did the actual period exceed the desired period?
            actualPeriodLength -= lastPeriodInSeconds;  // Yes. Remove the last time.
            pulseCount -= lastPulseCount;               // Remove the last pulse count.
        }                                               // The pulse count and time period are at their final values.
        *seconds = actualPeriodLength;                  // Set the actual number of seconds in the period.
        return pulseCount;                              // Provide the caller with the number of pulses in the period.
    }

    unsigned int SCPulseMeter::getDemand() {
        unsigned int demand = 0;
        unsigned int period = demandPeriod_;
        // The demand is for the last demand period. However, we cannot control when storage occurs.
        // Thus we will ask for the pulses over the demand period but the actual period will probably be
        // something slightly different.
        unsigned int pulseCount = getPulsesInTheLast(&period);// Get pulses in the last demand period. The period is modified.
        if (period == 0) {          // Does the sample period have no length?
            return 0;               // Yes. Avoid division by zero, there is no way to calculate the demand over a 0 time period.
        }                           // The period exists (non-zero).
        demand = pulseCount * pulseConstant_;   // Convert the pulse count to watt-hours.
        demand = demand * 3600 / period;        // Convert the watt-hours to watts.
        return demand;              // Provide the caller with the plant's demand(watts).
    }
    
    unsigned int SCPulseMeter::getHeatUsageInTheLast(unsigned int* seconds) {
        unsigned int heatUsage = 0;                     // Contains the calculated electric heat usage for the given period.
        unsigned int lastPeriodInSeconds = 0;           // Contains the number of seconds in the last sample period.
        unsigned int lastUsage = 0;                     // Contains the electric heat usage in the last sample period.
        unsigned int actualPeriodLength = 0;            // Contains the actual # of seconds over which the pulse count occurred.
        unsigned int startSampleIndex = getLastSampleIndex();// Contains the sample index of the most recently stored data.
        unsigned int index = startSampleIndex;          // The index of the sample currently being evaluated.
        unsigned int lastTime = demandSamples_.at(index)->getTime();// Contains the time of the last sample
        // (second of the day, 86400 -> none).
        unsigned int prevTime = 86400;              // Contains the time of the previous sample(second of the day, 86400 -> none).
        unsigned int nSamples = 0;                      // The number of samples used to accumulate the pulse count.
        // A demand sample contains the current time, the number of pulses that occurred in the last sample period and
        // the amount of energy used by electric heat in the lasat sample period. In order to know the period's time
        // length, we must take the difference in time between this sample and the previous sample.
        do {    // Loop through the demand sample array to get the electric heat usage which occurred over the given time period.
            lastUsage = demandSamples_.at(index)->getUsage();// Get this sample's electric heat usage.
            prevTime = demandSamples_.at(prevoiusIndex(index))->getTime();// Get the time the previous sample was taken.
            // Done with the index for this sample. Decrement the index so the next sample can be processed.
            if (index == 0) {                           // Is the index greater than zero? (Can it be decremented?)
                index = nDemandSamples_;                // No. The index needs to be wrapped.
            }                                           // The index can now be decremented.
            index--;                                    // Go to the previous demand sample.
            nSamples++;                                 // Increment the number of samples processed.
            if (lastTime == 86400 || prevTime == 86400) {// Has one of the sample times not been set?
                lastTime = prevTime;                    // Yes. We cannot process this sample. Save the next last time.
                continue;                               // Go to the next sample.
            }                                           // The last sample time and the previous sample time are both valid.
            if(lastTime > prevTime) {                   // Is the clock running forwards?
                lastPeriodInSeconds = lastTime - prevTime;// Yes. Calculate the number of seconds the last sample was taken over.
            }                                           // The sample duration has been calculated for a normal clock.
            else {                                      // The clock ran backwards.
                // Use the estimated sample period
                lastPeriodInSeconds = estimatedDemandSamplePeriod_;// since the clock went backwards.
            }                                           // The sample duration has been calculated.
            actualPeriodLength += lastPeriodInSeconds;  // Increment the time the usages were accumulated over.
            heatUsage += lastUsage;                     // Increment the electric heat usage over the desired period.
            lastTime = prevTime;                        // As we process back, the last time becomes the previous time.
        }                                               // A temporary value for the usage and time span were determined.
        while (actualPeriodLength < *seconds && nSamples < nDemandSamples_);// Loop until the the entire time span is processed.
        // However, don't double process samples.
        if (actualPeriodLength > *seconds && nSamples > 1) {// Did the actual period exceed the desired period?
            actualPeriodLength -= lastPeriodInSeconds;  // Yes. Remove the last sample's period.
            heatUsage -= lastUsage;                     // Remove last sample's electric heat usage.
        }                                               // The electric heat usage and time period are at their final values.
        *seconds = actualPeriodLength;                  // Set the actual number of seconds in the period.
        return heatUsage;                               // Provide the caller with the electric heat usage(watt-hrs) in the period.
    }

    unsigned int SCPulseMeter::getDemandHeat() {
        unsigned int heatDemand = 0;
        unsigned int period = demandPeriod_;
        // The demand is for the last demand period. However, we cannot control when storage occurs.
        // Thus we will ask for the pulses over the demand period but the actual period will probably be
        // something slightly different.
        unsigned int heatUsage = getHeatUsageInTheLast(&period);// Get the usage in the last demand period. The period is modified.
        if (period == 0) {          // Does the sample period have no length?
            return 0;               // Yes. Avoid division by zero, there is no way to calculate the demand over a 0 time period.
        }                           // The period exists (non-zero).
        heatDemand = heatUsage * 3600 / period; // Convert the watt-hours to watts.
        return heatDemand;                      // Provide the caller with the electric heat's demand(watts).
    }
    
    unsigned int SCPulseMeter::getDemandHeatInstantaneous() {
        unsigned int heatUsage = 0;                     // Contains the electric heat usage for the last sample period.
        unsigned int actualPeriodLength = 0;            // Contains the actual # of seconds over which the heat usage occurred.
        unsigned int index = getLastSampleIndex();      // The index of the sample most recently taken.
        unsigned int lastTime = demandSamples_.at(index)->getTime();// Contains the time of the last sample
        // (second of the day, 86400 -> none).
        unsigned int prevTime = 86400;              // Contains the time of the previous sample(second of the day, 86400 -> none).
        heatUsage = demandSamples_.at(index)->getUsage();// Get this sample's electric heat usage.
        prevTime = demandSamples_.at(prevoiusIndex(index))->getTime();// Get the time the previous sample was taken.
        if (lastTime == 86400 || prevTime == 86400) {   // Has one of the sample times not been set?
            return 9999999;                             // Yes. Provide the caller with a very lage number to indicate a problem.
        }                                               // The last sample time and the previous sample time are both valid.
        if(lastTime > prevTime) {                       // Is the clock running forwards?
            actualPeriodLength = lastTime - prevTime;   // Yes. Calculate the number of seconds the last sample was taken over.
        }                                               // The sample duration has been calculated for a normal clock.
        else {                                          // The clock ran backwards.
            actualPeriodLength = estimatedDemandSamplePeriod_;// Use the estimated sample period since the clock went backwards.
        }                                               // The sample duration has been calculated.
        return heatUsage * 3600 / actualPeriodLength;   // Provide the caller with the instantaneous electric heat demand(watts).
    }
    
    unsigned int SCPulseMeter::getDemandInstantaneous() {
        unsigned int nPulses = 0;                       // Contains the meter's pulse count for the last sample period.
        unsigned int sampleLength = 0;                  // Contains the actual # of seconds over which the sample occurred.
        unsigned int index = getLastSampleIndex();      // The index of the sample most recently taken.
        unsigned int lastTime = demandSamples_.at(index)->getTime();// Contains the time of the last sample
        // (second of the day, 86400 -> none).
        unsigned int prevTime = 86400;              // Contains the time of the previous sample(second of the day, 86400 -> none).
        nPulses = demandSamples_.at(index)->getPulses();// Get this sample's pulse count.
        prevTime = demandSamples_.at(prevoiusIndex(index))->getTime();// Get the previous sample time.
        if (lastTime == 86400 || prevTime == 86400) {   // Has one of the sample times not been set?
            return 9999999;                             // Yes. Provide the caller with a very lage number to indicate a problem.
        }                                               // The last sample time and the previous sample time are both valid.
        if(lastTime > prevTime) {                       // Is the clock running forwards?
            sampleLength = lastTime - prevTime;         // Yes. Calculate the last sample's duration in seconds.
        }                                               // The sample duration has been calculated for a normal clock.
        else {                                          // The clock ran backwards.
            sampleLength = estimatedDemandSamplePeriod_;// Use the estimated sample period since the clock went backwards.
        }                                               // The sample duration has been calculated.
        return nPulses * pulseConstant_ * 3600 / sampleLength;// Provide the caller with the instantaneous plant demand(watts).
    }
    
    void SCPulseMeter::setEstimatedSamplePeriod(unsigned int value) {
        estimatedDemandSamplePeriod_ = value;       // There are no limits.
    }

    unsigned int SCPulseMeter::allowedDemand() {
        unsigned int permittedDemand = 0;   // The amount of electric heat allowed to still maintain the demand limit(watts).
        unsigned int heatUsage = 0;             // Energy used by the electric heating system in watt-hours.
        unsigned int heatEstimatePeriod = demandPeriod_ / 3;// Use 1/3 of the demand period to
                                                // estimate/calculate the non-controllable demand.
        unsigned int plantEstimatePeriod = heatEstimatePeriod;// Use the same period to get the plant usage.
        // The non-controllable usage is the difference plant usage and the heat usage.
        unsigned int plantUsage = 0;            // Energy used by the plant in watt-hours.
        unsigned int plantDemand = 0;           // The demand of the plant in watts.
        unsigned int heatDemand = 0;            // The demand of the electric heat in watts.
        unsigned int nonControllableLoad = 0;   // The non-controllable demand in watts.
        unsigned int pulsePeriod = 0;           // The period in seconds between the demandPeriod_ and the Demand Control Period.
        unsigned int plantPulses = 0;           // The number of pulses that occured in the last pulsePeriod seconds.
        unsigned int allowableEnergyUsage = 0;  // The number of watt-hours allowed when operating at the present demand limit.
        
        // In a demand period (typically 15 minutes, 900 secs) the amount of energy which
        // can be used by the plant is the current demand limit * (demandPeriod_ / 3600) (assuming DemandPeriod
        // is measured in seconds). Assume the plant used EnergyUsed KWH in the previous demandPeriod_
        // - 60 seconds.  The available energy to use in the next 60 seconds is
        // CurrentDemandLimit * (demandPeriod_ / 3600) - EnergyUsed. The available demand is
        // (CurrentDemandLimit * (demandPeriod_ / 3600) - EnergyUsed) * (3600 / 60) - Estimated non-controlled load.
        
        // Esitmate the non-controlled demand.
        heatUsage = getHeatUsageInTheLast(&heatEstimatePeriod); // Get the heat usage during the estimate period.
                                                                // The heatEstimatePeriod will be modified.
        plantUsage = getPulsesInTheLast(&plantEstimatePeriod);  // Get the pulse count during the estimate period.
                                                                // The plantEstimatePeriod will be modified.
        if (heatEstimatePeriod == 0 || plantEstimatePeriod == 0) { // Is there danger of division by zero?
            return getDemandLimit() / 3;            // Yes. Only allow 1/3 of the limit since we do not have a good estimate.
        }
        plantUsage = plantUsage * pulseConstant_;   // plantUsage now contains the usage in watt-hours.
        plantDemand = plantUsage * 3600 / plantEstimatePeriod;  // plantDemand = watt-hours * 3600 secs/hour / seconds = watts.
                                                    // As plantEstimate period approaches 3600, the demand goes to the usage.
        heatDemand = heatUsage * 3600 / heatEstimatePeriod;     // heatDemand is calculated just like plantDemand.
        // Determine non-controlled load.
        if (heatDemand > plantDemand) {             // Does heat demand exceed plant demand?
            nonControllableLoad = plantDemand / 5;  // Yes. There is very little non-controllable load.
                                                    // The heatDemand can exceed the plantDemand because of synchronization.
        }                                           // The non-controllable load has been estimated.
        else {                                      // Calculate non-controlled load.
            nonControllableLoad = plantDemand - heatDemand;// Everything that isn't heatDemand is non-controllable.
        }                                           // The non-controllable load has been determined.
        // Determine the energy used by the plant in the last DemandPeriod - _DemandCtrlPeriodEstimate seconds.
        // (typically the last 14 minutes)
        pulsePeriod = demandPeriod_ - estimatedDemandCtrlPeriod_;// The demand period minus 60 seconds.
        plantPulses = getPulsesInTheLast(&pulsePeriod);// The number of pulses that have occurred for the next ctrl period already.
        plantUsage = plantPulses * pulseConstant_;  // The energy used in watt-hours during the last pulsePeriod seconds.
        // Finish figuring the allowable demand from the above equation.
        allowableEnergyUsage = getDemandLimit() * demandPeriod_ / 3640;
        // The allowable energy usage is calculated using 3640 instead of 3600 thus reducing the allowed usage by .99.
        // This is done to be conservative.
        if (allowableEnergyUsage < plantUsage) {    // Is the plant already using more than it is allowed?
            return 0;                           // Yes, the demand limit has been exceeded. There is no more demand available.
        }                                           // There is still some energy available to use in the next 60 seconds.
        allowableEnergyUsage -= plantUsage;     // This is the amount of energy that can be used in the next demand control period.
        permittedDemand = allowableEnergyUsage * 3600 / estimatedDemandCtrlPeriod_;// Convert the allowable energy usage
                                                    // to an allowable demand.
        if (permittedDemand < nonControllableLoad) {// Is the allowablle demand less than the uncontrollable load?
            return 0;                           // Yes. There is no availablle demand for electric heat.
        }                                       // It is all being used by the uncontrollable load.
        permittedDemand -= nonControllableLoad; // Reduce the demand allowed for electric heat by
                                                // the amount being consummed by the uncontrollable load.
        return permittedDemand;                 // Provide the caller with the allowed demand in watts.
    }



    
#pragma mark - SCPulseMeter class implementation
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    SCPulseMeters::SCPulseMeters() {
        filePath_ = "";
        version_ = 1;                               // Any version prior to 1 is MFC or .NET.
        fileName_ = "PulseMeters.xml";              // The default name for the file containing pulse meter configurations.
        demandHistoryStorageInterval_ = 6;          // The default period for demand storage is once every six minutes.
        versionDemandHistory_ = 2;                  // Demand history version on 4/24/2015.
    }

    SCPulseMeters::~SCPulseMeters() {
        for(unsigned int i = 0; i < this->size(); i++){	// Go through each pulse meter.
            delete this->at(i);						// Free the memory allocated to it.
        }                                           //
        this->clear();								// Clear all the array of all pulse meters.
        loaded = false;                             // The configurations for the pulse meters have not been loaded.
    }

    void SCPulseMeters::addMeter(SCClasses::SCPulseMeter* meter) {
        unsigned int meterNo = meter->getIndex();   // Get this meter's desired position.
        if(meterNo >= this->size()) {               // Is the desired position past the end?
            meterNo = (unsigned int)this->size();   // Yes. Put the meter at the end of the vector.
        }                                           // The meter number is legal.
        meter->setIndex(meterNo);                   // Reset this meter's index.
        this->insert(this->begin() + meterNo, meter);// Insert the new meter into the vector.
        for(unsigned int j = meterNo + 1; j < this->size(); j++) {// Adjust each of the following
            this->at(j)->setIndex(j);               // meter numbers.
        }                                           // Done adjusting subsequent meter numbers.
    }

bool SCPulseMeters::loadFromDB(){
    sqlite3 *db;                            // Create a location for the database.
    int  rc;                                // Create a return variable.
    string sql;                             // Create a string for SQL commands.
    string dbName = filePath_ + "SCConfig.db";// Set the configuration database name.
                                            // Open the database named SC_Config.db.
    rc = sqlite3_open(dbName.c_str(), &db); // It has a table which contains the pulse meter configurations (pulseMeters).
    if( rc ){                               // Was the database opened?
        errorMessage_ = "Can't open SCConfig.db to read pulse meter configurations: ";// No. Create a message.
        errorMessage_ += string(sqlite3_errmsg(db));// Add the database error to the message.
        cout << errorMessage_ << endl;      // Display the error.
        return false;                       // Let the caller know the maturity was not read.
    }                                       // The database is open.
    sqlite3_busy_timeout(db, 50);           // If the DB is locked wait up to 50 milliseconds before giving up.
    string tableName = "pulseMeters";       // Create the table name for the pulse meters.
    sql = "SELECT * from " + tableName;     // Create an SQL statement to get all the pulse meter configurations.
    sqlite3_stmt *stmt;                     // Will contain the results of the querry.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
    if (rc != SQLITE_OK) {                  // Was it successful?
        errorMessage_ = "Failed to query 'pulseMeters' for configurations: ";// No, there was a problem. Create an error message.
        errorMessage_ += string(sqlite3_errmsg(db));// Add the database error to the message.
        cout << errorMessage_ << endl;      // Display the error.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
        return false;                       // Let the caller know the pulse meter configurations couldn't be retrieved from the database.
    }                                       // The pulse meter configurations were retreived.
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {// Go through each row in the results.
        int id = sqlite3_column_int(stmt, 0);// The id is in the first column.
        SCPulseMeter* meter = new(std::nothrow) SCPulseMeter();// Create a pulse meter.
        if (meter == nullptr) {             // Was the space allocated for the pulse meter?
            errorMessage_ = "Couldn't add allocate memry for meter-" + to_string(id) + ".";// No.
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
            sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
            delete this;                    // Delete any meters which may have been created.
            return false;                   // Let the caller know there was an error.
        }                                   // Memory was allocated for this meter.
        try {
            meter->initialize();            // Try to initialize the new pulse meter.
        } catch(string ex) {                // There was an error.
            errorMessage_ = "Couldn't initialize meter-" + to_string(id) + ": " + ex;// Create an error message.
            cout << errorMessage_ << endl;  // Display the error.
            sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
            sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
            delete meter;                   // Free the meter since it never became part of the meters.
            delete this;                    // Delete any meters which may have been created.
            return false;                   // Let the caller know there was an error.
        }
        catch(...) {
            errorMessage_ = "Couldn't initialize meter-" + to_string(id) + ": Unknown reason!";// Create an error message.
            cout << errorMessage_ << endl;  // Display the error.
            sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
            sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
            delete meter;                   // Free the meter since it never became part of the meters.
            delete this;                    // Delete any meters which may have been created.
            return false;                   // Let the caller know there was an error.
        }
        for (int limitDex = 0; limitDex < 4; limitDex++) {
            int time = sqlite3_column_int(stmt, 1 + limitDex * 2);// Get the minute of the day this limit starts.
            int limit = sqlite3_column_int(stmt, 2 +  + limitDex * 2);// Get the limit for this period.
            if(meter->addDemandLimit(limit, time) == false) {// Could the demand limit be added to the list of demand limits?
                errorMessage_ = "Couldn't add Demand limit " + to_string(limitDex) + " to meter-" + to_string(id) + ".";// No.
                cout << errorMessage_ << endl;// Display the error.
                sqlite3_finalize(stmt);     // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
                sqlite3_close(db);          // Close SCConfig.db. Since finalize was just called there should be no errors.
                delete meter;               // Free the meter since it never became part of the meters.
                delete this;                // Delete any meters which may have been created.
                return false;               // Let the caller know there was an error.
            }                               // This demand limit was added to the list.
        }                                   // All the demands limits have been added.
        int period = sqlite3_column_int(stmt, 9);// The demand period (in seconds) is in the table's 10th column.
        int eSP = sqlite3_column_int(stmt, 10);// The estimated sample period is in the table's 11th column.
        int nDS = sqlite3_column_int(stmt, 11);// The number of demand samples is in the table's 12th column.
        int pConstant = sqlite3_column_int(stmt, 12);// The pulse constant is in the table's 13th column.
        int add = sqlite3_column_int(stmt, 13);// The pulse counter's address is in the table's 14th column.
        int sSDex = sqlite3_column_int(stmt, 14);// The sub-system index for this pulse counter is in the table's 15th column.
        meter->setIndex(id);                // Set the meter's index.
        meter->setDemandPeriod(period);     // Set this meter's demand period in seconds.
        meter->setEstimatedSamplePeriod(eSP);// Set this meter's estimated sample period in seconds.
        meter->setNumberOfDemandSamples(nDS);// Set this meter's number of samples.
        meter->setPulseConstant(pConstant); // Set this meter's pulse constant.
        meter->setPulseCounterAddress(add); // Set this meter's address.
        meter->setSubSystemIndex(sSDex);    // Set this meter's sub-system index.
        try {
            meter->configureDemandSamples();// Configure the demand samples.
        } catch(string ex) {
            errorMessage_ = "Couldn't configure demand samples for meter-" + to_string(id) + ": " + ex;// No.
            cout << errorMessage_ << endl;  // Display the error.
            sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
            sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
            delete meter;                   // Free the meter since it never became part of the meters.
            delete this;                    // Delete any meters which may have been created.
            return false;                   // Let the caller know there was an error.
        }
        catch(...) {
            errorMessage_ = "Couldn't configure demand samples for meter-" + to_string(id) + ": Unknown reason!";// No.
            cout << errorMessage_ << endl;  // Display the error.
            sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
            sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
            delete meter;                   // Free the meter since it never became part of the meters.
            delete this;                    // Delete any meters which may have been created.
            return false;                   // Let the caller know there was an error.
        }
        addMeter(meter);                    // Add the meter to the vector of meters.
    }                                       // The maturity has been read for each channel
    if (rc != SQLITE_DONE) {                // Was there an error going through the SCChnls in the DB?
        errorMessage_ = "Error loading pulse meter configurations: " + string(sqlite3_errmsg(db));// Yes. Create an error message.
        cout << errorMessage_ << endl;  // Display the error.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
        delete this;                        // Delete any meters which may have been created.
        return false;                       // Return failure.
    }                                       // The each pulse meter's configuration has been successfully read.
    sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
    sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
    return true;                            // The pulse meter configurations have been loaded successfully.
}

string SCPulseMeters::getErrorMessage() {
    return errorMessage_;                       // Provide the caller with the current error message.
}

string SCPulseMeters::getFilePath() {
    return filePath_;                           // Provide the caller with the file path.
}
    
void SCPulseMeters::setFilePath(string strPath) {
    filePath_ = strPath;                        // Set the file path.
}


unsigned int SCPulseMeters::getDemandHistoryStorageInterval() {
    return demandHistoryStorageInterval_;       // Provide the caller with the demand history storage interval.
}
    
void SCPulseMeters::setDemandHistoryStorageInterval(unsigned int value) {
    errorMessage_ = "";                                     // Clear the error message.
    if (value == 1 || value == 2 || value == 3 || value == 4 || value == 5 || value == 6 || value == 10 ||
        value == 12 || value == 15 || value == 20 || value == 60) {// Does the value go into 60 evenly?
        demandHistoryStorageInterval_ = value;              // Yes. Set the storage interval.
    } else {                                                // No.
        errorMessage_ = "Error: illegal value. The value must go into 60 evenly.";// Set the error message.
    }                                                       // Done.
}

   
int SCPulseMeters::getVersion() {
    return version_;                            // Provide the caller with the configuration version.
}

bool SCPulseMeters::removeMeter(unsigned int nDex) {
    if (nDex >= size()) {                       // Is the index too large?
        errorMessage_ = "The index " + to_string(nDex) + ", is greater than the largest index " + to_string(size() - 1) + ".";
        return false;                           // The index doesn't reference anything that can be removed.
    }                                           // The index is valid.
    delete this->at(nDex);                      // Free the memory allocated to the pulse meter.
    this->erase(this->begin() + nDex);			// Remove the pulse meter from the list.
    errorMessage_ = "";                         // Reset the error message.
    return true;                                // Let the caller know the pulse meter was removed.
}

bool SCPulseMeters::storeDemandToDB() {
    if(this->size() == 0) {                 // Are there any pulse meters?
        return true;                        // No, there is no demand to store.
    } // End of condition no pulse meters.
    sqlite3 *db;                            // Create a location for the database.
    char* zErrMsg = 0;                      // Contains an error message for a failed sqlite3_exec function.
    string sql;                             // Create a string for SQL commands.
    sqlite3_stmt *stmt = nullptr;           // Will contain the results of the querrys.
    string dbName = getFilePath() + "SC_dmd.db";// Create the path plus name for SC_dmd.db.
    int rc = sqlite3_open(dbName.c_str(), &db);// Open the database named SC_tim.db. It will be created if it doesn't exist.
    if( rc ){                               // Was the database opened?
        errorMessage_ = "Can't open SC_dmd.db: " + string(sqlite3_errmsg(db));// Create an error message.
        cout << errorMessage_ << endl;      // Display the error.
        return false;                       // This error prevents writting each relay's time-on to the database.
    }                                       // The database is open.
    sqlite3_busy_timeout(db, 50);           // If the DB is locked wait up to 50 milliseconds before giving up.
    time_t nowTime = time(0);               // Contains the current time.
    struct tm* tmNow = gmtime(&nowTime);    // Will contain a more usable form of the current time.
    int year = tmNow->tm_year;              // Contains the UTC year.
    year += 1900;                           // The year starts at 1900.
    string timeNow = to_string(year) + "-"; // Start the time string with the year.
    int month = tmNow->tm_mon + 1;          // Get the month. tm_mon is the number of months since the start of the year. Thus add 1.
    string sMonth = month < 10 ? "0" + to_string(month) : to_string(month);// Create a two digit month.
    timeNow += sMonth + "-";                // Next add the month.
    int day = tmNow->tm_mday;               // Get the day.
    string sDay = day < 10 ? "0" + to_string(day) : to_string(day);// Create a two digit day.
    timeNow += sDay + " ";                  // Next add the day.
    int hour = tmNow->tm_hour;              // Get the hour.
    string sHour = hour < 10 ? "0" + to_string(hour) : to_string(hour);// Create a two digit hour.
    timeNow += sHour + ":";                 // Next add the hour.
    int min = tmNow->tm_min;                // Get the minute.
    string sMin = min < 10 ? "0" + to_string(min) : to_string(min);// Create a two digit minute.
    timeNow += sMin;                        // Next add the minute.
    for (int nDex = 0; nDex < this->size(); nDex++) {// Go through each meter to store its demand in the last storage period.
        string tableName = "dmd" + to_string(nDex);// Create the table name for this meter.
        sql = "SELECT * from " + tableName + " LIMIT 1"; // Create an SQL statement to see if the table exists.
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Send the command to the database.
        if (rc != SQLITE_OK) {              // Did the desired meter table exist in SC_dmd.db?
                                            // No. Create an error message.
            errorMessage_ = "Table " + tableName +" doesn't exist: " + string(sqlite3_errmsg(db));
            cout << errorMessage_ << endl;
            sql = "CREATE TABLE " + tableName + " (";// Create a SQL statement to create this meter's table.
            sql += "id INTEGER PRIMARY KEY,";// The table contains a primary key.
            sql += "sc_time TEXT,";         // The table contains date/time.
            sql += "demand INTEGER );";     // The table also contains this meter's demand in the last recording period.
            cout << "The SQL create statement is: " << sql << endl;
                                            // Execute SQL statement
            rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);// Send the command to the database.
            if (rc != SQLITE_OK) {          // Was there an error creating this meter's table in SC_dmd.db?
                errorMessage_ = "Can't create " + tableName + " in SC_tim.db: " + string(sqlite3_errmsg(db));// Create an error message.
                cout << errorMessage_ << endl;// Display the error.
                sqlite3_free(zErrMsg);      // Free the resources allocated to handling an sqlite3_exec error.
                sqlite3_finalize(stmt);     // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
                sqlite3_close(db);          // Close SC_dmd.db. Since finalize was just called there should be no errors.
                return false;               // Let the caller know the demand could not be stored.
            }                               // The table has been created!
            cout << "The table has been created!" << endl;
      }                                     // The meter's demand history table accepted the querry.
      sql = "INSERT INTO " + tableName;     // Create an sql statement to insert
      sql += " (sc_time,demand) ";          // the time and the demand for the last recording period.
      SCPulseMeter* meter = this->at(nDex); // Get access to this meter.
      uint32_t dmd = meter->getDemand();    // Get its demand (in watts).
      sql += "VALUES ('" + timeNow + "', " + to_string(dmd) + ")";// Add the values of the time and demand to the statement.
      rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);// Send the command to the database.
      if (rc != SQLITE_OK) {                // Was there an error storing this meter's demand in SC_dmd.db?
          errorMessage_ = "Can't store data in " + tableName + " in SC_tim.db: " + string(sqlite3_errmsg(db));// Create an error message.
          cout << errorMessage_ << endl;    // Display the error.
          sqlite3_free(zErrMsg);            // Free the resources allocated to handling an sqlite3_exec error.
          sqlite3_finalize(stmt);           // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
          sqlite3_close(db);                // Close SC_dmd.db. Since finalize was just called there should be no errors.
          return false;                     // The caller know storing the demand failed.
      }                                     // This meter's demand was stored.
  }                                         // All meters demands have been stored.
  sqlite3_finalize(stmt);                   // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
  sqlite3_close(db);                        // Close SC_dmd.db. Since finalize was just called there should be no errors.
  return true;                              // Inform the caller the demands were successfully stored.
}

}
