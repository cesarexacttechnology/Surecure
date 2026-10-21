//
//  main.cpp
//  PlantManager
//
//  Created by Mark Davis on 2/20/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#include <iostream>
#include "SCInput.h"
#include "Relays.h"
#include "SCSystems.h"
#include "SCMaturity.h"
#include "SCProgram.h"
#include "SCPulseMeters.h"
#include "SCSecurity.hpp"
#include "SCChnls.h"
#include <string>
#include <fstream>
#define _POSIX_SOURCE
#include <unistd.h>
#undef _POSIX_SOURCE
#include <thread>
#include <chrono>
#include "SCSockets.h"
#include "Manager.h"
#include "OneWire.h"
#include "ServerSocket.h"
#include "SocketException.h"

#include "SCEquipment.h"
#include "Beds.hpp"
#include "ThermoModule.hpp"
#include "RelayModule.hpp"


using namespace std;
using namespace SCClasses;

Manager theManager;                                         // Create the Manager(runs all the hardware).
bool continueRunning = true;                                // The flag to keep the Plant Manager running.


void getSCCommandThread() {
    int count = 1;
    try
    {
        ServerSocket server ( 64900 );                  // Create a server socket to receive a client's request for data.
        std::cout << "Server Socket created." << std::endl;
        
        while ( theManager.isToContinueRunning == true ) {
            Socket new_sock;                      // Create a socket for the client.
            server.accept ( new_sock );                 // Accept an incoming request from a client.
            std::cout << "Executing the outer while loop." << std::endl;
            try {                                         // Try to process the request.
                while ( true ) {
                    std::string data;
                    int result = new_sock.recv(data);
                    if (result == -1) {
                        std::cout << "Server exception! " << std::endl;
                        break;
                    } else if (result == 0) {           // There wasn't anything in the socket.
                        break;                          // Get out of the receiving loop and try again to get something from
                    }                                   // the socket.
                    // A request has been received from a client.
                    string argument = "";               // The default argument doesn't exist until it has been found.
                    string command = "";                // The default command doesn't exist until it has been found.
                    // Find the location of the "-" that seperates the command from the argument (if it exists).
                    size_t seperatorLocation = data.find_first_of('-');// Find the location of the "-".
                    if (seperatorLocation == string::npos) {
                        command = data;
                    } else {
                        command = data.substr(0, seperatorLocation);
                        argument = data.substr(seperatorLocation + 1, data.size() - command.size() - 1);
                    }
                    string reply = "";                  // Contains the response to the command & its arguments.
                    if (command == "GetTemperature") {  // Is the caller asking for a current temperature?
                                                        // Yes. Get the temperature of the chnl with name argument.
                        float theTemp = theManager.getTemperature(argument);
                        if ( theTemp == -500.0) {       // Was the name valid?
                            reply = "Error! Invalid channel name!";// No. Create an error message.
                        } else {                        // The name was valid.
                            reply = to_string(theTemp); // Convert the temperature to a string to transmit.
                        }                               // The command was not to get a current temperature.
                    } else if (command == "RelayTest") {// Is the caller asking to test a relay?
                        int relayDex = (int)strtol(argument.c_str(), nullptr, 10);
                        if (relayDex < 0 || relayDex >= theManager.getNumberOfRelays()) {// Is the relay index valid?
                            reply = "Error! Invalid relay index!";// No. Create an error message.
                        } else {                        // The given relay index is valid.
                            theManager.relayTest((unsigned int)relayDex);// Start testing the relay.
                            reply = "Testing relay " + to_string(relayDex + 1) + ".";// Let the caller know testing is starting.
                        }                               // Relay testing request processed.
                    } else if (command == "RelayTestTerminate") {
                        theManager.relayTestTerminate();
                        reply = "Terminating relay testing.";
                    } else if (command == "GetTemperatureFromFile") {// Is the caller asking for a temperature from a file?
                        float theTemp = 500.0;          // Yes.
                        try {                           // Try to get the temperature using the given argument.
                            theTemp = theManager.getTTValue(argument);// The argument should contain the date, time & input index.
                        } catch(string error) {         // There was a problem.
                            reply = "Error! " + error;  // Return the problem to the caller.
                        } catch(...) {                  // There was an unrecognized problem.
                            reply = "Error! Uncaught error.";// Return the problem to the caller.
                        }                               // The temperature was obtained.
                        reply = to_string(theTemp);     // Convert the temperature to a string to transmit.
                    } else if (command == "GetTemperaturesFromFile") {// Is the caller asking for all temperatures
                        // from a file at a given time?
                        string values = "";             // Yes. Will contains all the temperatures.
                        try {                           // Try to get the temperatures using the given argument.
                            values = theManager.getTTValues(argument);// The argument should contain the date & time.
                        } catch(string error) {         // There was a problem.
                            values = "Error! " + error;  // Return the problem to the caller.
                        } catch(...) {                  // There was an unrecognized problem.
                            values = "Error! Uncaught error.";// Return the problem to the caller.
                        }                               // The temperatures were obtained.
                        reply = values;                 // Pass the temperatures to the reply string.
                    } else if (command == "GetChnlTvTValues") {
                        // Gets the temperatures for a given input index, for a given year, for a given month,
                        // for a given minute of the month, covering a given number of minutes.
                        // It returns the temperatures in the format of the sample interval(in minutes) followed by a comma.
                        // Then the actual start minute of the month(perhaps not the same as the desired start minute of the month)
                        // followed by a comma. Then each temperature up to or just past the desired span of minutes.
                        // If the desired time span is passed or there is no temperature data, the temperature is set to -501.
                        // -501 indicates the end of the temperatures. It should always be the last number in the return stream.
                        string values = "";             // Yes. Will contains all the temperatures.
                        try {                           // Try to get the temperatures using the given argument.
                            values = theManager.getChnlTvTValues(argument);// The argument the previously mentioned data.
                        } catch(string error) {         // There was a problem.
                            values = "Error! " + error; // Return the problem to alert the caller.
                        } catch(...) {                  // There was an unrecognized problem.
                            values = "Error! Uncaught error.";// Return the problem to the caller.
                        }                               // The temperatures were obtained.
                        reply = values;                 // Pass the temperatures to the reply string.
                    } else if (command == "Stop") {     // Someone is calling to stop the Plant Manager.
                        //theManager.isToContinueRunning = false;
                        cout << "Main received the stop command." << endl;
                        theManager.checkAlerts();
                        reply = "Stopping the Plant Manager.";
                    } else if (command == "ProgramPath") {// Is the caller asking for the PM's path?
                        reply = theManager.getFilePath();// Yes. Get the path.
                    } else if (command == "GetProgram") {// Is the caller asking for a chnl's program?
                        SCChnl* scChnl = theManager.getSCChnl(argument);// Yes. Get the SCChnl with this name.
                        if (scChnl == nullptr) {        // Was the given name associated with a chnl?
                            reply = "Error! Invalid name!";  // No. Return the problem to the caller.
                        } else {                        // The name is valid.
                            if (scChnl->program.getControlType() == SCProgram::TYPE_NONE) {
                                reply = "None";
                            }
                        }
                    } else if (command == "GetSetpoint") {// Does a user want a channel's current setpoint?
                        SCChnl* scChnl = theManager.getSCChnl(argument);// Yes. Get the SCChnl with this name.
                        if (scChnl == nullptr) {        // Was the given name associated with a chnl?
                            reply = "Error! Invalid name!";  // No. Return the problem to the caller.
                        } else {                        // The name is valid.
                            reply = to_string(scChnl->program.presentSetPoint());
                        }
                    } else if (command == "GetBeds") { // Does the caller wants the SCBeds?
                        // Yes. Send an XML version of the SCBeds to the caller.
                        reply = theManager.theBeds_.createXMLVersion();
                    }
                    reply += '\r';                      // Append carraige return.
                    reply += '\n';                      // Append line feed.
                    cout << "Reply is : " << reply << endl;
                    //new_sock << reply;                  // Send the data back to the client.
                    count += 1;                         // Increment the number of received messages.
                }                                       // End of the receive loop.
            }
            catch ( SocketException& e) {
                std::cout << "Server exception: " << e.description() << std::endl;
            }
        }
    }
    catch ( SocketException& e )
    {
        std::cout << "Exception was caught:" << e.description() << "\nExiting.\n";
    }
    
    std::cout << "exiting getSCCommandThread." << std::endl;
    return;
}


string serialCommand = "";
bool isSerialCommandResponseExpected = true;            // By default a command sent to a serial port should receive a response.
bool isSerialServerConnected = false;                   // The serial server is not connected until the code to do so runs.

// As of 4/23/2020 this thread is not used.
/*
void serialServerThread() {
    std::cout << "The serial server thread is starting." << std::endl;
    try {                                       // Try to connect to the NetCOM device acting as a client to the mini devices.
        ServerSocket server ( 65001 );          // Create a server socket to receive a serial client's request for connection.
        std::cout << "Serial Server Socket created." << std::endl;
        ServerSocket new_sock;                  // Create a socket for the serial client.
        int result = 0;
        while (theManager.isToContinueRunning == true) {
            result = server.poll(50);           // Wait up to 50 milliseconds for the NetCOM serial client to connect.
            cout << "The poll returned: " << to_string(result) << endl;
            if (result > 0) {                   // Are there any connections?
                break;                          // Yes. There is a connection. Done trying to connect.
            }                                   // There isn't a connection yet.
            this_thread::sleep_for(chrono::milliseconds(5000));// Wait for five seconds to check for a connection again.
            // The reason for waiting for five seconds is so that the network will not be flooded with polling.
        }                                       // Either a connection was made or the Plant Manager is being shut down.
        if (theManager.isToContinueRunning == false) {
            std::cout << "Exiting  the serialServerThread before a connection was made." << std::endl;
            return;
        }
        server.accept ( new_sock );                 // Accept an incoming request from a serial client.
        isSerialServerConnected = true;             // The serial server is connected.
        std::cout << "Executing the serial server outer while loop." << std::endl;
        try {                                         // Try to process the request.
            while ( theManager.isToContinueRunning == true ) {
                if (serialCommand != "") {
                    new_sock << serialCommand;
                    std::cout << "Issuing command: " << serialCommand;
                    this_thread::sleep_for(chrono::milliseconds(10));
                    std::string data;
                    int result = new_sock.receive(data);
                    if (result == -1) {
                        std::cout << "Serial Server exception! " << std::endl;
                    } else if (result != 0) {           // There was something in the serial socket.
                        // Data has been received from a serial client.
                        std::cout << "Recieved from serial port: " << data << std::endl;
                        serialCommand = "";             // Clear the command since a response was received.
                    }
                }
                this_thread::sleep_for(chrono::milliseconds(500));// Wait half a second to see if there is a command to send.
            }
        }
        catch ( SocketException& e) {
            std::cout << "Serial Server exception: " << e.description() << std::endl;
        }
    }
    catch ( SocketException& e )
    {
        std::cout << "Serial exception was caught:" << e.description() << "\nExiting.\n";
    }
    
    std::cout << "exiting serialServerThread." << std::endl;
    return;
}
*/

void osShutDownHandler(int signum)
{
    theManager.showRelaysOff();         // Make sure the browser will show all relays as being off.
    theManager.dbUpdateReadings();      // Update the reading before the Plant Manager shuts down.
    theManager.shutDown();              // The sub-system threads and allocated memory must be
                                        // shut down and deallocated before closing the Plant Manager.
                                        // This doesn't happen correctly unless these items are shut
                                        // down and deallocated before theManager is closed. The
                                        // sub-system threads require some time to close.
    this_thread::sleep_for(chrono::seconds(10)); // Wait for all threads to stop running.
    
    std::cout << "The Plant Manager is being shut down by the operating system.\n";
    exit(signum);                       // Exit the Plant Manager.
}

int main(int argc, const char * argv[])
{
    
//    SCSecurity key;
//    key.plantManagerPath = theManager.getFilePath();
//    string dir = theManager.getFilePath();
//    dir += theManager.getTemperatureDataPath();
//    key.setNumOfTemperatureInputs(1);
    
//    bool result = key.directoryExists(dir);
//    if(result == true){
//        cout << "The directory exists." << endl;
//        ifstream ttFile(dir + "201512.tt");
//        if (ttFile) {
//            cout << "The file exists." << endl;
//            ttFile.close();
//        } else {
//            cout << "The file doesn't exist." << endl;
//            if(key.ttFileCreate(2015, 12) == false) {
//            cout << "Failed to create 201512.tt" << endl;
//            } else {
//                cout << "Created 201512.tt" << endl;
//            }
//        }
//    } else {
//        cout << "There is no such place." << endl;
//        if(key.ttFileCreate(2015, 12) == false) {
//            cout << "Failed to create " << theManager.getTemperatureDataPath() << endl;
//        } else {
//            cout << dir << " was created!" << endl;
//        }
//    }
/*
    if(key.readTTHeader(2015, 12) == true) {
        cout << "The record size is " << to_string(key.getRecordSize()) << endl;
        cout << "The month number is " << to_string(key.getTTMonth()) << endl;
        cout << "The year number is " << to_string(key.getTTYear()) << endl;
        cout << "The seconds operated are " << to_string(key.getTTSecondsOperated()) << endl;
        cout << "The Plant Bitmap Key is " << to_string(key.getPlantBMKey()) << endl;
    } else {
        cout << "Failed to read the header correctly." << endl;
    }
*/

    
    
/*
    thread scSerialThread(serialServerThread);
    scSerialThread.detach();
    int count = 0;
    while (isSerialServerConnected == false) {
        this_thread::sleep_for(chrono::seconds(1));
        count++;
        cout << "Waiting to connect to the Mini server: " << to_string(count) << endl;
    }
    serialCommand = "$03M\r\n";
    this_thread::sleep_for(chrono::seconds(2));
    continueRunning = false;
    this_thread::sleep_for(chrono::seconds(6));
    
    return 0;
*/
    
//    thread scCommThread(getSCCommandThread);                    // Create the thread for taking requests for data.
    
//    scCommThread.detach();                                      // Start running the thread for proccessing requests.

    // Check to be sure the PlantManager is not already running.
    FILE *fp;                               // A file pointer used to open a pipe stream to get process id of the PlantManager.
    // The PlantManager will only have a process id if it is already running.
    int status;                             // A flag to determine if the pipe stream closed correctly.
    char buff[512];                         // Will contain the process id of the PlantManager or 0 if the PM is not running.
    string command = "pgrep PlantManager";  // The unix system command to get the  process id of PlantManager.
    fp = popen(command.c_str(), "r");       // Open the pipe stream and issue the command to get the process id.
    if (fp == NULL) {                       // Did the pipe stream fail to open?
        cout << "Error opening the pipe to get the Plant Manager's process id!" << endl;// Failure. Create an error.
        return 0;                           // Return 0 to shut down.
    } // End of condition testing for an open pipe.
    if(fgets(buff, 512, fp) != NULL) {      // Was the process id obtained from the pipe stream?
        cout << "The Plant Manager is already running." << endl;// Yes. This means a copy of the Plant Manager is already running.
        cout << "Only one copy of the Plant Manager is allowed to run at one time." << endl;
        cout << "This copy of the Plant Manager is being closed." << endl;
        pclose(fp);                         // Close the pipe stream.
        return 0;                           // Return 0 to shut down.
    } // End of condition: Failed to get the Plant Manager's process id.
    status = pclose(fp);                    // Close the pipe stream.
    if (status == -1) {                     // Did the pipe stream close successfully?
        cout << "Error closing the pipe to get Plant Manager's process id" << endl;// No. Create an error.
        return 0;                           // Return 0 to shut down.
    } // End of condition: Pipe failed to close correctly.
    // A
    
    // The Plant Manager wasn't already running.
    signal(SIGTERM, osShutDownHandler);// Create a function to handle an OS request to shut down.
    
    time_t tTime = time(0);                             // Contains the current time.
    string ts = ctime(&tTime);
    ts.resize(ts.size() - 1);
    cout << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "Starting the Plant Manager on " << ts << "." << endl;             // Display a positive message.

    if(theManager.initialize() == false) {                      // Did the Manager initialiaze?
        cout << "The Manager could not be initialized." << endl;// No. Display an error.
        theManager.shutDown();              // The sub-system threads and allocated memory must be
        return 1;                                               // Return an error code.
    }                                                           // The Manager is running.
    this_thread::sleep_for(chrono::seconds(1));
    
// The following two line used to open the thread that communicated with the HTML user applications.
// That thread created a socket connection to the HTML user applications and talked to them using
// that socket. However, now the HTML user applications write to the pm_alerts table in the database.
// The Plant Manager periodically checks the pm_alerts table in the the database to see if there are
// any alerts from the HTML user applications and responds.
//    thread scCommThread(getSCCommandThread);                    // Create the thread for taking requests for data.
//    scCommThread.detach();                                      // Start running the thread for proccessing requests.
    int w = 0;
    this_thread::sleep_for(chrono::seconds(1));
    while (theManager.isToContinueRunning == true) {
        theManager.control();
        this_thread::sleep_for(chrono::seconds(2));             // Wait for two seconds
        w += 2;
        if(theManager.miniSystemRequestingShutDown() == true) {
            break;
        }
    }
    theManager.showRelaysOff();         // Make sure the browser will show all relays as being off.
    theManager.dbUpdateReadings();      // Update the reading before the Plant Manager shuts down.
    theManager.shutDown();              // The sub-system threads and allocated memory must be
                                        // shut down and deallocated before closing the Plant Manager.
                                        // This doesn't happen correctly unless these items are shut
                                        // down and deallocated before theManager is closed. The
                                        // sub-system threads require some time to close.
    this_thread::sleep_for(chrono::seconds(1)); // Wait for all threads to stop running.
    
    std::cout << "This is the Plant Manager saying goodbye.\n";
    return 0;
/*
    SCXML scErrorData;

    scErrorData.setFilePath(workingDirectory);
    scErrorData.fileName = "SCErrors.xml";
    if(scErrorData.readXMLFile() == false) {
        scErrorData.root.name = "SCErrors";
    }
    scErrorData.nSubNodesAllowed = 20;
 */
/*
    Relay* aRelay = theSystems->relays->at(0);

    aRelay->display();

    aRelay->isOn = true;
    aRelay->incrementTimeOn();

    this_thread::sleep_for (chrono::seconds(2));
    aRelay->incrementTimeOn();
    aRelay->display();
*/
    
//    delete scErrorData;

    return 0;
}

