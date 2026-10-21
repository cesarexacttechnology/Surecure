//
//  Mini.hpp
//  PlantManager
//
//  Created by Mark Davis on 11/30/15.
//  Copyright © 2015 Products Engineering. All rights reserved.
//

#ifndef Mini_hpp
#define Mini_hpp

#include <stdio.h>
#include <cstdlib>
#include "SCEquipment.h"
#include "ThermoModule.hpp"
#include "RelayModule.hpp"
#include "CountModule.hpp"
#include "Socket.h"
#include "ServerSocket.h"
#include "SocketException.h"

using namespace std;
using namespace SCClasses;

class MiniEquip : public SCEquipment  {
public:
    /*! The default constructor. */
    MiniEquip();
    /*! The default destructor. */
    ~MiniEquip();
    /*! A list of pointers to the counter modules included as a part of the mini equipment in this sub-systems. */
    vector<CountModule*> countModules;
    /*! A list of pointers to the thermocoule modules included as a part of the mini equipment in this sub-systems. */
    vector<ThermoModule*> thermoModules;
    /*! A list of pointers to the relay modules included as a part of the mini equipment in this sub-systems. */
    vector<RelayModule*> relayModules;
    /*! Assigns the relays contained in this sub-system to the modules contained in this sub-system.
     * \param plantRelays A pointer to the list of all the plant's relays. */
    void initializeRelays(Relays* plantRelays);
    /*! Loads the configuration of this mini equipment. Uses 'filePath'.
     * \returns true if the configuration was successfully loaded. */
    bool loadFromDB();
    /*! The main loop to operate mini devices. */
    static void operateEquipment(MiniEquip* miniSys, Relays* plantRelays);
    /*! Sets the command to issued to this mini equipment. 
     * \param value The Alpha numric characters which make up the command. There is no checking for valid commands. */
    void setCommand(string value);
    /*! Gets the response obtained from this sub-system after issuing a command. */
    string getCommandResponse();
    /*! Clears the response obtained from this sub-system after issuing a command. */
    void resetCommandResponse();
    /*! \returns The number of thermocouple modules which are a part of this sub-system. */
    int getNoOfThermoMods();
    /*! \returns The number of relay modules which are a part of this sub-system. */
    int getNoOfRelayMods();
    /*! Creates and adds a new thermocouple module with the given address to the list of thermocouple modules.
     * \returns A pointer to the thermocouple module if successful. nullptr on failure.
     * \param address The desired address of the new thermocouple module.
     * \param isActivated true if the new thermocouple module will be active.
     * \param pauseForRetry Contains the number of minutes between retries when a failure in communication occurs.
     * \param startIndex Contains the input index of this module with respect to the starting input index of this sub-system.
     * \param type Is either 4018 or 6018 to indicate the module type.
     * \param nInputs The number of thermocouples connected to this thermocouple module. */
    ThermoModule* addThermoMod(unsigned int address,
                               bool isActivated,
                               unsigned int pauseForRetry,
                               int startIndex,
                               string type,
                               int nInputs);
    /*! Creates and adds a new relay module with the given address to the list of relay modules.
     * \returns A pointer to the relay module if successful. nullptr on failure.
     * \param address The desired address of the new relay module.
     * \param isActivated true if the new thermocouple module will be active.
     * \param pauseForRetry Contains the number of minutes between retries when a failure in communication occurs.
     * \param startIndex Contains the relay index of this module with respect to the starting relay index of this sub-system.
     * \param type Is either 4050 or 6050 to indicate the module type.
     * \param nRelays The number of relays connected to this relay module. */
    RelayModule* addRelayMod(unsigned int address,
                             bool isActivated,
                             unsigned int pauseForRetry,
                             int startIndex,
                             string type,
                             int nRelays);
    /*! Increments the time-on, if the relay was on, for each relay in the given module.
     *  May not be accurate if the thread is interrupted and delayed.
     * \param relayMod The relay module containing the relays to increment time on. */
    void incrementRelayTimes(RelayModule* relayMod);
    /*! \returns true If none of the thermocouple or relay modules are communicating. */
    bool isCommunicationDownOnAllModules();
    /*! Resets all the isLastCommAttemptValid to true for each thermocouple module & relay module.
     *  This is necessary after a sub-system reconnection or this sub-system will immediately try to reconnect again. */
    void resetCommunicationsOnAllModules();
    /*! Turn off all the relays but the one to be tested. It is turned on if it is part of this sub-system. */
    void relayTestPeform();
    /*! Turn off all the relays that are part of this sub-system. */
    void relaysAllOff();
    /*! The flag used to read special voltages.
     * 0 -> Not reading special voltages.
     * 1 -> Special voltage reading process in effect.*/
    unsigned int readVoltageState;
    /*! The socket used to communicate with the mini modules. */
    Socket* miniSocket;
    /*! \returns The this mini sub-systems server name. */
    string getServerName();
    /*! \returns true if the serial server name can be obtained. */
    bool isSerialServerConnected();
    /*! The IP address of the connected serial server (NetCom device) . */
    string connectedIPAddress;

private:
    /*! The port number for this sub-system of mini modules */
    int socketPort_;
    /*! The command being issued to the mini modules connected to this sub-system. */
    string command;
    /*! The result obtained from the mini module when a command was issued. */
    static string commandResponse;
    /*! Sends commands using 'miniSocket' to the mini modules.
     * \param miniSys The mini system sending the command.
     * \param replyLength The number of characters that shold be returned when the command's reply is sent.
     * \returns true if the command was successfully sent and there is a response of the correct length waiting. */
    static bool sendCommand(MiniEquip* miniSys, int replyLength);
    /*! Sends commands using 'miniSocket' to the mini modules.
     * \param miniSys The mini system sending the command.
     * \param replyLength The number of characters that shold be returned when the command's reply is sent.
     * \returns true if the command was successfully sent and there is a response of the correct length waiting. */
    static bool sendCommand2(MiniEquip* miniSys, int replyLength);
    /*! Sets the relays on the given relay module in accordance with relayMod->desiredRelayStates.
     *  Works in conjunction with the operateEquipment method which is static. Thus, the relay module pointer must be passed.
     * \param relayMod The relay module containing the relays to set. */
    void setRelaysOnModule(RelayModule* relayMod);
    /*! The server name associated with this mini sub-system. */
    string serverName_;
    /*! \returns The server name for the given IP address. Also stores the server name in serverName_.
     *  Queries the RS232 server using system commands to do get the server name.
     *  If the string is empty, an error occured.
     *  \param The IP address for the RS232 server. */
    string getSerialServerName(string ipAddress);
};
#endif /* Mini_hpp */
