//
//  SCSystems.cpp
//  PlantManager
//
//  Created by Mark Davis on 6/3/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#include "SCSystems.h"
#include "SCInput.h"
#include "Relays.h"
#include <iostream>
#include <fstream>
#include "math.h"


namespace SCClasses {
    
    
    
    SCSystem::SCSystem() {
        baudCode_ = 1;                      // The default baud rate is 4800.
        baudRate_ = 4800;                   // The default baud rate is 4800.
        type_ = SystemType::SYSTYPE_NONE;   // There is no system type yet.
        filter_ = 1;                        // The default filter is 40 Hz.
        averageCode_ = 0;                   // The default is no averaging of samples.
        dataRate_ = 30;                     // The default data rate is 30 samples per second.
        cycleLength_ = 300;                 // The default number of milliseconds between data cycles.
        subSystemNo_ = -1;                  // The default is an invalid number.
        firstInput_ = -1;                   // There are no inputs for this sub-system.
        nInputs_ = 0;                       // The number of inputs is zero(until set).
        firstRelay_ = -1;                   // There are no relays for this sub-system.
        nRelays_ = 0;                       // There number of relays is zero(until set).
        portName_ = "none";                 // The default com port is none (invalid).
        isWireless = false;                 // The default sub-system is no t wireless.
        isOnLine = false;                   // By default this sub-system is off line.
        wirelessID_ = 0;                    // The default wireless ID.
        connectionType = SCSystem::CONN_SOCKET;// A socket is the default connection to the I/O equipment.
    }
    
    SCSystem::~SCSystem() {
    }

    SCSystem& SCSystem::operator=(SCSystem otherSys) {
        baudCode_ = otherSys.getBaudCode();     // Copy the baud code.
        baudRate_ = otherSys.getBaudRate();     // Copy the baud rate.
        type_ = otherSys.getType();             // Copy the system type.
        filter_ = otherSys.getFilter();         // Copy the filter.
        averageCode_ = otherSys.getAverageCode();// Copy the averaging of samples.
        dataRate_ = otherSys.getDataRate();     // Copy the data rate.
        cycleLength_ = otherSys.getCycleLength();// Copy the number of milliseconds between data cycles.
        subSystemNo_ = otherSys.getSubSystemNo();// Copy the sub-system number.
        firstInput_ = otherSys.getFirstInput(); // Copy the first input.
        nInputs_ = otherSys.getNumberOfInputs();// Copy the number of inputs.
        firstRelay_ = otherSys.getFirstRelay(); // Copy the first relay.
        nRelays_ = otherSys.getNumberOfRelays();// Copy the number of relays.
        portName_ = otherSys.portName_;         // Copy the port name.
        isWireless = otherSys.isWireless;       // Copy the wireless state.
        isOnLine = otherSys.isOnLine;           // Copy the online status.
        wirelessID_ = otherSys.getWirelessID(); // Copy the wireless ID.
        connectionType = otherSys.connectionType;// Copy the connection type.
        return *this;
    }
    
    int SCSystem::getBaudCode() {
        return baudCode_;           // Provide the baud code to the caller.
    }
    
    void SCSystem::setBaudCode(int baudCode) {
        if (baudCode < 0 || baudCode > 5) { // Is this an invalid code?
            // Yes. This code is not valid.
            if (getType() == SYSTYPE_MINI || getType() == SYSTYPE_ONEWIRE) { // Is this a non-Lawson system?
                baudCode_ = 0;      // Yes. The default baud code is for a 9600 baud rate.
                baudRate_ = 9600;   // The default baud rate is 9600.
            } // Done setting baud rate and code for a non-Lawson system.
            else {                  // This is a Lawson based system.
                baudCode_ = 1;      // Set the baud code for a 4800 baud rate.
                baudRate_ = 4800;   // Set the baud rate to 4800.
            } // Done setting baud rate and code for a Lason based system.
        } // Done with an invalid code.
        else {                      // The code was valid.
            if (getType() == SYSTYPE_MINI || getType() == SYSTYPE_ONEWIRE) { // Is this a non-Lawson system?
                baudCode_ = 0;      // Yes. The default baud code is for a 9600 baud rate.
                baudRate_ = 9600;   // The default baud rate is 9600.
            } // Done setting baud rate and code for a non-Lawson system.
            else {                  // This is a Lawson based system.
                baudCode_ = (int16_t)baudCode;// Set the baud code.
                int16_t n = 1;      // Set the shift bit.
                n <<= baudCode_;    // Shift the bit up a power of 2 for the # of times indicated by the baud code.
                baudRate_ = 9600 / n;// Calculate the baud rate.
            } // Done setting a Lawson based system.
        } // Done setting a valid code.
    }
    
    int SCSystem::getBaudRate() {
        return baudRate_;               // Provide the baud rate to the caller.
    }
    
    void SCSystem::setBaudRate(int baudRate) {
        if (baudRate == 300 || baudRate == 600 || baudRate == 1200 || baudRate == 2400 || baudRate == 4800 || baudRate == 9600) // Is this a valid Rate?
        {                           // Yes.
            baudRate_ = (int16_t)baudRate;// Set the baud rate.
            double d = 9600 / baudRate_;// Calculate the number of times the baudRate_ goes into 9600.
            baudCode_ = log2(d);    // The log base two is the baud code.
        }                           // Done setting a valid baud rate & code.
        else {                      // An invalid baud rate was provided.
            if (getType() == SYSTYPE_MINI || getType() == SYSTYPE_ONEWIRE)// Is this a non-Lawson system?
            {                       // Yes.
                baudCode_ = 0;      // The default baud code is for a 9600 baud rate.
                baudRate_ = 9600;   // The default baud rate is 9600.
            }                       // Done setting default baud rate and code for a non-Lawson system.
            else {                  // This is a Lawson based system.
                baudCode_ = 1;      // The default baud code is for a 4800 baud rate.
                baudRate_ = 4800;   // The default baud rate is 4800.
            }                       // Done setting default baud rate & code for a Lawson based system.
        } // Done setting the default values for baud rate and code when an invalid rate was provided.
    }

    SCSystem::SystemType SCSystem::getType() {
        return type_;                   // Provide this sub-system's type.
    }

    void SCSystem::setType(SCSystem::SystemType type) {
        if (type < SYSTYPE_STANDARD || type > SYSTYPE_NONE) {// Is this a valid system type?
            type_ = SYSTYPE_NONE;       // No. Set the system type to none.
        } else {                        // Yes.
            type_ = type;               // Set the system's type to that provided.
        }                               // Done setting this sub-system's type.
    }
    
    int SCSystem::getFilter() {
        return filter_;                 // Provide this sub-system's filter number.
    }
    
    void SCSystem::setFilter(int filterNo) {
        if (filterNo == 0 || filterNo == 1 || filterNo == 2) {// Was a valid filter number provided?
            filter_ = (int16_t)filterNo;         // Yes. Set the filter number.
        }                               // Done setting a valid filter number.
        else {                          // An invalid filter number was provided.
            filter_ = 1;                // Set the filter to 40 Hz.
        }                               // Done proccessing an invalid filter number.
    }
    
    int SCSystem::getAverageCode() {
        return averageCode_;            // Provide this sub-system's averaging code.
    }
    
    void SCSystem::setAverageCode(int averageCode) {
        // Was a valid samples code provided?
        if (averageCode == 0 || averageCode == 1 || averageCode == 2 || averageCode == 3 || averageCode == 4) {
            averageCode_ = (int16_t)averageCode; // Yes. Set the samples
        }                               // code.
        else {                          // An invalid samples code was provided.
            averageCode_ = 0;           // Set the averaging code to no sample averaging.
        }
    }
    
    int SCSystem::getDataRate() {
        return dataRate_;               // Provide this sub-system's sampling rate.
    }
    
    void SCSystem::setDataRate(int dataRate) {
        if (dataRate >= 10 && dataRate <= 1000) { // Is the provided data rate valid?
            dataRate_ = (int16_t)dataRate;// Yes. Set the data rate.
        }
        else {                          // The value was out of range.
            dataRate_ = 30;             // Set the sampling rate to the default of 30 samples per second.
        }
    }
    
    int SCSystem::getCycleLength() {
        return cycleLength_;            // Provide the number of milliseconds between data cycles.
    }
    
    void SCSystem::setCycleLength(int cycleLength) {
        if (cycleLength_ >= 100) {      // Is the provided cycle length at least 100 mSecs?
            cycleLength_ = (int16_t)cycleLength;// Yes. Set the cycle length to the provided value.
        } else {                        // An in valid nuber was provided.
            cycleLength_ = 300;         // Set the cycle length to 300 mSecs.
        }
    }

    int SCSystem::getSubSystemNo() {
        return subSystemNo_;            // Provide this sub-system's number(one based).
    }
    
    void SCSystem::setSubSystemNo(int value) {
        if (value < 1) {                // Is the desired sub-system number less than one?
            subSystemNo_ = 1;           // Yes. This is illegal. Set the number to one.
        } else {                        // The desired number is legal.
            subSystemNo_ = value;       // Set this sub-system's number.
        }
    }
    
    int SCSystem::getFirstInput() {     // Provide the index(zero based of the entire system)
        return firstInput_;             // for this sub-system's first input.
    }
    
    void SCSystem::setFirstInput(int value) {
        if (value > -1) {               // Is the desired first input value valid?
            firstInput_ = (int16_t)value;// Yes. Set the first input.
        } else {                        // No. Not a valid first input.
            firstInput_ = -1;           // Set the first input to show no inputs.
        }
    }
    
    int SCSystem::getNumberOfInputs() {
        return nInputs_;                // Provide this sub-system's number of inputs.
    }
    
    void SCSystem::setNumberOfInputs(int value) {
        if (value >= 0) {               // Is the desired value at least zero?
            nInputs_ = (int16_t)value;  // Yes, set the number of inputs to the desired value.
        } else {                        // Invalid number of inputs.
            nInputs_ = 0;               // Set the number of inputs to zero.
        }
    }
    
    int SCSystem::getFirstRelay() {     // Provide the index(zero based of the entire system)
        return firstRelay_;             // for this sub-system's first relay.
    }

    void SCSystem::setFirstRelay(int value) {
        if (value > -1) {               // Is the desired first relay value valid?
            firstRelay_ = (int16_t)value;// Yes. Set the first relay.
        } else {                        // No. Not a valid first relay.
            firstRelay_ = -1;           // Set the first relay to show no relays.
        }
    }

    int SCSystem::getNumberOfRelays() {
        return nRelays_;                // Provide this sub-system's number of relays.
    }
    
    void SCSystem::setNumberOfRelays(int value) {
        if (value >= 0) {               // Is the desired value at least zero?
            nRelays_ = (int16_t)value;  // Yes, set the number of relays to the desired value.
        } else {                        // Invalid number of relays.
            nRelays_ = 0;               // Set the number of relays to zero.
        }
    }

    string SCSystem::getPortName() {
        return portName_;               // Provide the sub-system's port name.
    }

    void SCSystem::setPortName(string portName) {
        portName_ = portName;           // Set this sub-system's port name.
    }
    
    unsigned int SCSystem::getWirelessID() {
        return wirelessID_;             // Provide this sub-system's wireless ID.
    }
    
    void SCSystem::setWirelessID(unsigned int value) {
        wirelessID_ = value;            // Set this sub-system's wireless ID.
    }
    
    string SCSystem::getTypeString() {
        string typeString = "None";                 // Set the type string to none for the definition.
        switch (type_) {                            // Switch based on the type of hardware.
            case SYSTYPE_NONE:                      // The hardware type was none.
                break;                              // The type string is already set to none.
            case SYSTYPE_STANDARD:                  // The hardware type is Standard(Lawson full feature);
                typeString = "Standard"; break;     // Set the type string to standard.
            case SYSTYPE_MICRO:                     // The hardware type is Micro.
                typeString = "Micro"; break;        // Set the type string to Micro.
            case SYSTYPE_MINI:                      // The hardware type is Adlink or ADAM.
                typeString = "Mini"; break;         // Set the string type to Mini.
            case SYSTYPE_ONEWIRE:                   // The hardware type is one wire.
                typeString = "One Wire"; break;     // Set the string to One Wire.
            default:                                // This is for an illegitamate type.
                typeString = "Unknown System";      // Set the string to unknown system.
                break;
        }
        return typeString;
    }
    
    void SCSystem::display() {
        cout << "This sub-system is numbered " << getSubSystemNo() << " and contains:" << endl;
        cout << "     Baud code - " << getBaudCode() << endl;
        cout << "     Baud rate - " << getBaudRate() << endl;
        cout << "   System type - " << getTypeString() << endl;
        cout << "   Filter code - " << getFilter() << endl;
        cout << "  Average code - " << getAverageCode() << endl;
        cout << "     Data rate - " << getDataRate() << endl;
        cout << "  Cycle length - " << getCycleLength() << endl;
        cout << "   First input - " << getFirstInput() << endl;
        cout << "   # of inputs - " << getNumberOfInputs() << endl;
        cout << "   First relay - " << getFirstRelay() << endl;
        cout << "   # of relays - " << getNumberOfRelays() << endl;
        cout << "Comm Port Name - " << getPortName() << endl;
        cout << "   Wireless ID - " << getWirelessID() << endl;
        cout << "     Wireless? - ";
        if (isWireless == true) {
            cout << "true" << endl;
        } else {
            cout << "false" << endl;
        }
        cout << "      On Line? - ";
        if (isOnLine == true) {
            cout << "true" << endl;
        } else {
            cout << "false" << endl;
        }
        cout << endl;
    }
    
#pragma mark - SCSystem class implementation
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    SCSystems::SCSystems() {
        filePath_ = "";
        version_ = 6;                               // Any version prior to 6 is MFC.
        inputs = nullptr;                           // There is not an input list yet.
        relays = nullptr;                           // There is not a relay list yet.
    }
    
    SCSystems::~SCSystems() {
        if (inputs != nullptr) {                    // Have inputs been allocated?
            delete inputs;                          // Yes. Free the memory for the inputs.
        }                                           //
        if (relays != nullptr) {                    // Have relays been allocated?
            delete relays;                          // Yes. Free the memory for the relays.
        }                                           //
        for(unsigned int i = 0; i < this->size(); i++){	// Go through each sub-system.
            delete this->at(i);						// Free the memory allocated to it.
        }                                           //
        this->clear();								// Clear all the system of all sub-systems.
    }

    int SCSystems::getVersion(){
        return version_;
    }

    string SCSystems::getFilePath(void){
        return filePath_;
    }
    
    string SCSystems::getErrorMessage() {
        return errorMessage_;
    }

    void SCSystems::setFilePath(std::string strPath){
        filePath_ = strPath;                        // Set the file path to the file containg the system configuration.
        char slash = strPath.at(strPath.size() - 1);// Get the last character from the path.
        if (slash != '/') {                         // Is the last character a slash?
            filePath_.append("/");                  // No, make the last character a slash.
        }
    }
    
    bool SCSystems::areInputsRelaysLoaded() {
        return areDBInputsRelaysLoaded();
    }

    bool SCSystems::areDBInputsRelaysLoaded() {
        if (inputs == nullptr) {                // Have the inputs been allocated?
            inputs = new SCInputs();            // Create the inputs.
            inputs->setFilePath(filePath_);     // Be sure the inputs file path is set.
            if (inputs->loadFromDB() == false) {// Try to load the input configurations.
                errorMessage_ = "Failed to load the inputs for the system configuration!";
                cout << errorMessage_ << endl;  // Display the error.
                return false;                   // The load failed, let the caller know.
            }                                   //
        } else if (inputs->areLoaded == false) {// The inputs have been allocated but are not loaded.
            inputs->setFilePath(filePath_);     // Be sure the inputs file path is set.
            if (inputs->loadFromDB() == false) {// Try to load the input configurations.
                errorMessage_ = "Failed to load the inputs for the system configuration!";
                cout << errorMessage_ << endl;  // Display the error.
                delete inputs;                  // Since the load failed, free the inputs memory.
                return false;                   // The load failed, let the caller know.
            }                                   //
        }                                       // The inputs have been loaded.
        if (relays == nullptr) {                // Have the relays been allocated?
            relays = new Relays();              // Create the relays.
            relays->setFilePath(filePath_);     // Be sure the relays file path is set.
            if (relays->loadFromDB()== false) { // Try to load the relay configurations.
                errorMessage_ = "Failed to load the relays for the system configuration!";
                cout << errorMessage_ << endl;  // Display the error.
                delete inputs;                  // Free the inputs memory.
                return false;                   // The load failed, let the caller know.
            } else if (relays->areLoaded == false) {// The relays have been allocated but are not loaded.
                relays->setFilePath(filePath_); // Be sure the relays file path is set.
                if (relays->loadFromDB() == false) {// Try to load the relay configuration.
                    errorMessage_ = "Failed to load the relays for the system configuration!";
                    cout << errorMessage_ << endl;// Display the error.
                    delete relays;              // Since the load failed, free the relays memory.
                    delete inputs;              // Free the inputs memory.
                    return false;               // The load failed, let the caller know.
                }                               //
            }                                   //
        }                                       //
        return true;                            // Both the inputs and the relays have been loaded.
    }

    bool SCSystems::loadFromDB() {
        // Have the sub-systems already been loaded?
        // If so, we need to remove them from the vector of subsystems.
        // The following lines remove the inputs, relays, and sub-systems from this system if they exist.
        if (inputs != nullptr) {                // Have inputs been allocated?
            delete inputs;                      // Yes. Free the memory used by the inputs.
        }                                       //
        if (relays != nullptr) {                // Have relays been allocated?
            relays->electricRelays.clear();     // Yes. Clear the electric relay list.
            delete relays;                      // Yes. Free the memory used by the relays.
        }                                       //
        for(unsigned int i = 0; i < this->size(); i++){// Go through each sub-system.
            delete this->at(i);                 // Free the memory allocated to it.
        }                                       //
        this->clear();                          // Clear the system of all sub-systems.
                                                // Ckecking if the inputs and relays are loaded will cause them to load.
        if (areDBInputsRelaysLoaded() == false) {// Are the inputs and relays loaded?
            return false;                       // No. There was a problem.
        }                                       // The inputs and the relays have been loaded.
        sqlite3 *db;                            // Create a location for the database.
        int  rc;                                // Create a return variable.
        string sql;                             // Create a string for SQL commands.
        bool result = true;                     // The desired result of reading the system configuration is to succeed.
        string dbName = getFilePath() + "SCConfig.db";// Set the configuration database name.
                                                // Open the database named SC_Config.db.
        rc = sqlite3_open(dbName.c_str(), &db); // It has a table containing each sub-system's configuration.
        if( rc ){                               // Was the database opened?
                                                // No. Create an error message.
            errorMessage_ = "Can't open SCConfig.db to load the sub-system configurations: " + string(sqlite3_errmsg(db));
            cout << errorMessage_ << endl;      // Display the error.
            return false;                       // This error prevents loading the system configuration.
        }                                       // The database is open.
        string tableName = "systems";           // The table name for the system configuration.
        sql = "SELECT * from " + tableName;     // Create an SQL statement to read the configuration for each and every sub-system.
        sqlite3_stmt *stmt;                     // Will contain the results of the querry.
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
        if (rc != SQLITE_OK) {                  // Was it successful?
            errorMessage_ = "Can't query the systems table in SCConfig.db: " + string(sqlite3_errmsg(db));// No. create an error message.
            cout << errorMessage_ << endl;      // Display the error.
            sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
            sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
            return false;                       // Let the caller know the system configuration could not be retrieved from the database.
        }                                       // The system configuration was retreived.
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {// Go through each row in the results.
            SCSystem* theNewSys = new SCSystem();// Allocate memory for a new sub-system.
                                                // The sub-system's index is in the first column.
            int n  = sqlite3_column_int(stmt, 0);// DB indexes are ones based.
            theNewSys->setSubSystemNo(n);       // Set the sub-system's index.
            n = sqlite3_column_int(stmt, 1);    // The sub-system's first input is in the table's second column.
            theNewSys->setFirstInput(n);        // Set the sub-system's first input.
            n = sqlite3_column_int(stmt, 2);    // The sub-system's number of inputs is in the third column.
            theNewSys->setNumberOfInputs(n);    // Set the sub-system's number of inputs.
            n = sqlite3_column_int(stmt, 3);    // The sub-system's first relay is in the fourth column.
            theNewSys->setFirstRelay(n);        // Set the sub-system's first relay.
            n = sqlite3_column_int(stmt, 4);    // The sub-system's number of relays is in the fifth column.
            theNewSys->setNumberOfRelays(n);    // Set the sub-system's number of relays.
            const unsigned char* pName = sqlite3_column_text(stmt, 5);// The sub-system's port name is in the sixth column.
            string strName = std::string(reinterpret_cast<const char*>(pName));// Convert the port name to a string.
            theNewSys->setPortName(strName);    // Set the sub-system's port name ( probably a serial port).
            n = sqlite3_column_int(stmt, 6);    // The sub-system's wireless state is in the seventh column.
            theNewSys->isWireless = false;      // By default, the sub-system is not wireless.
            if(n == 1) {                        // Is this sub-system wireless?
                theNewSys->isWireless = true;   // Yes. Set the sub system to be wireless.
            }                                   //
            n = sqlite3_column_int(stmt, 7);    // The sub-system's wireless Id is in the eighth column.
            theNewSys->setWirelessID(n);        // Set the sub-system's wireless Id.
            n = sqlite3_column_int(stmt, 8);    // The sub-system's type is in the nineth column.
            theNewSys->setType((SCSystem::SystemType)n);// Set the sub-system's type.
            n = sqlite3_column_int(stmt, 9);    // The sub-system's online state is in the tenth column.
            theNewSys->isOnLine = false;        // By default, the sub-system is not on line.
            if(n == 1) {                        // Is this sub-system on line?
                theNewSys->isOnLine = true;     // Yes. Set the sub system to be on line.
            }                                   //
            n = sqlite3_column_int(stmt, 10);   // The sub-system's baud code is in the eleventh column.
            theNewSys->setBaudCode(n);          // Set the sub-system's baud code.
            n = sqlite3_column_int(stmt, 11);   // The sub-system's filter is in the twelfth column.
            theNewSys->setFilter(n);            // Set the sub-system's filter.
            n = sqlite3_column_int(stmt, 12);   // The sub-system's average code is in the thirtennth column.
            theNewSys->setAverageCode(n);       // Set the sub-system's average code.
            n = sqlite3_column_int(stmt, 13);   // The sub-system's dataRate is in the fourteenth column.
            theNewSys->setDataRate(n);          // Set the sub-system's data rate.
            n = sqlite3_column_int(stmt, 14);   // The sub-system's cycle length is in the fifteenth column.
            theNewSys->setCycleLength(n);       // Set the sub-systems's cycle length.
            n = sqlite3_column_int(stmt, 15);   // The sub-system's hardware connection is in the sixteenth column.
            if(n == 1) {                        // Is the I/O equipment connected through a serial port?
                theNewSys->connectionType = SCSystem::CONN_SERIAL;// Yes.
            } else {                            // The I/O equipment is
                theNewSys->connectionType = SCSystem::CONN_SOCKET;// connected through a serial port.
            }                                   //
            addSubSystem(theNewSys);            // Add this new sub-system to the list of sub-systems.
            if(isValidSubSystem(theNewSys) == false) {// Is the ne sub-system valid?
                sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
                sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
                return false;                   // No. Stop loading. There is a configuration error.
            }                                   //
        }                                       // The sub-system configurations were read.
        if (rc != SQLITE_DONE) {                // Was there an error going through the configurations?
            // Yes. Create an error message.
            errorMessage_ = "The relay configurations query didn't finish: " + string(sqlite3_errmsg(db));
            cout << errorMessage_ << endl;      // Display the error.
            result = false;                     // The configuration of the relays was not read.
        }                                       // The configuration of the relays query completion result was evaluated.
        sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
        return result;                          // Let the caller know if the relay config was loaded.
    }
    
    void SCSystems::addSubSystem(SCClasses::SCSystem* subSystem) {
        int subSystemNo = subSystem->getSubSystemNo();// Get this sub-system's desired position.
        if(subSystemNo > this->size() + 1) {    // Is the desired position past the end?
            subSystemNo = (int)this->size() + 1;// Yes. Put the sub-system at the end of the vector.
        } else if (subSystemNo < 1) {           // Is the desired position before the start?
            subSystemNo = 1;                    // Yes. Put the sub-system at the start.
        }                                       // Otherwise the desired position was legal.
        subSystem->setSubSystemNo(subSystemNo);	// Set the sub-system's number.
        this->insert(this->begin() + subSystemNo - 1, subSystem);// Insert the new sub-system into the vector.
        for(unsigned int j = subSystemNo; j < this->size(); j++) {// Adjust each of the following
            this->at(j)->setSubSystemNo(j+1);   // sub-system numbers.
        }                                       // Done adjusting subsequent sub-system numbers.
    }
    
    void SCSystems::removeSubSystem(int i) {
        if (i < 1 || i > this->size()) {                // Is this a valid sub-system number?
            return;                                     // No, there is nothing to remove.
        }                                               //
        delete this->at(i - 1);                         // Free the memory allocated to the system.
        this->erase(this->begin() + i - 1);				// Remove the system from the vector of systems.
        int subSystemNo = 1;                            // The first sub-system number is one.
        int previousNoOfInputs = 0;                     // Contains a running total of the number of inputs.
        int previousNoOfRelays = 0;                     // Contains a running total of the number of relays.
        for (std::vector<SCSystem*>::iterator it = this->begin(); it != this->end(); ++it)
        {   // Go through each of the sub-systems and reassign its number, first input, and first relay.
            (*it)->setSubSystemNo(subSystemNo++);       // Reassign this sub-system's number.
            if ((*it)->getFirstInput() != -1) {         // Does this sub-system have inputs?
                (*it)->setFirstInput(previousNoOfInputs);// Yes. Reassign the first input to the previous total.
                previousNoOfInputs += (*it)->getNumberOfInputs();// Update the total number of inputs.
            }                                           //
            if ((*it)->getFirstRelay() != -1) {         // Does the sub-system have relays?
                (*it)->setFirstRelay(previousNoOfRelays);// Yes. Reassign the first relay to the previous total.
                previousNoOfRelays += (*it)->getNumberOfRelays();// Update the total number of relays.
            }                                           //
        }                                               // Done going through the remaining sub-systems.
    }
    
    int SCSystems::getSysDexContainingRelay(int relayIndex) {
        int	lastRelay = 0;
        // Check each sub-system until the sub-system containg the relay is found.
        for (int sysIndex = 0; sysIndex < this->size(); sysIndex++) {
            SCSystem* sys = this->at(sysIndex);
            lastRelay = sys->getFirstRelay() + sys->getNumberOfRelays() - 1;
            if (relayIndex >= sys->getFirstRelay() && relayIndex <= lastRelay) {// Is the relay in this sub-system?
                return sysIndex;                        // Yes. Return the sub-system's index.
            }                                           //
        }                                               // Done going through each sub-system.
        return -1;                                      // System not found, return invalid number.
    }
    
    int SCSystems::getSysDexContainingInput(int inputIndex) {
        int	lastInput = 0;
        // Check each sub-system until the sub-system containg the input is found.
        for (int sysIndex = 0; sysIndex < this->size(); sysIndex++) {
            SCSystem* sys = this->at(sysIndex);
            lastInput = sys->getFirstInput() + sys->getNumberOfInputs() - 1;
            if (inputIndex >= sys->getFirstInput() && inputIndex <= lastInput) {// Is the input in this sub-system?
                return sysIndex;                        // Yes. Return the sub-system's index.
            }                                           //
        }                                               // Done going through each sub-system.
        return -1;                                      // System not found, return invalid number.
    }
    
    bool SCSystems::isValidSubSystem(SCClasses::SCSystem *subSys) {
        errorMessage_ = "Error";                           // Set the default error mesage.
        if (areInputsRelaysLoaded() == false) {     // Are the inputs and realys already loaded?
            return false;                           // No, they weren't and couldn't be loaded.
        }                                           // It is not possible to evaluate if this a valid sub-system.
        if (subSys->getNumberOfInputs() == 0 && subSys->getNumberOfRelays() == 0) {
            errorMessage_ = " This sub-system is not valid because it has neither inputs, outputs, or both.";
            cout << errorMessage_ << endl;          // Display the error.
            return false;
        }
        if (subSys->getFirstInput() < 0 && subSys->getNumberOfInputs() > 0) {
            errorMessage_ = "The first input must be greater than or equal to zero when there are inputs.";
            cout << errorMessage_ << endl;          // Display the error.
            return false;
        }
        if (subSys->getFirstRelay() < 0 && subSys->getNumberOfRelays() > 0) {
            errorMessage_ = "The first relay must be greater than or equal to zero when there are relays.";
            cout << errorMessage_ << endl;          // Display the error.
            return false;
        }
        int lastIndex = subSys->getFirstInput() + subSys->getNumberOfInputs();
        int noInputs = inputs->getNumberOfInputs();
        if ( lastIndex > noInputs) {
            errorMessage_ = "The last input number, " + to_string(lastIndex);
            errorMessage_ += ", exceeds the total number of inputs, " + to_string(inputs->getNumberOfInputs());
            errorMessage_ += ".\nUse the Input Configuration application to change the total number of inputs.";
            cout << errorMessage_ << endl;          // Display the error.
            return false;
        }
        lastIndex = subSys->getFirstRelay() + subSys->getNumberOfRelays();
        if ( lastIndex > relays->getNumberOfRelays()) {
            errorMessage_ = "The last relay number, " + to_string(lastIndex);
            errorMessage_ += ", exceeds the total number of relays, " + to_string(relays->getNumberOfRelays());
            errorMessage_ += ".\nUse the Relay Configuration application to change the total number of relays.";
            cout << errorMessage_ << endl;          // Display the error.
            return false;
        }
        if (subSys->getType() < SCSystem::SYSTYPE_STANDARD || subSys->getType() > SCSystem::SYSTYPE_ONEWIRE) {
            errorMessage_ = "Invalid system type.";
            cout << errorMessage_ << endl;          // Display the error.
            return false;
        }
        int totalInputs = subSys->getNumberOfInputs();
        int totalOutputs = subSys->getNumberOfRelays();
        // Evaluate this sub-system in light of all the other sub-systems.
        for (std::vector<SCSystem*>::iterator it = this->begin() ; it != this->end(); ++it){
            // Is the given sub-system the same as the one we are checking against?
            if ((*it)->getSubSystemNo() == subSys->getSubSystemNo()) {
                continue;                               // Yes. Do not check a sub-system against itself.
            }                                           // Done processing a sub-system match.
            totalInputs += (*it)->getNumberOfInputs();  // Increment the total number of inputs there could be.
            if (totalInputs > inputs->getNumberOfInputs()) {// Will this sub-system cause the total # of inputs to be exceeded?
                errorMessage_ = "This sub-system causes the total number of inputs, ";// Yes. Create the error message.
                errorMessage_ += to_string(inputs->getNumberOfInputs()) + ", to be exceeded.";
                cout << errorMessage_ << endl;          // Display the error.
                return false;                           // Let the caller know the sub-system is invalid.
            }
            totalOutputs += (*it)->getNumberOfRelays(); // Increment the total number of relays there could be.
            if (totalOutputs > relays->getNumberOfRelays()) {// Will this sub-system cause the total # of relays to be exceeded?
                errorMessage_ = "This sub-system causes the total number of outputs, ";// Yes. Create the error message.
                errorMessage_ += to_string(relays->getNumberOfRelays()) + ", to be exceeded.";
                cout << errorMessage_ << endl;          // Display the error.
                return false;                           // Let the caller know the sub-system is invalid.
            }
            // Check for duplicate port names.
            if ((*it)->getPortName() == subSys->getPortName()) {// Same serial port?
                errorMessage_ = "The port, " + subSys->getPortName();// Create an error message.
                errorMessage_ += " on sub-system number " + to_string(subSys->getSubSystemNo());
                errorMessage_ += ", is already used by sub-system ";
                errorMessage_ += to_string((*it)->getSubSystemNo()) + ".";
                cout << errorMessage_ << endl;          // Display the error.
                return false;                           // Let the caller know the sub-system is invalid.
            }
            // Check for overlap by the first chnl.
            if (subSys->getNumberOfInputs() > 0 && (*it)->getNumberOfInputs() > 0) {// Do both sub-systems have inputs?
                //Yes. Both have inputs.
                int first = subSys->getFirstInput();            // Remember the first input.
                // Does the test sub-system's first input fall in another sub-system's input range?
                if (first >= (*it)->getFirstInput() && first < (*it)->getFirstInput() + (*it)->getNumberOfInputs()) {
                    errorMessage_ = "The first input, " + to_string(subSys->getFirstInput() + 1);
                    errorMessage_ += ", is already contained in sub-system " + to_string((*it)->getSubSystemNo());
                    cout << errorMessage_ << endl;      // Display the error.
                    return false;
                }
            }
            // Check for overlap by the first relay.
            if (subSys->getNumberOfRelays() > 0 && (*it)->getNumberOfRelays() > 0) {// Do both sub-systems have outputs?
                // Yes. Both sub-systems have relays.
                int first = subSys->getFirstRelay();            // Remember the first relay.
                if (first >= (*it)->getFirstRelay() && first < (*it)->getFirstRelay() + (*it)->getNumberOfRelays()) {
                    errorMessage_ = "The first relay, " + to_string(subSys->getFirstRelay() + 1);
                    errorMessage_ += ", is already contained in sub-system " + to_string((*it)->getSubSystemNo());
                    cout << errorMessage_ << endl;      // Display the error.
                    return false;
                }
            }
            // Check for overlap by the last chnl.
            if (subSys->getNumberOfInputs() > 0 && (*it)->getNumberOfInputs() > 0) {// Do both sub-systems have inputs?
                //Yes. Both have inputs.
                int last = subSys->getFirstInput() + subSys->getNumberOfInputs() - 1;// Remember the last input.
                if (last >= (*it)->getFirstInput() && last < (*it)->getFirstInput() + (*it)->getNumberOfInputs()) {
                    errorMessage_ = "The last input, " + to_string(last + 1);
                    errorMessage_ += ", is already contained in sub-system " + to_string((*it)->getSubSystemNo());
                    cout << errorMessage_ << endl;      // Display the error.
                    return false;
                }
            }
            // Check for overlap by the last relay.
            if (subSys->getNumberOfRelays() > 0 && (*it)->getNumberOfRelays() > 0) { // Do both sub-systems have outputs?
            // Yes. Both have relays.
                int last = subSys->getFirstRelay() + subSys->getNumberOfRelays() - 1;// Remeber the last relay.
                if (last >= (*it)->getFirstRelay() && last < (*it)->getFirstRelay() + (*it)->getNumberOfRelays()) {
                    errorMessage_ = "The last relay, " +  to_string(last + 1);
                    errorMessage_ += ", is already contained in sub-system " + to_string((*it)->getSubSystemNo());
                    cout << errorMessage_ << endl;      // Display the error.
                    return false;
                }
            }
        }
        errorMessage_ = "No errors.";
        return true;
    }

}
