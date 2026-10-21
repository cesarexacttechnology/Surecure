//
//  Manager.h
//  PlantManager
//
//  Created by Mark Davis on 4/17/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#ifndef __PlantManager__Manager__
#define __PlantManager__Manager__

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "SCInput.h"
#include "Relays.h"
#include "SCChnls.h"
#include "SCSystems.h"
#include "Beds.hpp"
#include "SCPulseMeters.h"
#include "OneWire.h"
#include "Lawson.hpp"
#include "Mini.hpp"
#include "SCSecurity.hpp"
#include <thread>
#include <cmath>
#include <stdlib.h>
#include <sqlite3.h>
#include <sstream>
#include <iomanip>
#include <atomic>
#include "ServerSocket.h"
#include "Socket.h"
#include "SocketException.h"
using namespace std;
using namespace SCClasses;

class Manager {
private:
    /*! The default path to all data/config files. */
    string path_;
    /*! The object contains all sub-systems. */
    SCSystems theSystems_;
    /*! The object contains all sub-systems. */
    SCPulseMeters theMeters_;
    /*! The version of the Plant Manager's configuration file. */
    string version_;
    /*! A list of pointers to the One Wire systems. */
    vector<OneWireEquip*> oneWireSystems_;
    /*! A list of pointers to the Lawson (Full Feature or Micro) systems. */
    vector<LawsonEquip*> lawsonSystems_;
    /*! A list of pointers to the mini sub-systems. */
    vector<MiniEquip*> miniSystems_;
    /*! The part of the control cycle being proccessed. The control cycle runs in parts. */
    int controlCycle_;
    /*! Read the maturity for each SCChnl from the readings table in SCConfig.db.
     * \returns True if all maturities are loaded successfully. */
    bool loadMaturityFromDB();
    /*! The version of the maturity file. */
    int versionMaturity_;
    /*! Contains an error message which provides additional information if a function fails. */
    string errorMessage_;
    /*! Contains the minute of the day when data was last stored. */
    int lastStorageMinute_;
    /*! The number of minutes between storing samples. Legal values are 1, 2, 3, 4, 5, 6, 12, 15, 30, and 60. */
    unsigned int storageInterval_;
    /*! \returns The total usage for all electric heat relays on the meter with the given index in watt-hours.
     *  \param meterDex The zero based index of the pulse meter that is being queried for relay usage. */
    unsigned int getRelayUsageOn(int meterDex);
    /*! Store all data accumulated by the Manager. */
    void storeData();
    /*! Store the input values as a SCLite database file. */
    bool storeValuesAsSQL();
    /*! \returns The current UTC time in a string. */
    string utcTimeNowString();
    /*! The sub-folder of the Plant Manager's working directory used to store temperature data. */
    string temperatureDataPath_;
    /*! The security key for this system. */
    SCSecurity theKey_;
    /*! Set the desired state of the alarm aassociated with the given SCChnl.
     *  \param scChnl A pointer to the channel which needs to have its alarm activated. */
    void startAlarmProcess(SCChnl* scChnl);
    /*! Is a list of all the channels which have inputs. */
    vector<SCChnl*> chnlsWithInputs_;
    /*! Is a list of all the channels which have relays. */
    vector<SCChnl*> chnlsWithRelays_;
    /*! Adds scChnl's relay to the list of mimic relays for scChnl's reference's relay. */
    void adjustMimicList(SCChnl* scChnl);
    /*! \returns The setpoint for the given SCChnl based on its program type.
     *  \param scChnl A pointer to the given SCChnl. */
    float presentSetPoint(SCChnl* scChnl);
    /*! \returns True if a connect was completed.
     *  \param ServerSocket socket - The server socket to use to connect to a serial client (NetCom) device requesting a connection.
     *  \param Socket& miniSocket - The socket returned by the operating system to use to communicate with the serial client requesting a connection. */
    bool connectToSocket(ServerSocket socket, Socket& miniSocket);
    /*! The port number for communication to sub-systems. */
    int socketPort_;
    /*! \returns The IP address in string form for the IP address in the form of an unsigned long integer.
     *  \param add The IP address in the form of an unsigned long integer. */
    string convertIntToIPString(unsigned long add);
    /*! \returns The index of the sub-system which has the given server name.
     *  This only works for mini sub-systems.
     *  \param name The server name obtained from the NetCom device associated with a mini sub-system. */
    int serverNameToSubSystemDex(string name);
    /*! \returns The server name for the given IP address.
     *  Queries the RS232 server using system commands to do get the server name.
     *  If the string is empty, an error occured.
     *  \param The IP address for the RS232 server. */
    string getSerialServerName(string ipAddress);
    /*! \returns The sub-system index (zero based) associated with the server name. -1 indicates the given server name did not match a sub-system.
     *  \param serverName The server name that was obtaing from a network connection. */
    int getSysDexFromServerName(string serverName);
    /*! Checks each mini sub-system to see if it is still operating. If it finds one or more that aren't, it attempts to establish a connection with a client serial device
     *  requesting a connection. If it connects, it assigns the socket it obtains to the mini sub-system with the matching server name (portName). It also removes
     *  the request to sign-on for that mini sub-system. If there is another mini sub-system which is not operating, it repeats the process until it has tried once
     *  for each non-operational mini sub-system. */
    void restoreMinis();
    /*! Clears the database after an alert happens.*/
    void clearAlert();
    /*! Writes a message to the console and adds a time stamp.
     *  \param note The message to send to the console. */
    void toLog(string note);
    
public:
    /*! The default constructor. */
    Manager();
    /*! The default destructor. */
    ~Manager();
    /*! Deallocates everthing used by the Manager. */
    void shutDown();
    /*! The object containing all SCChnls operated by the PlantManager. */
    SCChnls theChnls_;
    /*! The object containing all inputs controlled by the PlantManger. */
    SCInputs theInputs_;
    /*! The object containing all relays operated by the PlantManger. */
    Relays theRelays_;
    /*! The object containing all beds. */
    SCBeds theBeds_;
    /*! Initializes all the data used by the manager.
     * \returns True if everything is initialized sufficiently to run the PlantManager. */
    bool initialize();
    /*! Start each I/O sub-system. 
     * \returns false if the I/O sub-system cannot be initialized. This closes the Plant Manager. */
    bool initiateIOSystems();
    /*! \returns A pointer to the given input.
     *  \ param index The zero based index of the SCInput. */
    SCInput* getSCInput(unsigned int index);
    /*! \returns A pointer to the SCChnl with the given name. It returns a nullptr if there is no SCChnl with that name.
     *  \ param chnlName The SCChnl's name. */
    SCChnl* getSCChnl(string chnlName);
    /*! \returns A pointer to the given relay.
     *  \ param index The zero based index of the relay. */
    Relay* getRelay(unsigned int index);
    /*! Starts testing a relay.
     * \param relayDex The zero based index of the relay to test. */
    void relayTest(unsigned int relayDex);
    /*! Terminates a relay test. */
    void relayTestTerminate();
    /*! \returns The number of relays in the system. */
    unsigned int getNumberOfRelays();
    /*! \returns A pointer to the SCBed with the given name. It returns a nullptr if there is no SCBed with that name.
     *  \ param bedName The SCBed's name.*/
    SCBed* getSCBed(string bedName);
    /*! Loads the PlantManager's configuration.
     * \returns True if the PlantManager's configuration was loaded successfully. */
    bool loadFromDB();
    /*! True if the Plant Manger is running in artificial mode.*/
    bool isArtificialMode;
    /*! True if the readings table in SCConfig.db contains at least enough rows to hold readings for each chnl.*/
    bool isReadingsTableConfigured;
    /*! True if the Plant Manaer is shutting down. */
    atomic<bool> isShuttingDown;
    /*! The one and only main control loop. */
    void control();
    /*! \returns The temperature of the SCChnl with the given name. If the name doesn't exist, -500 is returned.
     *  \param name The name of the channel to retreive the temperature. */
    float getTemperature(string name);
    /*! Returns the number of minutes between storing samples. */
    unsigned int getStorageInterval();
    /*! Set the number of minutes between storing samples. 
     * Legal values are 1, 2, 3, 4, 5, 6, 12, 15, 30, and 60.
     * \returns The value the storage interval was set to.
     * \param value The desired number of minutes between storing samples. */
    unsigned int setStorageInterval(unsigned int value);
    /*! \returns The path to the file containing the Manager's configuration. */
    string getFilePath(void);
    /*! Sets the path to the files containing the Manager's configuration.
     * \param strPath The path to the system configuration file. */
    void setFilePath(std::string strPath);
    /*! Sets the sub-folder containing the Manager's tempearture data files.
     * \param strPath The sub-folder to contain the Manager's temperature data files. */
    void setTemperaturePath(std::string strTTPath);
    /*! Remove this function after developement is complete. It is for testing private functions. */
    void test();
    /*! \returns The path to the file containing the Manager's temperature data. */
    string getTemperatureDataPath(void);
    /*! Is used for testing. May be copied to create code in subsequent applications or classes.
     * \returns The signature stored in the .tt file for the give date & time.
     * \param year The year part of the date.
     * \param month The month part of the date (1-12).
     * \param day The day part of the date (1-31).
     * \param hour The hour part of the time (0-23).
     * \param minute The minute part of the time (0-59). */
    short getTTSignature(int year, int month, int day, int hour, int minute);
    /*! Is used for testing. May be copied to create code in subsequent applications or classes.
     * \returns The value stored in the .tt file for the give date, time, and input index.
     * \param year The year part of the date.
     * \param month The month part of the date (1-12).
     * \param day The day part of the date (1-31).
     * \param hour The hour part of the time (0-23).
     * \param minute The minute part of the time (0-60).
     * \param nDex The index of the desired input's value (zero based). 
     * \throws A string exception when the tt file for the given date and time could not be opened. */
    float getTTValue(int year, int month, int day, int hour, int minute, int nDex);
    /*! Is used for testing. May be copied to create code in subsequent applications or classes.
     * \throws A string exception indicating how the argument was not valid.
     * \returns The value stored in the .tt file for the give date, time, and input index.
     * \param argument A string representation of the date and time to get the deisred input's value. (probably temperature)
     * \ year The year is contained next. It is followed by a comma.
     * \ month The month is contained next. It is followed by a comma.
     * \ day The day  is contained next. It is followed by a comma.
     * \ hour The hour  is contained next. It is followed by a comma.
     * \ minute The minute  is contained next. It is followed by a comma. 
     * \ nDex The input index is contained next. It is zero based. */
    float getTTValue(string argument);
    /*! Is used to get all recorded values at a given time.
     * \returns The values stored in the .tt file for the give date and time.
     * \param argument A string representation of the date and time.
     * \ year The year is contained first. It is followed by a comma.
     * \ month The month is contained next. It is followed by a comma.
     * \ day The day is contained next. It is followed by a comma.
     * \ hour The hour is contained next. It is followed by a comma.
     * \ minute The minute is contained next. It is followed by a comma.
     * \throws A string exception indicating how the argument was not valid. */
    string getTTValues(string argument);
    /*! Is used to get all recorded values at a given time.
     * \returns The values stored in the .tt file for the give input, date, time, and span.
     * \ storageInterval - The number of minutes between temperature samples in miutes. It is followed by a comma.
     * \ actualStartMinute - The actual start minute of the month(perhaps not the same as the desired start minute of the month).
     * \                      It is followed by a comma.
     * \ temperatures - Each temperature up to or just past the desired span of minutes. Each temperature is followed by a comma.
     * \                 If the present time has been passed or the TvT file doesn't exist, the temperature is set to -501.
     * \                 The last temperature is always set to -501.
     * \param argument A string representation of the give input, date, time, and span.
     * \ inputIndex - The index of the input to retrieve the temperatures for.
     * \ year - The year is contained first. It is followed by a comma.
     * \ month - The month is contained next. It is followed by a comma.
     * \ minuteOfTheMonth - The minute of the month is contained next. It is followed by a comma.
     * \ span - The span(in minutes) is contained next. It is followed by a comma.
     * \throws A string exception indicating how the argument was not valid. */
    string getChnlTvTValues(string argument);
    /*! Is set to true as long as the equipment in the Manager is to continue running. */
    bool isToContinueRunning;
    /*! \returns true if the temperature and the set point are both valid.
     * \param temperature The temperature to test for validity.
     * \param setpoint The setpoint associated with the temperature being tested. */
    bool isValidTemeprature(float temperature, float setpoint);
    /*! Sets the pm_replies table in SCConfig to either Success or Error. Error may be followed with a message.
     *  \param result is true if the command was successfully completed or false if there was an error. */
    void dbReply(string result);
    /*! Stores the current UTC time in the pmSettings table. */
    void dbUpdateRunTime();
    /*! Reads the command sent from a Sure Cure application to the pm_alerts table in the SCConfig database.
     * \param number The variable to contain the number associated with this alert. */
    string dbReadAlert(int* number);
    /*! Update the readings table in the database with the latest readings for each chnl. */
    void dbUpdateReadings();
    /*! Checks for any alerts from the users and handles them. */
    void checkAlerts();
    /*! The highest temperature which is considered a valid temperature in Fahrenheit */
    float upperTemperatureLimit;
    /*! The lowest temperature which is considered a valid temperature in Fahrenheit */
    float lowerTemperatureLimit;
    /*! Sets the relays to show as off when the PM shuts down.
     *  This should only be called when the PM is being shut down. */
    void showRelaysOff();
    /*! \returns true  If any one of the mini systems has signaled to stop the Plant Manager by setting isOperationAllowed to false. */
    bool miniSystemRequestingShutDown();
};

#endif /* defined(__PlantManager__Manager__) */
