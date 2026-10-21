//
//  SCMaturity.cpp
//  PlantManager
//
//  Created by Mark Davis on 6/25/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#include "SCMaturity.h"

namespace SCClasses {
    double SCMaturity::baselineF_ = 0;      // The default value for the maturity baseline (Fahrenheit).
    double SCMaturity::baselineC_ = -17.778;// The default value for the maturity baseline (Celsius) is zero Fahrenheit.
    
    SCMaturity::SCMaturity() {
        timeBeganAt_ = chrono::system_clock::now();// The default value for the last update.
        timeLastUpdated_ = chrono::system_clock::now();// The default value for when calculations began.
        maturityF_ = 0;                     // The default value for the maturity (Fahrenheit).
        maturityC_ = 0;                     // The default value for the maturity (Celsius).
    }
    
    SCMaturity::~SCMaturity() {
        
    }
    
    void SCMaturity::setTimeBeganAt(chrono::time_point<chrono::system_clock> timeValue) {
        chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();
        if(timeValue < now) {               // Is the desired time for when maturity began being incremented before now?
            timeBeganAt_ = now;             // Yes. Set it to now.
        } else {                            // The time is OK.
            timeBeganAt_ = timeValue;       // Set the time maturity began incrementing.
        }                                   // timeBeganAt_ is set.
    }
    
    double SCMaturity::getMaturityF() {
        return maturityF_;
    }
    
    void SCMaturity::setMaturityF(double value) {
        if (value >= 0) {                   // Is the desired value at least sero?
            maturityF_ = value;             // Yes. Set the maturity.
        } else {                            // No.
            maturityF_ = 0;                 // Set the maturity to zero.
        }                                   // The maturity in Fahrenheit is set.
    }
    
    double SCMaturity::getMaturityC() {
        return maturityC_;
    }
    
    void SCMaturity::setMaturityC(double value) {
        if (value >= 0) {                   // Is the desired value at least sero?
            maturityC_ = value;             // Yes. Set the maturity.
        } else {                            // No.
            maturityC_ = 0;                 // Set the maturity to zero.
        }                                   // The maturity in Celsius is set.
    }
    
    double SCMaturity::getBaselineF() {
        return baselineF_;                  // Provide the baseline in degrees Fahreheit.
    }
  
    double SCMaturity::getBaselineC() {
        return baselineC_;                  // Provide the baseline in degrees Celsius.
    }
    
    void SCMaturity::setBaselineF(double value) {
        baselineF_ = value;                 // Set the baseline in Fahrenheit.
        baselineC_ = (value - 32) / 1.8;    // Set the baseline in Celsius.
    }

    void SCMaturity::increment(double temperature) {
        chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// now is the present time.
        chrono::duration<double> timePast;// The amount of time which has passed since the last update.
        double	seconds;
        if (now < timeBeganAt_) {       // Past Start time?
            maturityF_ = 0;             // No. Reset the maturity (Fahrenheit).
            maturityC_ = 0;             // Reset the maturity (Celsius).
            timeLastUpdated_ = now;     // Reset the last time the maturity was updated.
            return;	                    // No, don't increment maturity.
        }                               // Done processing a maturity which hasn't started to increment.
        // Calculate elapsed time.
        timePast = now - timeLastUpdated_;// Iime past is the difference between now and when the maturity was last updated.
        seconds = timePast.count();
        if(seconds < 4)
            return;	// Too soon to update, wait till later.
        timeLastUpdated_ = now;	// Adjust time for next increment.
        
        // Increment maturity (elapsed time(hours) * temperature)
        maturityF_ += (temperature - baselineF_) * seconds / 3600;// Increment the Fahrenheit version of maturity.
        temperature = (temperature - 32) / 1.8;                   // Convert the temperature to Celsius.
        maturityC_ += (temperature - baselineC_) * seconds / 3600;// Increment the Celsius version of maturity.

    }
    
   
}