//
//  SCSecurity.hpp
//  PlantManager
//
//  Created by Mark Davis on 12/9/15.
//  Copyright © 2015 Products Engineering. All rights reserved.
//

#ifndef SCSecurity_hpp
#define SCSecurity_hpp

#include <stdio.h>
#include <string>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <chrono>
using namespace std;

namespace SCClasses {
    
class SCSecurity {
public:
    /*! The default constructor. */
    SCSecurity();
    /*! The default destructor. */
    ~SCSecurity();
    /*! Creates a .tt file for the given month and year.
     * \param year The year this .tt file is for.
     * \param month The month this .tt file is for. 
     * \returns True if the file is created. */
    bool ttFileCreate(unsigned int year, unsigned int month);
    /*! The sub-folder of the Plant Manager's working directory used to store temperature data. */
    string temperatureDataPath;
    /*! The sub-folder of the Plant Manager's working directory used to store temperature data. */
    string plantManagerPath;
    /*! \returns true if the given directory exists. 
     * \param path the directory to test to see if it exists. */
    bool directoryExists( string path );
    /*! \returns The number of temperature inputs. */
    unsigned int getNumOfTemperatureInputs();
    /*! Sets the number of temperature inputs. 
     * \param value The number of temperature inputs. */
    void setNumOfTemperatureInputs(unsigned int value);
    /*! \returns The number of bytes in a time-temperature record (possibly from the last .tt header read). */
    short getRecordSize();
    /*! \returns true If all the security files correlate. */
    bool areSecurityFilesOK();
    /*! Reads the header for the .tt file with the given year and month. 
     * \param yearNo The year number (ex.: 2015).
     * \param monthNo The month number (1 through 12 are valid). 
     * \returns true If the header is completely read. */
    bool readTTHeader(unsigned int yearNo, unsigned int monthNo);
    /*! \returns The last year read from a .tt file header. */
    short getTTYear();
    /*! \returns The last month read from a .tt file header. */
    short getTTMonth();
    /*! \returns The last BitMap Key read from a .tt file header. */
    int getPlantBMKey();
    /*! \returns The number of inputs used for creating a time-temperature records. */
    unsigned int getNumOfRecordInputs();
    /*! \returns The last number of seconds operated read from a .tt file header. */
    unsigned int getTTSecondsOperated();
    /*! Increments and then writes the seconds operated & the key to the .tt file for this month and year. 
     * \returns true if the seconds operated are incrementd and stored successfully. */
    bool incrementSecondsOperated();
    
private:
    /*! The number of temperature inputs. */
    unsigned int nTemperatureInputs_;
    /*! The number of inputs used for creating a time-temperature records. */
    unsigned int nRecordInputs_;
    /*! The size of a time-temperature record in bytes. */
    short recordSize_;
    /*! Contains the date/time when this program starts. */
    chrono::time_point<chrono::system_clock> securityStartTime_;
    /*! 0 indicates that a Plant.bmp key does not exist. */
    int plantBMPKey_;
    /*! The total number of seconds the Plant Manager has operated. */
    unsigned int secondsOperated_;
    /*! The year number last read from a .tt file header. */
    short ttYear_;
    /*! The month number last read from a .tt file header. */
    short ttMonth_;
    /*! Contains the date/time when seconds operated were last stored. */
    chrono::time_point<chrono::system_clock> secondsOperatedStoredAt_;

};
    
}
#endif /* SCSecurity_hpp */
