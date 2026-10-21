//
//  SCMaturity.h
//  PlantManager
//
//  Created by Mark Davis on 6/25/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#ifndef PlantManager_SCMaturity_h
#define PlantManager_SCMaturity_h



#include <chrono>

using namespace std;

namespace SCClasses {
    class SCMaturity {
    private:
        /*! The time this maturity began being incremented. */
        chrono::time_point<chrono::system_clock> timeBeganAt_;
        /*! The time this maturity was last updated. */
        chrono::time_point<chrono::system_clock> timeLastUpdated_;
        /*! The maturity value in degree Fahrenheit hours. */
        double maturityF_;
        /*! The maturity value in degree Celsius hours. */
        double maturityC_;
        /* The baseline used for calculating all maturities using Fahrenheit. */
        static double baselineF_;
        /* The baseline used for calculating all maturities using Celsius. */
        static double baselineC_;
    public:
        /*! Set the time maturity began being incremented.
         * \param timeValue The date/time when this maturity began to be incremented. */
        void setTimeBeganAt(chrono::time_point<chrono::system_clock> timeValue);
        /*! Get the maturity value in degree Fahrenheit hours. */
        double getMaturityF();
        /*! Set the maturity value in degree Fahrenheit hours. 
         * \param value The desired value of the maturity in degree hours Fahrenheit(must be at least zero). */
        void setMaturityF(double value);
        /*! Get the maturity value in degree Celsius hours. */
        double getMaturityC();
        /*! Set the maturity value in degree Celsius hours.
         * \param value The desired value of the maturity in degree hours Celsius(must be at least zero). */
        void setMaturityC(double value);
        /*! Gets the maturity baseline in degrees Fahrenheit.
         * \returns The baseline in degrees Fahrenheit. */
        static double getBaselineF();
        /*! Gets the maturity baseline in degrees Celsius.
         * \returns The baseline in degrees Celsius. */
        static double getBaselineC();
        /*! Sets the maturity baseline.
         * This will set the baseline in both Fahrenheit and Celsius.
         * \param value The desired value of the baseline in degrees Fahrenheit. */
        static void setBaselineF(double value);
        /*! Increments the maturity by the product of the given temperature in Fahrenheit and the time past since the last update.
         * This will increment the maturity in both Fahrenheit and Celsius.
         * \param temperature The temperature used to increment this maturity is in degrees Fahrenheit. */
        void increment(double temperature);
        SCMaturity();
        virtual ~SCMaturity();
    };

}

#endif
