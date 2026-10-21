/*
 * Relays.h
 *
 *  Created on: Jun 27, 2013
 *      Author: markdavis
 */

#ifndef RELAYS_H_
#define RELAYS_H_

using namespace std;

#include <string>
#include <vector>
#include <sys/time.h>
#include <dirent.h>
#include <chrono>
#include <sqlite3.h>
#include <thread>


namespace SCClasses {

class Relay {
public:
	Relay();
	virtual ~Relay();

	/*!
	 * The types of loads which can be connected to relays.
	 */
	enum Load {
		LOAD_ELECTRIC,
        LOAD_STEAM,
        LOAD_CYLINDER,
        LOAD_HYDRATION,
        LOAD_ALARM,
        LOAD_NONE,
	};
	/*!
	 * True if this relay is actually activated.
	 */
	bool isOn;
    /*!
     * This relay's desired state. true = should be turnd on, false = should be turned off ( don't confuse with isOn ).
     */
    bool isWantingOn;
    /*!
     * Contains all the relays which are mimics to this relay.
     */
    vector<Relay*> mimicList;
    /*!
     * The zero based index of this relay.
     */
	unsigned int indexNo;           // This relay's index.
    /*! \returns The demand of this relay including the demands of any relays which may be mimiced to this relay. */
	int getDemandWithMimics();
    /*! \returns This relay's electrical demand(watts). */
    int getDemand();
    /*! \param deamnd This relay's desired electrical demand(watts). */
    void setDemand(int demand);
    /*! \returns This relay's time-on(seconds) when storage occured. */
    long getTimeOnAtStorage(void);
    /*! \param timeOn This relay's time-on(seconds) when storage occured. */
    void setTimeOnAtStorage(double timeOn);
    /*! The date/time when this relay is turned on. It might not be when the relay is first turned on.
     * timeTurnedOnAt is adjusted when the time-on is incremented. */
    chrono::time_point<chrono::system_clock> timeTurnedOnAt;
    /*! \returns The minimum amount of time this relay must stay in it's present state before changing (seconds). */
    int getTimeItMustBeOnOrOff(void);
    /*! \param timeDuration The minimum amount of time this relay must stay in it's present state before changing (seconds). */
    void setTimeItMustBeOnOrOff(int timeDuration);
    /*! \returns This relay's priority. (zero implies no priority, one is the highest priority). */
    int getPriority();
    /*! \param priorityNo This relay's priority. (zero implies no priority, one is the highest priority). */
    void setPriority(int priorityNo);
    /*! \returns This relay's electrical usage(watt-hours). */
    unsigned int getUsage();
    /*! Set this relay's electrical usage.
     * \param usage This relay's electrical usage(watt-hours). */
    void setUsage(unsigned int);
    /*! \returns This relay's pulse meter number(ones based, zero implies no pulse meter). */
    int getPulseMeter(void);
    /*! Set this relay's pulse meter number.
     * \param meterNo This relay's pulse meter number(ones based, zero implies no pulse meter). */
    void setPulseMeter(int meterNo);
    /*! \returns The time this relay has been on(seconds). */
    double getTimeOn(void);
    /*! Sets the time the relay has been on.
     * \param timeOn The time this relay has been on(seconds). */
    void setTimeOn(double timeOn);
    /*! \returns This relay's maximum time allowed on(minutes). */
    double getTimeAllowedOn(void);
    /*! Sets this relay's maximum time allowed on.
     * \param timeAllowedOn This relay's maximum time allowed on(minutes). */
    void setTimeAllowedOn(int timeAllowedOn);
    /*! \returns True if the maximum time-on has been exceeded. */
    bool maximumTimeExceeded(void);
    /*! Called to request this relay be turned on. */
    void requestToTurnOn(void);
    /*! Called to request this relay be turned off. */
    void requestToTurnOff();
    /*! Increment this relay's electrical usage.
     * \param microSecs The number of micro seconds this relay has been on. */
    void incrementUsage(long microSecs);
    /*! Increments this relay's time-on, but only if this relay is activated. */
    void incrementTimeOn(void);
    /*! Display some of the properties of the relay. */
    void display();
    /*! \returns The seconds since this relay's last change of state. */
    double getSecondsSinceStateChange(void);
    /*! \returns This relay's power panel number (0 implies no power panel). */
    int getPanel();
    /*! Set this relay's power panel number (0 implies no power panel).
     * \param panelNo This relay's panel number. */
    void setPanel(int panelNo);
    /*! \returns The load type connected to this relay. */
    Load getConnectedLoad();
    /*! Set the load type connected to this relay.
     * \param type This relay's connected load type. */
    void setConnectedLoad(Load type);
    /*! \returns The degrees this relay's input is allowed to be away from its desired temperature. */
    float getTemperatureDeviation();
    /*! Set the degrees this relay's input is allowed to be away from its desired temperature.
     * \param degrees The degrees this relay's input is allowed to be away from its desired temperature. */
    void setTemperatureDeviation(float degrees);
    /*! Sets this relay's parameters for being turned off now. Should be called when this relay is being turned off. */
    void enteringOffState();
    /*! Sets this relay's parameters for being turned on now. Should be called when this relay is being turned on. */
    void enteringOnState();
    /*! Returns this relay's time-on as a string. */
    std::string getTimeOnString();
    /*! \returns The time-on in seconds since the last storage occurred. */
    short getTimeOnForStorage();
    /*! Remove the given relay from this relay' mimic list.
     *  \param mimicRelay A pointer to one of this relay's mimic relays. */
    bool removeMimic(Relay* mimicRelay);
    
private:
	/*!
	 * The ones based index of the pulse meter ( zero implies no pulse meter ).
	 */
	int pulseMeterNo_;
	/*!
	 * This relay's electric demand in watts.
	 */
	int demand_;
	/*!
	 * The time-on, in seconds, this relay had at the moment storage occurred.
	 */
	double timeOnAtStorage_;
	/*! Contains the date/time this relay last changed state. */
    chrono::time_point<chrono::system_clock> timeStateChanged_;
    /*! The minimum amount of time this relay must stay in it's present state before changing (seconds). */
    int timeItMustBeOnOrOff_;
    /*!
     * The electrical power panel this relay belongs to(zero implies this relay is not in a panel).
     */
    int panelNo_;
    /*!
     * This relay's priority. (zero implies no priority, one is the highest priority)
     * Priority is used in electrical demand limiting to determine which relays to turn off
     * first when an electrical demand limit is going to be exceeded. Relays with the
     * lowest priorities are turned off before relays with the highest priorities.
     */
    int priority_;
    /*!
     * The usage in watt-hours associated with this relay.
     */
    unsigned int usage_;
    /*! The time the relay has been on in seconds. */
    double timeOn_;
    /*!
     * The maximum time the relay is allowed to be on (minutes).
     *  (Zero means no limit)
     */
    int timeAllowedOn_;
	/*!
	 * The type of load connected to this relay.
	 */
    Load connectedLoad_;
    /*!
     * The amount this relay's associated temperature deviates from it's desired value.
     */
    float temperatureDeviation_;
};

/////////////////////////////////////Relays/////////////////////////////////////////////////////////////////

class Relays: public vector<Relay*> {
public:
	Relays();
	virtual ~Relays();
	/*! The list of relays containing electric loads. */
	vector<Relay*> electricRelays;
	/*! returns\ The storage interval for these relays. */
	int getStorageInterval(void);
	/*! Sets the storage interval for these relays.
     * \returns True if the storage interval is successfully set to the given value.
     * \param interval The desired time interval between storing relay history values.
	 * The interval must go evenly into 60. (Ex: 1, 2, 3, 4, 5, 6, 10). */
	bool setStorageInterval(int interval);
    /*! \returns The version of the relay configurations. */
	static int getVersion(void);
    /*! Sets the version of the relay configurations. 
     * \param version The version of the relay configuration. */
	static void setVersion(int version);
    /*! \returns The number of relays in this set of relays. */
	int getNumberOfRelays(void);
    /*! \returns The number of relays with electric loads. */
	int getNumberOfElectricRelays(void);
    /*! \returns The file path to the relay configuration data. */
	string getFilePath(void);
    /*! Sets the path to the relay configuration data.
     * \param path The path to the relay configurtion data. */
	void setFilePath(string path);
	/*! True if the relay configurations have been loaded. */
	static bool areLoaded;
	/*!
     * Returns the demand (in watts) of the relays which are currently energized belonging to the given pulse meter (ones based).
     * \param pulseMeter. The number of the pulse meter (ones based).
     * \returns The current total demand in watts on the given meter.
     */
    int getElectricHeatDemandNow(int pulseMeter);
    /*! Adds a default relay to the end of the list. */
	void add(void);
    /*! Adds the given relay to the vector of relays at the position given by this relay's index.
     * \param relay A pointer to the relay being added to the list. */
    void addRelay(Relay* relay);
    /*! Loads each relay's configuration from the relays table in SCConfig.db.
     * \returns True if each relay's configurations is loaded successfully. */
    bool loadFromDB();
    /*! Saves each relay's time-on and usage to the relays table in SCConfig.db.
     * \returns True if the time-on and usage values are saved successfully. */
    bool saveTimeOnUsageToDB();
    /*! Saves each relay's time-on in the last storage period to the relay's table in SC_tim.db.
     * \returns True if each relay's time-on is saved successfully. */
    bool saveHistoryToDB();
    /*! \returns The number of electrical power panels. */
    int getNumberOfPanels();
    /*! Removes the relay from the position in the list given by the passed nDex.
     * \param nDex The index of the relay to remove from the relays list. */
    void removeRelay(int nDex);
    /*! \returns An error message which provides additional information for a failing function. */
    string getErrorMessage();
    /*! \returns True if there was no error updating the data in the relays table in SCConfig.db
     * \param rc is the result code from performing a database operation.
     * \param db is the pointer to the open database.
     * \param stmt is the compiled sqlite3 querry statment. */
    bool checkRelayDBError(int rc, sqlite3* db, sqlite3_stmt* stmt);
    /*! Writes a message to the console and adds a time stamp.
     *  \param note The message to send to the console. */
    void toLog(string note);

private:
    /*!
     * The version of the file holding the relay configurations.
     */
    static int32_t version_;
    /*!
     * The storage interval in minutes for the relay history.
     * The interval must go evenly into 60. (Ex: 1, 2, 3, 4, 5, 6, 10).
     */
    int storageInterval_;
    /*! The number of relays in this set of relays. */
    int32_t count_;
    /*!
     * The version of the time-on file.
     */
    int32_t versionTimeOn_;
    /*!
     * The version of the usage file.
     */
    int versionUsage_;
    /*!
     * Teh version of the priority file.
     */
    uint16_t versionPriority_;
    /*!
     * The location of the file holding the relay configurations.
     */
    string filePath_;
    /*! Contains an error message which provides additional information if a function fails. */
    string errorMessage_;

};

} /* namespace SCClasses */
#endif /* RELAYS_H_ */
