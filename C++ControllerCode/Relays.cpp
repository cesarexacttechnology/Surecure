/*
 * Relays.cpp
 *
 *  Created on: Jun 27, 2013
 *      Author: markdavis
 */

#include <iostream>
#include <fstream>
#include "Relays.h"
#include <ctime>
#include <algorithm>

namespace SCClasses {



Relay::Relay() {
	connectedLoad_ = Relay::LOAD_NONE;	// The default load is none.
	pulseMeterNo_ = 0;					// By default, there is no pulse meter associated with this relay.
	isOn = false;							// This relay is not turned on yet.
	demand_ = 0;							// This relay does not have an electrical demand.
	timeOnAtStorage_ = 0;					// No recording yet, no time-on yet.
    timeTurnedOnAt = chrono::system_clock::now();// The date and time when the relay was turned on.
	timeItMustBeOnOrOff_ = 5;				// The relay must be on or off for at least 5 seconds.
	indexNo = 0;							// The default index is 0;
    timeStateChanged_ = chrono::system_clock::now();// The date/time the relay changed states is now minus the time
    timeStateChanged_ -= chrono::seconds(timeItMustBeOnOrOff_);// it must be on or off so it can change state right away.
	isWantingOn = false;					// The relay do not want to be on.
	panelNo_ = 0;							// The relay is not associated with an electric power panel.
	temperatureDeviation_ = 0;				// Initially there is no temperature deviation.
	priority_ = 0;							// Relays do not have electric loads by default.
	usage_ = 0;								// No electrical usage.
	timeOn_ = 0;							// The relay has not been on yet or its previous value has not been loaded.
	timeAllowedOn_ = 0;						// The time this relay is allowed on is not limited.
}

Relay::~Relay() {
}

/*
 * Calculates the total demand of this relay plus any relays which are to mimic this one.
 * Returns: The total electrical demand which is switched by this relay.
 */
int Relay::getDemandWithMimics() {
    int totalDemand = getDemand();      // Is used to accumulate the total demand of this relay plus
                                        // any relays which are mimiced to this one.
    for(size_t ndex = 0; ndex < mimicList.size(); ndex++) { // Go through all the relays in the mimic list.
    	Relay* mimicedRelay = mimicList.at(ndex); // Get the relay programmed as a mimic to this relay.
        totalDemand += mimicedRelay->getDemand();// Add this relay's demand to the total.
    }
    return totalDemand;                         // Return the total demand which is switched.
}

/*
 * Returns: The time-on, in seconds, this relay had at the moment storage occurred.
 */
long Relay::getTimeOnAtStorage(void) { return this->timeOnAtStorage_; }

/*
 * Set the time-on in seconds this relay has at the moment storage occurred.
 * Parameter: The time-on, in seconds, this relay has at the moment storage occurred.
 */
void Relay::setTimeOnAtStorage(double timeOnAtStorage) {
    timeOnAtStorage_ = 0;
    if(timeOnAtStorage > 0) {
        timeOnAtStorage_ = timeOnAtStorage;
    }
}
    
/*
 * Returns the time-on in seconds since the last storage occurred. This also resets the timeOnAtStorage_.
 */
short Relay::getTimeOnForStorage() {
    short nTimeOnSinceStorage = 0;      // Contains the number of seconds this relay has been on since its time-on was last stored.
    incrementTimeOn();                  // Bring the time-on up to date.
    long timOn = getTimeOn();           // Get the current time-on.
    long timOnAtStore = getTimeOnAtStorage();// Get the time-on when it was last stored.
    if(timOn > timOnAtStore)            // Has Time-on increased since it was last stored?
    {                                   // Yes.
        nTimeOnSinceStorage = (short)(timOn - timOnAtStore);// Time-on in seconds is the difference
    }                                   // between TimeOn and TimeOnAtStorage.
    setTimeOnAtStorage(timOn);          // Remember the new time-on when storage occurred.
    return nTimeOnSinceStorage;         // Return the time-on in seconds since storage last occurred.
}

/*
 * Returns: The minimum time in seconds this relay must maintain its state.
 */
int Relay::getTimeItMustBeOnOrOff(void) { return timeItMustBeOnOrOff_; }

/*
 * Sets the minimum amount of time this relay must stay in it's present state before changing (seconds).
 * Parameter value: The minimum amount of time this relay must stay in it's present state before changing (seconds).
 */
void Relay::setTimeItMustBeOnOrOff(int timeItMustBeOnOrOff) {
	timeItMustBeOnOrOff_ = 5;
    if(timeItMustBeOnOrOff > 0) {
    	timeItMustBeOnOrOff_ = timeItMustBeOnOrOff;
    }
}

/*
 * 	Returns: This relay's priority. (zero implies no priority, one is the highest priority)
 */
int Relay::getPriority() { return priority_; }

/*
 * Sets this relay's priority. (zero implies no priority, one is the highest priority)
 * Parameter value: The desired priority.
 */
void Relay::setPriority(int priority) {
    priority_ = 0;
    if(priority > 0) {
        priority_ = priority;
    }
}

/*
 * Returns: This relay's electrical usage in watt-hours (if any).
 */
unsigned int Relay::getUsage() { return usage_; }

/*
 * Parameter value: The desired usage in watt-hours.
 */
void Relay::setUsage(unsigned int usage) { usage_ = usage; }

/*
 * Returns: This relay's electrical demand in watts.
 */
int Relay::getDemand() { return demand_; }

/*
 * Set this relay's electrical demand in watts.
 * Parameter value: The desired demand in watts.
 */
void Relay::setDemand(int demand) {
    demand_ = 0;
    if(demand > 0) {
        demand_ = demand;
    }
}

/** Returns: The one based index of the pulse meter ( zero implies no pulse meter ). */
int Relay::getPulseMeter(void) { return pulseMeterNo_; }
/*
 * Sets the one based index of the pulse meter ( zero implies no pulse meter ).
 * Parameter value: The one based index of the pulse meter ( zero implies no pulse meter ).
 */
void Relay::setPulseMeter(int pulseMeter) {
    pulseMeterNo_ = 0;
    if(pulseMeter > 0) {
        pulseMeterNo_ = pulseMeter;
    }
}

/** Returns the time this relay has been on in seconds. */
double Relay::getTimeOn(void) { return timeOn_; }
/*
 * Sets the time this relay has been on in seconds.
 * Parameter value: The time this relay has been on in seconds.
 */
void Relay::setTimeOn(double timeOn) {
    if(timeOn >= 0) {       // Is the desired time-on non-negative?
        timeOn_ = timeOn;   // Go ahead and set the time-on.
    }                       // The time-on is set.
}

/*
 * Returns: The maximum time the relay is allowed to be on (minutes).
 *         (Zero means no limit)
 */
double Relay::getTimeAllowedOn() { return timeAllowedOn_; }
/*
 * Sets the maximum time the relay is allowed to be on (minutes). (Zero means no limit)
 * Parameter value: The maximum time the relay is allowed to be on (minutes).
 */
void Relay::setTimeAllowedOn(int timeAllowedOn) {
    timeAllowedOn_ = 0;
    if(timeAllowedOn > 0) {
        timeAllowedOn_ = timeAllowedOn;
    }
}

/*
 * Determines if the relay's time-on has exceeded the maximum time allowed on.
 * Returns: True if the maximum has been exceeded. Otherwise false.
 */
bool Relay::maximumTimeExceeded(void) {
    if(getTimeAllowedOn() == 0) // Is there a maximum time allowed on?
    {                           // No, this relay can be on as long as it likes.
        return false;           // Thus, the maximum time allowed on has not been exceeded.
    }
    if(getTimeOn() >= getTimeAllowedOn())   // is the time on greater or equal to the maximum allowed?
    {                           // Yes.
        return true;            // The maximum has been exceeded.
    }
    return false;               // The maximum has not been exceeded.
}

/*
 * Call to get the number of seconds since this relay last changed state.
 */
double Relay::getSecondsSinceStateChange(void) {
    chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// Contains the current date/time.
    chrono::duration<double> secondsSinceStateChange = now - timeStateChanged_;// Number of seconds since the last state change.
 	return secondsSinceStateChange.count();			// Return the seconds since the change orf state.
}

/*
 * Called when this relay is suppose to go to the on state. The relay
 * will not change desired state unless minimum time-on requirements
 * have been met. Also turns off relays which exceed the maximum time allowed on.
 */
void Relay::requestToTurnOn() {
    if(getSecondsSinceStateChange() < getTimeItMustBeOnOrOff()) // Has this relay been in its current state long enough?
    {                                       // No, leave it in
        return;                             // its current state.
    }
    if (maximumTimeExceeded() == true)      // Has this relay exceeded the maximum amount it is allowed to be on?
    {                                       // Yes, it should be turned off.
        isWantingOn = false;	            // Request that this relay be turned off.
        for(unsigned int dex = 0; dex < mimicList.size() ; dex++) {	// Go through the mimic list.
        	Relay* relay = this->mimicList.at(dex);// Get a relay in the list
            relay->isWantingOn = false;   	// and request that it turn off.
        }
        return;                             // Nothing else to do to this relay.
    }                                       // Done dealing with Maximum time exceeded.
    isWantingOn = true;                  	// Request this relay to go on, all conditions have been met.
    for(unsigned int dex = 0; dex < mimicList.size() ; dex++) {	// Go through the mimic list.
    	Relay* relay = this->mimicList.at(dex);// Get a relay in the list
        relay->isWantingOn = true;   		// and request that it turn on.
    }
} // Done requesting that this relay and its mimics turn on.

/*
 * Called when this relay is suppose to go to the off state. The relay
 * will not change desired state unless minimum time-on requirements
 * have been met.
 */
void Relay::requestToTurnOff()
{
    if(getSecondsSinceStateChange() < getTimeItMustBeOnOrOff()) // Has this relay been in its current state long enough?
    {                                       // No, leave it in
        return;                             // its current state.
    }
    isWantingOn = false;                 	// Request this relay to go off, all conditions have been met.
    for(unsigned int dex = 0; dex < mimicList.size() ; dex++) {	// Go through the mimic list.
    	Relay* relay = this->mimicList.at(dex);// Get a relay in the list
        relay->isWantingOn = false;   		// and request that it turn off.
    }
} // Done requesting that this relay and its mimics turn off.

/*
 * Increment this relay's electrical usage.
 * Parameter uSecs = The number of microseconds since the last usage update.
 */
void Relay::incrementUsage(long uSecs)
{
    if(uSecs <= 0) {                    // Is the time passed positive?
        return;                        	// No. This is odd. Don't increment the usage.
    }                                   //
    long theUsage;                      // Used to calculate the usage in watt-hours.
    theUsage = (getDemand() * uSecs) / 3600 / 1000000;// Convert watt-microseconds to watt-hours.
    setUsage((unsigned int)theUsage + getUsage());  // Store the updated usage.
}

void Relay::incrementTimeOn(void) {         // Increments this relay's time-on(seconds), but only if this relay is activated.
    chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// Contains the current time.
    if(!isOn) {                        		// Is this relay already off?
        return;                             // Yes, no need to increment time on.
    }
    chrono::duration<double> elapsedSeconds = now - timeTurnedOnAt;// Calculate the time in seconds since the relay was turned on.
    setTimeOn(elapsedSeconds.count() + getTimeOn());// Increment the time on.
    timeTurnedOnAt = now;                           // Store time for next increment.
    if(getConnectedLoad() == Relay::LOAD_ELECTRIC) {// Does this relay have an electrical load?
        incrementUsage(elapsedSeconds.count() * 1000000);// Yes. Increment the electrical usage for the give number of micro seconds.
    } // End of condition this is an electric load.
}

void Relay::display(){
	vector<string> loadType;
	loadType.push_back("electric");
	loadType.push_back("steam");
	loadType.push_back("cylinder");
	loadType.push_back("hydration");
	loadType.push_back("alarm");
	loadType.push_back("none");
	cout << "This relay has index " << this->indexNo << "." << endl;
	cout << "The load type is " << loadType.at(getConnectedLoad());
	if(getConnectedLoad() == Relay::LOAD_ELECTRIC){
		cout << " with a demand of "<< this->getDemand();
        cout << " watts, panel " << this->getPanel() << ", and meter " << this->getPulseMeter() << endl;
	}
	else{
		cout << "." << endl;
	}
	cout << "The relay must be on or off for " << this->getTimeItMustBeOnOrOff() << " seconds."<< endl;
	if(this->getTimeAllowedOn() == 0){
		cout << "This relay is allowed to be on for as long as it wants to." << endl;
	}
	else{
		cout << "The relay is allowed to be on for " << this->getTimeAllowedOn() << " minutes." << endl;
	}
    cout << "The relay has been on for " << this->getTimeOn() << " seconds." << endl;
    if (getConnectedLoad() == Relay::LOAD_ELECTRIC) {
        cout << "The relay has a usage of " << getUsage() << " watt-hours." << endl;
    }
}

/*
 * Return this relay's panel number (0 implies there is no power panel).
 */
int Relay::getPanel(){
	return panelNo_;					// Return the number of the power panel associated with this relay.
}
/*
 * Set this relay's power panel number (0 implies there is no power panel).
 */
void Relay::setPanel(int panel){
	panelNo_ = 0;						// By default there is no panel associated with this relay.
	if(panel > 0){					// Is this a positive, thus valid, panel number?
		panelNo_ = panel;				// It is. Set the panel number to the new value.
	}
}
/*
 * Returns the type of load connect to this relay.
 */
Relay::Load Relay::getConnectedLoad(){
    return connectedLoad_;
}
    /*
     * Sets the type of load connected to this relay.
     */
    void Relay::setConnectedLoad(Relay::Load load){
        connectedLoad_ = Relay::LOAD_NONE;// The default load is NONe.
        if (load >= Relay::LOAD_ELECTRIC && load <= Relay::LOAD_ALARM) {// Is the load type valid?
            connectedLoad_ = load;      // Set the load type.
        }                               // The connected load type is set.
    }
    /*
     * Returns the amount the chnl associated with this relay is away from it desired temperature.
     */
    float Relay::getTemperatureDeviation(){
        return temperatureDeviation_;
    }
    /*
     * Set the amount the chnl associated with this relay is away from it desired temperature.
     */
    void Relay::setTemperatureDeviation(float temperatureDeviation){
        temperatureDeviation_ = temperatureDeviation;
    }

    void Relay::enteringOffState(){
        incrementTimeOn();                      // Account for the time the relay has been on.
        timeStateChanged_ = chrono::system_clock::now();// This relay's state is changing now.
        isOn = false;                           // This relay is now off.
    }

    void Relay::enteringOnState(){
        timeTurnedOnAt = chrono::system_clock::now();// This relay is being turned on right now.
        timeStateChanged_ = chrono::system_clock::now();// This relay's state is changing now.
        isOn = true;                            // This relay is now on.
    }
    /*
     * Returns this relay's time-on as a string.
     */
    std::string Relay::getTimeOnString(){
        long secondsOn = getTimeOn();           // Get the number of seconds this relay has been on.
        int secondsOnly = secondsOn % 60;       // Calculate the seconds portion of the time on.
        long timeOnNoSecs = secondsOn - secondsOnly;    // Remove the seconds remainder from the time on.
        long minutesOn = timeOnNoSecs / 60;     // Calculate the minutes on.
        int minutesOnly = minutesOn % 60;       // Calculate the minutes portion of the time on.
        int timeOnHoursLeft = (int)(minutesOn - minutesOnly) / 60;  // Remove the minutes remainder from the time on.
        char str[8];                            // Will contain the characterized digits for a number.
        sprintf(str, "%d:", timeOnHoursLeft);   // create the hour
        std::string strTimeOn = str;            //  portion of the string.
        if(minutesOnly < 10)                    // Are the minutes portion less than 10?
        {                                       // Yes.
            sprintf(str,"0%d:", minutesOnly);   // Put a zero in front of the minutes.
        }
        else                                    // The minutes portion
        {                                       // is greater than ten.
            sprintf(str,"%d:", minutesOnly);    // Create the minutes portion of the TimeOn string.
        }
        strTimeOn.append(str);                  // Add the minutes to the TimeOn string.
        if(secondsOnly < 10)                    // Are the seconds less than 10?
        {                                       // Yes.
            sprintf(str,"0%d", secondsOnly);    // Put a zero in front of the seconds.
        }
        else                                    // The seconds portion
        {                                       // is greater than ten.
            sprintf(str,"%d", secondsOnly);     // Create the seconds portion of the TimeOn string.
        }
        strTimeOn.append(str);                  // Add the seconds to the TimeOn string.
        return strTimeOn;                       // Return the string representation of the time-on.
    }

    bool Relay::removeMimic(Relay* mimicRelay) {
        for (int index = 0; index < mimicList.size(); index++) {// Go through each relay in the mimic list.
            Relay* mimic = mimicList.at(index);             // Access this mimic from the list.
            if (mimic->indexNo == mimicRelay->indexNo) {    // Does this mimic have the same index as the one to remove?
                mimicList.erase(mimicList.begin() + index); // Yes. Remove the mimic at the given index from the list.
            }
        }
        return true;
    }
    
#pragma mark - Relays class implementation
/////////////////////////////////Relays//////////////////////////////////////////////////////////////

//int Relays::version_ = 1;                 // Set on 7/6/2013.
int32_t Relays::version_ = 2;               // Set on 3/29/2014 to accomidate the demand being sotored as an int instead of a float.
bool Relays::areLoaded = false;             // The relay configurations have not been loaded yet.
    
Relays::Relays() {
	// TODO Auto-generated constructor stub
	versionTimeOn_ = 1;			// Set on 7/6/2013.
	versionUsage_ = 1;			// Set on 7/6/2013.
    versionPriority_ = 1;       // Set on 5/23/2014.
	filePath_ = "";				// The default path is the current directory.
    storageInterval_ = 6;       // The default storage interval is 6 minutes.
    count_ = 0;                 // To start out there are no relays.
}

Relays::~Relays() {
	for(unsigned int i = 0; i < this->size(); i++){	// Go through each relay.
		delete this->at(i);								// Free the memory allocated to it.
	}
	this->clear();										// Clear all the relays.
}

/*
 * Returns the storage interval for these relays.
 */
int Relays::getStorageInterval() {
	return storageInterval_;
}

bool Relays::setStorageInterval(int interval) {
	if(interval <= 0) {				// Is this interval zero or less?
        toLog("The storage interval couldn't be reset, the desired value was zero or less.");
		return false;				// Yes. This is illegal. Don't change the interval.
	}
	int newInterval = interval;		// Set the proposed new interval.
	int r = 60 % newInterval;		// Find the remainder when 60 is divided by the new interval.
	if(r != 0) {					// Is it other than zero?
        toLog("The storage interval couldn't be reset, the desired value didn't go into 60.");
		return false;				// Yes. This is illegal. Don't change the interval.
	}
	storageInterval_ = newInterval;	// Set the new interval.
    return  true;                   // the storage interval was set successfully.
}

/*
 * Returns the number of relays.
 */
int Relays::getNumberOfRelays() {
    return count_;      // Return the number of relays.
}

/*
 * Return the number of relays with electric loads.
 */
int Relays::getNumberOfElectricRelays() {
	return (int)this->electricRelays.size();
}


/*
 * Returns the version of the relay configurations file.
 */
int Relays::getVersion(){
	return (int)version_;
}

/*
 * Sets the version of the version of the relay configurations file.
 */
void Relays::setVersion(int newVersion){
	version_ = (int32_t)newVersion;
}


/*!
 * Return the path to the relay configurations file.
 * \returns The path to the relay configurations file.
 */
std::string Relays::getFilePath(void){
	return filePath_;
}
    
string Relays::getErrorMessage() {
    return errorMessage_;                       // Provide the caller with the current error message.
}

/*!
 * Sets the file path.
 * \param strPath The desired path to the relays' configuration file.
 */
void Relays::setFilePath(std::string strPath){
    filePath_ = strPath;                        // Set the file path to the file containg the configurations for the systems.
    char slash = strPath.at(strPath.size() - 1);// Get the last character from the path.
    if (slash != '/') {                         // Is the last character a slash?
        filePath_.append("/");                  // No, make the last character a slash.
    }
}

int Relays::getElectricHeatDemandNow(int pulseMeter)
{
    int totalDemand = 0;                        // So far there is no electric demand.
    for(size_t ndex = 0; ndex < this->size(); ndex++) { // Go through each relay in the system.
    	Relay* relay = this->at(ndex);
        if(relay->getConnectedLoad() != Relay::LOAD_ELECTRIC)// Is this relay's load electric?
        {                                       // No.
            continue;                           // Go on to the next one.
        }
        if(relay->getPulseMeter() != pulseMeter) // Does this relay use the given pulse meter?
        {                                       // No.
            continue;                           // Go on to the next one.
        }
        if(relay->isOn == true)                // Is this relay on?
        {                                       // Yes.
            totalDemand += relay->getDemandWithMimics(); // Increment the total demand.
        }
    }                                           // Done going through all of this system's relays.
    return totalDemand;                         // Return the total demand in watts.
}

void Relays::add(){
    Relay* thisRelay = new Relay();				// Create a new default relay.
    thisRelay->indexNo = (int)this->size();		// Adjust its index to go at the end of Relays.
    this->push_back(thisRelay);					// Add it to the end of Relays.
    count_ = (int32_t)this->size();             // Adjust the number of relays to include the new relay.
}

void Relays::addRelay(Relay* theNewRelay){
    int dexNo = theNewRelay->indexNo;           // Get this relay's desired position.
    if(dexNo > this->size()) { dexNo = (int)this->size(); }	// Reset the desired position to the end if it is too big.
    theNewRelay->indexNo = dexNo;					// Set the index number.
    this->insert(this->begin() + dexNo, theNewRelay);// Insert the new relay into the vector.
    for(unsigned int j = dexNo + 1; j < this->size(); j++){	// Adjust each of the following
        this->at(j)->indexNo = j;				// index numbers.
    }                                           // Done incrementing indexes.
    count_ = (int32_t)this->size();                      // Adjust the number of relays to include the new relay.
}
    
bool Relays::loadFromDB() {
    if (getNumberOfRelays() != 0) {         // Is the relay configuration already loaded?
        for(unsigned int i = 0; i < this->size(); i++){// Yes, The existing relays must be deleted so the new
            delete this->at(i);                // relays can be loaded. The memory used for this relay must be set free.
        }                                   //
        this->electricRelays.clear();       // Clear the electric relay list.
        this->clear();                      // Clear all the relays.
    }                                       //
    sqlite3 *db;                            // Create a location for the database.
    int  rc;                                // Create a return variable.
    string sql;                             // Create a string for SQL commands.
    bool result = true;                     // The desired result of reading the Plant Manager's settings is to succeed.
    string dbName = getFilePath() + "SCConfig.db";// Set the configuration database name.
    rc = sqlite3_open(dbName.c_str(), &db); // Open the database named SC_Config.db. It has a table containing each input's configuration.
    if( rc ){                               // Was the database opened?
        errorMessage_ = "Can't open SCConfig.db to load the relay configurations: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);               // Display the error.
        return false;                       // This error prevents loading the relay configurations.
    }                                       // The database is open.
    sqlite3_busy_timeout(db, 50);           // If the DB is locked wait up to 50 milliseconds before giving up.
    string tableName = "relays";            // The table name for the relay configurations.
    sql = "SELECT * from " + tableName;     // Create an SQL statement to read the configuration for each and every relay.
    sqlite3_stmt *stmt;                     // Will contain the results of the querry.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
    if (rc != SQLITE_OK) {                  // Was it successful?
        errorMessage_ = "Can't query the relays table in SCConfig.db: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);               // Display the error.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
        return false;                       // Let the caller know the inputs configurations could not be retrieved from the database.
    }                                       // The inputs configurations were retreived.
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {// Go through each row in the results.
        int nDex  = sqlite3_column_int(stmt, 0) - 1;// The relay's index is in the first column. DB indexes are ones based. We want zeros based.
        float demand = sqlite3_column_double(stmt, 1);// The relay's demand is in the table's second column.
        int cl = sqlite3_column_int(stmt, 2);// The relay's connected load is in the third column.
        int panelNo = sqlite3_column_int(stmt, 3);// The relay's panel number is in the fourth column.
        int priority = sqlite3_column_int(stmt, 4);// The relay's priority is in the fifth column.
        int pmNo = sqlite3_column_int(stmt, 5);// The relay's pulse meter number is in the sixth column.
        int timeAO = sqlite3_column_int(stmt, 6);// The relay's time allowed on is in the seventh column.
        int timeO_O = sqlite3_column_int(stmt, 7);// The relay's time it must be on or off is in the eighth column.
        int timeOn = sqlite3_column_int(stmt, 8);// The relay's time on is in the nineth column.
        int usage = sqlite3_column_int(stmt, 9);// The relay's usage is in the tenth column.
        Relay* theNewRelay = new Relay();   // Allocate memory for a new relay.
        theNewRelay->indexNo = nDex;        // Set this relay's index.
        theNewRelay->setDemand(demand);     // Set this relay's demand.
        theNewRelay->setConnectedLoad((Relay::Load)cl);// Set this relay's connected load.
        theNewRelay->setPanel(panelNo);     // Set this relay's panel number.
        theNewRelay->setPriority(priority); // Set this relay's priority.
        theNewRelay->setPulseMeter(pmNo);   // Set this relay's pulse meter number.
        theNewRelay->setTimeAllowedOn(timeAO);// Set this relay's maximum time allowed in the on state.
        theNewRelay->setTimeItMustBeOnOrOff(timeO_O);// Set this relay's minimum time it must stay on or off.
        theNewRelay->setTimeOn(timeOn);     // Set this relay's time-on.
        theNewRelay->setTimeOnAtStorage(timeOn);// The time-on at storage must be set so that the next time-on between storage times
                                            // can be calculated.
        theNewRelay->setUsage(usage);       // Set this relay's usage.
        addRelay(theNewRelay);              // Add this new relay to the vector of relays.
        if(theNewRelay->getConnectedLoad() == Relay::LOAD_ELECTRIC){// Does this relay have an electric load?
            electricRelays.insert(electricRelays.end(), theNewRelay);// Yes. Add the relay to the electric relay list.
        }                                   // Done processing an electric relay.
    }                                       // The input configurations were read and inputs were created corresponding to the configurations.
    if (rc != SQLITE_DONE) {                // Was there an error going through the configurations?
        // Yes. Create an error message.
        errorMessage_ = "The relay configurations query didn't finish: " + string(sqlite3_errmsg(db));
        toLog(errorMessage_);               // Display the error.
        result = false;                     // The configuration of the relays was not read.
    }                                       // The configuration of the relays query completion result was evaluated.
    sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
    sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
    areLoaded = result;                     // Remember if the relay config is loaded.
    return result;                          // Let the caller know if the relay config was loaded.
}

bool Relays::saveTimeOnUsageToDB() {
    if(this->getNumberOfRelays() == 0) {    // Are there any relays?
        return true;                        // No, there is no relay time-on or usage to save.
    } // End of condition: no relays.
    bool result = true;                     // By default saving the time-on and usage succeeded.
    sqlite3 *db;                            // Create a variable for the database.
    string sql;                             // Create a string for SQL commands.
    string tableName = "relays";            // Time-on and Usage are both stored in the relays table.
    sqlite3_stmt *stmt;                     // Will contain the results of the querrys.
    string dbName = getFilePath() + "SCConfig.db";// Create the path plus name for SCConfig.db.
    //  new SQLiteConnection("Data Source=" + file + ";PRAGMA journal_mode=WAL;")
    //SQLiteConnection sqlConn = new SQLiteConnection("Data Source=" + dbName + ";PRAGMA journal_mode=WAL;");
    //dbName = "Data Source=" + dbName + ";PRAGMA journal_mode=WAL;";
    int rc = sqlite3_open(dbName.c_str(), &db);// Open the SCConfig database to store time-on and usage.
    if( rc ){                               // Was the database opened?
        errorMessage_ = "Can't open SCConfig.db to save timeOn and usage: " + string(sqlite3_errmsg(db));// Create an error message.
        toLog(errorMessage_);               // Display the error.
        return false;                       // Inform the caller that saving the time-on and usage failed.
    }                                       // The database is open.
    sqlite3_busy_timeout(db, 50);           // If the DB is locked wait up to 50 milliseconds before giving up.
    //sqlite3_exec(db, "PRAGMA journal_mode=WAL;", 0, 0, 0);
    sql = "update " + tableName + " set TimeOn = ?, Usage = ? where id = ?";// Create a parametric querry to time-on and usage.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Send the command to the database.
    if (checkRelayDBError(rc, db, stmt) == false) {// Did the database's relays table accept the querry?
        // No. Create an error message.
        errorMessage_ = "SaveTimeOnUsageToDB: The relays table will not accept the UPDATE command: " + string(sqlite3_errmsg(db));
        toLog(errorMessage_);               // Display the error.
        return false;                       // Return to the enless while loop. Perhaps the database accepts the parameters next time.
    }                                       // The relays table accepted the querry.
    for(int nDex = 0; nDex < this->getNumberOfRelays(); nDex++){// Go through each relay.
        Relay* theRelay = this->at(nDex);   // Get access to this relay
        rc = sqlite3_bind_int(stmt, 1, (int)theRelay->getTimeOn());// Set this relay's time-on.
        if (checkRelayDBError(rc, db, stmt) == false) { return false;}// If the time-on couldn't be set return false.
        rc = sqlite3_bind_int(stmt, 2, (int)theRelay->getUsage());
        if (checkRelayDBError(rc, db, stmt) == false) { return false;}// If the usage couldn't be set return false.
        int nRelay = theRelay->indexNo + 1; // Get his relay's number (ones based).
        rc = sqlite3_bind_int(stmt, 3, nRelay);// Set the relay to update.
        if (checkRelayDBError(rc, db, stmt) == false) { return false;}// If the relay number couldn't be set return false.
        rc = sqlite3_step(stmt);            // Update this relay's time-on and usage.
        if(rc == SQLITE_BUSY) {             // Was the relay table being used by another process?
            errorMessage_ = "Relays table was busy and did not accept the update command.";// No. Create an error message.
            toLog(errorMessage_);           // Display the error on the console.
            errorMessage_ = "SQL: " + sql;
            toLog(errorMessage_);
            errorMessage_ = "This relay's data will be updated next time.";
            toLog(errorMessage_);
            this_thread::sleep_for(chrono::milliseconds(10));// Yes. Wait 10 milliSecs to allow the other application to finish.
            continue;                       // Go onto the next chnl.
        }
        if (rc != SQLITE_DONE && rc != SQLITE_ROW) {// Was this relay's time-on and usage updated?
            // No. Create an error message.
            errorMessage_ = "Unable to set the time-on and usage for relay " +  to_string(nRelay)+ ": " + string(sqlite3_errmsg(db));
            toLog(errorMessage_);               // Display the error.
            result = false;                 // This database opertion failed.
            continue;                       // Try the next relay.
        }                                   // No errors.
        rc = sqlite3_reset(stmt);           // Clear the paramters to the querry so the next relay can be done.
        if (rc != SQLITE_OK) {              // Was there an error creating this relay's time on table in SC_tim.db?
            errorMessage_ = "Can't reset " + tableName + " in SCConfig.db: " + string(sqlite3_errmsg(db));// Create an error message.
            toLog(errorMessage_);           // Display the error.
            result = false;                 // This database opertion failed.
            continue;                       // Try the next relay.
        }                                   // No errors.
    }                                       // The loop to store time-on and usage for each relay is complete.
    sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
    sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
    return result;
}

bool Relays::saveHistoryToDB() {
    if(this->getNumberOfRelays() == 0) {    // Are there any relays?
        return true;                        // No, there is no relay time-on or usage to save.
    } // End of condition: no relays.
    sqlite3 *db;                            // Create a location for the database.
    char* zErrMsg = 0;                      // Will contain error messages for an sqlite3_exec error.
    string sql;                             // Create a string for SQL commands.
    string dbName = getFilePath() + "SC_tim.db";// Create the path plus name for SC_tim.db.
    int rc = sqlite3_open(dbName.c_str(), &db);// Open the database named SC_tim.db. It will be created if it doesn't exist.
    if( rc ){                               // Was the database opened?
        errorMessage_ = "Can't open SC_tim.db: " + string(sqlite3_errmsg(db));// Create an error message.
        toLog(errorMessage_);               // Display the error.
        return false;                       // This error prevents writting each relay's time-on to the database.
    }                                       // The database is open.
    sqlite3_busy_timeout(db, 50);       // If the DB is locked wait up to 50 milliseconds before giving up.
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
    for (int nDex = 0; nDex < this->size(); nDex++) {// Go through each relay to store its time-on in the last storage period.
        string tableName = "tim" + to_string(nDex);// Create the table name for this relay.
        sql = "SELECT * from " + tableName + " LIMIT 1"; // Create an SQL statement to see if the table exists.
        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);// Get a row from this inputs table.
        if (rc != SQLITE_OK) {              // Did the desired relays table exist in SC_tim.db?
            // No. Create an error message.
            errorMessage_ = "Table " + tableName +" doesn't exist: " + string(zErrMsg);
            sqlite3_free(zErrMsg);          // Free the resources allocated for handling an sqlite3_exec error.
            toLog(errorMessage_);           // Display the error.
            sql = "CREATE TABLE " + tableName + " (";// Create a SQL statement to create this input's table.
            sql += "id INTEGER PRIMARY KEY,";// The table contains a primary key.
            sql += "sc_time TEXT,";         // The table contains date/time.
            sql += "secondsOn INTEGER );";  // The table also contains this relay's seconds on in the last recording period.
            toLog("The SQL create statement is: " + sql);
            rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);// Send the command to the database.
            if (rc != SQLITE_OK) {          // Was there an error creating this relay's time on table in SC_tim.db?
                errorMessage_ = "Can't create " + tableName + " in SC_tim.db: " + string(sqlite3_errmsg(db));// Create an error message.
                toLog(errorMessage_);       // Display the error.
                sqlite3_free(zErrMsg);      // Free the resources allocated for handling an sqlite3_exec error.
                continue;
            }                               // This relay's table was created.
        }                                   // A table exists for this relay's time on.
        sql = "INSERT INTO " + tableName;   // Create an sql command to store this relay's
        sql += " (sc_time,secondsOn) ";     // time and time-on.
        Relay* thisRelay = this->at(nDex);  // Get access to this relay.
        short timeOn = (short)thisRelay->getTimeOnForStorage();// Get the amount of time it has been on since the last storage.
        sql += "VALUES ('" + timeNow + "', " + to_string(timeOn) + ")";// Add the values of the time and time-on.
        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);// Send the command to the database.
        if (rc != SQLITE_OK) {              // Was there an error stroring this relay's time-on in SC_tim.db?
            errorMessage_ = "Can't store data in " + tableName + " in SC_tim.db: " + string(sqlite3_errmsg(db));// Create an error message.
            toLog(errorMessage_);           // Display the error.
            sqlite3_free(zErrMsg);          // Free the resources allocated for handling an sqlite3_exec error.
            continue;
        }                                   // This relay's time-on in the last period has been updated.
    }                                       // All relays have had their time-on in the last period updated.
    sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
    return true;                            // Let the caller know all relays have had their time-on in the last period updated.
}

bool Relays::checkRelayDBError(int rc, sqlite3* db, sqlite3_stmt* stmt) {
    if (rc != SQLITE_OK) {                  // Was there an error accessing the relays table in SCConfig.db?
        // Yes. Create an error message.
        errorMessage_ = "Database error using the relays table: " + string(sqlite3_errmsg(db));
        toLog(errorMessage_);               // Display the error.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
        // In this case sqlite3_finalize will return an error because the previous statement caused a database error.
        sqlite3_close(db);                  // Close the database. There is no need to check for errors because finalize was just called.
        return false;                       // This database opertion failed.
    }                                       // No errors.
    return true;                            // Let the caller know there were no errors and thus no error processing.
}

int Relays::getNumberOfPanels() {
    int nPanels = 0;                            // There are no panels yet.
    for (std::vector<Relay*>::iterator it = electricRelays.begin() ; it != electricRelays.end(); ++it)
    {                                           // Go through each relay with an electric load.
        Relay* thisRelay = *it;                 // Access this relay.
        if(thisRelay->getPanel() > nPanels) {   // Is this relay's panel greater than the number of panels?
            nPanels = thisRelay->getPanel();    // Yes. Set the number of electric panels to the larger number.
        }                                       // The number of panels has been tested and possibly adjusted.
    }                                           // The number of electrical panels has been determined.
    return nPanels;                             // Returns the number of electrical power panels.
}

void Relays::removeRelay(int i) {
    if (i < 0 || i >= count_) {                     // Is this a valid relay number?
        return;                                     // No, there is nothing to remove.
    }                                               //
    delete this->at(i);								// Free the memory allocated to the relay.
    this->erase(this->begin() + i);					// Remove the relay from the vector of relays.
    count_ = (int32_t)this->size();                 // Reset the number of relays.
    for (std::vector<Relay*>::iterator it = this->begin() + i; it != this->end(); ++it)
    {   // Go through each of the relays following the removed relay and decrement their indexes.
        Relay* aRelay = *it;                        // Get access to the relay.
        aRelay->indexNo--;                          // Decrement its index.
    }
}

void Relays::toLog(string note) {
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string time = "";
//    time += to_string(1900 + ltm->tm_year) + "/";
    int theMonth = ltm->tm_mon + 1;
    if(theMonth < 10) {
        time += "0";
    }
    time += to_string(theMonth) + "/";
    if(ltm->tm_mday < 10) {
        time += "0";
    }
    time += to_string(ltm->tm_mday) + "_";
    int theHour = ltm->tm_hour;
    if(theHour < 10) {
        time += "0";
    }
    time += to_string(theHour) + ":";
    int theMin = ltm->tm_min;
    if(theMin < 10) {
        time += "0";
    }
    time += to_string(theMin) + ":";
    int theSec = ltm->tm_sec;
    if(theSec < 10) {
        time += "0";
    }
    time += to_string(theSec);
    cout << time << " " << note << endl;       // Send the time and message to the console.
}


} /* namespace SCClasses */
