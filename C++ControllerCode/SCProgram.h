//
//  SCProgram.h
//  PlantManager
//
//  Created by Mark Davis on 6/30/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#ifndef PlantManager_SCProgram_h
#define PlantManager_SCProgram_h

#include <vector>
#include <string>
#include <chrono>
#include <algorithm>
#include <sqlite3.h>
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

namespace SCClasses {

    class TimeTempPoint {
    private:
        /*! This point's number (0 through 20 are valid). */
        unsigned int pointNo_;
        /*! The elapsed time in minutes. */
        unsigned int time_;
        /*! The temperature in degrees Fahrenheit. */
        float temp_;
        /*! The priority number.
         * One is the most important. A high number is less important than a low number. */
        unsigned int priorityNo_;
        /*! The time which indicates the program is over. Also, one minute more than the largest valid time. */
        static unsigned int timeLimit_;
        
    public:
        TimeTempPoint();
        TimeTempPoint(unsigned int pointNo, unsigned int time, float temp, unsigned int priority);
        virtual ~TimeTempPoint();
        /*! True if the temperatures are to be passed in Fahrenheit. */
        static bool isMeasuredInFahrenheit;
        /*! \returns The point number. */
        unsigned int getPointNo();
        /*! Sets this point's number. Zero through twenty are valid.
        * \param pointNo This point's number. */
        void setPointNo(unsigned int pointNo);
        /*! \returns This point's time in elapsed minutes. */
        unsigned int getTime();
        /*! \returns The hour component of the elapsed minutes since the start of the programmed curve.*/
        unsigned int getElapsedHour();
        /*! \returns The minute component of the elapsed minutes since the start of the programmed curve.*/
        unsigned int getElapsedMinute();
        /*! Sets this point's time in elapsed minutes.
         * \param elapsedMinutes The minutes elapsed since the start of the programmed curve. */
        void setTime(unsigned int elapsedMinutes);
        /*! Sets this point's time in elapsed minutes.
         * \param elapsedHour The hour component of the elapsed minutes since the start of the programmed curve.
         * \param elapsedMinute The minute component of the elapsed minutes since the start of the programmed curve. */
        void setTime(unsigned int elpasedHour, unsigned int elapsedMinute);
        /*! \returns This point's temperature.
         * The units are Fahrenheit if isMeasuredInFahrenheit is true, otherwise the units are in Celsius. */
        float getTemperture();
        /*! Sets this point's temperature in degrees Fahrenheit.
         * If isMeasuredInFahrenheit is false, the given temperature is assumed to be in Celsisus 
         * and will be converted to Fahrenheit.
         * \param temperture This point's temperature.
         * The units are Fahrenheit if isMeasuredInFahrenheit is true, otherwise the units are in Celsius. */
        void setTemperature(float temperature);
        /*! \returns This point's priority number. */
        unsigned int getPriortyNo();
        /*! Sets this point's priority number. 
         * \param priorityNo This point's priority number. */
        void setPriorityNo(unsigned int priorityNo);
        /*! \returns A minute one greater than the largest valid minute. */
        static unsigned int getTimeLimit();
    };
    
    class CutoffPoint {
    private:
        /*! The cutoff temp in hundreths of a degree Fahrenheit. */
        float cutoffTemp_;
        /*! The maintenance temp in hundreths of a degree Fahrenheit. */
        float maintTemp_;
    public:
        CutoffPoint();
        virtual ~CutoffPoint();
        /*! True if the temperatures are to be passed in Fahrenheit. */
        bool isMeasuredInFahrenheit;
        /*! \returns The cutoff temperature.
         * The units are Fahrenheit if isMeasuredInFahrenheit is true, otherwise the units are in Celsius. */
        float getCutoffTemp();
        /*! Sets the cutoff temperature in degrees.
         * \param cutoffTemp The cutoff temperature in degrees.
         * The units are Fahrenheit if isMeasuredInFahrenheit is true, otherwise the units are in Celsius. */
        void setCutoffTemp(float cutoffTemp);
        /*! \returns The maintenance temperature.
         * The units are Fahrenheit if isMeasuredInFahrenheit is true, otherwise the units are in Celsius. */
        float getMaintenanceTemp();
        /*! Sets the maintenance temperature in degrees.
         * \param maintenanceTemp The maintenance temperature in degrees.
         * The units are Fahrenheit if isMeasuredInFahrenheit is true, otherwise the units are in Celsius. */
        void setMaintenanceTemp(float maintenanceTemp);
        /*! Sets the cuttof and maintenance temperatures in degrees.
         * \param cutoffTemp The cutoff temperature in degrees.
         * \param maintenanceTemp The maintenance temperature in degrees.
         * The units are Fahrenheit if isMeasuredInFahrenheit is true, otherwise the units are in Celsius. */
        void set(float cutoffTemp, float maintenanceTemp);
    };
    
    class SCCutoffs: public std::vector<CutoffPoint*> {
    private:
        /*! The version of the file holding the cutoff settings. */
        int version_;
        /*! The location of the file holding the cutoff settings. */
        string filePath_;
        /*! The name of the files contains the cutoff settings. */
        string fileName_;
        /*! The number of SCChnls. */
        unsigned int nSCChnls_;
        /*! Is true if the cutoff data is stored in CutoffSetting.SC. */
        bool isSCFile_;
        /*! Is true if the cutoff data is stored in Cuttoffs.xml. */
        bool isXMLFile_;
        
    public:
        SCCutoffs();                                // Default constructor.
        virtual ~SCCutoffs();
        /*! True if the temperatures are to be passed in Fahrenheit. */
        bool isMeasuredInFahrenheit;
        /*! Loads the cutoff settings.
         * \returns True if cutoff setting are loaded successfully.
         */
        /*! \returns The system's version. */
        int getVersion();
        /*! \returns The path to the file containing the cutoff settings. */
        string getFilePath(void);
        /*! Sets the path to the file containing the cutoff settings.
         * \param strPath The path to the cutoff settings file. */
        void setFilePath(std::string strPath);
        /*! \returns The cutoff temperature for the given point.
         * The units are Fahrenheit if isMeasuredInFahrenheit is true, otherwise the units are in Celsius. 
         * \param index The index to set the temperatures. The index corresponds to the SCChnl index(zero based). */
        float getCutoffTemp(unsigned int index);
        /*! \returns The maintenance temperature for the given point.
         * The units are Fahrenheit if isMeasuredInFahrenheit is true, otherwise the units are in Celsius.
         * \param index The index to set the temperatures. The index corresponds to the SCChnl index(zero based). */
        float getMaintenanceTemp(unsigned int point);
        /*! Sets the cuttof and maintenance temperatures in degrees for the given point.
         * \param index The index to set the temperatures. The index corresponds to the SCChnl index(zero based).
         * \param cutoffTemp The cutoff temperature in degrees.
         * \param maintenanceTemp The maintenance temperature in degrees.
         * The units are Fahrenheit if isMeasuredInFahrenheit is true, otherwise the units are in Celsius. */
        void set(unsigned int index, float cutoffTemp, float maintenanceTemp);
        /*! Sets the number of SCChnls. 
         * \param nSCChnls The number of SCChnls. */
        void setNumberOfSCChnls(unsigned int nSCChnls);
        /*! \returns True if any cutoff setting has been loaded. */
        bool areLoaded();
        /*! \returns True if the cutoff data is stored in a file ending with .SC. */
        bool isStoredInSCFormat();
        /*! \returns True if the cutoff data is stored in an XML file. */
        bool isStoredInSureCureFormat();
    };
    

    class SCProgram {

    public:
        SCProgram();
        virtual ~SCProgram();
        /*! Is set to true to display any program in Fahrenheit and false to display any program in Celsius. */
        static bool isDisplayedInFahrenheit;
        /*! The number of minutes between the current time zone and UTC. */
        static int timeZoneOffset;
        /*! The name of the object that created this program. */
        string owner;
        /*! \returns The path to the file containing this program. */
        static string getFilePath(void);
        /*! Sets the path to the file containing this program.
         * \param strPath The path to this program file. */
        static void setFilePath(string strPath);
        /*! \returns The index of the channel this program is referenced to. */
        unsigned int getRefChnl();
        /*! Sets the channel this program is referenced to. 
         * \param channel The index of the channel this program is referenced to. */
        void setRefChnl(unsigned int channel);
        /*! \returns An error message which provides additional information for a failing function. */
        string getErrorMessage();
        /*! \returns The date/time when this program starts. */
        chrono::time_point<chrono::system_clock> getStartTime();
        /*! Sets the date/time when this program starts. 
         * \param startTime The date/time when this program starts. */
        void setStartTime(chrono::time_point<chrono::system_clock> startTime);
        /*! Sets the date/time when this program starts.
         * \param year The year when this program starts. Years must be greater than 1900.
         * \param month The month when this program starts. Months are between 0 and 11.
         * \param day The day when this program starts. Days are between 1 and 31.
         * \param hour The hour when this program starts. Hours are between 0 and 23.
         * \param minute The minute when this program starts. Minutes are between 0 and 59. */
        void setStartTime(int year, int month, int day, int hour, int minute);
        /*! The types of control to be used by this program . */
        enum Type {
            TYPE_MASTER = 1,
            TYPE_SLAVE,
            TYPE_CUTOFF,
            TYPE_MIMIC,
            TYPE_NONE,
        };
        /*! \returns The type of control this program uses. */
        Type getControlType();
        /*! Sets the type of control this program uses. 
         * \param controlType The type of control this program uses. */
        void setControlType(Type controlType);
        /*! Is true if this is an active program. By default programs are assumed to be active. */
        bool isActive;
        /*! \returns This program's maintenance temperature. */
        float getMaintenanceTemperature();
        /*! Sets this program's maintenance temperature. 
         * \param maintenanceTemp The desired maintenance temperature. */
        void setMaintenanceTemperature(float maintenanceTemp);
        /*! \returns This program's cutoff temperature. */
        float getCutoffTemperature();
        /*! Sets this program's cutoff temperature.
         * When a channel is programmed as a "cutoff", it follows it's program
         * until it's reference's temperature exceeds "cutoffTemp_",
         * then the channel stops following it's program and uses
         * "maintenanceTemp_" as a setpoint. If the reference's temperature
         * falls back below "cutoffTemp_" the channel resumes following it's program.
         * \param cutoffTemp The desired cutoff temperature. */
        void setCutoffTemperature(float cutoffTemp);
        /*! Initializes this program's points to off. */
        void initializeTimeTemp();
        /*! Initializes this program's points to off its start date to 1990, and its time created to now. */
        void initialize();
        /*! \returns This program's last active segment number. */
        int lastSegmentNo();
        /*! Loads a program from the database SCConfig.db.
         * \param chnlDex The zero based index of the channel which will have its program loaded.
         * \returns True if this program is successfully loaded. */
        bool loadFromDB(int chnlDex);
        /*! \returns The present program segment(0 means the program hasn't started). */
        unsigned int presentSegmentNo();
        /*! \returns The program setpoint. For a parent program, this is the actual setpoint.
         *  However, for any other program type, it is necessary to modify the program setpoint
         *  based on the reference chnl's temperature.
         *  If there is no program setpoint, -500 is returned to indicate the program is not running. */
        float presentSetPoint();
        /*! Removes a TimeTempPoint from this program. 
         * \param pointNo The number of the TimeTempPoint to remove. */
        void deletePoint(unsigned int pointNo);
        /*! \returns The time that this program finishes. */
        chrono::time_point<chrono::system_clock> getFinishTime();
        /*! Used to get this program's time (minutes since the start time) for the given point.
         * \param pointNo The point number of the TimeTempPoint to be accessed.
         * \returns The time of the selected TimeTempPoint. */
        unsigned int getElapsedMinutes(unsigned int pointNo);
        /*! Used to set this program's time (minutes since the start time) for the given point.
         * \param pointNo The point number of the TimeTempPoint to be accessed.
         * \param elapsedMinutes The number of minutes since the start time.
         * \returns True if the time of the selected TimeTempPoint could be set. */
        bool setElapsedMinutes(unsigned int pointNo, unsigned int elapsedMinutes);
        /*! Used to get this program's temperature for the given point.
         * \param pointNo The point number of the TimeTempPoint to be accessed.
         * \returns The tempearture of the selected TimeTempPoint. */
        float getTemperature(unsigned int pointNo);
        /*! Used to set this program's temperature for the given point.
         * \param pointNo The point number of the TimeTempPoint to be accessed.
         * \param temperature The value of the temperature to be set.
         * \returns True if the temperature of the selected TimeTempPoint could be set. */
        bool setTemperature(unsigned int pointNo, float temperature);
        /*! Used to get this program's priority number for the given point.
         * \param pointNo The point number of the TimeTempPoint to be accessed.
         * \returns The priority number of the selected TimeTempPoint. */
        unsigned int getPriorityNo(unsigned int pointNo);
        /*! Used to set this program's priority for the given point.
         * \param pointNo The point number of the TimeTempPoint to be accessed.
         * \param prNo The value of the priority number to be set.
         * \returns True if the priority number of the selected TimeTempPoint could be set. */
        bool setPriorityNo(unsigned int pointNo, unsigned int prNo);
        /*! \returns The time for the give TimeTempPoint as a string.
         * The format is "eh:em" where eh is elapsed hours and em is elapsed minutes.
         * \param pointNo The point number of the TimeTempPoint to be accessed. */
        string getTimeString(unsigned int pointNo);
        /*! \returns This program's control type as a string. */
        string getTypeString();
        /*! \returns The length of this program in minutes. */
        unsigned int length();
        /*! Converts the given string time into a chrono::time_point which can be stored as part of a program.
         *  The string representation is yyyymmddhhii where yyyy is the alpha-numeric year(exactly four digits).
         *  mm is the alpha-numeric month(exactly two digits, 1 - 12).
         *  dd is the alpha-numeric day(exactly two digits, 1 - 31).
         *  hh is the alpha-numeric hour(exactly two digits, 0 - 23).
         *  ii is the alpha-numeric minute(exactly two digits, 0 - 59).
         * \returns A chrono::time_point which can be used in a program.
         * \param theTime A string time to convert to a chrono::time_point which can be used in a program. */
        chrono::time_point<chrono::system_clock> createChronoTime(string theTime);
        /*! Converts the given string time from the database into a struct tm.
         *  The string representation is yyyy-mm-dd?hh:ii where yyyy is the alpha-numeric year(exactly four digits) followed by a dash.
         *  mm is the alpha-numeric month(exactly two digits, 1 - 12) followed by a dash.
         *  dd is the alpha-numeric day(exactly two digits, 1 - 31) followed by either a space or a 'T'.
         *  hh is the alpha-numeric hour(exactly two digits, 0 - 23) followed by a colon.
         *  ii is the alpha-numeric minute(exactly two digits, 0 - 59).
         *  The database stores the time as UTC.
         * \returns A struct tm time which can be used to get the local time.
         * \param strTime A string time (UTC) to convert to a struct tm which can be used to get its local time. */
        struct tm createTimeStructFromDBString(string strTime);
        /*! Sets the left program object equal to the right program object. */
        SCProgram& operator=(SCProgram other);
        /*! Displays the program. */
        string display();

    private:
        /*! The name of the file used to store this program. */
        string fileName_;
        /*! The location of the file holding this program. */
        static string filePath_;
        /*! Contains the index (zero based) of the channel this program is referenced to.
            A reference index of 9999 indicates there is no reference chnl. */
        unsigned int refChnl_;
        /*! Contains an error message which provides additional information if a function fails. */
        string errorMessage_;
        /*! The set of time-temperature points which define the program. */
        vector<TimeTempPoint>points_;
        /*! Contains the date/time when this program starts. */
        chrono::time_point<chrono::system_clock> startTime_;
        /*! The database (ones based) id for this program. */
        int id_;
        /*! The type of control to which this program applies. */
        Type controlType_;
        /*! The time this program was created. */
        chrono::time_point<chrono::system_clock> timeCreated_;
        /*! The version of the program (used for program files). */
        unsigned int version_;
        /*! This program's maintenance temperature. */
        float maintenanceTemp_;
        /*! This program's cutoff temperature.
		 * When a channel is programmed as a "cutoff", it follows it's program until it's
		 * reference's temperature exceeds "cutoffTemp_", then the channel stops following it's
		 * program and uses "maintenanceTemp_" as a setpoint. If the reference's temperature
		 * falls back below "cutoffTemp_" the channel resumes following it's program. */
        float cutoffTemp_;
        /*!Sets the active status for the given program ID in the programs DB table.
         * \param status Is true if the program is active and false if it is not.   */
        void dbSetActiveStatus(bool status);
        /*! \returns True if there was no error updating the data in the programs table in SCConfig.db
         * \param rc is the result code from performing a database operation.
         * \param db is the pointer to the open database.
         * \param stmt is the compiled sqlite3 querry statment. */
        bool dbCheckForError(int rc, sqlite3* db, sqlite3_stmt* stmt);
        /*! Creates a default program in the database. It ignores errors.
         * \param scChnlDex is the index of the scChnl to add a default program. */
        void dbCreateDefaultProgram(int scChnlDex);


    };
    
}


#endif
