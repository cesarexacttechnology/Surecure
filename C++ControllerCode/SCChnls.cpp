//
//  SCChnls.cpp
//  PlantManager
//
//  Created by Mark Davis on 3/7/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#include "SCChnls.h"
#include <iostream>
#include <fstream>


namespace SCClasses {
    
    SCChnl::SCChnl() {
        relayIndex_ = -1;                           // This channel has no relay's until they are set.
        inputIndex_ = 0;                            // This channel has the first input until it is reset.
        approachLimit_ = 0;                         // The approach limit is disabled.
        deviation = -1.0;                           // The deviation alarm is disabled.
        theSCChnls_ = nullptr;                      // A pointer to the SCChnls to which this channel belongs.
        maxNameLength_ = 15;                        // The maximum number of characters in a name is 15.
        name_ = "None";                             // The name is "None". No two channels should have the same name.
    }
    
    SCChnl::SCChnl(SCChnls* scChnls) {
        relayIndex_ = -1;                           // This channel has no relay's until they are set.
        inputIndex_ = 0;                            // This channel has the first input until it is reset.
        approachLimit_ = 0;                         // The approach limit is disabled.
        deviation = -1.0;                           // The deviation alarm is disabled.
        theSCChnls_ = scChnls;                      // A pointer to the SCChnls to which this channel belongs.
        maxNameLength_ = 15;                        // The maximum number of characters in a name is 15.
        long nameNumber = theSCChnls_->size();      // Create a number that represents the name.
        string testName = to_string(++nameNumber);  // Convert that number to a string.
        while (theSCChnls_->nameExists(testName) == true) {// Does another SCChnl have that name?
            nameNumber++;                           // Yes. Increment the number in order to try a new name.
            testName = to_string(nameNumber);       // Convert that number to a string.
        }                                           // A unique name has been found.
        name_ = testName;                           // Set the SCChnl's name.
    }

    
    SCChnl::~SCChnl() {
        
    }
    
    int SCChnl::getRelayIndex() {
        return relayIndex_;                         // Provide the caller with the relay index.
    }

    bool SCChnl::setRelayIndex(int value) {
        if(theSCChnls_ != nullptr) {                // Is this SCChnl part of an SCChnls array?
            if (theSCChnls_->relayIndexExists(value) == true) {// Yes. Does another SCChnl have this relay index?
                return false;                       // Yes. Don't set the relay index. Let the caller know the index wasn't set.
            }                                       // Another SCChnl does not have this relay index.
        }                                           // The relay index has been checked against a duplicate on another SCChnl.
        relayIndex_ = value;                        // Set the relay index to the value provided by the caller.
        return true;                                // Let the caller know the relay index was set to the desired value.
    }
    
    int SCChnl::getInputIndex() {
        return inputIndex_;                         // Provide the caller with the input index.
    }
    
    void SCChnl::setInputIndex(int value) {
        inputIndex_ = value;                        // Set the input index to the value provided by the caller.
    }
    
    
    string SCChnl::getName() {
        return name_;                               // Provide the caller with this channel's name.
    }
    
    bool SCChnl::setName(string value) {
        if (value.length() == 0) {                  // Does the desired name exist(have characters)?
            return false;                           // No. The name can't be set to nothing.
        }                                           // The name has characters.
        if(theSCChnls_ != nullptr) {                // Is this SCChnl part of an SCChnls array?
            if (theSCChnls_->nameExists(value) == true) {// Yes. Does another SCChnl have this name?
                return false;                       // Yes. Don't set the name. Let the caller know the name wasn't set.
            }                                       // Another SCChnl does not have this name.
        }                                           // The name as been checked against a duplicate on another SCChnl.
        name_ = value.substr(0, 12);                // Limit the name to twelve characters.
        return true;                                // Let the caller know the name was set to the desired value.
    }
  
    unsigned int SCChnl::getApproachLimit() {
        return approachLimit_;                      // Provide the caller with the approach limit.
    }

    void SCChnl::setApproachLimit(unsigned int value) {
        approachLimit_ = value;                     // Set the approach limit to the value provided by the caller.
    }

    void SCChnl::disableDeviationAlarm() {
        deviation = -1.0;                           // A negative deviation disables deviation alarms.
    }

    void SCChnl::removeInput() {
        inputIndex_ = -1;                           // A negative input index implies no input is associated with this SCChnl.
    }
    
    void SCChnl::removeRelay() {
        relayIndex_ = -1;                           // A negative relay index implies no relay is associated with this SCChnl.
    }
    
    int SCChnl::getIndex() {                        // Return this SCChnl's index into the arrary of SCChnls.
        if (theSCChnls_ == nullptr) {               // Is this SCChnl part of an array of SCChnls?
            return -1;                              // No. This SCChnl does not have an index. -1 represents no index.
        }                                           // This SCChnl is part of an array of SCChnls.
        for (int index = 0; index < theSCChnls_->size(); index++) {// Go through the array of SCChnls.
            SCChnl* scChnl = theSCChnls_->at(index);// Get access to the SCChnl with this index.
            if (scChnl->getName() == name_) {       // Does this SCChnl's name match the name from the array's SCChnl?
                return index;                       // Yes. Since names are unique, this is the index of this SCChnl.
            }                                       // The names didn't match. Evaluate the next SCChnl from the array.
        }                                           // There was no match.
        return -1;                                  // This SCChnl does not have an index. -1 represents no index.
    }

#pragma mark - SCChnls class implementation
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    SCChnls::SCChnls() {
        version_ = 1;                               // This is the 1st version of the new SCChnl file.
        filePath_ = "";                             // The path is the default.
        fileName_ = "SCChnls.xml";                  // The name of the fiel containing the SCChnl configurations.
        errorMessage_ = "";                         // There is no error message until an error occurs.
        lowerTemperatureLimit_ = -50.0;             // The default lower temperature limit(Fahrenheit);
        upperTemperatureLimit_ = 200.0;             // The default upper temperature limit(Fahrenheit);
        maximumInputIndex_ = -1;                    // Must be set before a new SCChnl can be added.
        maximumRelayIndex_ = -1;                    // Must be set before a new SCChnl can be added.
        maturityBase = 0;                           // The default maturity base line is zero.
        isMaturityReferencedToProgram = false;      // By default the maturity is not referenced to the program.
    }
    
    SCChnls::~SCChnls() {
        if (this->size() == 0) {                    // Are there any SCChnls?
            return;                                 // No. There is nothing to delete and nothing to clear from the vector.
        }                                           // There are SCChnls.
        for(unsigned int i = 0; i < this->size(); i++){	// Go through each SCChnl.
            delete this->at(i);						// Free the memory allocated to it.
        }                                           //
        this->clear();								// Clear all the array of SCChnls.
    }
    
    unsigned int SCChnls::getVersion() {
        return version_;                            // Provide the caller with the SCChnl file version.
    }
    
    void SCChnls::setFilePath(string value) {
        filePath_ = value;                          // Set the file path to what ever the caller wants. No error checking.
    }
    
    string SCChnls::getErrorMessage() {
        return errorMessage_;                       // Provide the caller with the current error message.
    }
    
    float SCChnls::getLowerTemperatureLimit() {
        return lowerTemperatureLimit_;              // Provide the caller with the lower temperature limit.
    }
    
    void SCChnls::setLowerTemperatureLimit(float value) {
        if (value < -200.0 ) {                      // Is the desired lower limit below -200F?
            value = -200.0;                         // Yes. Set the desired lower limit to -200F.
        }                                           //
        if (value > 400.0) {                        // Is the desired lower limit above 400F?
            value = 400.0;                          // Yes. Set the desired lower limit to 400F.
        }                                           // The desired value is within range.
        lowerTemperatureLimit_ = value;             // Set the lower temperature limit.
    }

    float SCChnls::getUpperTemperatureLimit() {
        return upperTemperatureLimit_;              // Provide the caller with the upper temperature limit.
    }
    
    void SCChnls::setUpperTemperatureLimit(float value) {
        if (value < -200.0 ) {                      // Is the desired upper limit below -200F?
            value = -200.0;                         // Yes. Set the desired upper limit to -200F.
        }                                           //
        if (value > 400.0) {                        // Is the desired upper limit above 400F?
            value = 400.0;                          // Yes. Set the desired upper limit to 400F.
        }                                           // The desired value is within range.
        upperTemperatureLimit_ = value;             // Set the upper temperature limit.
    }
    
    void SCChnls::add() {                           // Add a default name to SCChnls.
        SCChnl* aChnl = new SCChnl(this);           // Create a new SCChnl.
        long nameNumber = this->size();             // Create a number that represents the name.
        string testName = to_string(++nameNumber);  // Convert that number to a string.
        while (nameExists(testName) == true) {      // Does another SCChnl have that name?
            nameNumber++;                           // Yes. Increment the number in order to try a new name.
            testName = to_string(nameNumber);       // Convert that number to a string.
        }                                           // A unique name has been found.
        aChnl->setName(testName);                   // Set the SCChnl's name.
        this->push_back(aChnl);                     // Add the new chnl to SCChnls.
    }                                               // Note: The SCChnl pointer will be destroyes when SCChnls are destroyed.

    bool SCChnls::add(string name, int inputDex, int relayDex, float dev, unsigned int aLimit, int index) {
        errorMessage_ = "No error calling SCChnls::add().";// Set the default error message.
        if (name.length() == 0) {                   // Does the desired name contain characters?
            errorMessage_ = "The desired name has no characters.";// No. Create error message.
            return false;                           // Let the caller know a channel couldn't be added without a name.
        }
        if (nameExists(name) == true) {             // Is another channel already using the desired name?
            errorMessage_ = "The name \"" + name + "\" already is being used by another channel.";// Yes.
            return false;                           // Set the error message and return failure.
        }                                           // The name is OK to use.
        if (inputDex == -1 && relayDex == -1) {     // Is there at least a relay or an input?
            errorMessage_ = "A channel must have either an input or a relay or both.";// No. Create an error message.
            return false;                           // Let the caller know a channel couldn't be added without an input or relay.
        }                                           // There is either a relay or an input.
        if (inputDex > maximumInputIndex_) {        // Is the input too large?
            errorMessage_ = " The given input index " + to_string(inputDex) + " exceeds the maximum index allowed ";// Yes.
            errorMessage_ += to_string(maximumInputIndex_) + ".";// Create an error message.
            return false;                           // Let the caller know a channel with this large of input index cannot be added.
        }                                           // The given input index is OK.
        if (relayDex > maximumRelayIndex_) {        // Is the relay index too large?
            errorMessage_ = " The given relay index " + to_string(relayDex) + " exceeds the maximum index allowed ";// Yes.
            errorMessage_ += to_string(maximumRelayIndex_) + ".";// Create an error message.
            return false;                           // Let the caller know a channel with this large of relay index cannot be added.
        }                                           // The given relay index is OK.
        SCChnl* aChnl = new SCChnl(this);           // Create a new SCChnl.
        aChnl->setName(name);                       // Set the SCChnl's name.
        aChnl->setInputIndex(inputDex);             // Set the input index.
        aChnl->setRelayIndex(relayDex);             // Set the relay index.
        aChnl->deviation = dev;                     // Set the SCChnl's allowable temperature deviation from the setpoint.
        aChnl->setApproachLimit(aLimit);            // Set the SCChnl's approach limit.
        if (index == -1 || index >= size()) {       // Is the desired index somewhere in the middle of the array?
            this->push_back(aChnl);                 // No. Add the new SCChnl to the end of the array.
        }                                           // The SCChnl was either added to the end or needs to be added to the middle.
        else {                                      // The SCChnl needs to be inserted into the array at index.
            this->insert(this->begin() + index, aChnl);// Insert the new SCChnl at the desired location.
        }                                           // The new SCChnl has been added to SCChnls.
        errorMessage_ = "";                         // Reset the error message.
        return true;                                // Let the user know an SCChnl with the given parameters was added to SCChnls.
    }

    bool SCChnls::remove(unsigned int index) {
        if (index >= size()) {                      // Is the index too large?
            errorMessage_ = "The index " + to_string(index) + ", is greater than the largest index " + to_string(size() - 1) + ".";
            return false;                           // The idex doesn't reference anything that can be removed.
        }                                           // The index is valid.
        delete this->at(index);                     // Free the memory allocated to the SCChnl.
        this->erase(this->begin() + index);			// Remove the relay from SCChnls.
        errorMessage_ = "";                         // Reset the error message.
        return true;                                // Let the caller know the SCChnl was removed.
    }

    
    bool SCChnls::nameExists(string value) {
        for (int z = 0; z < size(); z++) {          // Go through each SCChnl.
            if (at(z)->getName() == value) {        // Does this SCChnl's name match the given name?
                errorMessage_ = value + " matches the name used by SCChnl[" + to_string(z) + "].";
                return true;                        // The name already exists. Let the caller know there is a match.
            }                                       // This SCChnl's name does not match the given name.
        }                                           // The name doesn't exist.
        errorMessage_ = "";                         // Clear the error message.
        return false;                               // Let the caller know the name doesn't exist.
    }
    
    bool SCChnls::relayIndexExists(int value) {
        for (int z = 0; z < size(); z++) {          // Go through each SCChnl.
            if (at(z)->getRelayIndex() == value) {  // Does this SCChnl's relay index match the given relay index?
                errorMessage_ = to_string(value) + " matches the relay index used by SCChnl[" + to_string(z) + "].";
                return true;                        // The relay index already exists. Let the caller know there is a match.
            }                                       // This SCChnl's relay index does not match the given relay index.
        }                                           // The relay index doesn't exist.
        errorMessage_ = "";                         // Clear the error message.
        return false;                               // Let the caller know there isn't another SCChnl with the given relay index.
    }

    void SCChnls::setMaximumRelayIndex(int value) {
        maximumRelayIndex_ = value;                 // Set the maximum valid relay index to what ever the caller wants.
    }
    
    void SCChnls::setMaximumInputIndex(int value) {
        maximumInputIndex_ = value;                 // Set the maximum valid input index to what ever the caller wants.
    }
 
    bool SCChnls::loadFromDB() {            // Load the configurations of the SCChnls.
     sqlite3 *db;                           // Create a location for the database.
     int  rc;                               // Create a return variable.
     string sql;                            // Create a string for SQL commands.
     string dbName = filePath_ + "SCConfig.db";// Set the configuration database name.
                                            // Open the database named SC_Config.db.
     rc = sqlite3_open(dbName.c_str(), &db);// It has a table which contains the SCChnl configurations (Chnls).
     if( rc ){                              // Was the database opened?
         errorMessage_ = "Can't open SCConfig.db to load the SC channels: ";// No. Create a message.
         errorMessage_ += string(sqlite3_errmsg(db));// Add the database error to the message.
         cout << errorMessage_ << endl;     // Display the error.
         return false;                      // Let the caller know the maturity was not read.
     }                                      // The database is open.
     sqlite3_busy_timeout(db, 50);          // If the DB is locked wait up to 50 milliseconds before giving up.
     string tableName = "Chnls";            // Create the table name for the SCChnls.
     sql = "SELECT * from " + tableName;    // Create an SQL statement to get all the SCChnl configurations.
     sqlite3_stmt *stmt;                    // Will contain the results of the querry.
     rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
     if (rc != SQLITE_OK) {                  // Was it successful?
         errorMessage_ = "Failed to query 'Chnls' for SC channel configurations: ";// No, there was a problem. Create an error message.
         errorMessage_ += string(sqlite3_errmsg(db));// Add the database error to the message.
         cout << errorMessage_ << endl;     // Display the error.
         sqlite3_finalize(stmt);            // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
         sqlite3_close(db);                 // Close SCConfig.db. Since finalize was just called there should be no errors.
         return false;                      // Let the caller know the SCChnl configurations couldn't be retrieved from the database.
     }                                      // The SCChnl configurations was retreived.
     while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {// Go through each row in the results.
         int id = sqlite3_column_int(stmt, 0);// The id is in the first column.
         const unsigned char *cName = sqlite3_column_text(stmt, 1);// The name is in the table's second column.
         string sName = std::string(reinterpret_cast<const char*>(cName));// Convert the name to a string.
         float fDev = sqlite3_column_double(stmt, 2);// The deviation is in the third column.
         int al = sqlite3_column_int(stmt, 3);// The approach limit is in the fourth column.
         int inDex = sqlite3_column_int(stmt, 4) - 1;// The input index is in the fifth column. The DB uses ones based the PM uses zero based.
         int outDex = sqlite3_column_int(stmt, 5) - 1;// The relay index is in the sixth column. The DB uses ones based the PM uses zero based.
         if(add(sName, inDex, outDex, fDev, al, -1) == false) {// Try adding an SCChnl with the loaded parameters to SCChnls. Was it added?
             // No. Create an error message and discontinue talking to the SCConfig.db.
             errorMessage_ = "Error adding channel " + to_string(id) + ": ";
             errorMessage_ += string(sqlite3_errmsg(db));// Add the database error to the message.
             cout << errorMessage_ << endl; // Display the error.
             sqlite3_finalize(stmt);        // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
             sqlite3_close(db);             // Close SCConfig.db. Since finalize was just called there should be no errors.
             return false;                  // Let the caller know there was an error.
         }                                  // The SCChnl was added to SCChnls.
     }                                      // The maturity has been read for each channel
     if (rc != SQLITE_DONE) {               // Was there an error going through the SCChnls in the DB?
         errorMessage_ = "Error loading SC channels: " + string(sqlite3_errmsg(db));// Yes. Create an error message.
         cout << errorMessage_ << endl;     // Display the error.
         sqlite3_finalize(stmt);            // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
         sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
         return false;                      // Return failure.
     }                                      // The each SCChnl's configuration has been successfully read.
     sqlite3_finalize(stmt);                // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
     sqlite3_close(db);                     // Close SCConfig.db. Since finalize was just called there should be no errors.
     return true;                           // Let the caller know all SCChnl configurations were read.
    }
    
}
