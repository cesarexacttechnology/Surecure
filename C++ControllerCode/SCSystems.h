//
//  SCSystems.h
//  PlantManager
//
//  Created by Mark Davis on 6/3/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#ifndef PlantManager_SCSystems_h
#define PlantManager_SCSystems_h

#include <vector>
#include <string>
#include "SCInput.h"
#include "Relays.h"

using namespace std;

namespace SCClasses {

    class SCSystem {
    public:
        enum SystemType {               // The type of hardware used by this sub-system.
            SYSTYPE_STANDARD,           // Lawson Full Feature
            SYSTYPE_MICRO,              // Lawson Micro
            SYSTYPE_MINI,               // Adlink or ADAM
            SYSTYPE_ONEWIRE,            // One wire
            SYSTYPE_NONE                // The type has not been assigned.
        };
        
    private:
        /*! Contains the code for the baud rate used by this sub-system.
         * Baud codes: 0->9600, 1->4800, 2->2400, 3->1200, 4->600, 5->300.
         */
        int16_t baudCode_;
        /*! The baud rate used by this sub-system. */
        int baudRate_;
        /*! The type of hardware used by this sub-system. */
        SystemType type_;
        /*! The low pass filter for Lawson equipment. */
        int16_t filter_;
        /*! The code representing the number of samples to average together 
         * to produce a reading for Lawson equipment. */
        int16_t averageCode_;
        /*! Contains the rate at which data is sampled on a Lawson based system.
         * Between 10 and 1,000 samples per second is allowed. */
        int16_t dataRate_;
        /*! Contains the amount of time in milliseconds between data cycles. */
        int16_t cycleLength_;
        /*! Contains this sub-system's number. 
         * If there were 3 sub-systems, this system could have a SubSystemNo_ between 1 and 3.*/
        int32_t subSystemNo_;
        /*! This sub-system's first physical input index (zero based for the entire system). */
        int16_t firstInput_;
        /*! This sub-system's number of phsical inputs. */
        int16_t nInputs_;
        /*! This sub-system's first relay index (zero based for the entire system). */
        int16_t firstRelay_;
        /*! This sub-system's number of relays. */
        int16_t nRelays_;
        /*! This sub-system's com port number (between 1 and 16). */
        uint8_t portNo_;
        /*! This sub-system's unix com port name. This is also used to hold the configured server name on a mini sub-system. */
        string portName_;
        /*! This sub-system's wireless ID. */
        uint32_t wirelessID_;

    public:
        /*! The default constructor. */
        SCSystem();
        /*! Sets the left system object equal to the right system object. */
        SCSystem& operator=(SCSystem otherSys);
        /*! The default destructor. */
        virtual ~SCSystem();
        /*! \returns The code for the baud rate used by this sub-system.
         *
         * Baud codes: 0->9600, 1->4800, 2->2400, 3->1200, 4->600, 5->300.
         */
        int getBaudCode();
        /*! Used to set this sub-system's baud code.
         *
         * Baud codes: 0->9600, 1->4800, 2->2400, 3->1200, 4->600, 5->300.
         * \param baudCode The code for the baud rate used by this sub-system.
         */
        void setBaudCode(int baudCode);
        /*! \returns The baud rate used by this sub-system. */
        int getBaudRate();
        /*! Used to set this sub-system's baud rate.
         *
         * Valid baud rates: 9600, 4800, 2400, 1200, 600, and 300.
         * \param baudRate The baud rate to be used by this sub-system.
         */
        void setBaudRate(int baudRate);
        /*! \returns This sub-system's type(The hardware it uses). */
        SystemType getType();
        /*! Used to set this sub-system's type(The hardware it uses).
         * \param type The type of hardware which makes up this sub-system.
         */
        void setType(SystemType type);
        /*! \returns This sub-system's filter number 0->4, 1->40, 2->400 hz. */
        int getFilter();
        /*! Used to set this sub-system's low pass filter number; 0->4, 1->40, 2->400 hz. 
         * \param filterNo This sub-system's filter number.
         */
        void setFilter(int filterNo);
        /*! Contains the code for the number of samples to average per reading
         * on a Lawson based system. 0->1, 1->2, 2->4, 3->8, 4->16.
         * \returns This sub-system's code for the number of samples to average.
         */
        int getAverageCode();
        /*! Used to set the code representing the number of samples to average per reading.
         * The code: 0->1(no averaging), 1->2(2 samples/reading), 2->4(4 samples/reading), 
         * 3->8(8 samples/reading), 4->16(16 samples/reading).
         * \param averageCode The code representing the number of samples to average per reading.
         */
        void setAverageCode(int averageCode);
        /*! Contains the rate at which data is sampled on a Lawson based system. 
         * \returns This sub-systems data rate in samples per second. */
        int getDataRate();
        /*! Used to set the rate at which data is sampled on a Lawson based system.
         * Between 10 and 1,000 samples per second is allowed. 
         * \param dataRate The number of samples per second this sub-system is to take. */
        void setDataRate(int dataRate);
        /*! Contains the amount of time in milliseconds between data cycles. 
         * \returns The number of milliseconds between data cycles. */
        int getCycleLength();
        /*! Used to set the number of milliseconds between data cycles.
         * Must be greater than or equal to 100 mSecs.
         * \param cycleLength The number of milliseconds between cycles. */
        void setCycleLength(int cycleLength);
        /*! \returns This sub-system's number.
         * If there were 3 sub-systems, this sub-system could have a SubSystemNo between 1 and 3.
         * -1 or 0 means this sub-system is not part of the system. */
        int getSubSystemNo();
        /*! Used to set this sub-system's number(one based).
         * \param subSystemNo The number this sub-system is going to be associated with. */
        void setSubSystemNo(int subSystemNo);
        /*! \returns The index of this sub-system's first input (zero based) */
        int getFirstInput();
        /*! Used to set this sub-system's first input (zero based, system wide).
         * \param firstInput This sub-system's first input index(zero based, system wide, -1 means no inputs). */
        void setFirstInput(int firstInput);
        /*! \returns The number of inputs attached to this sub-system. */
        int getNumberOfInputs();
        /*! Used to set the number of inputs. 
         *  \param noInputs The number inputs this sub-system should have. */
        void setNumberOfInputs(int noInputs);
        /*! \returns The index of this sub-system's first relay (zero based) */
        int getFirstRelay();
        /*! Used to set this sub-system's first relay (zero based, system wide).
         * \param firstRelay This sub-system's first relay index(zero based, system wide, -1 means no relays). */
        void setFirstRelay(int firstRelay);
        /*! \returns The number of relays attached to this sub-system. */
        int getNumberOfRelays();
        /*! Used to set the number of relays.
         *  \param noRelays The number relays this sub-system should have. */
        void setNumberOfRelays(int noRelays);
        /*! \returns This sub-systems port name. */
        string getPortName();
        /*! Sets this sub-system's port name.
         * \param partName The name of the serial port used by this sub-system. */
        void setPortName(string portName);
        /*! Is true if this sub-system uses wireless settings. */
        bool isWireless;
        /*! Is true if this sub-system is trying to be used. */
        bool isOnLine;
        /*! Sets this sub-system's wireless ID.
         * \param wirelessID The desired wireless ID for this sub-system. */
        void setWirelessID(unsigned int wirelessID);
        /*! Get this sub-system's wireless ID. */
        unsigned int getWirelessID();
        /*! Displays the configuration for this sub-system. */
        void display();
        /*! \returns A string representation of this system's hardware type. */
        string getTypeString();
       /*! The type of port used to connect to the I/O equipment. */
       enum Connection {
           CONN_SOCKET,
           CONN_SERIAL
       };
       Connection connectionType;

    };

    
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    class SCSystems: public std::vector<SCSystem*>  {
    private:
        /*! The version of the file holding the system's configuration. */
        int32_t version_;
        /*! The location of the file holding the system's configuration. */
        string filePath_;
        /*! Contains an error message which provides additional information if a function fails. */
        string errorMessage_;

    public:
        SCSystems();                                // Default constructor.
        virtual ~SCSystems();

        /*! The Inputs associated with this system. */
        SCInputs* inputs;
        /*! The Relays associated with this system. */
        Relays* relays;
        
        /*! \returns The system's version. */
        int getVersion();
        
        /*! \returns The path to the file containing the system configuration. */
        string getFilePath(void);
        /*! \returns An error message providing additional information about the last failed function call. */
        string getErrorMessage();
        /*! Sets the path to the file containing the system configuration. 
         * \param strPath The path to the system configuration file. */
        void setFilePath(std::string strPath);
        /*! Loads each sub-system's configuration from the systems table in SCConfig.db.
         * \returns True if each sub-system's configurations is loaded successfully. */
        bool loadFromDB();
        /*! Loads the input configuration and the relay configuration if required.
         *  \returns True if the configurations have already beeen loaded or are successfully loaded. */
        bool areInputsRelaysLoaded();
        /*! Loads the input configuration and the relay configuration from the database if required.
         *  \returns True if the configurations have already beeen loaded or are successfully loaded. */
        bool areDBInputsRelaysLoaded();
        /*! Adds the given sub-system to the vector of sub-systems.
         * \param subSystem* A pointer to the sub-system to be added. */
        void addSubSystem(SCSystem* subSystem);
        /*! Removes the given sub-system from the vector of sub-systems.
         * \param i The number of the sub-system to remove(ones based). */
        void removeSubSystem(int i);
        /*! \returns The index of the sub-system(zero based) containing the given relay index.
         *  \param relayIndex The relay's index(zero based, system wide) used to determine its sub-system index. */
        int getSysDexContainingRelay(int relayIndex);
        /*! \returns The index of the sub-system(zero based) containing the given input index.
         *  \param inputIndex The input's index(zero based, system wide) used to determine its sub-system index. */
        int getSysDexContainingInput(int inputIndex);
        /*! \returns True if the given sub-system is valid.
         *  \param subSys The sub-system to evaluate for validity.  */
        bool isValidSubSystem(SCSystem* subSys);

    };
}


#endif // PlantManager_SCSystems_h
