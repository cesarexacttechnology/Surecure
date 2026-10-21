/*
 * SCInput.h
 *
 *  Created on: Jun 10, 2013
 *      Author: markdavis
 */

#ifndef SCINPUT_H_
#define SCINPUT_H_

using namespace std;

#include <string>
#include <vector>
#include "SCMaturity.h"
#include <sqlite3.h>

namespace SCClasses {


class SCInput {
public:
	SCInput();
	virtual ~SCInput();

    /*!
     * The type of sensor connected to this input.
     */
    enum Sensor {
    	SENSOR_NONE,
    	SENSOR_THERMOCOUPLE_T,
    	SENSOR_THERMOCOUPLE_K,
    	SENSOR_CURRENT_TRANSFORMER,
    	SENSOR_VOLTAGE_TRANSDUCER,
    	SENSOR_PRESSURE_TRANSDUCER,
    	SENSOR_ONE_WIRE
    };
    Sensor sensorType;
    /*!
     * The units used to measure this sensor.
     */
    enum Units{
        UNITS_NONE,
        UNITS_FAHRENHEIT,
        UNITS_CELSIUS,
        UNITS_AMPS,
        UNITS_VOLTS,
        UNITS_PSI
    };
    /*! The measurement units for this channel. */
    Units measuredUnits;
    /*! The zero based index of this input. */
	uint32_t indexNo;
    /*! \returns This input's most recent measured value. */
	double getMeasuredValue(void);
    /*! Sets this input's most recent value. 
     * \param value The most recent value to assign to this input. */
	void   setMeasuredValue(double value);
    /*! Increment this input's accumulated value. */
	void   incrementAccumulatedValue(void);
    /*! Resets this input's accumulated value so a new average value can be calculated. */
	void   resetAccumulatedValue(void);
    /*! \returns This input's average measured value since last reset. */
	double getMeasuredValueAverage(void);
    /*! \returns the ID(serial number) for this input device. */
    string getID();
    /*! Sets the ID(serial number) for the device associated with this input.
     * \param idString The string representation of the device associated with this input. 
     * The ID string should be 16 characters in length. If it is less, '_'s will be appended until the length is 16. */
    void setID(string idString);
    /*! \returns The sub-system to which this input belongs. Ones based. */
    int getSystemNo(void);
    /*! Sets the sub-system to which this input belongs.
     * \param value The sub-system index. The index is ones based. Thus, the index of the first sub-system is one. 
     *              A value of zero implies the index has not been set. */
    void setSystemIndex(int value);
	void display();

private:
    /*! The number of measured values entered into the accumulated value. */
	int accumulatedCount_;
    /*! The most recent measured value for the input. */
	double measuredValue_;
    /*! The sum of the most recent measure values used to calculate an average. */
	double accumulatedValue_;
    /*! The serial number for this input. It is exactly 16 characters in length. */
    string id_;
    /*! The number of the sub-system which includes this input.
     *  This value is assigned once the input file and the SCSystem file have been read.
     *  It is calculated from these two files to avoid configuration conflicts. */
    int systemNo_;

};

////////////////////////////////////////SCInputs///////////////////////////////////////////////////////////

class SCInputs: public vector<SCInput*>{
public:
	SCInputs();
	virtual ~SCInputs();
    /*! \returns The version of the input configurations. */
	int getVersion(void);
    /*! Sets The version of the input configurations. 
     * \param value The value for the input configurations. */
	void setVersion(int value);
    /*! \returns The file path to the input configuration data. */
	static string getFilePath(void);
    /*! Sets the path to the input configuration data.
     * \param path The path to the input configurtion data. */
	static void setFilePath(string path);
    /*! \returns The number of inputs. */
	int getNumberOfInputs(void);
    /*! Is true if the SCInputs have been loaded. */
	static bool areLoaded;
    /*! Adds a default input to the inputs. */
	void addInput(void);
    /*! Adds the given input to the end of the vector of inputs. 
     * \param theInput The input to be added to the vector of inputs. */
	void addInput(SCInput* theInput);
    /*! Adds the given input to the vector of inputs at the given position.
     * Changes the indexes of all the inputs following this one.
     * \param position The place the input will be placed.
     * \param theInput The input to add to the vector of inputs. */
	void addInput(unsigned int position, SCInput* theInput);
    /*! Removes the input at the given position.
     * Changes the indexes of all the inputs following removed input.
     * \param position The position in the vector of the input to remove.*/
	void removeInput(int position);
    /*! Loads the input configurations from SCConfig.db, inputs table .
     * \returns True if the configurations of all inputs were successfully loaded. */
    bool loadFromDB(void);
    /*! \returns An error message which provides additional information for a failing function. */
    string getErrorMessage();

private:
    /*! The version of the input configuration. */
	int version_;
    /*! The path to the file containing the input configuration. */
	static string filePath_;
    /*! Contains an error message which provides additional information if a function fails. */
    string errorMessage_;

};


///////////////////////////////////////TemperatureInput/////////////////////////////////////////////////////

class TemperatureInput: public SCInput{
public:
	TemperatureInput();
	virtual ~TemperatureInput();

};


///////////////////////////////////////PressureInput////////////////////////////////////////////////////////

class PressureInput: public SCInput{
public:
	PressureInput();
	virtual ~PressureInput();
    /*! A constant in the equation Pressure = k1 + k2 * volts. */
	double k1;
    /*! A constant in the equation Pressure = k1 + k2 * volts. */
	double k2;
};

} /* namespace SCInput */
#endif /* SCINPUT_H_ */
