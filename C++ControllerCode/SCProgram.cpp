//
//  SCProgram.cpp
//  PlantManager
//
//  Created by Mark Davis on 6/30/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#include "SCProgram.h"

namespace SCClasses {
    
    CutoffPoint::CutoffPoint(){
        cutoffTemp_ = 999.0;       // The default cutoff temperature is higher than any temperature should reach.
                                    // Thus it is effectively not active.
        maintTemp_ = 68.0;          // The default maintenance temperature sixt-eight,
                                    // is far enough above freezing that nothing should freeze.
        isMeasuredInFahrenheit = true;// By default temperatures are in Fahrenheit.
    }
    
    CutoffPoint::~CutoffPoint() {
        
    }
    
    float CutoffPoint::getCutoffTemp() {
        if (isMeasuredInFahrenheit == true) {// Are temperatures passed in Fahrenheit.
            return cutoffTemp_;            // Return the cutoff temperature in Fahrenheit.
        }                                   //
        return (cutoffTemp_ - 32.0) / 1.8; // Return the cutoff temperature in Celsius.
    }

    void CutoffPoint::setCutoffTemp(float cutoffTemp) {
        if (isMeasuredInFahrenheit == true) {// Are temperatures passed in Fahrenheit.
            cutoffTemp_ = cutoffTemp;      // Yes. Set the cutoff temperture.
        } else {                            // Temperatures are passed in Celsius.
            float ct = cutoffTemp * 1.8;    // Convert the temperature from Celsius
            ct += 32.0;                     // to Fahrenheit.
            cutoffTemp_ = ct;              // Reset the cutoff temperature.
        }
    }

    float CutoffPoint::getMaintenanceTemp() {
        if (isMeasuredInFahrenheit == true) {// Are temperatures passed in Fahrenheit.
            return maintTemp_;              // Return the maintenance temperature in Fahrenheit.
        }                                   //
        return (maintTemp_ - 32.0) / 1.8;   // Return the maintenance temperature in Celsius.
    }
    
    void CutoffPoint::setMaintenanceTemp(float maintenanceTemp) {
        if (isMeasuredInFahrenheit == true) {// Are temperatures passed in Fahrenheit.
            maintTemp_ = maintenanceTemp;   // Yes. Set the maintenance temperture.
        } else {                            // Temperatures are passed in Celsius.
            float mt = maintenanceTemp * 1.8; // Convert the temperature from Celsius
            mt += 32.0;                     // to Fahrenheit.
            maintTemp_ = mt;                // Reset the maintenance temperature.
        }
    }
    void CutoffPoint::set(float cutoffTemp, float maintenanceTemp) {
        float ct = cutoffTemp;             // Contains the caller's cutoff temperature.
        float mt = maintenanceTemp;         // Contains the caller's maintenance temperature.
        if (isMeasuredInFahrenheit == false) {// Are temperatures passed in Celsius.
            ct *= 1.8;                      // Yes. Convert them to Fahrenheit.
            ct += 32.0;                     // The cutoff point class stores
            mt *= 1.8;                      // all private temperatures in
            mt += 32.0;                     // Fahrenheit.
        }                                   //
        cutoffTemp_ = ct;                  // Save the new cutoff tempearture.
        maintTemp_ = mt;                    // Save the new maintenance temperature.
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    SCCutoffs::SCCutoffs() {
        version_ = 1;                       // Version as of 7/3/2014.
        filePath_ = "";                     // There is no file path until it has been set.
        nSCChnls_ = 0;                      // There are is no number of SCChnls until it have been set.
        isMeasuredInFahrenheit = true;      // By default, temperatures are in Fahrenheit.
        isSCFile_ = false;                  // The storage of cutoff settings in CutoffSetting.SC is not known.
        isXMLFile_ = false;                 // The storage of cutoff settings in Cuttoffs.xml is not known.
        fileName_ = "Cuttoffs.xml";         // The default name for the file containing cutoff settings.
    }
    
    SCCutoffs::~SCCutoffs() {
        for(unsigned int i = 0; i < this->size(); i++){	// Go through each cutoff point.
            delete this->at(i);						// Free the memory allocated to it.
        }                                           //
        this->clear();								// Clear all the cutoffs of all cutoff points.
    }
    
    int SCCutoffs::getVersion(){
        return version_;
    }
    
    string SCCutoffs::getFilePath(void){
        return filePath_;
    }
    
    void SCCutoffs::setFilePath(string strPath){
        filePath_ = strPath;                        // Set the file path to the file containg the cutoff settings.
        char slash = strPath.at(strPath.size() - 1);// Get the last character from the path.
        if (slash != '/') {                         // Is the last character a slash?
            filePath_.append("/");                  // No, make the last character a slash.
        }
    }
    
    float SCCutoffs::getCutoffTemp(unsigned int index) {
        if (index >= nSCChnls_) {                   // Is the idex valid?
            cout << "The give index " << index << ", is not between 0 and " << nSCChnls_ << "." << endl;
            return 500.0;                           // No. Return a very high value.
        }                                           // The index is valid.
        return this->at(index)->getCutoffTemp();    // Return the cutoff temperature.
    }
    
    float SCCutoffs::getMaintenanceTemp(unsigned int index) {
        if (index >= nSCChnls_) {                   // Is the idex valid?
            cout << "The give index " << index << ", is not between 0 and " << nSCChnls_ << "." << endl;
            return 500.0;                           // No. Return a very high value.
        }                                           // The index is valid.
        return this->at(index)->getMaintenanceTemp();// Return the maintenance temperature.
    }
    
    void SCCutoffs::set(unsigned int index, float cutoffTemp, float maintenanceTemp) {
        if (index >= nSCChnls_) {                   // Is the idex valid?
            cout << "The give index " << index << ", is not between 0 and " << nSCChnls_ << "." << endl;
            return;                                 // No.
        }                                           // The index is valid.
        CutoffPoint* cp = this->at(index);          // Get the cutoff point to modify.
        cp->set(cutoffTemp, maintenanceTemp);       // Set the cutoff point.
    }
    
    void SCCutoffs::setNumberOfSCChnls(unsigned int nSCChnls) {
        nSCChnls_ = nSCChnls;                       // Set the number of SCChnls.
    }

    bool SCCutoffs::areLoaded() {
        if (nSCChnls_ == 0) {                       // Are there any SCChnls?
            return false;                           // No. Nothing is loaded.
        }                                           // There are SCChnls.
        return true;                                // Let the caller know settings have been loaded.
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool TimeTempPoint::isMeasuredInFahrenheit = true;// The default setting for units is Fahrenheit.
    unsigned int TimeTempPoint::timeLimit_ = 65000; // One minute greater than the maximum valid minute.
    // For now the limit is 65000 to work with the .NET applications. 65,000 minutes is approximately 45 days.
    
    TimeTempPoint::TimeTempPoint() {
        pointNo_ = 9999;                            // The default point number is invalid.
        time_ = timeLimit_;                         // The default elapsed time in minutes.
        temp_ = -500.0;                             // The temperature in degrees Fahrenheit.
        priorityNo_ = 999;                          // The default priority number.
    }
    
    TimeTempPoint::TimeTempPoint(unsigned int pointNo, unsigned int time, float temp, unsigned int priority) {
        pointNo_ = pointNo;                         // Set the point number.
        time_ = time;                               // Set the elapsed time in minutes.
        if (isMeasuredInFahrenheit == true) {       // Are temperatures in Fahrenheit?
            temp_ = temp;                           // Yes. Set the temperature as passed.
        } else {                                    // Temperatures are in Celsius.
            temp_ = temp * 1.8 + 32.0;              // Convert the temperature to Fahrenheit and store.
        }                                           // The temperature has been stored.
        priorityNo_ = priority;                     // Set the priority number.
    }

    
    TimeTempPoint::~TimeTempPoint() {
        
    }

    unsigned int TimeTempPoint::getPointNo() {
        return pointNo_;                            // Send the caller the point number.
    }
    
    void TimeTempPoint::setPointNo(unsigned int pointNo) {
        if (pointNo < 21) {                         // Is the point number valid?
            pointNo_ = (uint32_t)pointNo;           // Yes. Set the point number.
        }
    }
    
    unsigned int TimeTempPoint::getTime() {
        return time_;
    }
    
    unsigned int TimeTempPoint::getElapsedHour() {
        return time_ / 60;                          // Calculate and return the elapsed hour.
    }
    
    unsigned int TimeTempPoint::getElapsedMinute() {
        unsigned int m = time_;                     // Use m to manipulate the time.
        m -= 60 * getElapsedHour();                 // Remove the hour portion of the time.
        return m;                                   // Supply the caller with the minute portion of the elapsed time.
    }

    void TimeTempPoint::setTime(unsigned int elapsedMinutes) {
        if (elapsedMinutes > timeLimit_) {     // Are the elapsed minutes valid (less than 450 days)?
            time_ = timeLimit_;                // No. Set the elpased minutes to arrox. 450 days, signifies end of program.
        } else {                               // The elapsed minutes are valid.
            time_ = elapsedMinutes;            // Reset the elapsed minutes since the start time for this point.
        }
        
    }
    
    void TimeTempPoint::setTime(unsigned int elpasedHour, unsigned int elapsedMinute) {
        if (elpasedHour > 10833) {                  // Will the elpased hour cause the time_ to exceed 650,000?
            time_ = timeLimit_;                     // Yes. Set the time_ to signify the program's end.
            return;                                 // Done setting the time.
        }                                           // The elapsed hour will not cause the time_ to exceed 650,000.
        if (elapsedMinute > timeLimit_) {           // Will the elpased minute cause the time_ to exceed 650,000?
            time_ = timeLimit_;                     // Yes. Set the time_ to signify the program's end.
            return;                                 // Done setting the time.
        }                                           // The elapsed minute will not cause the time_ to exceed 650,000.
        time_ = 60 * elpasedHour + elapsedMinute;   // Set the time_ use the elapsed minute and elapsed hour.
        if (time_ > timeLimit_) {                   // Does the time_ exceed 650,000?
            time_ = timeLimit_;                     // Yes. Set the time_ to signify the program's end.
        }                                           // The time_ has been set.
    }

    float TimeTempPoint::getTemperture() {
        if (isMeasuredInFahrenheit == true) {       // Are temperatures measurde in Fahrenheit?
            return temp_;                           // Yes. Provide the caller with the temperature.
        }                                           // The temperature should be returned in Celsius.
        return (temp_ - 32.0) / 1.8;                // Provide the caller with the temperature in Celsius.
    }

    void TimeTempPoint::setTemperature(float temperature) {
        if (isMeasuredInFahrenheit == true) {       // Are temperatures measurde in Fahrenheit?
            temp_ = temperature;                    // Yes. Set the temperature as passed.
        } else {                                    // The passed temperature must be converted to Celsius.
            temp_ = temperature * 1.8 + 32.0;       // Convert and store the temperature.
        }                                           // The temperature has benn stored.
    }

    unsigned int TimeTempPoint::getPriortyNo() {
        return priorityNo_;                         // Provide the caller with the priority number.
    }

    void TimeTempPoint::setPriorityNo(unsigned int priorityNo) {
        priorityNo_ = priorityNo;                   // Set the priority number as passed.
    }
    
    unsigned int TimeTempPoint::getTimeLimit() {
        return timeLimit_;                          // Provide the time limit to the caller.
    }

   
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    bool SCProgram::isDisplayedInFahrenheit = true; // By default, programs are displayed in Fahrenheit.
    string SCProgram::filePath_ = "";

    SCProgram::SCProgram() {
        fileName_ = "";                             // There is no file name until it has been set elsewhere.
//        filePath_ = "";                             // There is no file path until it has been set elsewhere.
        refChnl_ = 9999;                            // The reference channel is set to an invalid number.
        errorMessage_ = "None";                     // There is no error message.
        for (int i = 0; i < 21; i++) {              // Go through each of the program's points.
            TimeTempPoint ttp;                      // Create the point.
            ttp.setPointNo(i);                      // Set it's point number.
            points_.push_back(ttp);                 // Place it in the list of points.
        }                                           // The default points have been set.
        startTime_ = chrono::system_clock::now();   // The default start time is now.
        timeCreated_ = chrono::system_clock::now(); // The default creation time is now.
        version_ = 1;                               // The default version on 7/12/2014.
        cutoffTemp_ = 999.0;                        // The default cutoff temperature is very high.
        maintenanceTemp_ = 68.0;                    // The default maintenance temperature is above freezing but not too warm.
        owner = "PlantManager";                     // The default owner of a program curve.
        controlType_ = SCProgram::TYPE_MASTER;      // The deafult control type is master.
        isActive = false;                           // The program is not active until it is set to be active.
    }
    
    SCProgram::~SCProgram() {
        
    }
    
    SCProgram& SCProgram::operator=(SCProgram other) {
        setControlType(other.getControlType());
        setRefChnl(other.getRefChnl());
        setCutoffTemperature(other.getCutoffTemperature());
        setMaintenanceTemperature(other.getMaintenanceTemperature());
        timeCreated_ = other.timeCreated_;
        setStartTime(other.getStartTime());
        isActive = other.isActive;
        for (int nDex = 0; nDex < 21; nDex++) {
            points_.at(nDex).setTime(other.points_.at(nDex).getTime());
            points_.at(nDex).setTemperature(other.points_.at(nDex).getTemperture());
            points_.at(nDex).setPriorityNo(other.points_.at(nDex).getPriortyNo());
        }
        return *this;
    }
    
    string SCProgram::getFilePath() {
        return filePath_;                           // Provide the caller with the file path.
    }
    
    void SCProgram::setFilePath(string strPath) {
        filePath_ = strPath;                        // Set the file path as passed.
    }

    unsigned int SCProgram::getRefChnl() {
        return refChnl_;                            // Provide the caller with the channel this program is referenced to.
    }

    void SCProgram::setRefChnl(unsigned int value) {
        errorMessage_ = "";                         // There has not been an error yet.
        refChnl_ = 9999;                            // Set the reference channel index to 9999 which indicates there is no refence chnl.
        if (value <= 9999) {                        // Is the value passed a legal reference index?
            refChnl_ = value;                       // Yes. Set the index of the SC channel this program is referenced.
        } else {                                    // The reference has been set.
            errorMessage_ = to_string(value) + " is an illegal reference channel index in SCProgram.";// Create a message.
            cout << errorMessage_ << endl;          // Display the error.
        }
    }

    string SCProgram::getErrorMessage() {
        return errorMessage_;                       // Provide the caller with the error message.
    }
    
    chrono::time_point<chrono::system_clock> SCProgram::getStartTime() {
        return startTime_;                          // Provide the caller with the start time.
    }

    void SCProgram::setStartTime(chrono::time_point<chrono::system_clock> startTime) {
        startTime_ = startTime;                     // Set the start time.
    }
  
    void SCProgram::setStartTime(int year, int month, int day, int hour, int minute) {
        errorMessage_ = "";                         // Clear the error message.
        if (year < 1970 || year > 2200) {           // Is the year legal?
            errorMessage_ = "The year is illegal(<1970 or >2200) in setStartTime().";// No, create a message.
            cout << errorMessage_ << endl;          // Put the error on the console.
            return;                                 // It is not possible to set the start time.
        }                                           // The year is legal.
        if (month < 0) {                            // Is the month negative?
            errorMessage_ = "The month is illegal(<0) in setStartTime().";// Yes, create a message.
            month = 0;                              // Set the month to zero.
        }                                           // The month is not negative.
        if (month > 11) {                           // Is the month greater than December?
            errorMessage_ = "The month is illegal(>11) in setStartTime().";// Yes, create a message.
            cout << errorMessage_ << endl;          // Put the error on the console.
            return;                                 // It is not possible to set the start time.
        }                                           // The month is legal.
        if (day < 1) {                              // Is the day less than 1?
            errorMessage_ = "The day is illegal(<0) in setStartTime().";// Yes, create a message.
            day = 1;                                // Set the day to one.
        }                                           // The day is not too small.
        int dayLimit = 31;                          // The largest number of days in a month.
        if (month == 4 || month == 6 || month == 9 || month == 11) {// Is this a month with 30 days?
            dayLimit = 30;                          // Yes. Reset the limit to 30.
        } else if (month == 2) {                    // Is the month February?
            dayLimit = 29;                          // There are no more than 29 days in February.
        }                                           // The daylimit has been set for the current month.
        if (day > dayLimit) {                       // Is the day greater than the maximum number of days in the month?
            errorMessage_ = "The day is illegal(>"; // Yes, create a message.
            errorMessage_ += to_string(dayLimit);   //
            errorMessage_ += ") in setStartTime().";//
            day = dayLimit;                         // Set the day to the last day in the month.
            if (month == 2) {                       // Is the month February?
                day = 28;                           // Set the day to 28, don't worry about leap years.
            }                                       // The day is legal
        }                                           // The day is legal.
        if (hour < 0) {                             // Is the hour negative?
            errorMessage_ = "The hour is illegal(<0) in setStartTime().";// Yes, create a message.
            hour = 0;                               // Set the hour to zero.
        }                                           // The hour is not negative.
        if (hour > 23) {                            // Is the hour greater than 23?
            errorMessage_ = "The month is illegal(>23) in setStartTime().";// Yes, create a message.
            hour = 23;                              // Set the hour to 23;
        }                                           // The hour is legal.
        if (minute < 0) {                           // Is the minute negative?
            errorMessage_ = "The minute is illegal(<0) in setStartTime().";// Yes, create a message.
            minute = 0;                             // Set the minute to zero.
        }                                           // The minute is not negative.
        if (minute > 59) {                          // Is the minute greater than 59?
            errorMessage_ = "The month is illegal(>59) in setStartTime().";// Yes, create a message.
            minute = 59;                            // Set the minute to 59;
        }                                           // The minute is legal.
        // There is no way to directly set a time_point object. Therefore we start with a tm structure.
        tm desiredStartTime;                        // Contains the desired start time.
        desiredStartTime.tm_year = year - 1900;     // Set the desired start year. Years are the # of years since 1900.
        desiredStartTime.tm_mon = month;            // Set the desired start month. Months are 0-11.
        desiredStartTime.tm_mday = day;             // Set the desired start day. Days are 1-31.
        desiredStartTime.tm_hour = hour;            // Set the desired start hour.  Hours are 0-23.
        desiredStartTime.tm_min = minute;           // Set the desired start minute. Minutes are 0-59.
        desiredStartTime.tm_sec = 0;                // The second is always zero.
        desiredStartTime.tm_isdst = 0;      // Guess that it is not daylight savings time. mktime will change the hour if it is.
        time_t t = mktime(&desiredStartTime);       // To set a time_point, the tm object must be converted to a time_t object.
        if(desiredStartTime.tm_hour != hour) {      // Was the hour changed by mktime?
            desiredStartTime.tm_isdst = 1;          // Yes. Guessed wrong. Change the daylight savings time to true.
            desiredStartTime.tm_hour = hour;        // Reset the hour.
            t = mktime(&desiredStartTime);          // Reset the (time_t)time.
        }                                           // The (time_t) time is correctly set to the desired time.
        startTime_ = chrono::system_clock::from_time_t(t);// The time_t object can then be converted to a time_point.
    }

    SCProgram::Type SCProgram::getControlType() {
        return controlType_;                        // Provide the caller with the control type.
    }

    void SCProgram::setControlType(SCProgram::Type controlType) {
        controlType_ = controlType;                 // Set the control type.
    }

    float SCProgram::getMaintenanceTemperature() {
        return maintenanceTemp_;                    // Provide the caller with the maintenance temperature.
    }
    
    void SCProgram::setMaintenanceTemperature(float value) {
        errorMessage_ = "";                         // Clear the error message.
        if (value >= -200 && value <= 400) {        // Is the desired value between -200 & 400?
            maintenanceTemp_ = value;               // Yes. Set the maintenance temperature.
        } else if (value < -200) {                  // Is the desired value less than -200?
            errorMessage_ = "The value is illegal(<-200) in setMaintenanceTemperature().";// Yes, create a message.
            maintenanceTemp_ = -200;                // Yes. Limit the maintenance temperature to -200.
        } else {                                    // The desired value was above 500.
            errorMessage_ = "The value is illegal(>400) in setMaintenanceTemperature().";// Yes, create a message.
            maintenanceTemp_ = 400.0;               // Limit the maintenance temperature to 400.
        }
    }

    float SCProgram::getCutoffTemperature() {
        return cutoffTemp_;                         // Provide the caller with the cutoff temperature.
    }
    
    void SCProgram::setCutoffTemperature(float value) {
        errorMessage_ = "";                         // Clear the error message.
        if (value >= -200 && value <= 400) {        // Is the desired value between -200 & 400?
            cutoffTemp_ = value;                    // Yes, set the cutoff temperature.
        } else if (value < -200) {                  // Is the desired value less than -200?
            errorMessage_ = "The value is illegal(<-200) in setCutoffTemperature().";// Yes, create a message.
            cutoffTemp_ = -200;                     // Yes. Limit the cutoff temperature to -200.
        } else {                                    // The desired value was above 400.
            errorMessage_ = "The value is illegal(>400) in setCutoffTemperature().";// Yes, create a message.
            cutoffTemp_ = 400;                      // Limit the cutoff temperature to 400.
        }
    }
 
    void SCProgram::initializeTimeTemp() {
        for (int n = 0; n < points_.size(); n++) {  // Reset each existing point.
            points_.at(n).setPointNo(n);            // Reset the point number.
            points_.at(n).setTime(TimeTempPoint::getTimeLimit());// Set the time one minute higher than valid.
            points_.at(n).setTemperature(-500);     // Set the temperature to a very cold value.
        }
        for (int n = (int)points_.size(); n < 21; n++) {// For each point that doesn't exist.
            TimeTempPoint ttPoint;                  // Create the point. The constructor will set the desired values.
            ttPoint.setPointNo(n);                  // Set the point number.
            points_.push_back(ttPoint);             // Add the point to the array of points.
        }
    }

    void SCProgram::initialize() {
        initializeTimeTemp();                       // Initialize the program points.
        setStartTime(2000, 0, 1, 0, 0);             // Set the start time to an old date.
        timeCreated_ = chrono::system_clock::now(); // Set the time this program was created to now.
        setControlType(SCProgram::TYPE_MASTER);     // A program is a master by default.
        isActive = true;                            // A progrma is active until it is deactivated.
    }
    
    int SCProgram::lastSegmentNo() {
        int lastSeg = 1;                            // The last segment can be no less than one.
        for(; lastSeg < 21; lastSeg++) {            // Loop through each segment.
            if(points_.at(lastSeg).getTime() == TimeTempPoint::getTimeLimit()) {// Is the elapsed time invalid?
                return --lastSeg;                   // The last segment was the previous segment.
            }                                       // This is not the last segment.
        }                                           // The last segment was the last one posiible.
        return lastSeg;                             // Let the caller know the last segment.
    }
    
    unsigned int SCProgram::getElapsedMinutes(unsigned int pointNo) {
        if (pointNo > 20) {                         // Is the given point legal?
            errorMessage_ = "The given point number was greater then 20.";// Set the error message.
            return TimeTempPoint::getTimeLimit();   // No, return an illegal time.
        }                                           // The point is legal.
        return points_.at(pointNo).getTime();       // Let the caller know the elapsed minutes component of this time/temp point.
    }
    
    bool SCProgram::setElapsedMinutes(unsigned int pointNo, unsigned int elapsedMinutes) {
        if (pointNo > 20) {                         // Is the given point legal?
            errorMessage_ = "The given point number was greater then 20.";// Set the error message.
            return false;                           // No, let the caller know there was a problem.
        }                                           // The point is legal.
        points_.at(pointNo).setTime(elapsedMinutes);// Set this point's elpasaed minutes.
        return true;                                // Let the caller know the elapsed minutes were successully set.
    }
    
    float SCProgram::getTemperature(unsigned int pointNo) {
        if (pointNo > 20) {                         // Is the given point legal?
            errorMessage_ = "The given point number was greater then 20.";// Set the error message.
            return -500;                            // No, return an illegal temperature.
        }                                           // The point is legal.
        return points_.at(pointNo).getTemperture(); // Let the caller know the temperature component of this time/temp point.
    }

    bool SCProgram::setTemperature(unsigned int pointNo, float temperature) {
        if (pointNo > 20) {                         // Is the given point legal?
            errorMessage_ = "The given point number was greater then 20.";// Set the error message.
            return false;                           // No, return failure.
        }                                           // The point is legal.
        points_.at(pointNo).setTemperature(temperature);// Set this point's temperature.
        return true;                                // Let the caller know the tmperature was successfully set.
    }
    
    unsigned int SCProgram::getPriorityNo(unsigned int pointNo) {
        if (pointNo > 20) {                         // Is the given point legal?
            errorMessage_ = "The given point number was greater then 20.";// Set the error message.
            return 0;                               // No, return an illegal priority number.
        }                                           // The point is legal.
        return points_.at(pointNo).getPriortyNo();  // Provide the caller with the priority number.
    }
    
    bool SCProgram::setPriorityNo(unsigned int pointNo, unsigned int prNo) {
        if (pointNo > 20) {                         // Is the given point legal?
            errorMessage_ = "The given point number was greater then 20.";// Set the error message.
            return false;                           // No, let the caller know there was a problem.
        }                                           // The point is legal.
        points_.at(pointNo).setPriorityNo(prNo);    // Set this point's priority number.
        return true;                                // Let the caller know the priority number was successully set.
    }

    bool SCProgram::loadFromDB(int chnlDex) {
        int dbChnlDex = chnlDex + 1;            // The database works with ones based indexes.
        sqlite3 *db;                            // Create a location for the database.
        int  rc;                                // Create a return variable.
        string sql;                             // Create a string for SQL commands.
        string dbName = getFilePath() + "SCConfig.db";// Set the configuration database name.
        int count = 0;                          // The number of programs read.
        rc = sqlite3_open(dbName.c_str(), &db); // Open the database named SC_Config.db. It has a table which contains the programs.
        if( rc ){                               // Was the database opened?
            errorMessage_ = "Can't open SCConfig.db to load program: " + string(sqlite3_errmsg(db));// No. create an error message.
            cout << errorMessage_ << endl;      // Display the error.
            return false;                       // This error prevents writting the temperatures to the database.
        }                                       // The database is open.
        sqlite3_busy_timeout(db, 50);           // If the DB is locked wait up to 50 milliseconds before giving up.
        string tableName = "programs";          // Create the table name for the programs.
        sql = "SELECT * from " + tableName + " WHERE id=" + to_string(dbChnlDex);     // Create an SQL statement to read all the programs.
        sqlite3_stmt *stmt;                     // Will contain the results of the querry.
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
        if (rc != SQLITE_OK) {                  // Was it successful?
            errorMessage_ = sqlite3_errmsg(db); // Create an error message.
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
            sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
            return false;                       // Let the caller know tthe program could not be retrieved from the database.
        }                                       // The program was retreived.
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {// Go through each row in the results.
            id_ = sqlite3_column_int (stmt, 0); // The program's id is in the first column.
            controlType_ = (SCProgram::Type)sqlite3_column_int(stmt, 1); // The control type is in the second column of the table.
            setRefChnl(sqlite3_column_int(stmt, 2));// The reference chnl is in the third column of the table.
            setCutoffTemperature(sqlite3_column_double(stmt, 3));// The Cutoff Temperature is in the fourth column of the table.
            setMaintenanceTemperature(sqlite3_column_double(stmt, 4));// The Maintenance Temperature is in the fifth column of the table.
            const unsigned char *timeToStart = sqlite3_column_text(stmt, 5);// The start time is in the sixth column of the table.
            string startTime = std::string(reinterpret_cast<const char*>(timeToStart));// Convert the time to a string.
            // The start time is stored as a UTC time in string format.
            // It is necessary to convert the UTC start time to a local time so that it can be used with
            // the local clock on the computer. The function timegm(struct tm*) is required to convert a UTC
            // time into a local time.
            // Create a struct tm for the start time which comes from the database as a string.
            struct tm tmUTCStart = createTimeStructFromDBString(startTime);
            time_t tFromUTC = timegm(&tmUTCStart);// Should be a local time.
            startTime_ = chrono::system_clock::from_time_t(tFromUTC);// Should be a local time.

/*            cout << "Start time string: " << startTime << endl;
            time_t tTime = chrono::system_clock::to_time_t(startTime_);
            string ts = ctime(&tTime);
            ts.resize(ts.size() - 1);
            cout << "Showing start time from the chrono::point Start Time: " << ts << endl; */
            
            const unsigned char *timeCreated = sqlite3_column_text(stmt, 6);// The time created is in the seventh column of the table.
            string createdTime = std::string(reinterpret_cast<const char*>(timeCreated));// Convert the time to a string.
            struct tm tmUTCCreated = createTimeStructFromDBString(createdTime);
            tFromUTC = timegm(&tmUTCCreated);// Should be a local time.
            timeCreated_ = chrono::system_clock::from_time_t(tFromUTC);// Should be a local time.

/*            cout << "Created time string: " << createdTime << endl;
            tTime = chrono::system_clock::to_time_t(timeCreated_);
            ts = ctime(&tTime);
            ts.resize(ts.size() - 1);
            cout << "Showing created time from the chrono::point Created Time: " << ts << endl << endl; */
            
            isActive = sqlite3_column_int (stmt, 7);// The active flag is in the eigth column of the table.
            version_ = sqlite3_column_int(stmt, 8);// The version is in the nineth column of the table.
            count = 1;                          // A program has been read.
        }                                       // The first part of the given chnl's progrma has been read.
        if (rc != SQLITE_DONE) {                // Was there an error?
            errorMessage_ = sqlite3_errmsg(db); // Create an error message.
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
            sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
            return false;                       // Let the caller know there was an error.
        }                                       // The program's first part was read successfully.
        sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt".
        sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
        if(count == 0) {
            dbCreateDefaultProgram(chnlDex);
            return true;
        }
        // Read the time/temp points for this program.
        rc = sqlite3_open(dbName.c_str(), &db); // Open the database named SC_Config.db. It have a table which contains the program's points.
        if( rc ){                               // Was the database opened?
            errorMessage_ = "Can't open SCConfig.db to read program points: " + string(sqlite3_errmsg(db));// No. create an error message.
            cout << errorMessage_ << endl;      // Display the error.
            return false;                       // This error prevents writting the temperatures to the database.
        }                                       // The database is open.
        sqlite3_busy_timeout(db, 50);       // If the DB is locked wait up to 50 milliseconds before giving up.
        tableName = "programPoints";            // Create the table name for this program's time/temp points.
        sql = "SELECT * from " + tableName + " WHERE programId=" + to_string(dbChnlDex);// Create an SQL statement to read this the program's points.
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Querry the database.
        if (rc != SQLITE_OK) {                  // Was the querry successful?
            errorMessage_ = sqlite3_errmsg(db); // Create an error message.
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
            sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
            return false;                       // Failed to read this program.
        }                                       // The progrma points were read.
        int n = 0;                              // The index of the first program point.
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {// Go through the points that were read.
//            int id           = sqlite3_column_int (stmt, 0);// The id is in the first column.
//            int prgmID = (SCProgram::Type)sqlite3_column_int(stmt, 1); // The point's id is in the table's second column.
            int eMinutes = (sqlite3_column_int(stmt, 2));// The elapsed minutes are in the table's third column.
            setElapsedMinutes(n, eMinutes);     // Set the point's elpased minutes for point n.
            float temperature = (sqlite3_column_double(stmt, 3));// The temperature is in the table's fourth column.
            setTemperature(n, temperature);// Set the point's temperature for point n.
            int prNo = (sqlite3_column_int(stmt, 4));// The priority number is in the table's fifth column.
            setPriorityNo(n, prNo);             // Set the point's priority number for point n.
            n++;                                // Increment the program point counter.
        }                                       // The program's points have been retreived.
        if (rc != SQLITE_DONE) {                // Was there a failure to read a row?
            errorMessage_ = sqlite3_errmsg(db); // Create an error message.
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
            sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
            return false;                       // Let the caller know there was an error.
        }                                       // All rows were read.
        sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt".
        sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
        return true;                            // Let the caller know the program was read.
    }
    
    unsigned int SCProgram::presentSegmentNo() {
        if(isActive == false) {             // Is this program active?
            return 999;                     // This program is not active. Send the segment no that represents the program is done.
        } // End of condition: the program is not active.
        chrono::time_point<chrono::system_clock> timeNow = chrono::system_clock::now();// Get the present time.
        chrono::duration<double> timeElapsed;// Will contain the number of minutes since the program started.
/*
        time_t tTime = chrono::system_clock::to_time_t(startTime_);
        string ts = ctime(&tTime);
        ts.resize(ts.size() - 1);
        cout << "In presentSegemntNo(), Start Time is " << ts << endl;
        tTime = chrono::system_clock::to_time_t(timeNow);
        ts = ctime(&tTime);
        ts.resize(ts.size() - 1);
        cout << "In presentSegemntNo(), the time now is " << ts << endl;
*/
        if (timeNow < startTime_) {         // Has the start time been reached?
            return 0;                       // No. Return zero to indicate the start time has not passed yet.
        }                                   // The start time has passed.
        timeElapsed = timeNow - startTime_; // Get the amount of time since the start time.
        chrono::minutes durMinutesPast = chrono::duration_cast<chrono::minutes>(timeElapsed);// Convert to minutes.
        long nMinutesPast = durMinutesPast.count();// Convert to a number.
        if (nMinutesPast >= TimeTempPoint::getTimeLimit()) { // Have more minutes passed than are valid since the start time?
            isActive = false;               // Yes. Deactivate the program.
            dbSetActiveStatus(isActive);    // Change the active status in the database's program's table to inactive.
            return 999;                     // Return a segment past the end.
        }                                   // The minutes since the start time have not exceeded 650,000 (451 days).
        for (unsigned int p = 0; p < 21; p++) {// Go through each time/temp point of the program.
            if (getElapsedMinutes(p) >= TimeTempPoint::getTimeLimit()) {// Does this point contain an end time?
                isActive = false;           // Yes. Set the program to inactive.
                dbSetActiveStatus(isActive);// Change the active status in the database's program's table to inactive.
                return 999;                 // Return a segment past the end.
            }                               // This point does not contain an end time.
            if (nMinutesPast < getElapsedMinutes(p)) {// Are the minutes since the start time less than this point's minutes?
                return p;                   // Return the segment number.
            }                               // The minutes since the start time are greater than this point's minutes.
        }                                   // All the points have been evaluated & the minutes are greater than the last point's.
        isActive = false;                   // Turn this program off.
        dbSetActiveStatus(isActive);        // Change the active status in the database's program's table to inactive.
        return 999;                         // Return a segment past the end.
    }
 
    float SCProgram::presentSetPoint() {
        float deltaTemp;                    // Will contain the change in the temperature during the present segment.
        float setPoint;                     // Will contain the present set point.
        unsigned int segDuration;           // Will contain the length of the present segment in minutes.
        chrono::time_point<chrono::system_clock> timeNow = chrono::system_clock::now();// Get the present time.
        chrono::duration<double> timeElapsed;// Will contain the time(seconds) into the program.
        unsigned int secondsPast;           // Will contain the seconds into the program formatted to be used for calcs.
        unsigned int secondsIntoSeg;        // Will contain the number of seconds into the present segment.
        float fMinutesIntoSeg;              // Will contain the number of minutes into the present segment.
        if(isActive == false) { return -500.0f; }// An inactive chnl has no setpoint.
        unsigned int segNo = presentSegmentNo();// Get the present segment number.
        if (segNo < 1 || segNo > 20) {      // Is the porgram active with a valid segment?
            return -500.0f;                 // No. Let the caller know the program is inactive.
        }                                   // The program is active with a valid segment.
        float segStartTemp = getTemperature(segNo - 1); // Get the present segment's start temperature.
        unsigned int segStartTime = getElapsedMinutes(segNo - 1);// Get the elapsed minutes at the start of the present segment.
        deltaTemp = getTemperature(segNo) - segStartTemp;       // Calc the change in temperature for the present segment.
        segDuration = getElapsedMinutes(segNo) - segStartTime;  // Calc the change in time(minutes) for the present segment.
        timeElapsed = timeNow - getStartTime();         // Calc the amount of time into the cure cycle.
        chrono::seconds s = chrono::duration_cast<chrono::seconds>(timeElapsed);// Convert it into seconds.
        secondsPast = (unsigned int)s.count();          // Convert it to be used for calcs.
        secondsIntoSeg = secondsPast - segStartTime * 60;// Calc the seconds into the present segment.
        setPoint = segStartTemp;                        // Set the present setpoint to the present segment's start temperature.
        fMinutesIntoSeg = secondsIntoSeg / 60.0f;       // Calc the minutes into the present segment.
        setPoint += fMinutesIntoSeg * (deltaTemp / segDuration);// Increment present set point based on minutes into the segment.
        return setPoint;                                // Provide the caller with the present set point.
    }
    
    void SCProgram::deletePoint(unsigned int pointNo) {
        errorMessage_ = "";                 // Clear the error message.
        if (pointNo > 20) {                 // Is the point number valid?
            errorMessage_ = "The given point number was greater then 20.";// Set the error message.
            return;                         // No. Nothing to delete.
        }                                   // The point number is valid.
        for (unsigned int point = pointNo; point < 20; point++) {// Move each TT point after the deleted point forward one.
            setTemperature(point, getTemperature(point + 1));// Move the temperature forward one point.
            setElapsedMinutes(point, getElapsedMinutes(point + 1));// Move the time forward one point.
            setPriorityNo(point, getPriorityNo(point + 1));//Move the priority number forward onr point.
        }                                   // The TT points have been moved forward.
        setTemperature(20, -500);           // Reset the 20th temperature.
        setElapsedMinutes(20, TimeTempPoint::getTimeLimit());// Reset the 20th elapsed minutes.
        setPriorityNo(20, 999);             // Reset the 20th priority number.
    }

    chrono::time_point<chrono::system_clock> SCProgram::getFinishTime() {
        unsigned int lengthInSecs;                  // Contains the length of the program in seconds.
        lengthInSecs = getElapsedMinutes(lastSegmentNo());// Get the length of the program in minutes.
        lengthInSecs *= 60;                         // Convert the length from minutes to seconds.
        chrono::system_clock::duration programLength((chrono::duration<unsigned int>(lengthInSecs)));
        return startTime_ + programLength;          // Provide the caller with the finish time.
    }

    string SCProgram::getTimeString(unsigned int pointNo) {
        string str;                             // Used to construct the time string.
        string strTime;                         // Will contain the time string.
        if (pointNo > 20) {                     // Is the given point legal?
            return "Error, the point number is greater than 20.";// No, return an error message.
        }                                       // The point number was legal.
        // Convert elapsed time in minutes into hour & minutes.
        unsigned int elapsedTime = getElapsedMinutes(pointNo);//Get the number of minutes up to this point.
        unsigned int hour = elapsedTime / 60;   // Calculate the number of hours in the elapsed minutes.
        unsigned int minute = elapsedTime - hour * 60;// Calculate the number of minutes left in the hour.
        // Convert time to formatted string.
        if (hour < 10) {                        // Single digit for hours?
            strTime = " 0" + to_string(hour);   // Yes, add zero & convert
        }
        else if (hour < 100) {                  // Double digit?
            strTime = " " + to_string(hour);    // Yes, convert.
        }
        else {                                  // Must be triple digit.
            strTime = to_string(hour);          // No space in front.
        }
        if (minute < 10) {						// Single digit for minutes?
            str = ":0" + to_string(minute);     // Yes, prepend 0 & convert.
        }
        else {                                  // No, double digit.
            str = ":" + to_string(minute);      // Convert.
        }
        strTime += str;                         // Build formatted string.
        return strTime;                         // Return the time string.
    }

    string SCProgram::getTypeString() {
        string strType;                         // Contains the control type in string form.
        switch (controlType_) {                 // Create a
            case TYPE_MASTER:                   // string
                strType = "Parent";             // version of
                break;                          // the control
            case TYPE_SLAVE:                    // type.
                strType = "Child";
                break;
            case TYPE_MIMIC:
                strType = "Mimic";
                break;
            case TYPE_CUTOFF:
                strType = "Cutoff";
                break;
            default:                            // There is no
                strType = "None";               // control type.
                break;
        }
        return strType;                         // Return the control type to the called.
    }

    unsigned int SCProgram::length() {
        if (points_.at(1).getTime() == TimeTempPoint::getTimeLimit()) {// Has the time for point 1 been set?
            return 0;                           // No. Therefore, the program length is zero.
        }                                       // The program has been set.
        chrono::system_clock::duration len(getFinishTime() - startTime_);// Calculate the program's length.
        chrono::minutes mins = chrono::duration_cast<chrono::minutes>(len);// Convert it into minutes.
        return (unsigned int)mins.count();      // Provide the program's length in minutes to the caller.
    }

    chrono::time_point<chrono::system_clock> SCProgram::createChronoTime(string strTime) {
        time_t rawtime;                                 // Will contain the current time.
        time( &rawtime );                               // Get the current time.
        struct tm* utc = gmtime ( &rawtime );           // Get the current UTC time (the time in Greenwich) in a tm struct.
        struct tm* ltc = localtime ( &rawtime );        // Get the current local time zone time in a tm struct.
        int hoursFromUTC = ltc->tm_hour - utc->tm_hour; // Calculate the difference in hours between the local time zone and UTC.
        chrono::time_point<chrono::system_clock> tpTime;// Will contain the chrono format for the time in the string.
        time_t tTime;                                   // time_t can be converted to a chrono time.
        struct tm tInfo;                                // Will be used to transfer the string values to a tm structure (time structure);
        tInfo.tm_year = stoi(strTime.substr(0, 4)) - 1900;// Set the "struct tm" year(year zero is 1900).
        tInfo.tm_mon = stoi(strTime.substr(4, 2)) - 1;  // Set the "struct tm" month(month zero is January).
        tInfo.tm_mday = stoi(strTime.substr(6, 2)) - 1; // Set the "struct tm" day.
        tInfo.tm_hour = stoi(strTime.substr(8, 2));     // Set the "struct tm" hour.
        tInfo.tm_min = stoi(strTime.substr(10, 2));     // Set the "struct tm" minute.
        tInfo.tm_sec = 0;                               // Set the "struct tm" second.
        tInfo.tm_gmtoff = hoursFromUTC * 3600;          // The number of seconds you must add to UTC to get local time.
        tInfo.tm_hour += hoursFromUTC;                  // Adjust the hours for UTC.
        if (tInfo.tm_hour < 0) {                        // Are the hours now negative? Was there a day change?
            tInfo.tm_hour += 24;                        // Yes. Add 24 to make them positive.
            tInfo.tm_mday -= 1;                         // But, remove 1 day to prevent the time from changing.
            if (tInfo.tm_mday < 0) {                    // Are the days now negative? Did the month change?
                switch (tInfo.tm_mon) {                 // Yes. Switch based on the month.
                    case 0: //January                   // The month was January.
                        tInfo.tm_mday = 30;             // The day becomes the 30th day (zero based).
                        tInfo.tm_mon = 11;              // The month becomes the 11th month (zero based).
                        tInfo.tm_year--;                // The year decreases by one.
                        break;                          // The adjustment for the day becoming in December is complete.
                    case 1: //February
                    case 3: //April
                    case 5: //June
                    case 7: //August
                    case 8: //September
                    case 10://November
                        tInfo.tm_mday = 30;             // All of these months are preceeded with months having 31 days. (30, zero based)
                        tInfo.tm_mon--;                 // Change to the previous month.
                        break;                          // The adjustment for the day becoming in a month with 31 days is complete.
                    case 2: //March                     // February comes before March and could have either 29 or 28 days.
                        if (tInfo.tm_year % 4 == 0) {   // Is this a leap year?
                            tInfo.tm_mday = 28;         // February has 29 days (28th day zero based).
                        } else {                        // This is not a leap year.
                            tInfo.tm_mday = 27;         // February has 28 days (27th day zero based).
                        } // End of condition to set the correct number of days in February.
                        tInfo.tm_mon--;                 // Change the month to February (zero based).
                        break;                          // The adjustment for the day becoming in February is complete.
                    case 4: //May
                    case 6: //July
                    case 9: //October
                    case 11://November                  // All of these months are preceeded with months having 30 days. (29, zero based)
                        tInfo.tm_mday = 29;             // Set the day to the 29th (zero based, last day) day of the month.
                        tInfo.tm_mon--;                 // Change to the previous month.
                        break;                          // The adjustment for the day becoming in a month with 30 days is complete.
                    default:                            // This should never be called.
                        break;                          // There is nothing to do.
                } // End of switch based on the month.
            } // End of the condition that the change in day caused a change in month.
        } // End of the condition that the change in hour caused a change in day.
        tTime = mktime(&tInfo);                         // Convert the "struct tm" to time_t.
        tpTime = chrono::system_clock::from_time_t(tTime);// Convert the time_t to a chrono::time_point.
/*      Used for diagnostic time display.
        time_t aTime = chrono::system_clock::to_time_t(tpTime);
        string ts = ctime(&aTime);
        ts.resize(ts.size() - 1);
        cout << "The set time is " << ts << endl;
*/
        return tpTime;                                  // Return the chrono::time_point to the caller.
    }
    
    struct tm SCProgram::createTimeStructFromDBString(string strTime) {
        struct tm utcTime;
        string yearStr = strTime.substr(0, 4);      // Start with the year.
        utcTime.tm_year = stoi(yearStr.c_str());    // Convert it to a year number.
        utcTime.tm_year -= 1900;                    // struct tm uses years since 1900.
        string monthStr = strTime.substr(5, 2);     // Next get the month.
        utcTime.tm_mon = stoi(monthStr.c_str());    // Convert it to a month number.
        utcTime.tm_mon -= 1;                        // The DB string contains months from 1 to 12.
        string dayStr = strTime.substr(8, 2);       // Next get the day.
        utcTime.tm_mday = stoi(dayStr.c_str());     // Convert it to a day number.
        string hourStr = strTime.substr(11, 2);     // Next get the hour.
        utcTime.tm_hour = stoi(hourStr.c_str());    // Convert it to an hour number.
        string minStr = strTime.substr(14, 2);      // Finally, get the minute.
        utcTime.tm_min = stoi(minStr.c_str());      // Convert it to a minute number.
        utcTime.tm_sec = 0;
        return utcTime;
    }
    
    string SCProgram::display() {
        string str = "";                                // Will contain the string representation of the program.
        str = "Control   Type: " + to_string(controlType_) + "\n";  // Add the control type.
        str += "Reference Chnl: " + to_string(refChnl_) + "\n";     // Add the reference channel.
        str += "Cutoff  Temper: " + to_string(cutoffTemp_) + "\n";  // Add the Cutoff temperature.
        str += "Maint.  Temper: " + to_string(maintenanceTemp_) + "\n";// Add the maintenance temperature.
        time_t tTime = chrono::system_clock::to_time_t(startTime_); // Get the start time.
        string ts = ctime(&tTime);                      // Convert the start time
        ts.resize(ts.size() - 1);                       // into a string.
        str += "Starting  Time: " + ts + "\n";          // Add the start time.
        tTime = chrono::system_clock::to_time_t(timeCreated_);      // Get the program's creation time.
        ts = ctime(&tTime);                             // Convert the creation time
        ts.resize(ts.size() - 1);                       // into a string.
        str += "Creation  Time: " + ts + "\n";          // Add the creation time.
        str += "Active   State: " + to_string(isActive) +"\n";// Add the active state.
        str += "Version Number: " + to_string(version_) + "\n";// Add the version number.
        str += "Point\tTime\tTemp\tPrNo\n";             // Create a heading for the times, temperatures, and priority numbers.
        int pointNo = 0;                                // Start with the first point.
        while (getElapsedMinutes(pointNo) != TimeTempPoint::getTimeLimit()) {// Go through all the points until the stop time is reached.
            str += "  ";                                // Add formatting spaces.
            if (pointNo < 10) {                         // Add formatting spaces
                str += " ";                             // to right justify the
            }                                           // point number.
            str += to_string(pointNo) + " \t";          // Add the point number.
            unsigned int elapsedMinutes = getElapsedMinutes(pointNo);// Get the time (elapsed minutes).
            if (elapsedMinutes < 10) {                  // Format
                str += "    ";                          // the elapsed
            } else if (elapsedMinutes < 100) {          // minutes
                str += "   ";                           // to
            } else if (elapsedMinutes < 1000) {         // left
                str += "  ";                            // justify
            } else if (elapsedMinutes < 10000) {        // the
                str += " ";                             // number.
            }                                           //
            str += to_string(getElapsedMinutes(pointNo)) + "\t ";// Add the elapsed minutes.
            int nTemperature = (int)getTemperature(pointNo);// Get the temperature.
            if (nTemperature < 10) {                    // Format the
                str += "  ";                            // temperature
            } else if (nTemperature < 100) {            // to left
                str += " ";                             // justify
            }                                           // the number.
            str += to_string(nTemperature) + "\t ";     // Add the temperature.
            str += to_string(getPriorityNo(pointNo++)) + "\n";// Add the priority number.
        }                                               // Done adding the program's points.
        return str;                                     // Return the progrma string to the caller.
    }

    void SCProgram::dbSetActiveStatus(bool status) {
        sqlite3 *db;                            // Create a location for the database.
        string sql;                             // Create a string for SQL commands.
        string tableName = "programs";          // Active status is stored in the programs table.
        sqlite3_stmt *stmt;                     // Will contain the results of the querrys.
        string dbName = getFilePath() + "SCConfig.db";// Create the path plus name for SCConfig.db.
        int active = 1;                         // Byt default, the program is active.
        if(status == false) { active = 0;}      // The caller wants the program to not be active.
        int rc = sqlite3_open(dbName.c_str(), &db);// Open the SCConfig database to store the program's active status.
        if( rc ){                               // Was the database opened?
            errorMessage_ = "Can't open programs in SCConfig.db: " + string(sqlite3_errmsg(db));// Create an error message.
            cout << errorMessage_ << endl;      // Display the error.
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // The database is open.
        sqlite3_busy_timeout(db, 50);       // If the DB is locked wait up to 50 milliseconds before giving up.
        sql = "update " + tableName + " set active = ? where id = ?";// Create a parametric querry to the active column.
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Send the command to the database.
        if (dbCheckForError(rc, db, stmt) == false) {// Did the database's programs table accept the querry?
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // The programs table accepted the querry.
        rc = sqlite3_bind_int(stmt, 1, active); // Set this program's active state.
        if (dbCheckForError(rc, db, stmt) == false) { return; }// Failed. No error is passed because everythig should still work OK.
        rc = sqlite3_bind_int(stmt, 2, id_);    // Provide the program's id to save to the correct record.
        if (dbCheckForError(rc, db, stmt) == false) { return; }// Failed. No error is passed because everythig should still work OK.
        rc = sqlite3_step(stmt);                // Update this program's active status.
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {// Was this program's active status updated?
            // No. Create an error message.
            errorMessage_ = "Unable to set the active status for program " +  to_string(id_)+ ": " + string(sqlite3_errmsg(db));
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
            // In this case sqlite3_finalize will return an error because the previous statement caused a database error.
            sqlite3_close(db);                  // Close the database. There is no need to check for errors because finalize was just called.
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // No errors.
        rc = sqlite3_reset(stmt);               // Clear the paramters to the querry so the another program can be done.
        if (dbCheckForError(rc, db, stmt) == false) { return; }// Failed. No error is passed because everythig should still work OK.
        sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
    }

    bool SCProgram::dbCheckForError(int rc, sqlite3* db, sqlite3_stmt* stmt) {
        if (rc != SQLITE_OK) {                  // Was there an error accessing the relays table in SCConfig.db?
            // Yes. Create an error message.
            errorMessage_ = "Database error using the programs table: " + string(sqlite3_errmsg(db));
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
            // In this case sqlite3_finalize will return an error because the previous statement caused a database error.
            sqlite3_close(db);                  // Close the database. There is no need to check for errors because finalize was just called.
            return false;                       // This database opertion failed.
        }                                       // No errors.
        return true;                            // Let the caller know there were no errors and thus no error processing.
    } // End of function dbCheckForError().

    void SCProgram::dbCreateDefaultProgram(int scChnlDex) {
        int dbSCChnlId = scChnlDex + 1;         // The program's Id is one greater than the scChnl's index.
        sqlite3 *db;                            // Create a location for the database.
        string sql;                             // Create a string for SQL commands.
        string tableName = "programs";          // Active status is stored in the programs table.
        sqlite3_stmt *stmt;                     // Will contain the results of the querrys.
        string dbName = getFilePath() + "SCConfig.db";// Create the path plus name for SCConfig.db.
        int rc = sqlite3_open(dbName.c_str(), &db);// Open the SCConfig database to store the program's active status.
        if( rc ){                               // Was the database opened?
            errorMessage_ = "Can't open programs in SCConfig.db: " + string(sqlite3_errmsg(db));// Create an error message.
            cout << errorMessage_ << endl;      // Display the error.
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // The database is open.
        sqlite3_busy_timeout(db, 50);       // If the DB is locked wait up to 50 milliseconds before giving up.
        sql = "INSERT INTO " + tableName;       // Create an SQL statement to add a default program for the chnl.
        sql += " (id,type,refChnlDex,cutoffTemp,maintenanceTemp,timeToStart,timeCreated,active,version) ";
        sql += "VALUES (" + to_string(dbSCChnlId) + ", 1, -1, 400, 68, '2020-01-01 12:00', '2020-01-01 12:00', 0, 1)";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Send the command to the database.
        if (dbCheckForError(rc, db, stmt) == false) {// Did the database's programs table accept the querry?
            return;                             // No. Error is passed to the caller because everythig should still work OK.
        }                                       // The programs table accepted the querry.
        rc = sqlite3_step(stmt);                // Insert a new program.
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {// Was this new program inserted?
            // No. Create an error message.
            errorMessage_ = "Unable to create a new program for chnl " +  to_string(dbSCChnlId)+ ": " + string(sqlite3_errmsg(db));
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
            sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // No errors.
        sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
        tableName = "programPoints";            // Create the table name for this program's time/temp points.
        rc = sqlite3_open(dbName.c_str(), &db); // Open the SCConfig database to store the program's active status.
        if( rc ){                               // Was the database opened?
            errorMessage_ = "Can't open programPoints in SCConfig.db: " + string(sqlite3_errmsg(db));// Create an error message.
            cout << errorMessage_ << endl;      // Display the error.
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // The database is open.
        sqlite3_busy_timeout(db, 50);           // If the DB is locked wait up to 50 milliseconds before giving up.
        /* Create SQL statement */
        string sqlA = "INSERT INTO " + tableName;
        sqlA += " (programId,time,temp,prNo) ";
        sql = sqlA + "VALUES (" + to_string(dbSCChnlId) + ", 0, 70, 999)";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Send the command to the database.
        if (dbCheckForError(rc, db, stmt) == false) {// Did the database's programs table accept the querry?
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // The programs table accepted the querry.
        rc = sqlite3_step(stmt);                // Insert a new program point.
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {// Was this program's active status updated?
            // No. Create an error message.
            errorMessage_ = "Unable to set program point 1 for chnl " +  to_string(dbSCChnlId)+ ": " + string(sqlite3_errmsg(db));
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
            sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // No errors.
        sql = sqlA + "VALUES (" + to_string(dbSCChnlId) + ", 120, 70, 999)";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Send the command to the database.
        if (dbCheckForError(rc, db, stmt) == false) {// Did the database's programs table accept the querry?
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // The programs table accepted the querry.
        rc = sqlite3_step(stmt);                // Insert a new program point.
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {// Was this program's active status updated?
            // No. Create an error message.
            errorMessage_ = "Unable to set program point 2 for chnl " +  to_string(dbSCChnlId)+ ": " + string(sqlite3_errmsg(db));
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
            sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // No errors.
        sqlA = sqlA + "VALUES (" + to_string(dbSCChnlId) + ", 65000, -500, 999)";
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Send the command to the database.
        if (dbCheckForError(rc, db, stmt) == false) {// Did the database's programs table accept the querry?
            return;                             // No error is passed to the caller because everythig should still work OK.
        }                                       // The programs table accepted the querry.
        rc = sqlite3_step(stmt);                // Insert a new program point.
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {// Was this program's active status updated?
            // No. Create an error message.
            errorMessage_ = "Unable to set program point 3 for chnl " +  to_string(dbSCChnlId)+ ": " + string(sqlite3_errmsg(db));
            cout << errorMessage_ << endl;      // Display the error.
        }                                       // No errors.
        sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
    }

}
