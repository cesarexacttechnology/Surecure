/*
 * SCInput.cpp
 *
 *  Created on: Jun 10, 2013
 *      Author: markdavis
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "SCInput.h"
#include "SCMaturity.h"
#include <algorithm>

namespace SCClasses {

std:: string SCInputs::filePath_ = "";
bool SCInputs::areLoaded = false;						// The inputs have not been loaded yet.

SCInput::SCInput() {
	// TODO Auto-generated constructor stub
	measuredValue_ = 0;			// The default measured value is zero.
	measuredUnits = SCInput::UNITS_FAHRENHEIT;// Fahrenheit is the default units.
	indexNo = 0;				// The default index is zero.
	sensorType = SCInput::SENSOR_THERMOCOUPLE_T;	// The default sensor is the Type T thermocouple.
	accumulatedValue_ = 0;		// Initially there is no accumulated value.
	accumulatedCount_ = 0;		// Initially no measured values have been added to the accumulated value.
    id_ = "________________";   // The initial ID is sixteen underscores.
    systemNo_ = 0;              // Zero means the sub-system number has not been assigned.
}

SCInput::~SCInput() {
	// TODO Auto-generated destructor stub
}

/*
 * Returns this input's most recent measured value.
 */
double SCInput::getMeasuredValue(void){
	return measuredValue_;
}

/*
 * Sets this input's most recent measured value.
 */
void SCInput::setMeasuredValue(double value){
	this->measuredValue_ = value;
}
    
string SCInput::getID() {
    return id_;                 // Provide the caller with this input's ID.
}

void SCInput::setID(string idString) {
    id_ = idString.substr(0, 16);           // Don't allow more than 16 characters for the ID.
    while (id_.length() < 16) {             // If the ID is less than 16 add
        id_.append("_");                    // "_" underscore characters to the end.
    }                                       // The ID is exactly 16 characters long.
}
    
int SCInput::getSystemNo() {
    return systemNo_;                    // Provide the caller with this input's sub-system index.
}
    
void SCInput::setSystemIndex(int value) {
    if (value > 0) {                        // Is the value ones based?
        systemNo_ = value;               // Yes. Set the system index.
    }
}

/*
 * Increment this input's accumulated value by the current measured value.
 */
void SCInput::incrementAccumulatedValue(){
	accumulatedValue_ += measuredValue_;
	accumulatedCount_++;
}

/*
 * Resets this input's accumulated value so a new average value can be calculated.
 */
void SCInput::resetAccumulatedValue(){
	accumulatedValue_ = 0;
	accumulatedCount_ = 0;
}

/*
 * Returns this input's average measured value since the last time this
 * input's accumulated value was reset.
 */
double SCInput::getMeasuredValueAverage(){
	if(accumulatedCount_ == 0){
		return measuredValue_;
	}
	return accumulatedValue_ / accumulatedCount_;
}

void SCInput::display(){
	vector<string> inputType;
	inputType.push_back("none");
	inputType.push_back("Type T Thermocouple");
	inputType.push_back("Type K Thermocouple");
	inputType.push_back("Current Transformer");
	inputType.push_back("Voltage Transducer");
	inputType.push_back("Pressure Transducer");
	inputType.push_back("One-Wire");
    vector<string> units;
    units.push_back("none");
    units.push_back("Fahrenheit");
    units.push_back("Celsius");
    units.push_back("Amps");
    units.push_back("Volts");
    units.push_back("PSI");
	cout << "This input has index " << this->indexNo << endl;
	cout << "The sensor type is " << inputType.at(sensorType) << endl;
	cout << "The units are " << units.at(measuredUnits) << endl;
}


///////////////////////////////////////TemperatureInput/////////////////////////////////////////////////////

TemperatureInput::TemperatureInput(){

}

TemperatureInput::~TemperatureInput(){

}


///////////////////////////////////////PressureInput////////////////////////////////////////////////////////
PressureInput::PressureInput(){
	k1 = 0;				// Constants for converting a voltage to a pressure.
	k2 = 0;				// Pressure = k1 + k2 * volts.
}

PressureInput::~PressureInput(){

}

//////////////////////////////////////////SCInputs/////////////////////////////////////////////////////////
    
SCInputs::SCInputs(){
    version_ = 1;                           // The default configuration file version is one.
    errorMessage_ = "";                     // There is no error.
}

SCInputs::~SCInputs(){
	for(unsigned int i = 0; i < this->size(); i++){	// Go through each input.
		delete this->at(i);								// Free the memory allocated to it.
	}
	this->clear();										// Clear all the inputs.
}

/*
 * Remove the input at the given index.
 */
void SCInputs::removeInput(int i){
	delete this->at(i);								// Free the memory allocated to the input.
	this->erase(this->begin() + i);					// Remove the input from the vector of inputs.
}

/*
 * Returns the version of the inputs.
 */
int SCInputs::getVersion(){
    return version_;
}
    
string SCInputs::getErrorMessage() {
    return errorMessage_;                       // Provide the caller with the current error message.
}

void SCInputs::setVersion(int newVersion){
	version_ = newVersion;
}

/*
 * Return the file path.
 */
std::string SCInputs::getFilePath(void){
	return filePath_;
}

/*!
 * Sets the file path.
 * \param strPath The desired path to the inputs' configuration file.
 */
void SCInputs::setFilePath(std::string strPath){
    filePath_ = strPath;                        // Set the file path to the file containg the configurations for the systems.
    char slash = strPath.at(strPath.size() - 1);// Get the last character from the path.
    if (slash != '/') {                         // Is the last character a slash?
        filePath_.append("/");                  // No, make the last character a slash.
    }
}

/*
 * Return the number of inputs.
 */
int SCInputs::getNumberOfInputs(void) {
	return (int)this->size();		// Return the number of inputs.
}

/*
 * Adds a default input to the vector of inputs.
 */
void SCInputs::addInput(void){
	SCInput* theNewInput = new SCInput();
	theNewInput->indexNo = (int)this->size();
	this->push_back(theNewInput);
}

/*
 * Adds the given input to the vector of inputs at the end.
 */
void SCInputs::addInput(SCInput* theNewInput){
	theNewInput->indexNo = (int)this->size();
	this->push_back(theNewInput);
}

/*
 * Adds the given input to the vector of inputs at the given position.
 */
void SCInputs::addInput(unsigned int i, SCInput* theNewInput){
	if(i > this->size()) { i = (int)this->size(); }	// Reset the desired position to the end if it is too big.
	theNewInput->indexNo = i;                       // Set the index number.
	this->insert(this->begin() + i, theNewInput);   // Insert the new input into the vector.
	for(unsigned int j = i + 1; j < this->size(); j++){	// Adjust each of the following
		this->at(j)->indexNo = j;                   // index numbers.
	}
}

bool SCInputs::loadFromDB() {
    sqlite3 *db;                            // Create a location for the database.
    int  rc;                                // Create a return variable.
    string sql;                             // Create a string for SQL commands.
    bool result = true;                     // The desired result of reading the Plant Manager's settings is to succeed.
    string dbName = getFilePath() + "SCConfig.db";// Set the configuration database name.
    rc = sqlite3_open(dbName.c_str(), &db); // Open the database named SC_Config.db. It has a table containing each input's configuration.
    if( rc ){                               // Was the database opened?
        errorMessage_ = "Can't open SCConfig.db to load the input configurations: " + string(sqlite3_errmsg(db));// No. create an error message.
        cout << errorMessage_ << endl;      // Display the error.
        return false;                       // This error prevents loading the inputs configurations.
    }                                       // The database is open.
    string tableName = "inputs";            // The table name for the inputs configurations.
    sql = "SELECT * from " + tableName;     // Create an SQL statement to read the configuration for each and every input.
    sqlite3_stmt *stmt;                     // Will contain the results of the querry.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
    if (rc != SQLITE_OK) {                  // Was it successful?
        errorMessage_ = "Can't query the inputs table in SCConfig.db: " + string(sqlite3_errmsg(db));// No. create an error message.
        cout << errorMessage_ << endl;      // Display the error.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
        return false;                       // Let the caller know the inputs configurations could not be retrieved from the database.
    }                                       // The inputs configurations were retreived.
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {// Go through each row in the results.
        int nDex  = sqlite3_column_int(stmt, 0) - 1;// The input's index is in the first column. DB indexes are ones based. We want zeros based.
        int type = sqlite3_column_int(stmt, 1); // The input's type is in the table's second column.
        int units = sqlite3_column_int (stmt, 2);// The input's units are in the third column.
        char buffer [50];                   // Create a buffer to hold the input's ID.
        int n = sprintf (buffer, "%s", sqlite3_column_text(stmt, 3));// Put the input's ID in the buffer.
        string iD(buffer, n);               // Convert the ID in the buffer to a string.
        double k1  = sqlite3_column_double(stmt, 4);// The input's first constant is in the fifth column.
        double k2  = sqlite3_column_double(stmt, 5);// The input's second constant is in the sixth column.
        switch (type) {                     // Create the input based on its type.
            case SCInput::SENSOR_ONE_WIRE : // ONE_WIRE is used to measure tmperatures.
            case SCInput::SENSOR_THERMOCOUPLE_K:// Type K thermocouples measure temperatures.
            case SCInput::SENSOR_THERMOCOUPLE_T: {// Type T thermocouples measure temperatures.
                TemperatureInput* tInput = new TemperatureInput();// Create a new temperature input.
                tInput->indexNo = nDex;     // Set this input's index.
                tInput->sensorType = (SCInput::Sensor)type;// Set this input's sensor.
                tInput->measuredUnits = (SCInput::Units)units;// Set this input's measuared units.
                tInput->setID(iD);          // Set the ID(serial number).
                addInput(tInput);           // Add the new input to the vector of inputs.
                break;                      // An input for measuring temperature has been created.
            }                               // Done setting a temperature type input.
            case SCInput::SENSOR_PRESSURE_TRANSDUCER: {// The input measures pressure.
                PressureInput* pInput = new PressureInput();// Create a new pressure input.
                pInput->indexNo = nDex;     // Set this input's index.
                pInput->sensorType = (SCInput::Sensor)type;// Set this input's sensor.
                pInput->measuredUnits = (SCInput::Units)units;// Set this input's measuared units.
                pInput->k1 = k1;            // Set the input's K1 constant.
                pInput->k2 = k2;            // Set the input's K2 constant.
                pInput->setID(iD);          // Set the ID(serial number).
                addInput(pInput);           // Add the new input to the vector of inputs.
                break;                      // An input for measuring pressure has been created.
            }                               // Done setting pressure inputs
            default: {                      // All other inputs are generic.
                SCInput* newInput = new SCInput();// Create a new input.
                newInput->indexNo = nDex;   // Set this input's index.
                newInput->sensorType = (SCInput::Sensor)type;// Set this input's sensor.
                newInput->measuredUnits = (SCInput::Units)units;// Set this input's measuared units.
                newInput->setID(iD);        // Set the ID(serial number).
                addInput(newInput);         // Add the new input to the vector of inputs.
                break;                      // An input has been created.
            }
        }                                   // An input was created based on its type.
    }                                       // The input configurations were read and inputs were created corresponding to the configurations.
    if (rc != SQLITE_DONE) {                // Was there an error going through the configurations?
        // Yes. Create an error message.
        errorMessage_ = "The input configurations query didn't finish: " + string(sqlite3_errmsg(db));
        cout << errorMessage_ << endl;      // Display the error.
        result = false;                     // The configuration of the inputs were not read.
    }                                       // The configuration of the inputs query completion result was evaluated.
    sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
    sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
    areLoaded = result;                     // These inputs have been loaded.
    return result;                          // Done reading the configuration of the inputs.
}

} /* namespace SCClasses */
