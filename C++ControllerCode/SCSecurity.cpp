//
//  SCSecurity.cpp
//  PlantManager
//
//  Created by Mark Davis on 12/9/15.
//  Copyright © 2015 Products Engineering. All rights reserved.
//

#include "SCSecurity.hpp"

namespace SCClasses {


SCSecurity::SCSecurity() {
    plantManagerPath = "";
    temperatureDataPath = "/Temperatures/"; // The default sub-folder used to store temperature data.
    securityStartTime_ = chrono::system_clock::now();   // The default start time is now.
    plantBMPKey_ = 0;                       // 0 indicates that a Plant.bmp key does not exist.
    secondsOperated_ = 0;                   // There are no seconds operated until it has been recorded and read.
    ttMonth_ = 0;                           // This is an invalid month number.
    ttYear_ = 0;                            // This is an invalid year number.
    recordSize_ = 0;                        // This is an invalid record size.
    secondsOperatedStoredAt_ = chrono::system_clock::now();//The system starts now. Thus the effective last storage time is now.
}

SCSecurity::~SCSecurity() {
    
}

bool SCSecurity::ttFileCreate(unsigned int year, unsigned int month) {
    string ttFileName = plantManagerPath + temperatureDataPath;
    if (directoryExists(ttFileName) == false) {// Does the Temperatures directory exist?
        string strDir = "mkdir " + ttFileName;// No. Create an operating system command for creating the Temperatures directory.
        if(system(strDir.c_str()) != 0) {   // Execute the command to create the Temperatures directory.
            cout << "Failed to create the folder to contain the .tt file!" << endl;
            return false;                   // No. Let the caller know about the failure.
        }                                   // The folder for the .tt file has been created.
    }                                       // The folder for the .tt file exists.
    ttFileName += to_string(year) + to_string(month) + ".tt";// Append the .tt temperature file name to the path.
    fstream ttFile(ttFileName, ios::binary | ios_base::out);// Create the .tt file.
    if (ttFile.is_open() == false) {        // Did the .tt file open?
        cout << "Failed to create " << ttFileName << "." << endl;
        return false;                       // No. Failed to create the .tt file. Let the caller know.
    }                                       // The .tt file is open.
    short recordSize = nRecordInputs_;      // The first value in the .tt file is the record size.
    if (recordSize < 6) {                   // Is the record size less than six?
        recordSize = 6;                     // In order for the first record to hold all the header info, the recordsize
    }                                       // must be at least six.
    recordSize++;                           // The record size is the number of inputs(>=6) + 1 for the minute of the month.
    recordSize *= 2;                        // Each input is stored as two bytes of data & so is the minute of the month.
    ttFile.write((char*)&recordSize, sizeof(short));// Save the record size.
    short s = (short)month;                 // Next need to save the month number as two bytes.
    ttFile.write((char*)&s, sizeof(short)); // Save the month contained in this time-temperature file.
    s = (short)year;                        // Next need to save the year number as two bytes.
    ttFile.write((char*)&s, sizeof(short)); // Save the year contained in this time-temperature file.
    chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// Contains the current date/time.
    double dSecondsOperated = 0;            // Will contain the total number of seconds operated.
    if (now > securityStartTime_) {         // Prevent a negative number being calculated for the total number of seconds operated.
        chrono::duration<double> secondsOperated = now - securityStartTime_;// Number of seconds since the last state change.
        dSecondsOperated = secondsOperated.count();// Convert the time duration into a double.
    }                                       // The seconds operated are now represented as a double.
    if (dSecondsOperated > 86400) {         // Has the system operated for at least day?
        dSecondsOperated -= 86400;          // Remove one day from the seconds operated to compensate for down time.
    }                                       // The seconds operated has been adjusted for down time.
    int nSecondsOperated = 0;               // Going to convert the double seconds operated to an integer for storage.
    if (dSecondsOperated < 4000000000 && dSecondsOperated > 0) {// Is the double such that it can convert?
        nSecondsOperated = (unsigned int)dSecondsOperated;// Yes. Convert it.
    }                                       // The seconds operated are in a form which can be stored.
    ttFile.write((char*)&nSecondsOperated, 4);// Write the seconds the system has operated.
    ttFile.write((char*)&plantBMPKey_, 4);  // Write the key from Plant.bmp to the .tt file.
    ttFile.close();                         // The .tt file should have been created with a header. Time to close it.
    ios_base::iostate state = ttFile.rdstate();// Read the status of all the operations on the file.
    if (state && ios_base::goodbit == false) {// Were there any errors writing to the .tt file?
        cout << "Unable to write header in Temperature file " << ttFileName << "." << endl;// Yes. Send an error message.
        return false;                       // Failed to create the .tt temperature file.
    }                                       // The .tt file was created with its header.
    return true;                            // Let the caller know the .tt file was created with its header.
}

bool SCSecurity::directoryExists( string path )
{
    if ( path == "") return false;
    
    DIR *pDir;
    bool bExists = false;
    
    pDir = opendir (path.c_str());
    
    if (pDir != NULL)
    {
        bExists = true;
        (void) closedir (pDir);
    }
    
    return bExists;
}
    
    
unsigned int SCSecurity::getNumOfTemperatureInputs() {
    return nTemperatureInputs_;                     // Provide the caller with the number of temperature inputs.
}
    
void SCSecurity::setNumOfTemperatureInputs(unsigned int value) {
    if (value < 6) {                                // Are there less than 6 temperature inputs?
        nRecordInputs_ = 6;                         // Yes. A record must be at least big enough for 6 because of the file header.
    } else {                                        // No. The number of temperature inputs is larger than 6.
        nRecordInputs_ = value;                     // Set the inputs used for the record to the value.
    }                                               // The number of inputs used to determine the record size is set.
    nTemperatureInputs_ = value;                    // SEt the number of temperature inputs.
}
    
bool SCSecurity::areSecurityFilesOK() {
    return true;
}

bool SCSecurity::readTTHeader(unsigned int yearNo, unsigned int monthNo) {
    string ttFileName = plantManagerPath + temperatureDataPath;
    if (directoryExists(ttFileName) == false) {// Does the Temperatures directory exist?
        cout << "The directory " << ttFileName << "Couldn't be opened!" << endl;
        return false;                       // No. Let the caller know about the failure.
    }                                       // The folder for the .tt file exists.
    ttFileName += to_string(yearNo) + to_string(monthNo) + ".tt";// Append the .tt temperature file name to the path.
    fstream ttFile(ttFileName, ios::binary | ios_base::in);// Open the .tt file to read the header.
    if (ttFile.is_open() == false) {        // Did the .tt file open?
        cout << "Failed to open " << ttFileName << "." << endl;
        return false;                       // No. Failed to open the .tt file. Let the caller know.
    }                                       // The .tt file is open.
    ttFile.read((char*)&recordSize_, 2);    // Read the record size for this .tt file.
    ttFile.read((char*)&ttMonth_, 2);       // Read the month number for this .tt file.
    ttFile.read((char*)&ttYear_, 2);        // Read the year number for this .tt file.
    ttFile.read((char*)&secondsOperated_, 4);// Read the seconds operated fot this .tt file.
    ttFile.read((char*)&plantBMPKey_, 4);   // Read the Plant Bitmap Key for this .tt file.
    ttFile.close();                         // The .tt file header should have been read. Time to close it.
    ios_base::iostate state = ttFile.rdstate();// Read the status of all the operations on the file.
    if (state && ios_base::goodbit == false) {// Were there any errors reading the .tt file header?
        cout << "Unable to read header in Temperature file " << ttFileName << "." << endl;// Yes. Send an error message.
        return false;                       // Failed to read the .tt temperature file header.
    }                                       // The .tt file header was read.
    return true;                            // Let the caller know  the .tt file header was read.
}

short SCSecurity::getRecordSize() {
    return recordSize_;                             // Provide the caller the current record size.
}
 
short SCSecurity::getTTYear() {
    return ttYear_;                                 // Provide the caller the current .tt header year.
}

short SCSecurity::getTTMonth() {
    return ttMonth_;                                // Provide the caller the current .tt header month.
}
   
int SCSecurity::getPlantBMKey() {
    return plantBMPKey_;                            // Provide the caller the current .tt header Plant Bit Map key.
}

unsigned int SCSecurity::getTTSecondsOperated() {
    return secondsOperated_;                        // Provide the caller the current .tt header seconds operated.
}
    
unsigned int SCSecurity:: getNumOfRecordInputs() {
    return nRecordInputs_;                          // Provide the caller with the number of record inputs.
}

bool SCSecurity::incrementSecondsOperated() {
// Start here, translate this function.
    time_t nowTime = time(0);           // Contains the current time.
    chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// Also contains the current time.
    chrono::duration<double> timePast;  // The amount of time which has passed since the seconds operated were last stored.
    double	dSecondsOperated;           // Will contain the number of seconds since the seconds operated were last stored.
    struct tm* tmNow;                   // Will contain a more usable form of the current time.
    tmNow = localtime(&nowTime);        // Convert the current time into a more usable format.
    string ttFileName = plantManagerPath;// The path to the temperature data files begins with the path to the Plant Manger.
    ttFileName += temperatureDataPath;  // The path then contains the sub-folder for temperature data files.
    int thisYear = tmNow->tm_year + 1900;// The year is in years since 1900. Thus, we must add 1900 to get the real year.
    int thisMonth = tmNow->tm_mon + 1;  // The month starts a zero based. Thus, we must add one to get the month number.
    ttFileName += to_string(thisYear) + to_string(thisMonth) + ".tt";// Append the .tt temperature file name to the path.
    fstream ttFile(ttFileName, ios::binary | ios_base::in);// Open the .tt file.
    if (ttFile.is_open() == false) {    // Did the .tt file open?
        cout << "Failed to open " << ttFileName << "." << endl;// No. The file didn't open.
        if(ttFileCreate(thisYear, thisMonth) == false) {// Was the .tt file created?
            cout << "Failed to create " << ttFileName << "." << endl;// No/ It wasn't created.
            return false;               // No. Failed to increment the seconds in the .tt file. Let the caller know.
        }                               // The .tt was created.
    }                                   // The .tt file is open.
    ttFile.close();                     // The .tt file needs to be closed so it can be reopened with the correct format.
    if (now < secondsOperatedStoredAt_) {// Past the previous storage time?
        secondsOperatedStoredAt_ = now; // No. This is strange. Reset the storage time so this doesn't happen again.
        return true;                    // Nothing else to do.
    }                                   // Now is after the previous storage time.
    timePast = now - secondsOperatedStoredAt_;// Time past is the difference between now and the storage time.
    dSecondsOperated = timePast.count();// Convert to seconds.
    int nSecondsOperated = 0;           // Going to convert the double seconds operated to an integer for storage.
    if (dSecondsOperated < 4000000000 && dSecondsOperated > 0) {// Is the double such that it can convert?
        nSecondsOperated = (unsigned int)dSecondsOperated;// Yes. Convert it.
    }                                   // The seconds operated are in a form which can be stored.
    ttFile.open(ttFileName, ios::binary | ios_base::out | ios::app);// Open the .tt file.
    ttFile.seekp(6, ios_base::beg);
    ttFile.write((char*)&nSecondsOperated, 4);// Write the seconds the system has operated.
    ttFile.close();
    ios_base::iostate state = ttFile.rdstate();// Read the status of all the operations on the file.
    if (state && ios_base::goodbit == false) {// Were there any errors writing to the .tt file?
        cout << "Unable to write to " << ttFileName << "." << endl;// Yes.
        return false;                   // Failed to write this record to the .tt file.
    }                                   // The .tt record was written.
    secondsOperatedStoredAt_ = now;     // The seconds operated were just stored. Save the time now for the next storage time.
    return  true;                       // Let the caller know the seconds operated were successfully stored.
    //Start here, test this function. Does it destroy all other data when it is called?
}
    

    
} // End of namespace SCClasses.