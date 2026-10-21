//
//  Manager.cpp
//  PlantManager
//
//  Created by Mark Davis on 4/17/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#include "Manager.h"

static int callback(void *data, int argc, char **argv, char **azColName){
/*
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
*/
    return 0;
}


Manager::Manager() {
    path_ = "/Users/Shared/Sites/bdat/";// The default path to all data/config files.
    version_ = "1.6";               // 10/07/20 (4/18/2015) is the date this version was created.
    isArtificialMode = false;       // The manager is not in aritificial mode unless it is set.
    isShuttingDown = false;         // The Plant Manafger is not shutting down.
    isReadingsTableConfigured = false;// The readings table has not been examined yet.
    controlCycle_ = 0;              // The control loop begins with the first control cycle.
    versionMaturity_ = 1;           // The default maturity file version is one.
    time_t nowTime = time(0);       // Contains the current time.
    struct tm* tmNow;               // Will contain a more usable form of the current time.
    tmNow = localtime(&nowTime);    // Convert the current time into a more usable format.
    lastStorageMinute_ = tmNow->tm_hour * 60 + tmNow->tm_min;// The minute of the day determines when data is stored.
    storageInterval_ = 6;           // The default number of minutes between storing samples is 6.
    temperatureDataPath_ = "Temperatures/";// The default sub-folder used to store temperature data.
    isToContinueRunning = true;     // Initially the Manager is supposed to run.
    socketPort_ = 65001;            // By default, the network port sub-systems will try to connect over is 65001.
}

Manager::~Manager() {
}

void Manager::shutDown() {
    isShuttingDown = true;                      // The Plant Manager is shutting down.
    if (oneWireSystems_.size() != 0) {          // Are there any one wire systems?
        this_thread::sleep_for(chrono::seconds(1));// Wait for the threads to stop.
        for (std::vector<OneWireEquip*>::iterator it = oneWireSystems_.begin(); it != oneWireSystems_.end(); ++it) {// Yes.
            (*it)->isOperationAllowed = false;  // Stop the thread running this one wire equipment.
            delete *it;                         // Free the memory allocated to it.
        }                                       // All one wire memory has been freed.
        oneWireSystems_.clear();                // Clear all the array of one wire systems.
    }                                           // One wire systems(if they exist) have had their memory freed & their list cleared.
    if (miniSystems_.size() != 0) {             // Are there any mini sub-systems?
        for_each(miniSystems_.begin(), miniSystems_.end(), [&] (MiniEquip* miniSys) {// Yes. Go through each one.
            miniSys->isOperationAllowed = false;// Stop the thread running this mini sub-system.
            while (miniSys->isOperating == true) {// Wait for the thread to stop.
                this_thread::sleep_for(chrono::milliseconds(250));// Wait .25 seconds
            }                                   // The thread running this mini sub-system quit.
            if (miniSys == nullptr) {
                toLog("This mini system was deleted before it was supposed to be.");
            } else {
                delete miniSys;         // Free the memory allocated to this sub-system.
                miniSys = nullptr;      // Prevent the memory from accidentally being used.
            } // End of the condition to free memory allocated to mini sub-systems.
        }); // End of the loop to shut down all the mini sub-systems.
        miniSystems_.clear();           // Total clear the array of mini sub-systems.
        toLog("Mini Systems freed.");
    }                                   // Mini systems(if they exist) have had their memory freed & their list cleared.
    if (lawsonSystems_.size() != 0) {   // Are there any lawson sub-systems?
        for_each(lawsonSystems_.begin(), lawsonSystems_.end(), [&] (LawsonEquip* lawSys) {// Yes. Go through each one.
            lawSys->isOperationAllowed = false;// Stop the thread running this mini sub-system.
            while (lawSys->isOperatingEquipment == true) {// Wait for the thread to stop.
                this_thread::sleep_for(chrono::milliseconds(250));// Wait .25 seconds
            }                           // The thread running this lawson sub-system quit.
            if (lawSys == nullptr) {    // Does the lawsys exist?
                toLog("This lawson system was deleted before it was supposed to be.");
            } else {                    // A pointer exists to this lawson sub-system.
                delete lawSys;          // Free the memory allocated to this sub-system.
                lawSys = nullptr;       // Prevent the memory from accidentally being used.
            } // End of the condition to free memory allocated to lawson sub-systems.
        });                             // All the mini sub-systems have been shut down.
        lawsonSystems_.clear();         // Total clear the array of mini sub-systems.
        toLog("Lawson Systems freed.");
    }                                   // Lawson systems(if they exist) have had their memory freed & their list cleared.
    for(int iDex = 0; iDex < theInputs_.size(); iDex++) {// Go through each input.
        // Is the input measuring temperature?
        if(theInputs_.at(iDex)->measuredUnits == SCInput::UNITS_FAHRENHEIT ||
           theInputs_.at(iDex)->measuredUnits == SCInput::UNITS_CELSIUS) {
            theInputs_.at(iDex)->setMeasuredValue(501.0f);// Yes. Set its value high to indicate open for closing the PM.
        } // End of condition to check for inputs used to measure temperature.
    } // End of loop through all the inputs.
    dbUpdateReadings();                 // Update the readings table so the Plant Manager shows as shut down.
    for_each(theChnls_.begin(), theChnls_.end(), [&] (SCChnl* aChnl) {// Go through each SCChnl.
        delete aChnl;                   // Delete the resources allocated to this SCChnl.
    }); // End of the loop to deallocate resouces used by each SCChnl.
    theChnls_.clear();                  // Clear the array of SCChnls.
    for_each(theRelays_.begin(), theRelays_.end(), [&] (Relay* aRelay) {// Go through all the relays.
        delete aRelay;                  // Free the memory they use.
    }); // End of the loop to deallocate resouces used by each Relays.
    theRelays_.clear();                 // Clear the relay list.
    for_each(theInputs_.begin(), theInputs_.end(), [&] (SCInput* anInput) {// Go through all the inputs.
        delete anInput;                 // Delete the resources allocated to this input.
    }); // End of the loop to deallocate resouces used by each input.
    theInputs_.clear();                 // Clear the input list.
    for_each(theBeds_.begin(), theBeds_.end(), [&] (SCBed* aBed) {// Go through all the beds.
        delete aBed;                    // Delete the resources allocated to this input.
    }); // End of the loop to deallocate resouces used by each bed.
    theBeds_.clear();                   // Clear the bed list.
    for_each(theMeters_.begin(), theMeters_.end(), [&] (SCPulseMeter* aMeter) {// Go through all the meters.
        delete aMeter;                  // Delete the resources allocated to this bed.
    }); // End of the loop to deallocate resouces used by each meter.
    theMeters_.clear();                 // Clear the meter list.
    for_each(theSystems_.begin(), theSystems_.end(), [&] (SCSystem* aSystem) {// Go through all the sub-system.
        delete aSystem;                 // Delete the resources allocated to this sub-system.
    }); // End of the loop to deallocate resouces used by each sub-system.
    theSystems_.clear();                // Clear the sub-system list.
    dbUpdateRunTime();                  // Let any user know the PM is not running.
} // End of function shutDown.

bool Manager::initialize() {
    // Load the email settings. Set the next legal time to send an email.
    // Do this later.
    
    if(loadFromDB() == false) {     // Were the settings for the PlantManager loaded?
        return false;               // No. There are problems with the database if these didn't load. Errors are stored by laodFromDB().
    }                               // The PM settings loaded.
    // Do this later. In the past, display units were part of the PM config, this is probably not neccessary in the future.

    theInputs_.setFilePath(path_);              // Set the location of the database for the inputs.
    if (theInputs_.loadFromDB() == false) {     // Did the inputs load?
        return false;                           // No. The PlantManger cannot operate without inputs.
    }                                           // The inputs have been loaded.
    theRelays_.setFilePath(path_);              // Set the location of the database for the relays.
    if (theRelays_.loadFromDB() == false) {     // Did the relays load?
        return false;                           // No. The PlantManger cannot operate without relays.
    }                                           // The relays have been loaded.
    unsigned int nMeters = 0;                   // Start with no pulse meters.
    for (std::vector<Relay*>::iterator it = theRelays_.begin(); it != theRelays_.end(); ++it) {
        // Go through each relay and if it has a pulse meter, adjust the number of pulse meters.
        Relay* aRelay = *it;                    // Get access to the relay.
        if (aRelay->getPulseMeter() > nMeters) {// Is this relay's meter number greater than the highest meter number found so far?
            nMeters = aRelay->getPulseMeter();  // Yes. Adjust the number of pulse meters.
        }                                       // The number of pulse meters has been updated.
    }                                           // The number of pulse meters has been determined.
    for (int nDex = 0; nDex < nMeters; nDex++) {// Go through each pulse meter.
        // Create an activator for each pulse meter.
        // Do this later.
    }                                           // Activators have been created for each pulse meter.
    theChnls_.setFilePath(path_);               // Set the location of the database for the SCChnls.
    theChnls_.setMaximumInputIndex((int)theInputs_.size());// Set the largest permissible input index.
    theChnls_.setMaximumRelayIndex((int)theRelays_.size());// Set the largest permissible relay index.
    theChnls_.setLowerTemperatureLimit(lowerTemperatureLimit);// Set the lowest temperature to consider valid.
    theChnls_.setUpperTemperatureLimit(upperTemperatureLimit);// Set the highest temperature to consider valid.
    if (theChnls_.loadFromDB() == false) {      // Did the SCChnls load?
        return false;                           // No. The PlantManager cannot run without the SCChnls.
    }                                           // The SCChnls have been loaded.

    SCProgram::setFilePath(getFilePath());      // Set the path to where the programs are stored.
    for (std::vector<SCChnl*>::iterator it = theChnls_.begin(); it != theChnls_.end(); ++it) {
        // Go through each SCChnl and try to load its program.
        SCChnl* aChnl = *it;                    // Get access to the SCChnl.
        aChnl->program.loadFromDB(aChnl->getIndex());// Load the SCChnl's program. This is non-critical, so errors are ignored.
        // Do this later. Decide if we still want to set the program's owner.
    }                                           // Each SCChnl's program has been loaded.
    loadMaturityFromDB();                       // Load the maturities for each SCChnl. Non-critical, no error checking.
    theSystems_.setFilePath(path_);             // Set the location of the database for the sub-systems.
    if (theSystems_.loadFromDB() == false) {    // Did the system configuration load?
        return false;                           // No. The PlantManager cannot run without the system configuration.
    }                                           // The system/sub-systems configuration has been loaded.
    // Check the configuration files for potential consistency errors.
    for (std::vector<SCChnl*>::iterator it = theChnls_.begin(); it != theChnls_.end(); ++it) {
        // Go through each SCChnl and test inputs and relays against total values in theInputs and theRelays.
        SCChnl* aChnl = *it;                    // Get access to the SCChnl.
        if (aChnl->getInputIndex() >= (int)theInputs_.size()) {// Does this SCChl's input exceed the total number of inputs?
            errorMessage_ = "Channel " + aChnl->getName() + " contains input ";
            errorMessage_ += to_string(aChnl->getInputIndex() + 1) + " which exceeds the total number of inputs, ";
            errorMessage_ += to_string(theInputs_.size()) + ".";
            toLog(errorMessage_);               // Add the error to the console.
            return false;                       // This is a fatal error.
        }                                       // The SCChnl's input index is consistent.
        if (aChnl->getRelayIndex() >= (int)theRelays_.size()) {// Does this SCChl's relay exceed the total number of relays?
            errorMessage_ = "Channel " + aChnl->getName() + " contains relay ";
            errorMessage_ += to_string(aChnl->getRelayIndex() + 1) + " which exceeds the total number of relays, ";
            errorMessage_ += to_string(theRelays_.size()) + ".";
            toLog(errorMessage_);               // Add the error to the console.
            return false;                       // This is a fatal error.
        }                                       // The SCChnl's relay index is consistent.
    }                                           // The SCChnl configuration is consistent with the input & relay configurations.
    int nTotalInputs = 0;                       // Keep track of the total number of inputs.
    int nTotalRelays = 0;                       // Keep track of the total number of relays.
    for (std::vector<SCSystem*>::iterator it = theSystems_.begin(); it != theSystems_.end(); ++it) {
        // Go through each sub-system and get the total number of inputs and relays for all sub-systems.
        SCSystem* aSys = *it;                   // Get access to the sub-system.
        nTotalInputs += aSys->getNumberOfInputs();// Increment the total number of inputs.
        nTotalRelays += aSys->getNumberOfRelays();// Increment the total number of relays.
    }                                           // The total number of inputs & relays have been calculated from the systems table.
    if (nTotalInputs != theInputs_.size()) {    // Does the system's number of inputs exceed the inputs from inputs table?
        errorMessage_ = "The systems table contains a different number of inputs than inputs table.\n";// Yes.
        toLog(errorMessage_);                   // Add the error to the console.
        return false;                           // This is a fatal error.
    }                                           // The # of system inputs doesn't exceed the # in the input configuration.
    if (nTotalRelays != theRelays_.size()) {    // Does the system's number of relays exceed the relays from relays table?
        errorMessage_ = "The systems table contains a different number of relays then relays table.\n";// Yes.
        toLog(errorMessage_);                   // Add the error to the console.
        return false;                           // This is a fatal error.
    }                                           // The # of system relays doesn't exceed the # in the relay configuration.
    if (theRelays_.electricRelays.size() > 0) { // Are there any relays with electric loads?
        theMeters_.setFilePath(path_);          // Set the location of the database for the pulse meters.
        if (theMeters_.loadFromDB() == false) { // Did the pulse meter configurations load successfully?
            return false;                       // No. This is a fatal error.
        }                                       // The pulse meter configurations loaded.
        bool isBad = false;                     // A flag to indicate if there is a bad configuration.
        for_each(theMeters_.begin(), theMeters_.end(), [&] (SCPulseMeter* meter) {// Check for bad sub-system indexes.
            int ssDex = meter->getSubSystemIndex();// Get the meter's sub-system index.
            if (ssDex >= theSystems_.size()) {  // Is the index valid?
                errorMessage_ = "Initialization halted. The pulse meter configuration contains a sub-system index of ";// No.
                errorMessage_ += to_string(ssDex) + " when there are only " + to_string(theSystems_.size()) + " sub-systems!";// Create error.
                toLog(errorMessage_);           // Add the error to the console.
                isBad = true;                   // There is an error with the pulse meter configurations.
            }                                   //
        });                                     // All the meters have been checked for a bad sub-system index.
        if (isBad == true) {                    // Was there a meter with a bad sub-system index?
            return false;                       // Yes. Return failure.
        }                                       // The pulse meter configurations were OK.
    }                                           // If electric loads, the pulse meter configuration has been loaded.
    dbUpdateRunTime();                          // Let any user know the PM is not running.
/*
    theKey_.plantManagerPath = getFilePath();   // Set the path to the security.
    string dir = getFilePath();                 // Start setting the path to the time-temperature files.
    dir += getTemperatureDataPath();            // Add the temperature folder to the Plant Manager's path.
    bool result = theKey_.directoryExists(dir); // Test to see if the path to the time temperature data exists.
    theKey_.setNumOfTemperatureInputs(theInputs_.getNumberOfInputs());// Set the number of inputs.
    time_t nowTime = time(0);                   // Contains the current time.
    struct tm* tmNow;                           // Will contain a more usable form of the current time.
    tmNow = localtime(&nowTime);                // Convert the current time into a more usable format.
    int thisYear = tmNow->tm_year + 1900;       // The year is in years since 1900. Thus, we must add 1900 to get the real year.
    int thisMonth = tmNow->tm_mon + 1;          // The month is zero based. Thus, we must add one to get the month number.
    string ttFileName = path_;                  // The path to the temperature data files begins with the path to the Plant Manger.
    ttFileName += temperatureDataPath_;         // The path then contains the sub-folder for temperature data files.
    ttFileName += to_string(thisYear) + to_string(thisMonth) + ".tt";// Append the .tt temperature file name to the path.
    if(result == true){                         // Does the path to the time temperature data exist?
        if(ifstream(ttFileName)){               // Yes. Does this months .tt file exist?
            toLog(to_string(thisYear) + to_string(thisMonth) + ".tt exists.");// Yes. It doesn't need to be created.
        } else {                            // The .tt file for this year and month does not exist. It needs to be created.
            if(theKey_.ttFileCreate(thisYear, thisMonth) == false) {// Was the .tt file created?
                errorMessage_ = "Failed to create " + ttFileName + "!";// No.
                toLog(errorMessage_);           // Add the error to the console.
                return false;                   // This is a fatal error.
            }                                   // The .tt file was created.
        }                                       // This month's .tt file exists.
    } else {                                    // The path for the .tt files does not exist. Try to create it.
        if(theKey_.ttFileCreate(thisYear, thisMonth) == false) {// Was the .tt file created?
            errorMessage_ = "Failed to create " + ttFileName + "!";// No.
            toLog(errorMessage_);               // Add the error to the console.
            return false;                       // This is a fatal error.
        }                                       // The .tt file was created.
    }                                           // This month's .tt file exists.
 */
    // Determine which sub-system each input is a part of and make its sub-system index as a part of the input.
    // This is done now so that in other parts of the application it can easily be used.
    for_each(theInputs_.begin(), theInputs_.end(), [&] (SCInput* scInput) {// Go through each input.
        for (int sysIndex = 0; sysIndex < theSystems_.size(); sysIndex++) {// Go through each sub-system.
            SCSystem* scSys = theSystems_.at(sysIndex);// Get access to the sub-system.
            int inputNo = scInput->indexNo;// This variable is created to make the following conditional more readable.
            int firstInput = scSys->getFirstInput();// This variable is also for readability.
            int lastInput = firstInput + scSys->getNumberOfInputs();// This variable is for readability.
            if (inputNo >= firstInput && inputNo < lastInput) {// Is this input a part of this sub-system?
                // Yes, this is the sub-system.
                scInput->setSystemIndex(scSys->getSubSystemNo());// Remember this input's sub-system.
                break;                           // No need to check additional mini sub-systems.
            } // Done testing if this input is a part of this sub-system.
        } // Done checking sub-systems.
    });// Done going through each input.
    // Make a list of the chnls with inputs.
    for_each(theChnls_.begin(), theChnls_.end(), [&] (SCChnl* aChnl) {// Go through each channel.
        int inputIndex = aChnl->getInputIndex();// Remember this SCChnl's input index.
        if (inputIndex >= 0) {                  // Does this channel have a valid input index?
            chnlsWithInputs_.push_back(aChnl);  // Yes. Add this channel to the list of channels with inputs.
        }                                       // Check the next channel to see if it has an input.
    });                                         // The vetor is now populated with all the channels which have inputs.
    // Make a list of the chnls with relays.
    for_each(theChnls_.begin(), theChnls_.end(), [&] (SCChnl* aChnl) {// Go through each channel.
        int relayIndex = aChnl->getRelayIndex();// Remember this SCChnl's relay index.
        if (relayIndex >= 0) {                  // Does this channel have a valid relay index?
            chnlsWithRelays_.push_back(aChnl);  // Yes. Add this channel to the list of channels with relays.
        }                                       // Check the next channel to see if it has an relay.
    });                                         // The vetor is now populated with all the channels which have relays.
    return initiateIOSystems();                 // Start the IO systems. If none are initialized, shut down the PM.
}

bool Manager::initiateIOSystems() {
    bool isASubSystemInitiated = false;         // If no sub-systems are initiated, the Plant Manager will close.
    SCEquipment::plantInputs = &theInputs_;     // Provide the main control loop access to the inputs.
    SCEquipment::plantRelays = &theRelays_;     // Provide the main control loop access to the relays.
    int noMiniSystems = 0;                      // Set the number of mini sub-systems to zero.
    for (int sysDex = 0; sysDex < theSystems_.size(); sysDex++) {
        checkAlerts();                          // Check for alerts from the user.
        switch (theSystems_.at(sysDex)->getType()) {// Initiate each sub-system based on its type.
            case SCSystem::SYSTYPE_ONEWIRE : {  // ONE_WIRE is used to measure temperatures.
                OneWireEquip* owSys = new OneWireEquip();// Create new one wire equipment.
                owSys->setSubSystem(*theSystems_.at(sysDex));// Set the sub-system associated with this equipment.
                owSys->setFilePath(path_);      // Set the location of the one wire configuration.
                if(owSys->loadConfig() == false) {// Load equipment configuration & in the process create all arduinos & devices.
                    errorMessage_ = "Since the OneWire configuration could not be loaded, this sub-system will not be started.";
                    toLog(errorMessage_);       // Add the error to the console.
                    delete owSys;               // Failed, delete the equipment.
                    break;                      // Failed to load.
                }                               // The equipment's configuration was loaded.
                oneWireSystems_.push_back(owSys);// Add this one wire equipment to the list of one wire system's.
                owSys->initialize();            // Initialize this one wire system.
                std::thread(OneWireEquip::operateEquipment, owSys).detach();// Start the one-wire sub-system operating.
                isASubSystemInitiated = true;   // This sub-system has been initialized.
                break;                          // Done starting the one-wire sub-system.
            }
            case SCSystem::SYSTYPE_MINI : {     // Initialize a sub-system consisting of Adlink or ADAM equipment.
                // This is tricky. The DB gives us the configuration of each mini sub-system. However, when we go to
                // connect, we don't know which sub-system we are going to connect with. Each VSCom/NetCom device sends
                // a request to connect. The one sending the request may not match this mini sub-system.
                // Therefore, the mini will only be configured here and its operation thread started. The operation
                // thread will then cause the mini to sign-on. The mini's operation thread will only connect to a
                // device whose server name matches the one in this mini's configuration.
                noMiniSystems++;                // Increment the number of mini sub-systems on-line.
                MiniEquip* miniSys = new MiniEquip();// Create new mini equipment.
                miniSys->setFilePath(path_);    // Set the location of the systems configuration file.
// set to false in constructor                miniSys->isInitialized = false; // This mini sub-system has not been initialized yet.
                miniSys->setSubSystem(*theSystems_.at(sysDex));// Set the sub-system associated with this equipment.
                if(miniSys->loadFromDB() == false) {// Load equipment configuration & in the process create all mini modules.
                    // This line is reached if this mini could not load its configuration from the DB.
                    errorMessage_ = "The mini configuration could not be loaded for sub-system ";
                    errorMessage_ += to_string(miniSys->getSubSystem().getSubSystemNo()) + ".";
                    toLog(errorMessage_);       // Add the error to the console.
                    break;                      // This mini cannot be operated without its configuration.
                } else {                        // The mini sub-system loaded.
                    miniSys->initializeRelays(&theRelays_);// Assign the relays to this mini sub-system's relay modules.
                } // End of the condition to check if the mini sub-system configuration was loaded.
                if (theSystems_.at(sysDex)->isOnLine == false) {// Is this mini supposed to be on-line?
                    errorMessage_ = "Mini sub-system ";// No. Create
                    errorMessage_ += to_string(sysDex);// a
                    errorMessage_ += " is off line and will not be initialized!";// message.
                    toLog(errorMessage_);       // Add the error to the console.
                    miniSys->isRequestedToSignOn = false;// This mini is off line. This mini should not be connected.
                    miniSystems_.push_back(miniSys);// Add this mini equipment to the list of mini system's.
                    // It is necessary to add this system to the list of lawson sub-systems even though it is off-line.
                    // Since this sub-system is now on the list, the control() function will set any inputs up scale.
                    // This may prevent over heating.
                } else {                        // This mini is on-line. It needs to be initialized.
                    // Set the time the mini sub-system was initialized to 2 minutes ago so that it attempts to sign-on right away.
                    chrono::time_point<chrono::system_clock> iniTime = chrono::system_clock::now() - chrono::minutes(2);
                    miniSys->setTimeInitializedTo(iniTime);// Set the initialization time.
                    if (SCEquipment::isInArtificialMode == true) {// Is the plant manager in artificial mode?
                        miniSys->isRequestedToSignOn = false;// The PM is in artificial mode. This mini should not be connected.
                    } else {                    // The PM is in real mode.
                        miniSys->isRequestedToSignOn = true;// Set the request to sign-on.
                    } // End of condition: Is the PM in artificial mode?
                    miniSystems_.push_back(miniSys);// Add this mini equipment to the list of mini system's.
                    std::thread(MiniEquip::operateEquipment, miniSys, &theRelays_).detach();// Start the thread for this mini sub-system.
                    isASubSystemInitiated = true;// If this line is executed, the mini is initiated and its thread is running.
                                                // This does not mean that the mini is actually working. It could be trying to
                                                // sign-on.
                } // End of condition: This mini sub-system is on-line.
                break;                          // Done starting this mini sub-system.
            } // End of the case of initializing a mini sub-system.
            case SCSystem::SYSTYPE_MICRO:
            case SCSystem::SYSTYPE_STANDARD : { // Initialize a sub-system consisting of lawson equipment.
                LawsonEquip* lawSys = new LawsonEquip();// Create new lawson equipment.
                lawSys->setFilePath(path_);     // Set hte location of the lawson configuration files.
                lawSys->setSubSystem(*theSystems_.at(sysDex));// Set the sub-system associated with this equipment.
                if (theSystems_.at(sysDex)->isOnLine == false) {// Is this lawson off-line?
                    errorMessage_ = "Lawson sub-system ";// Yes. Create
                    errorMessage_ += to_string(sysDex);// a
                    errorMessage_ += " is off line and will not be initialized!";// message.
                    toLog(errorMessage_);       // Add the error to the console.
                    lawsonSystems_.push_back(lawSys);// Add this lawson equipment to the list of lawson system's.
                    // It is necessary to add this system to the list of lawson sub-systems even though it is off-line.
                    // Since this sub-system is now on the list, the control() function will set any inputs up scale.
                    // This may prevent over heating.
                } else {                        // This lawson sub-system is on-line.
                    if(lawSys->lawIniChange == false) {
                        lawSys->initialize();       // Try to initialize this sub-system.
                        if(lawSys->isInitialized == false) {// Was this lawson sub-system initialized?
                            errorMessage_ = "Lawson sub-system ";// No. Create an
                            errorMessage_ += to_string(sysDex);// an error
                            errorMessage_ += " failed to initialize during \"initiateIOSystems\"!";// message.
                            toLog(errorMessage_);   // Add the error to the console.
                            lawSys->isRequestedToSignOn = true;// This sub-system still needs to connect.
                        } else {                    // The equipment was initialized.
                            isASubSystemInitiated = true;// This sub-system was initialized.
                        } // End of condition: Setting the lawson sub-system up as the result of initialazation.
                        lawsonSystems_.push_back(lawSys);// Add this lawson equipment to the list of lawson system's.
                        std::thread(LawsonEquip::operateEquipment, lawSys).detach();// Start the thread to operate this lawson sub-system.
                        this_thread::sleep_for(chrono::milliseconds(9000));// Wait 6 Secs to allow the lawson to get going.
                    } else {                    // A thread is established for the lawson sub-system, then communiction is established.
                        // Set the time the law sub-system was initialized to 2 minutes ago so that it attempts to sign-on right away.
                        chrono::time_point<chrono::system_clock> iniTime = chrono::system_clock::now() - chrono::minutes(2);
                        lawSys->setTimeInitializedTo(iniTime);// Set the initialization time.
                        if (SCEquipment::isInArtificialMode == true) {// Is the plant manager in artificial mode?
                            lawSys->isRequestedToSignOn = false;// The PM is in artificial mode. This lawson should not be connected.
                        } else {                // The PM is in real mode.
                            lawSys->isRequestedToSignOn = true;// Set the request to sign-on.
                        } // End of condition: Is the PM in artificial mode?
                        lawsonSystems_.push_back(lawSys);// Add this lawson equipment to the list of lawson system's.
                        std::thread(LawsonEquip::operateEquipment, lawSys).detach();// Start the thread to operate this lawson sub-system.
                        isASubSystemInitiated = true;// If this line is executed, the lawson is initiated and its thread is running.
                                                    // This does not mean that the lawson is actually working. It could be trying to
                                                    // sign-on.
                    } // End of condition: Coonnect to the lawson equipment within the lawson thread.
                } // End of condition: The lawson sub-system is on-line.
                break;                          // Done starting this lawson sub-system.
            } // End of the case for a MICRO or STANDARD lawson sub-system.
            default: {                          // This should never be called.
                break;                          // Nothing to do.
            } // End of default.
        } // End of switching for the sub-system type.
    } // End of looping through each sub-system.
    if (SCEquipment::isInArtificialMode == true) {
        toLog("The PM is in artificial mode.");
    } else {
        toLog("The PM is in real mode.");
    }
    if(isASubSystemInitiated == false) {        // Were any sub-systems initiated?
        errorMessage_ = "No sub-system were initiated. The Plant Manager is being shut down.";// No. Create an error.
        toLog(errorMessage_);                   // Add the error to the console.
    } // End of condition checking for an initiated sub-system.
    return isASubSystemInitiated;// If at least one sub-system was initiated, true is returned to allow that sub-system to operate.
} // End of function initiateIOSystems.

SCInput* Manager::getSCInput( unsigned int index) {
    if (index > theInputs_.size()) {            // Is the index legal?
        return nullptr;                         // No. return no pointer to an SCInput.
    }                                           // The index is legal.
    return theInputs_.at(index);                // Return the pointer to the desired SCInput.
}

SCChnl* Manager::getSCChnl(string chnlName) {
    SCChnl* theChnl = nullptr;                  // Will contain the SCChnl with the given name.
    for (int nDex = 0; nDex < theChnls_.size(); nDex++) {// Go through each SCChnl.
        SCChnl* aChnl = theChnls_.at(nDex);     // Get access to this chnl.
        if (chnlName.compare(aChnl->getName()) == 0) {// Is this chnl's name the same as the given name?
            theChnl = aChnl;                    // Yes. Remember this chnl.
            break;                              // The chnl has been found. There is no need to look farther.
        }// Done comparing names.
    }// Done going through all the chnls.
    return theChnl;                             // Either a pointer to the chnl with the name or nullptr is being returned.
}

SCBed* Manager::getSCBed(string bedName) {
    SCBed* theBed = nullptr;                    // Will contains the SCBed with the given name.
    for (int nDex = 0; nDex < theBeds_.size(); nDex++) {// Go through each SCBed.
        SCBed* aBed = theBeds_.at(nDex);        // Get access to this bed.
        if (bedName.compare(aBed->getName()) == 0) {// Is this bed's name the same as the given name?
            theBed = aBed;                      // Yes. Remember this bed.
            break;                              // The bed has been found. There is no need to look farther.
        }// Done comparing names.
    }// Done going through all the beds.
    return theBed;                              // Provide the caller with either a pointer to the bed or a nullptr.
}

Relay* Manager::getRelay(unsigned int index) {
    if (index > theRelays_.size()) {            // Is the index legal?
        return nullptr;                         // No. return no pointer to a Relay.
    }                                           // The index is legal.
    return theRelays_.at(index);                // Return the pointer to the desired Relay.
}


void Manager::relayTest(unsigned int relayDex) {
    for_each(miniSystems_.begin(), miniSystems_.end(), [&] (MiniEquip* miniSys) {// Go through each mini system.
        miniSys->isARelayTesting = true;        // Tell each one a relay is being tested. They all must respond.
        miniSys->setTestRelay(relayDex);        // Tell each one which relay is being tested.
    });
    for_each(lawsonSystems_.begin(), lawsonSystems_.end(), [&] (LawsonEquip* lawSys) {// Go through each lawson system.
        lawSys->isARelayTesting = true;        // Tell each one a relay is being tested. They all must respond.
        lawSys->setTestRelay(relayDex);        // Tell each one which relay is being tested.
    });
    
}

void Manager::relayTestTerminate() {
    for_each(miniSystems_.begin(), miniSystems_.end(), [&] (MiniEquip* miniSys) {// Go through each mini system.
        miniSys->isARelayTesting = false;       // Tell each one relay testing is done. They all need to know.
    });
    for_each(lawsonSystems_.begin(), lawsonSystems_.end(), [&] (LawsonEquip* lawSys) {// Go through each lawson system.
        lawSys->isARelayTesting = false;       // Tell each one relay testing is done. They all need to know.
    });
}

unsigned int Manager::getNumberOfRelays() {
    return (unsigned int)theRelays_.size();     // Return the number of relays.
}

bool Manager::loadFromDB() {
    sqlite3 *db;                            // Create a location for the database.
    int  rc;                                // Create a return variable.
    string sql;                             // Create a string for SQL commands.
    bool status = true;                     // The desired result of reading the Plant Manager's settings is to succeed.
    string dbName = path_ + "SCConfig.db";  // Set the configuration database name.
    rc = sqlite3_open(dbName.c_str(), &db); // Open the database named SC_Config.db. It has tables containing the Plant Manager's configuration.
    if( rc ){                               // Was the database opened?
        errorMessage_ = "Can't open SCConfig.db to load the PM configuration: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);               // Add the error to the console.
        return false;                       // This error prevents loading the Plant Manager's configuration.
    }                                       // The database is open.
    string tableName = "pmSettings";        // The table name for the Plant Manager's configuration.
    sql = "SELECT * from " + tableName;     // Create an SQL statement to read the whole configuration.
    sqlite3_stmt *stmt;                     // Will contain the results of the querry.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
    if (rc != SQLITE_OK) {                  // Was it successful?
        errorMessage_ = "Can't get the PM configuration: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);               // Add the error to the console.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
        // In this case sqlite3_finalize will return an error because the previous statement caused a database error.
        sqlite3_close(db);                  // Close the database. There is no need to check for errors because finalize was just called.
        return false;                       // Let the caller know pmSettings could not be retrieved from the database.
    }                                       // The Plant Manager's settings were retreived.
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {// Go through each row in the results.
        //int id   = sqlite3_column_int (stmt, 0);// The id is in the first column.
        //version_ = sqlite3_column_int(stmt, 1); // The version is in the second column of the table.
        int si = sqlite3_column_int (stmt, 2);// Get the storage interval.
        setStorageInterval(si);                     // Set the storage interval.
        int isArtificial = sqlite3_column_int(stmt, 3);
        if(isArtificial == 1) {
            SCEquipment::isInArtificialMode = true;
        } else {
            SCEquipment::isInArtificialMode = false;
        }
        lowerTemperatureLimit = sqlite3_column_double(stmt, 4);
        upperTemperatureLimit = sqlite3_column_double(stmt, 5);
    }                                       // The PM config has been read.
    if (rc != SQLITE_DONE) {                // Was there an error?
        // Yes. Create an error message.
        errorMessage_ = "The pmSettings query didn't finish: " + string(sqlite3_errmsg(db));
        toLog(errorMessage_);               // Add the error to the console.
        status = false;                     // Plant Manager settings were not read.
    }                                       // The Plant Manager's setting query completion staus was evaluated.
    sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
    sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
    return status;                          // The Plant Manager's setting were read.
}

unsigned int Manager::setStorageInterval(unsigned int value) {
    if (value <= 60) {                          // Is the desired storage interval 60 or less minutes?
        if (60 % value == 0) {                  // Yes. Does the desired storage interval go into 60 evenly?
            storageInterval_ = value;           // Yes. Set the storage interval to the desired value.
        } else {                                // The desired storage interval is not valid.
            storageInterval_ = 6;               // Set the storage interval to a default of six minutes.
        }                                       // The storage interval for a desired value of 60 or less has been set.
    } else {                                    // The desired storage interval is greater than 60. This is not allowed.
        storageInterval_ = 6;                   // Set the desired storage interval to 6.
    }                                           // The storage interval has been set.
    return storageInterval_;                    // Provide the caller with the new storage interval.
}

unsigned int Manager::getStorageInterval() {
    return storageInterval_;                    // Provide the caller with the storage interval.
}

string Manager::getFilePath(void) {
    return path_;                               // Provide the caller with the Manager's configuration path.
}

void Manager::setFilePath(std::string strPath) {
    path_ = strPath;                            // Set the path to the Manager's configuration files.
}



void Manager::control() {
    time_t nowTime = time(0);                   // Contains the current time.
    struct tm* tmNow;                           // Will contain a more usable form of the current time.
    tmNow = localtime(&nowTime);                // Convert the current time into a more usable format.
    int theMinuteNow = tmNow->tm_hour * 60 + tmNow->tm_min;// The minute of the day determines when data is stored.
    if (controlCycle_ == 0) {                   // Is this the cycle when temperatures are transfered?
        // Yes. Go through each one wire system and transfer collected temperatures to the coresponding system inputs.
        checkAlerts();                          // Check for any alerts from the users through the database.
        for (std::vector<OneWireEquip*>::iterator it = oneWireSystems_.begin(); it != oneWireSystems_.end(); ++it) {
            if((*it)->areAllInputsMeasured == true) {// Is this one wire equipment running?
                SCSystem sys = (*it)->getSubSystem();// Get the subsystem this associated with this one wire equipment.
                for (int tempIndex = 0; tempIndex < sys.getNumberOfInputs(); tempIndex++) { // Go through each temperature input.
                    float chnlTemperature = (*it)->getTemperature(tempIndex); // Get the temperature from the temperature list.
                    toLog("Hardware temperature is " + to_string(chnlTemperature));
                    if (SCEquipment::isInArtificialMode == true) {// Is the system in artificial mode?
                        chnlTemperature = 80.0f - tempIndex / 10;   // Yes. Create an artificial temperature.
                    }                           // chnlTemperature now contains a temperature, even in artificial mode.
                    // Transfer the hardware temperature to the input temperature.
                    theInputs_.at(sys.getFirstInput() + tempIndex)->setMeasuredValue(chnlTemperature);
                    theInputs_.at(sys.getFirstInput() + tempIndex)->incrementAccumulatedValue();// Increment storage temperature.
                    errorMessage_ = "Average temp " + to_string(tempIndex) + "= ";
                    errorMessage_ += theInputs_.at(sys.getFirstInput() + tempIndex)->getMeasuredValueAverage();
                    toLog(errorMessage_);       // Add the error to the console.
                }                               // Done going through each temperature input.
            }                                   // Temperatures for this one wire system have been transfered to the inputs.
        }                                       // Done going through each OneWire sub-system.
        for_each(lawsonSystems_.begin(), lawsonSystems_.end(), [&] (LawsonEquip* lawSys) {
            // Go through each Lawson system and transfer collected temperatures to the coresponding system inputs.
            for (int tempIndex = 0; tempIndex < lawSys->getSubSystem().getNumberOfInputs(); tempIndex++) {
                float chnlTemperature = lawSys->getTemperature(tempIndex); // Get the temperature from the temperature list.
                // Is this sub-system not operating and not in artificial mode?
                if (lawSys->isOperating == false && SCEquipment::isInArtificialMode == false) {
                    chnlTemperature = 501.0f;// Yes. This chnl is not operating & not in artificial mode. Send it up scale.
                    if(lawSys->getSubSystem().isOnLine == false) {// Is this sub-system off-line?
                        chnlTemperature = 502.0f;// Yes. This input is off-line. 502 is a code for off-line.
                    } // End of condition: Lawson sub-system is off-line.
                } // End of condition: The lawsys is not operating and it is not in artificial mode.
                if (SCEquipment::isInArtificialMode == true) {// Is the system in artificial mode?
                    chnlTemperature = 80.0f - tempIndex / 10; // Yes. Create an artificial temperature.
                }                           // chnlTemperature now contains a temperature, even in artificial mode.
                // Transfer the hardware temperature to the input temperature.
                theInputs_.at(lawSys->getSubSystem().getFirstInput() + tempIndex)->setMeasuredValue(chnlTemperature);
                // Increment the storage temperature.
                theInputs_.at(lawSys->getSubSystem().getFirstInput() + tempIndex)->incrementAccumulatedValue();
            }// Done going through each temperature input.
        });// Done processing temperatures on Lawson systems.
        for_each(miniSystems_.begin(), miniSystems_.end(), [&] (MiniEquip* miniSys) {
            // Go through each mini system and transfer collected temperatures to the coresponding system inputs.
            for (int tempIndex = 0; tempIndex < miniSys->getSubSystem().getNumberOfInputs(); tempIndex++) {
                float chnlTemperature = miniSys->getTemperature(tempIndex); // Get the temperature from the temperature list.
                if (miniSys->isOperating == false && SCEquipment::isInArtificialMode == false) {
                    chnlTemperature = 501.0f;// This chnl is not operating & not in artificial mode. Send it up scale.
                    if(miniSys->getSubSystem().isOnLine == false) {// Is this sub-system off-line?
                        chnlTemperature = 502.0f;// Yes. This input is off-line. 502 is a code for off-line.
                    } // End of condition: Mini sub-system is off-line.
                } // The chnl's temperature has been set for everything except artificial mode.
                if (SCEquipment::isInArtificialMode == true) {// Is the system in artificial mode?
                    chnlTemperature = 80.0f - (float)tempIndex / 10;   // Yes. Create an artificial temperature.
                }                           // chnlTemperature now contains a temperature, even in artificial mode.
                // Transfer the hardware temperature to the input temperature.
                int inputDex = miniSys->getSubSystem().getFirstInput() + tempIndex;// Calculate the system input index.
                if(inputDex >= theInputs_.size()) {// Does the system input index exceed the system's total number of inputs?
                    errorMessage_ = "Configuration error: The desired input number " + to_string(inputDex + 1);// Yes. Create an
                    errorMessage_ += ", exceeds the number of inputs " + to_string(theInputs_.size()) + ".";// error message.
                    toLog(errorMessage_);       // Add the error to the console.
                } else {                        // The input index is valid.
                    theInputs_.at(inputDex)->setMeasuredValue(chnlTemperature);// Store the input's present tempeareture.
                    theInputs_.at(inputDex)->incrementAccumulatedValue();// Increment the tempearture accumulator used for plotting.
                }                               //
            }// Done going through each temperature input.
        });// Done processing temperatures on Mini systems.
        // Go through each SCChnl with an input.
        for_each(chnlsWithInputs_.begin(), chnlsWithInputs_.end(), [&] (SCChnl* aChnl) {
            double temp = theInputs_.at(aChnl->getInputIndex())->getMeasuredValue();// Get this chnl's measured tmperature.
            if(temp > theChnls_.getLowerTemperatureLimit() && temp < theChnls_.getUpperTemperatureLimit()) {// Valid temp?
                aChnl->maturity.increment(temp);// Yes.
            }                                   // This chnl's maturity was incremented.
        });                                     // All chnl's maturities have been incremented.
        if (theMinuteNow != lastStorageMinute_) {// Did the minute just change?
                                                // Yes, there was a change in minute.
                                                // Process the pulse meters.
            for_each(theMeters_.begin(), theMeters_.end(), [&] (SCPulseMeter* meter) {// Update data for all pulse meters.
                unsigned int heatUsageRightNow = getRelayUsageOn(meter->getIndex());
                // meter.HeatUsageUpToNow contains the total usage on meter at the last storage period. Thus the difference of heatUsageRightNow and meter.HeatUsageUpToNow is the heat usage in the last period for meter.
                unsigned int heatUsageInTheLastPeriod = 0;// Contains the electric heat usage on this meter since last storage.
                // A relay could have it's usage reset to zero, thus causing a negative usage.
                if (heatUsageRightNow > meter->getUsageHeat()) { // Will the usage be positive?
                    heatUsageInTheLastPeriod = heatUsageRightNow - meter->getUsageHeat();// Yes. Set the usage.
                }                               // The electric heat usage since last storage has been calculated.
                // Store the heat usage of all relays on this meter for the next time this part of the control loop is active.
                meter->setUsageHeat(heatUsageRightNow);
                unsigned int pulsesRightNow = 0;// Contains the hardware reading for the pulse counter.
                // Get access to the sub-system containing the pulse counter for this meter.
                SCSystem* sys = theSystems_.at(meter->getSubSystemIndex());
                if (sys->getType() == SCSystem::SystemType::SYSTYPE_STANDARD) {// Is this a lawson type system?
                    for_each(lawsonSystems_.begin(), lawsonSystems_.end(), [&] (LawsonEquip* lawSys){// Yes. Which lawson sub-system?
                        if (lawSys->getSubSystem().getSubSystemNo() == sys->getSubSystemNo()) {// Is this the sub-system?
                            pulsesRightNow = lawSys->getPulseCount();// Yes. Get the pulses from this system.
                        }                       // If this is the sub-system for the meter, the pulses have been set.
                    });                         // Done going through all the lawson sub-systems.
                }                               // Done potentially setting the pulses from a lawson sub-system.
                else if (sys->getType() == SCSystem::SystemType::SYSTYPE_MINI) {// Is this an Adlink or ADAM type system?
                    for_each(miniSystems_.begin(), miniSystems_.end(), [&] (MiniEquip* miniSys){
                        if (miniSys->getSubSystem().getSubSystemNo() == sys->getSubSystemNo()) {
                            for_each(miniSys->countModules.begin(), miniSys->countModules.end(), [&] (CountModule* cMod) {
                                if (meter->getPulseCounterAddress() == cMod->getAddress()) {// Is this the counter for this meter?
                                    pulsesRightNow = cMod->getCounter();  // Yes. Get the pulses for this pulse meter.
                                }               // This counter is processed.
                            });                 // All the counters in this sub-system have been processed.
                        }                       // This mini sub-system has been compaired to this meter's sub-system.
                    });                         // All the mini sub-system have been processed.
                } // Done testing if this meter is part of a mini system & if it is, getting the pulses from the meter.
                // Determine how many pulses have occured since the last time demand data was stored.
                unsigned int pulsesInTheLastPeriod = 0;// Contains the number of pulses in the last period.
                if (pulsesRightNow > meter->getPulses()){// Are pulsesRightNow greater than the pulses up to now.
                    // Yes. This is normal. Only allow the pulses in the last period to be positive.
                    pulsesInTheLastPeriod = pulsesRightNow - meter->getPulses();// Calculate the pulses in the last period.
                } else {                         // The counter wrapped.
                    pulsesInTheLastPeriod = pulsesRightNow; // Adjust for the wrap. The hardware wraps to zero at 9,999,999
                    if (10000000 > meter->getPulses()) {// Is the difference caused by the wrap a positive number?
                          // Yes. The difference should be if the wrap went as planned.
                        pulsesInTheLastPeriod += 10000000 - meter->getPulses();// Increment the number of pulses in the last period.
                    }                            // The pulses in the last period have been set for a wrap.
                }                                // The pulses in the last minute have been set, even if a wrap occured.
                meter->setPulses(pulsesRightNow);// Remember meter reading so pulses in the last period can be calculated next time.
                unsigned int index = meter->incrementCurrentDemandSampleNo();// Go to the next demand sample to save our results.
                int todaysSecond = theMinuteNow * 60;// The second of the day is the minute of the day x 60.
                todaysSecond += tmNow->tm_sec;  // The second of the day must be increment by the second of this minute.
                // Store the pulses and heat usage in the last period for this meter.
                meter->setDemandSample(index, pulsesInTheLastPeriod, heatUsageInTheLastPeriod, todaysSecond);
            }); // Done updating the demand data on all pulse meters.
            if (theMinuteNow % storageInterval_ == 0) {   // Is it time to store data?
                                                       // Yes, time to store data.
                toLog("Storing data at minute " + to_string(theMinuteNow));
                storeData();                    // Store the data.
            }                                   // Done storing data.
            lastStorageMinute_ = theMinuteNow;  // Update the last minute.
            return;
        }
    } // This is the end of controlCycle_ == 0.
    // It is necessary to use AlarmShouldTurnOn to signal that an alarm should turn on.
    // This prevents the alarm from toggeling due to threading.
    // Using AlarmWantsOn directly could cause toggeling while deciding if the alarm should be activated.
    if (controlCycle_ == 1) {                   // Is it the control cycle to set alarms?
        checkAlerts();                          // Check for any alerts from the users through the database.
        for_each(miniSystems_.begin(), miniSystems_.end(), [&] (MiniEquip* miniSys) {
            // Go through each mini sub-system input.
            miniSys->shouldAlarmBeOn = false;     // Turn off the alarm, reevaluate later
        });                                     // All mini sub-systems have beed evaluated.
        for_each(lawsonSystems_.begin(), lawsonSystems_.end(), [&] (LawsonEquip* lawSys) {
            // Go through each lawson sub-system input.
            lawSys->shouldAlarmBeOn = false;     // Turn off the alarm, reevaluate later
        });                                     // All lawson sub-systems have beed evaluated.
        for_each(oneWireSystems_.begin(), oneWireSystems_.end(), [&] (OneWireEquip* owSys) {
            // Go through each one-wire sub-system input.
            owSys->shouldAlarmBeOn = false;     // Turn off the alarm, reevaluate later
        });                                     // All one-wire sub-systems have beed evaluated.
        for (int chnlDex = 0; chnlDex < theChnls_.size(); chnlDex++) {
            // Go through each SCChnl sub-system input.
            SCChnl* scChnl = theChnls_.at(chnlDex);// Access the SCChnl.
            int inDex = scChnl->getInputIndex();// Get this chnl's input index.
            if (inDex == -1) {                  // Does this chnl have an input?
                continue;                       // No. Go to the next chnl.
            }                                   // There connot be an alarm without an input.
            if (scChnl->program.isActive == false) {// Is this chnl's program active?
                continue;                       // No. Go to the next chnl.
            }                                   // Only chnls with active programs can have an alarm.
/*//////////////////////////////////////////////////////////////////////////*/
            if(scChnl->program.isActive) {
                toLog("The active chnl is " + scChnl->getName());
                toLog("Its segment is " + to_string(scChnl->program.presentSegmentNo()));
                time_t tTime = chrono::system_clock::to_time_t(scChnl->program.getStartTime());
                string ts = ctime(&tTime);
                ts.resize(ts.size() - 1);
                toLog("The Start Time is " + ts);
                chrono::time_point<chrono::system_clock> timeNow = chrono::system_clock::now();// Get the present time.
                tTime = chrono::system_clock::to_time_t(timeNow);
                ts = ctime(&tTime);
                ts.resize(ts.size() - 1);
                toLog("is the time now.");
            }
/*//////////////////////////////////////////////////////////////////////////*/
            float setpoint = scChnl->program.presentSetPoint();// Get its setpoint.
            float temperature = theInputs_.at(inDex)->getMeasuredValue();// Get this chnl's present temperature.
            if ( setpoint != -500.0f) {         // Is the chnl on?
                if (scChnl->deviation > 0) {    // Is the deviation alarm set?
                    // Yes. Is the chnl's temperature outside the allowable deviation?
                    if (abs(temperature - setpoint) > scChnl->deviation) {
                        startAlarmProcess(scChnl);// Yes. Tell the alarm to come on.
                    }// Done handling the condition where this input has a deviation alarm.
                }// Done handling the condition of a deviation alarm being set.
                if (isValidTemeprature(temperature, setpoint) == false) {// Is this chnl's temperature/setpoint valid?
                    startAlarmProcess(scChnl);  // No. Tell the alarm to come on.
                }// Done handling the condition of invalid temperature/setpoit alarm needing to be set.
            }// Done handling a chnl which is currently being controlled.
        }// Done going through each input.
        for_each(miniSystems_.begin(), miniSystems_.end(), [&] (MiniEquip* miniSys) {
            // Go through each mini sub-system input.
            if(miniSys->shouldAlarmBeOn == true) { // Did any input on this mini system need the alarm to be turned on?
                miniSys->doesAlarmWantOn = true;  // Yes. Set the alarm to come on.
            } else {                            // No.
                miniSys->doesAlarmWantOn = false; // Turn the alarm off.
            }// Done setting the state of the alarm for this mini system.
        });// All mini sub-systems have beed evaluated.
        for_each(lawsonSystems_.begin(), lawsonSystems_.end(), [&] (LawsonEquip* lawSys) {
            // Go through each lawson sub-system input.
            if(lawSys->shouldAlarmBeOn == true) {// Did any input on this lawson system need the alarm to be turned on?
                lawSys->doesAlarmWantOn = true; // Yes. Set the alarm to come on.
            } else {                            // No.
                lawSys->doesAlarmWantOn = false; // Turn the alarm off.
            }// Done setting the state of the alarm for this lawson system.
        });// All lawson sub-systems have beed evaluated.
        for_each(oneWireSystems_.begin(), oneWireSystems_.end(), [&] (OneWireEquip* owSys) {
            // Go through each one-wire sub-system input.
            if(owSys->shouldAlarmBeOn == true) { // Did any input on this one wire system need the alarm to be turned on?
                owSys->doesAlarmWantOn = true;  // Yes. Set the alarm to come on.
            } else {                            // No.
                owSys->doesAlarmWantOn = false; // Turn the alarm off.
            }// Done setting the state of the alarm for this one wire system.
        });// All one-wire sub-systems have beed evaluated.
    }
    if (controlCycle_ == 2) {                   // Is it the control cycle to set relays on or off?
        checkAlerts();                          // Check for any alerts from the users through the database.
        // Go through each chnl with a relay.
        for (int nDex = 0; nDex < chnlsWithRelays_.size(); nDex++) {
            SCChnl* scChnl = chnlsWithRelays_.at(nDex);// Get access to this chnl's relay.
            // Get the chnl's setpoint. This returns the correct setpoint no matter how
            float setpoint = presentSetPoint(scChnl);// the channel is programmed.
            float temperature = theChnls_.getUpperTemperatureLimit() + 200;// Set the temperature upscale as a default.
            int inputDex = scChnl->getInputIndex();// Get this chnl's input index.
            if (inputDex >= 0) {                // Does this chnl have an input?
                temperature = (float)theInputs_.at(inputDex)->getMeasuredValue();// Yes. Get its temperature.
            }// Done testing for an input.
            Relay* relay = theRelays_.at(scChnl->getRelayIndex());// Get this chnl's relay.
            if (setpoint > -500 && scChnl->program.isActive == true) {// Is the setpoint valid and the chnl's program active?
                string fb = "Chnl " + to_string(scChnl->getIndex() + 1) + " setpoint:" + to_string(setpoint) + ", ";// Yes.
                fb += "temperature: " + to_string(temperature);
                fb += ", Time-on: " + to_string(relay->getTimeOn());
                toLog(fb);
            }
            if (scChnl->program.getControlType() == SCProgram::TYPE_MIMIC) {// Is it a mimic?
                adjustMimicList(scChnl);        // Adjust the reference relay's mimic list.
            }// Done testing for a mimic.
            if (scChnl->program.isActive == false && relay->isOn == false) {// Is this chnl inactive and is its relay off?
                continue;                           // Yes. Nothing left to do, go to the next chnl with a relay.
            }                                       // Inactive chnls with relays that want off have been dealt with.
            if (isValidTemeprature(temperature, setpoint) == true) {// Are the temperature and setpoint valid?
                // Yes. Since the chnl's temperature and setpoint are both valid, now it is time to determine
                // if this chnl's relay should be opened or closed.
                if (temperature < setpoint) {       // Is the chnl too cold?
                    // Yes. This chnl should have its relay closed(turned on).
                    errorMessage_ = "Channel " + scChnl->getName() + " Temp = " + to_string(temperature);
                    errorMessage_ +=" Setpoint " + to_string(setpoint);
                    toLog(errorMessage_);               // Add the note to the console.
                    // Is this chnl programmed as something other than a mimic?
                    if (scChnl->program.getControlType() != SCProgram::TYPE_MIMIC) {
                        if (relay->getConnectedLoad() == Relay::LOAD_ELECTRIC) {// Does this relay have an electric load?
                            // Yes, the relay has an electric load.
                            errorMessage_ = "This channel has an electric load and the activator has not been written yet!";
                            toLog(errorMessage_);   // Add the error to the console.
                            // Start here, add the Activator class.
                        } else {                    // This relay has a non-electric load.
                            relay->requestToTurnOn();// Request to have this relay closed(turned on).
                            int relayNo = relay->indexNo;
                            toLog("Relay " + to_string(relayNo) + " has requested to turn on.");
                        }// Done turning on non-electric loads.
                    }// Done checking for mimic programs.
                } else {                            // The chnl is too hot.
                    if (scChnl->program.getControlType() != SCProgram::TYPE_MIMIC) {
                        if (relay->getConnectedLoad() == Relay::LOAD_ELECTRIC) {// Does this relay have an electric load?
                            // Yes, the relay has an electric load.
                            ;// Start here, add the Activator class.
                        } else {                    // This relay has a non-electric load.
                            relay->requestToTurnOff();// Request to have this relay opened(turned off).
                        }// Done turning off non-electric loads.
                    }// Done checking for mimic programs.
                }// Done dealing with chnls which are too hot.
            } else {                                // There was an invalid temperture. Open(turn off) its relay.
                if (scChnl->program.getControlType() != SCProgram::TYPE_MIMIC) {
                    if (relay->getConnectedLoad() == Relay::LOAD_ELECTRIC) {// Does this relay have an electric load?
                        // Yes, the relay has an electric load.
                        ;// Start here, add the Activator class.
                    } else {                    // This relay has a non-electric load.
                        relay->requestToTurnOff();// Request to have this relay opened(turned off).
                    }// Done turning off non-electric loads.
                }// Done checking for mimic programs.
            }// Done processing the relay of a chnl with an invalid temperature.
        }// Done going through each chnl which has relays.
    }// Done with the control cycle to relays on and off.
    if (controlCycle_ == 3) {                   // Activate electric load relays during the 4th cycle.
        // Start here, add the Activator class.
        checkAlerts();                          // Check for any alerts from the users through the database.
    }
    // Is this the control cycle to update the readings in the database and check to see if any mini sub-systems need
    if (controlCycle_ == 4) {                   // to have communications restored?
        // Yes. Update the database readings.
        dbUpdateReadings();                     // Update the readings table. The table will contain the current reading for all chnls.
        checkAlerts();                          // Check for any alerts from the users through the database.
        restoreMinis();                         // This checks each mini sub-systems to see if any need to be restarted.
                                                // If there are any that do, it will start the process.
    }                                           // Control cycle 4 is complete.
    controlCycle_++;                            // Move onto the next part of the control cycle.
    controlCycle_ %= 5;                         // Reset the control cycle to the start after five calls.
}

float Manager::getTemperature(string name) {
    float temp = -500.0f;                       // The default temperature is not real to indicate a name error.
    // Get the SCChnl with this name.
    if (theChnls_.nameExists(name) == false) {  // Is there an SCChnl with the given name?
        return temp;                            // No. Return the not real temperature to indicate a name error.
    }                                           // The name exists.
    // Get the SCChnl's input index.
    int inputDex = -1;                          // -1 is an illegal input index.
    int chnlDex = 0;                            // The default SCChnl index with the given name is 0.
    for (; chnlDex < theChnls_.size(); chnlDex++) {// Go through each SCChnl.
        if (theChnls_.at(chnlDex)->getName() == name) {// Does this SCChnl's name match the given name?
            inputDex = theChnls_.at(chnlDex)->getInputIndex();// Yes. Get its input index.
            break;                              // Done looking.
        }                                       // This SCChnl's name does not match the given name.
    }                                           // The input index of the SCChnl having the given name has been found.
    // Return the input's temperature.
    return (float)theInputs_.at(inputDex)->getMeasuredValue();// Return the input's temperature.
}

bool Manager::loadMaturityFromDB() {
    sqlite3 *db;                            // Create a location for the database.
    int  rc;                                // Create a return variable.
    string sql;                             // Create a string for SQL commands.
    string dbName = getFilePath() + "SCConfig.db";// Set the configuration database name.
                                            // Open the database named SC_Config.db.
    rc = sqlite3_open(dbName.c_str(), &db); // It has a table which contains the readings including maturity .
    if( rc ){                               // Was the database opened?
        errorMessage_ = "Can't open SCConfig.db to load the Maturity";// No. Create a message.
        errorMessage_ += ": " + string(sqlite3_errmsg(db));// Add the database error to the message.
        toLog(errorMessage_);               // Add the error to the console.
        return false;                       // Let the caller know the maturity was not read.
    }                                       // The database is open.
    string tableName = "readings";          // Create the table name for the readings.
    sql = "SELECT * from " + tableName;     // Create an SQL statement to get all the channel readings.
    sqlite3_stmt *stmt;                     // Will contain the results of the querry.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
    if (rc != SQLITE_OK) {                  // Was it successful?
        errorMessage_ = "Failed to query 'readings' for maturity: ";// No, there was a problem. Create an error message.
        errorMessage_ += string(sqlite3_errmsg(db));// Add the database error to the message.
        toLog(errorMessage_);               // Add the error to the console.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
        return false;                       // Let the caller know the maturity couldn't be retrieved from the database.
    }                                       // The maturity was retreived.
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {// Go through each row in the results.
        int id = sqlite3_column_int (stmt, 0);// The id is in the first column.
        SCChnl* theChnl = theChnls_.at(id - 1);// Get access to this chnl. The database is ones based but, the PM chnls are zero based.
        const unsigned char *cMat = sqlite3_column_text(stmt, 7);// The maturity F is in the table's eigth column.
        string sMat = "0.0";                // The default maturity F is zero.
        if(cMat != NULL) {                  // Did the DB have a maturity F for this chnl?
            string sMat = std::string(reinterpret_cast<const char*>(cMat));// Convert the maturity F to a string.
        }                                   //
        double mat = stod(sMat);            // Convert the string form of the maturity F to a number.
        theChnl->maturity.setMaturityF(mat);// Set this channel's maturity F.
        cMat = sqlite3_column_text(stmt, 9);// The maturity C is in the table's tenth column.
        sMat = "0.0";                       // The default maturity C is zero.
        if(cMat != NULL) {                  // Did the DB have a maturity C for this chnl?
            sMat = std::string(reinterpret_cast<const char*>(cMat));// Yes. Convert the maturity C to a string.
        }                                   //
        mat = stod(sMat);                   // Convert the string form of the maturity C to a number.
        theChnl->maturity.setMaturityC(mat);// Set this channel's maturity C.
    }                                       // The maturity has been read for each channel
    if (rc != SQLITE_DONE) {                // Was there an error?
        errorMessage_ = "Error loading maturity: " + string(sqlite3_errmsg(db));// No. Create error.
        toLog(errorMessage_);               // Add the error to the console.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
        sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
        return false;                       // Let the caller know each chnl's maturity didn't load.
    }                                       // The each chnl's maturity has been successfully read.
    sqlite3_finalize(stmt);                 // Free any resources associated with the prepared statement "stmt". Any errors are irrelevant.
    sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
    return true;                            // Let the caller know each chnl's maturity has been loaded.
}

unsigned int  Manager::getRelayUsageOn(int meterDex) {
    unsigned int usage = 0;                 // Contains the total usage(watt-hours) for all electric relays on this meter.
                                            // Go through each relay with an electric load.
    for_each(theRelays_.electricRelays.begin(), theRelays_.electricRelays.end(), [&] (Relay* aRelay) {
        if (aRelay->indexNo == meterDex) {  // Is this relay connected to the selected meter?
            usage += aRelay->getUsage();    // Yes. Increment the total relay usage by this relay's usage.
        }                                   // The total relay usage has been incremented
    });                                     // Done going through all the elctric relays.
    return usage;                           // Return the total usage for all electric load relays with connected to this meter.
}

void Manager::storeData() {
    if (storeValuesAsSQL() == false) {      // Stores the temperature data to SC_tt.db.
        // Something went wrong storing the temperatures to the DB.
        errorMessage_ = "Couldn't save the temperatures to SC_tt.db.";// Create an error message.
        toLog(errorMessage_);               // Add the error to the console.
    } // End of the condition check for a failure to store temperatures to the DB.
    if (theRelays_.saveTimeOnUsageToDB() == false) {// Stores the relay's time-on or useage to SCConfig.db.
        // Something went wrong storing the time-on or usage to the DB.
        errorMessage_ = "Couldn't save the relays time-on/usage.";// Create an error message.
        toLog(errorMessage_);               // Add the error to the console.
    } // End of the condition check for a failure to store time-on or usage to the DB.
    if (theMeters_.storeDemandToDB() == false) {// Stores the demand data to SC_dmd.db.
        // Something went wrong storing the demand data to the DB.
        errorMessage_ = "Couldn't save the demand history.";// Create an error message.
        toLog(errorMessage_);               // Add the error to the console.
    } // End of the condition check for a failure to store demand data to the DB.
    if (theRelays_.saveHistoryToDB() == false) { // Store the time-on history for each relay to SC_tim.db.
        // Something went wrong storing the relay history to the DB.
        errorMessage_ = "Couldn't save each relay's history.";// Create an error message.
        toLog(errorMessage_);               // Add the error to the console.
    } // End of the condition check for a failure to store relay history to the DB.
} // End of function storeData.

bool Manager::storeValuesAsSQL() {          // Store the time/temp data in a sqlite database.
    sqlite3 *db;                            // Create a location for the database.
    char *zErrMsg = 0;                      // Create a location for an error message.
    int  rc;                                // Create a return variable.
    string sql;                             // Create a string for SQL commands.
//***********The following line is required if a callback function is going to be used. ******************//
//    const char* data = "Callback function called.";// Create a string for the manditory callback function required by sqlite3_exec.
//***********The previous line is required if a callback function is going to be used. ******************//
    string dbName = path_ + "SC_tt.db";     // Set the time/temperature database name.
    rc = sqlite3_open(dbName.c_str(), &db); // Open the database named SC_tt.db. It will be created if it doesn't exist.
    if( rc ){                               // Was the database opened?
        errorMessage_ = "Can't open SC_tt.db: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);               // Add the error to the console.
        return false;                       // This error prevents writting the temperatures to the database.
    }                                       // The database is open.
    sqlite3_busy_timeout(db, 50);           // If the DB is locked wait up to 50 milliseconds before giving up.
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
    for (int nDex = 0; nDex < theInputs_.size(); nDex++) {// Go through each input to store its average measured value.
        string tableName = "tvt" + to_string(nDex);// Create the table name for this input.
        sql = "SELECT * from " + tableName + " LIMIT 1"; // Create an SQL statement to see if the table exists.
//***********The following line is what sqlite3_exec looks like if a callback function is used ******************//
//        rc = sqlite3_exec(db, sql.c_str(), callback, (void*)data, &zErrMsg);// Get a row from this inputs table.
//***********The previous line is what sqlite3_exec looks like if a callback function is used ******************//
        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);// Get a row from this inputs table.
        if( rc != SQLITE_OK ){              // Was a row returned?
            toLog("Creating " + tableName); // No. At this point, it is assumed this input's table does not exist.
            sqlite3_free(zErrMsg);          // Free the memory allocated for the error message.
            sql = "CREATE TABLE " + tableName + "(";// Create a SQL statement to create this input's table.
            sql += "id           INTEGER PRIMARY KEY      AUTOINCREMENT,";// The table contains a primary key.
            sql += "sc_time     TEXT,";     // The table contains date/time.
            sql += "temperature REAL );";   // The table also contains this input's temperature.
            toLog("The SQL statement is: " + sql);
            /* Execute SQL statement */
            rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);// Get a row from this input's table.
            if( rc != SQLITE_OK ){          // Was the table created?
                // No. Create an error message.
                errorMessage_ = "Failed to create a time/temp table for input " + to_string(nDex) + ": " + string(zErrMsg);
                toLog(errorMessage_);       // Add the error to the console.
                sqlite3_free(zErrMsg);      // Free the memory allocated for the error message.
                continue;                   // Go on to the next input.
            } // End of condition: Failure to create a new time temperature table for this input.
        } // End of condition: There was no time temperature table for this input?
        SCInput* theInput = theInputs_.at(nDex);// Get the input thats temperature is going to be stored.
        float averageTemp = theInput->getMeasuredValueAverage();// Get its avarge tempearture since the last time it was stored.
        theInput->resetAccumulatedValue();  // Reset the variables used to calculate the average.
        /* Create SQL statement */
        sql = "INSERT INTO " + tableName;   // A new time/temperature point is being inserted into the table.
        sql += " (sc_time,temperature) ";   // Call out the time and temperature columns.
        sql += "VALUES ('" + timeNow + "', " + to_string(averageTemp) + ")";// Put time and temperature values into those columns.
        /* Execute SQL statement */
        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);// Put the time temperature point in the table.
        if( rc != SQLITE_OK ){              // Did the time temperature point go into the table?
            // No. Create an error message.
            errorMessage_ = "Failed to insert a temperature for input " + to_string(nDex) + ": " + string(zErrMsg);
            toLog(errorMessage_);           // Add the error to the console.
            sqlite3_free(zErrMsg);          // Free the memory allocated for the error message.
        } // End of condition: The time temperature point failed to be inseted into the table.
    } // End of looping through all the inputs.
    if (sqlite3_close(db) != SQLITE_OK) {   // Close SC_tt.db. Did it close?
        // No. Create an error message and discontinue talking to the SC_tt.db.
        errorMessage_ = "Can't close SC_tt.db after attempting to INSERT temperatures: " + string(sqlite3_errmsg(db));
        toLog(errorMessage_);               // Add the error to the console.
        return false;                       // Let the caller know there was a problem.
    } // End of condition: The database didn't close.
    return true;                        // The time vs temperature values were successfully stored in a SQLite databasae.
} // End of function storeValuesAsSQL.

string Manager::utcTimeNowString() {
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
    timeNow += sMin + ":";                  // Next add the minute.
    int sec = tmNow->tm_sec;                // Get the second.
    string sSec = sec < 10 ? "0" + to_string(sec) : to_string(sec);// Create a two digit second.
    timeNow += sSec;                        // Next add the minute.
    return timeNow;                         // Provide the user with the current UTC time as a string.
}

void Manager::setTemperaturePath(std::string strTTPath) {
    temperatureDataPath_ = strTTPath;               // Set the sub-folder for the temperature data files.
}

void Manager::test() {
}

string Manager::getTemperatureDataPath(void) {
    return temperatureDataPath_;
}

short Manager::getTTSignature(int year, int month, int day, int hour, int minute) {
    short signature = -1;
    string ttFileName = path_;              // The path to the temperature data files begins with the path to the Plant Manger.
    ttFileName += temperatureDataPath_;     // The path then contains the sub-folder for temperature data files.
    ttFileName += to_string(year) + to_string(month) + ".tt";// Append the .tt temperature file name to the path.
    fstream ttFile(ttFileName, ios::binary | ios_base::in);// Open the .tt file.
    int offset = (day - 1) * 1440;          // Calculate the minute at the start of today.
    int minuteOfDay = hour * 60 + minute;   // Claculate the minute of today.
    int deviation = minuteOfDay % storageInterval_;// The amount past a storage interval the minute of the day is.
    if ( deviation != 0) {                  // Is the minute of the day on a storage interval?
        minuteOfDay -= deviation;           // No. Decrement the minute of the day to the previous storage minute.
    }                                       // The minute of the day is set to a storage minute.
    offset += minuteOfDay;                  // Add the minute of the day to get the minute of the month.
    offset = offset / storageInterval_ + 1; // Calculate where to write the temperature data.
                                            // (adding one corrects for the first record which is for file info)
    short nRecordInputs = theInputs_.getNumberOfInputs();// Get the number of inputs. ITs possible less than 6 inputs exist.
    if (nRecordInputs < 6) {                // Are there less than six inputs?
        nRecordInputs = 6;                  // Yes. Even though there aren't inputs, to maintain file format it is necessary each
    }                                       // record has at least six values.
    offset *= (nRecordInputs + 1) * 2;      // Adjust for the offset by then umber of temperature inputs.
                                            // Each record contains a temperature(requiring 2 bytes) for each channel.
                                            // The start of each record is the minute of the month which requires 2 bytes.
    ttFile.seekg(offset, ios_base::beg);    // Move the file pointer to the correct location for this record.
    ttFile.read((char*)&signature, 2);      // Read the signature, minute of the month.
    ttFile.close();                         // Done reading the signature, close the file.
    return signature;                       // Provide the caller with the signature.
}

float Manager::getTTValue(int year, int month, int day, int hour, int minute, int nDex) {
    float value = 0.0;                      // The default value.
    short signature = -1;                   // an illegal signature (minute of the month).
    string ttFileName = path_;              // The path to the temperature data files begins with the path to the Plant Manger.
    ttFileName += temperatureDataPath_;     // The path then contains the sub-folder for temperature data files.
    ttFileName += to_string(year) + to_string(month) + ".tt";// Append the .tt temperature file name to the path.
    fstream ttFile(ttFileName, ios::binary | ios_base::in);// Open the .tt file.
    if (ttFile.is_open() == false) {        // Was the tt file opened?
        string error = "The file " + ttFileName + " couldn't be opened.";// No. Create a message.
        throw error;                        // Let the user know the file wasn't opened.
    }                                       // The tt file is open.
    ttFile.exceptions(ifstream::failbit | ifstream::eofbit | ifstream::badbit);// Activate exceptions.
    int offset = (day - 1) * 1440;          // Calculate the minute of the month at the start of the desired day.
    int minuteOfDay = hour * 60 + minute;   // Calculate the minute of the desired day.
    int deviation = minuteOfDay % storageInterval_;// The amount the minute of the day is past a storage interval.
    if ( deviation != 0) {                  // Is the minute of the day on a storage interval?
        minuteOfDay -= deviation;           // No. Decrement the minute of the day to the previous storage minute.
    }                                       // The minute of the day is set to a storage minute.
    offset += minuteOfDay;                  // Add the minute of the day to get the minute of the month.
    offset = offset / storageInterval_ + 1; // Calculate where to read the temperature data.
    // (adding one corrects for the first record which is for file info)
    // Get the a records length for this particular .tt file.
    // From the record length, determine the number of inputs stored in each record.
    ttFile.seekg(0, ios_base::beg);         // Move the file pointer to the start of the file.
    short recordLength = 0;                 // Will contain the length of records in this file.
    ttFile.read((char*)&recordLength, 2);   // Read the length of records in this file.
    short nRecordInputs = recordLength / 2 - 1;// Calculate the number of inputs. (2 bytes for each input plus 2 bytes for the time)
    if (nRecordInputs < 6) {                // Are there less than six inputs?
        nRecordInputs = 6;                  // Yes. Even though there aren't inputs, to maintain file format it is necessary each
    }                                       // record has at least six values.
    offset *= (nRecordInputs + 1) * 2;      // Adjust for the offset by the number of inputs.
    // Each record contains a value(requiring 2 bytes) for each channel.
    // The start of each record is the minute of the month which requires 2 bytes.
    ttFile.seekg(offset, ios_base::beg);    // Move the file pointer to the correct location for this record.
    ttFile.read((char*)&signature, 2);      // Read the signature, minute of the month.
    ttFile.seekg(nDex * 2, ios::cur);       // Move past the current file get position by 2 bytes per input.
    unsigned short int nValue;              // Will contain the encoded value of the input's value.
    ttFile.read((char*)&nValue, 2);         // Read the input's value.
    ttFile.close();                         // Done reading the input's value, close the file.
    if(nValue >= 32768) {                   // Is the encoded value negative.
        nValue = 32768 - nValue & 32767;	// Yes. Don't know why, but need to AND 32768 to result.
        nValue = -nValue;                   // Convert the number to a negative value.
    }                                       // The negative part of a number, if it exists, has been processed.
    value = (float)nValue / 10;             // Convert to a float value (temperature).
    return value;
}

float Manager::getTTValue(string argument) {
    float value = 0.0;                      // The default value.
    int yearNo = -1;                        // The default year is invalid.
    int monthNo = -1;                       // The default month number is invalid.
    int dayNo = -1;                         // The default day number is invalid.
    int hourNo = -1;                        // The default hour number is invalid.
    int minuteNo = -1;                      // The default minute number is invalid.
    int nDex = -1;                          // The default input index is invalid.
    string strError = "";                   // Contains an error message.
    size_t seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the year!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string strYear = argument.substr(0, seperatorLocation);// Get the part of the argument containing the year.
        yearNo = stoi(strYear);             // Convert the year to a number.
        if (yearNo < 1980) {                // Is the year number valid?
            strError = "Error: The year must be greater than 1980!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The year number is probably valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - strYear.size() - 1);// Remove the year from the argument.
    }                                       // Next process the month.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the month!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the month.
        monthNo = stoi(str);                // Convert the month to a number.
        if (monthNo < 1 || monthNo > 12) {  // Is the month number valid?
            strError = "Error: The month must be between 1 and 12!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The month number is probably valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - str.size() - 1);// Remove the month from the argument.
    }                                       // Next process the day.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the day!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the day.
        dayNo = stoi(str);                  // Convert the day to a number.
        if (dayNo < 1 || dayNo > 31) {      // Is the day number valid?
            strError = "Error: The day must be between 1 and 31!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The day number is probably valid.
        if (monthNo == 2 || monthNo == 4 || monthNo == 6 || monthNo == 9 || monthNo == 11 ) {
            if (dayNo == 31) {  // Is the day 31? Yes, it is impossible to determine the date & time.
                strError = "Error: Day number 31 does not exist in month number " + to_string(monthNo) + "!";
                throw strError;             // Pass an error message to the caller.
            }                               // The 31st is not used in a month with only 30 days or less.
            if (monthNo == 2 && dayNo == 30) {// Is the 30th trying to be used in February?
                strError =  "Error: Day number 30 does not exist in month number 2!";// Yes.
                throw strError;             // Pass an error message to the caller.
            }                               // The 30th is not being used in February.
        }                                   // The day number is probably valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - str.size() - 1);// Remove the day from the argument.
    }                                       // Next process the hour.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the hour!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the hour.
        hourNo = stoi(str);                 // Convert the hour to a number.
        if (hourNo < 0 || hourNo > 23) {    // Is the hour number valid?
            strError = "Error: The hour must be between 0 and 23!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The hour number is valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - str.size() - 1);// Remove the hour from the argument.
    }                                       // Next process the minute.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the minute!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the minute.
        minuteNo = stoi(str);               // Convert the minute to a number.
        if (minuteNo < 0 || minuteNo > 59) {// Is the minute number valid?
            strError = "Error: The minute must be between 0 and 59!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The minute number is valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - str.size() - 1);// Remove the minute from the argument.
    }                                       // Next process the input index.
    nDex = stoi(argument);                  // Convert the input index to a number.
    try {                                                                   // Try to get the temperature at the
        value = getTTValue(yearNo, monthNo, dayNo, hourNo, minuteNo, nDex); // given time for the given input.
    } catch (string ex) {                   // There was a problem getting the temperature.
        strError = "Error calling getTTValue(): " + ex;
        throw strError;                     // Pass the problem to the caller.
    } catch (...) {                         // There was an unexpected problem getting the temperature.
        strError = "Unknown error calling getTTValue()!";// Create an error message.
        throw strError;                     // Let the caller know there the temperature could not be read.
    }                                       // The temperature was read.
    return value;                           // Provide the caller with the value. (probably temperature);
}

string Manager::getTTValues(string argument) {
    string values = "";
    int yearNo = -1;                        // The default year is invalid.
    int monthNo = -1;                       // The default month number is invalid.
    int dayNo = -1;                         // The default day number is invalid.
    int hourNo = -1;                        // The default hour number is invalid.
    int minuteNo = -1;                      // The default minute number is invalid.
    string strError = "";                   // Contains an error message.
    size_t seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the year!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string strYear = argument.substr(0, seperatorLocation);// Get the part of the argument containing the year.
        yearNo = stoi(strYear);             // Convert the year to a number.
        if (yearNo < 1980) {                // Is the year number valid?
            strError = "Error: The year must be greater than 1980!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The year number is probably valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - strYear.size() - 1);// Remove the year from the argument.
    }                                       // Next process the month.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the month!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the month.
        monthNo = stoi(str);                // Convert the month to a number.
        if (monthNo < 1 || monthNo > 12) {  // Is the month number valid?
            strError = "Error: The month must be between 1 and 12!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The month number is probably valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - str.size() - 1);// Remove the month from the argument.
    }                                       // Next process the day.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the day!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the day.
        dayNo = stoi(str);                  // Convert the day to a number.
        if (dayNo < 1 || dayNo > 31) {      // Is the day number valid?
            strError = "Error: The day must be between 1 and 31!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The day number is probably valid.
        if (monthNo == 2 || monthNo == 4 || monthNo == 6 || monthNo == 9 || monthNo == 11 ) {
            if (dayNo == 31) {  // Is the day 31? Yes, it is impossible to determine the date & time.
                strError = "Error: Day number 31 does not exist in month number " + to_string(monthNo) + "!";
                throw strError;             // Pass an error message to the caller.
            }                               // The 31st is not used in a month with only 30 days or less.
            if (monthNo == 2 && dayNo == 30) {// Is the 30th trying to be used in February?
                strError =  "Error: Day number 30 does not exist in month number 2!";// Yes.
                throw strError;             // Pass an error message to the caller.
            }                               // The 30th is not being used in February.
        }                                   // The day number is probably valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - str.size() - 1);// Remove the day from the argument.
    }                                       // Next process the hour.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the hour!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the hour.
        hourNo = stoi(str);                 // Convert the hour to a number.
        if (hourNo < 0 || hourNo > 23) {    // Is the hour number valid?
            strError = "Error: The hour must be between 0 and 23!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The hour number is valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - str.size() - 1);// Remove the hour from the argument.
    }                                       // Next process the minute.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the minute.
    minuteNo = stoi(str);                   // Convert the minute to a number.
    if (minuteNo < 0 || minuteNo > 59) {    // Is the minute number valid?
        strError = "Error: The minute must be between 0 and 59!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    }                                       // The minute number is valid.
    float value = 0.0;                      // The default value.
    short signature = -1;                   // an illegal signature (minute of the month).
    string ttFileName = path_;              // The path to the temperature data files begins with the path to the Plant Manger.
    ttFileName += temperatureDataPath_;     // The path then contains the sub-folder for temperature data files.
    ttFileName += to_string(yearNo) + to_string(monthNo) + ".tt";// Append the .tt temperature file name to the path.
    fstream ttFile(ttFileName, ios::binary | ios_base::in);// Open the .tt file.
    if (ttFile.is_open() == false) {        // Was the tt file opened?
        string error = "The file " + ttFileName + " couldn't be opened.";// No. Create a message.
        throw error;                        // Let the user know the file wasn't opened.
    }                                       // The tt file is open.
    ttFile.exceptions(ifstream::failbit | ifstream::eofbit | ifstream::badbit);// Activate exceptions.
    int offset = (dayNo - 1) * 1440;        // Calculate the minute at the start of today.
    int minuteOfDay = hourNo * 60 + minuteNo;// Calculate the minute of today.
    int deviation = minuteOfDay % storageInterval_;// The amount past a storage interval the minute of the day is.
    if ( deviation != 0) {                  // Is the minute of the day on a storage interval?
        minuteOfDay -= deviation;           // No. Decrement the minute of the day to the previous storage minute.
    }                                       // The minute of the day is set to a storage minute.
    offset += minuteOfDay;                  // Add the minute of the day to get the minute of the month.
    offset = offset / storageInterval_ + 1; // Calculate where to read the temperature data.
    // (adding one corrects for the first record which is for file info)
    // Get the a records length for this particular .tt file.
    // From the record length, determine the number of inputs stored in each record.
    ttFile.seekg(0, ios_base::beg);         // Move the file pointer to the start of the file.
    short recordLength = 0;                 // Will contain the length of records in this file.
    ttFile.read((char*)&recordLength, 2);   // Read the length of records in this file.
    short nRecordInputs = recordLength / 2 - 1;// Calculate the number of inputs. (2 bytes for each input plus 2 bytes for the time)
    if (nRecordInputs < 6) {                // Are there less than six inputs?
        nRecordInputs = 6;                  // Yes. Even though there aren't inputs, to maintain file format it is necessary each
    }                                       // record has at least six values.
    offset *= (nRecordInputs + 1) * 2;      // Adjust for the offset by the number of inputs.
    // Each record contains a value(requiring 2 bytes) for each channel.
    // The start of each record is the minute of the month which requires 2 bytes.
    ttFile.seekg(offset, ios_base::beg);    // Move the file pointer to the correct location for this record.
    ttFile.read((char*)&signature, 2);      // Read the signature, minute of the month.
    for (int nDex = 0; nDex < nRecordInputs; nDex++) {
        ttFile.seekg(offset + nDex * 2 + 2, ios_base::beg);// Move to the file get position for this input.
        unsigned short int nValue;          // Will contain the encoded value of the input's value.
        ttFile.read((char*)&nValue, 2);     // Read the input's value.
        if(nValue >= 32768) {               // Is the encoded value negative.
            nValue = 32768 - nValue & 32767;// Yes. Don't know why, but need to AND 32768 to result.
            nValue = -nValue;               // Convert the number to a negative value.
        }                                   // The negative part of a number, if it exists, has been processed.
        value = (float)nValue / 10;         // Convert to a float value (temperature).
        value += .01;                       // Take care of round off error.
        string v = to_string(value);        // Convert the value to a string.
        size_t periodLocation = v.find_first_of('.');// Find the location of the first period.
        v = v.substr(0, periodLocation + 2);// Get the part of the value plus one decimal place.
        values += v;                        // Add the value to the end of the return string.
        values += ",";                      // Follow this value with a comma.
    }
    ios_base::iostate state = ttFile.rdstate();// Get the state of the stream.
    ttFile.close();                         // Close the tt file.
    if (state && ios_base::goodbit == false) {  // Did the reading succeed?
        strError = "Unable to read " + ttFileName + ".";// No. Create an error message.
        throw strError;                     // Throw the message to the caller.
    }                                       // The values were read and put in a string.
    return values;                          // Return the values to the caller.
}

string Manager::getChnlTvTValues(string argument) {
    string values = "";                     // Will contain all the temperature data for the given input.
    int inputDex = -1;                      // The default input index is invalid.
    int yearNo = -1;                        // The default year is invalid.
    int monthNo = -1;                       // The default month number is invalid.
    int minuteNo = -1;                      // The default minute of the moth number is invalid.
    int nDays = 31;                         // Most months have 31 days.
    int span = -1;                          // The default span in minutes is invalid.
    string strError = "";                   // Contains an error message.
    size_t seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the input index!";// No, it is impossible to determine the input.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string strInputDex = argument.substr(0, seperatorLocation);// Get the part of the argument containing the input's index.
        try {                               // Try to
            inputDex = stoi(strInputDex);   // convert the input index to a number.
        } catch (...) {                     // The given idex wasn't a number.
            strError = "Error: The input index is not a number!";// It is impossible to determine input's index.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The given input index is a number.
        if (inputDex < 0 || inputDex >= theInputs_.size()) {// Is the input index valid?
            strError = "Error: The input index is invalid!";// No, it is impossible to determine input's index.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The input index is valid.
        // Remove the input's index from the argument.
        argument = argument.substr(seperatorLocation + 1, argument.size() - strInputDex.size() - 1);
    }                                       // Next process the year.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the year!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string strYear = argument.substr(0, seperatorLocation);// Get the part of the argument containing the year.
        yearNo = stoi(strYear);             // Convert the year to a number.
        if (yearNo < 1980) {                // Is the year number valid?
            strError = "Error: The year must be greater than 1980!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The year number is probably valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - strYear.size() - 1);// Remove the year from the argument.
    }                                       // Next process the month.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the month!";// No, it is impossible to determine the date & time.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the month.
        monthNo = stoi(str);                // Convert the month to a number.
        if (monthNo < 1 || monthNo > 12) {  // Is the month number valid?
            strError = "Error: The month must be between 1 and 12!";// No, it is impossible to determine the date & time.
            throw strError;                 // Pass an error message to the caller.
        }                                   // The month number is probably valid.
        argument = argument.substr(seperatorLocation + 1, argument.size() - str.size() - 1);// Remove the month from the argument.
    }                                       // Next process the minute of the month.
    seperatorLocation = argument.find_first_of(',');// Find the location of the first comma.
    if (seperatorLocation == string::npos) {// Was there a comma seperator?
        strError = "Error: Couldn't find a \",\" after the month!";// No, it is impossible to determine the minute of the month.
        throw strError;                     // Pass an error message to the caller.
    } else {                                // There was a comma seperator.
        string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the minute of the month.
        minuteNo = stoi(str);               // Convert the minute of the month to a number.
        if (monthNo == 2 || monthNo == 4 || monthNo == 6 || monthNo == 9 || monthNo == 11 ) { // Does the month have less than 31 days?
            nDays = 30;                     // These months have no more than 30 days.
            if (monthNo == 2) {             // Is the month February?
                nDays = 28;                 // Yes. Most Februarys have 28 days.
                if (yearNo % 4) {           // Is this a leap year?
                    if (yearNo != 2100) {   // Is this the year 2100?
                        nDays = 29;         // This is a leay year that isn't 2100, therefore there are 29 days. This fails in 2200.
                    }
                }
            }                               // The number of days in the given month has been set.
            if (minuteNo < 0 || minuteNo > nDays * 1440) {// Is the minute of the month number valid?
                strError = "Error: The minute of the month is invalid!";// No, it is impossible to determine the minute of the month.
                throw strError;                 // Pass an error message to the caller.
            }                                   // The minute of the month is probably valid.
            // Remove the minute of the month from the argument.
        }
        argument = argument.substr(seperatorLocation + 1, argument.size() - str.size() - 1);
    }                                       // Next process the number of minutes to retrive temperatures over.
    string str = argument.substr(0, seperatorLocation);// Get the part of the argument containing the span in minutes.
    span = stoi(str);                       // Convert the span to a number of minutes.
    if (span < 1) {                         // Is the span in minutes valid?
        strError = "Error: The span must be greater than 0!";// No, it is impossible to return temperatures.
        throw strError;                     // Pass an error message to the caller.
    }                                       // The hour number is valid.
    float value = 0.0;                      // The default value.
    string ttFileName = path_;              // The path to the temperature data files begins with the path to the Plant Manger.
    ttFileName += temperatureDataPath_;     // The path then contains the sub-folder for temperature data files.
    ttFileName += to_string(yearNo) + to_string(monthNo) + ".tt";// Append the .tt temperature file name to the path.
    fstream ttFile(ttFileName, ios::binary | ios_base::in);// Open the .tt file.
    if (ttFile.is_open() == false) {        // Was the tt file opened?
        string error = "The file " + ttFileName + " couldn't be opened.";// No. Create a message.
        throw error;                        // Let the user know the file wasn't opened.
    }                                       // The tt file is open.
    ttFile.exceptions(ifstream::failbit | ifstream::eofbit | ifstream::badbit);// Activate exceptions.
    int deviation = minuteNo % storageInterval_;// The amount past a storage interval the minute of the month is.
    if ( deviation != 0) {                  // Is the minute of the month on a storage interval?
        minuteNo -= deviation;              // No. Decrement the minute of the month to the previous storage minute.
        span += deviation;                  // Increase the span to get the same end point.
    }                                       // The minute of the month is set to a storage minute.
    int offset = minuteNo;                  // The offset into the file is based on the minute of the month.
    offset = offset / storageInterval_ + 1; // Calculate where to read the temperature data.
    // (adding one corrects for the first record which is for file info)
    // Get a record's length for this particular .tt file.
    // From the record length, determine the number of inputs stored in each record.
    ttFile.seekg(0, ios_base::beg);         // Move the file pointer to the start of the file.
    short recordLength = 0;                 // Will contain the length of records in this file.
    ttFile.read((char*)&recordLength, 2);   // Read the length of records in this file.
    short nRecordInputs = recordLength / 2 - 1;// Calculate the number of inputs. (2 bytes for each input plus 2 bytes for the time)
    if (nRecordInputs < 6) {                // Are there less than six inputs?
        nRecordInputs = 6;                  // Yes. Even though there aren't 6 inputs, to maintain file format it is necessary each
    }                                       // record has at least six values.
    offset *= (nRecordInputs + 1) * 2;      // Adjust the offset by the number of inputs.
    // Each record contains a value(requiring 2 bytes) for each channel.
    // The start of each record is the minute of the month which requires 2 bytes.
    int chnlOffset = offset + (inputDex + 1) * 2;// Adjust the time offset to point to the input in that record.
    values = to_string(storageInterval_) + ",";// Put the storage interval first.
    values += to_string(minuteNo) + ",";    // Put the actual start minute of the month next.
    int minutesPast = 0;                    // So far, none of the tempeartures within the time span have been processed.
    while (minutesPast <= span) {           // Has the whole span been processed?
        unsigned short int nValue;          // No. nValue will contain the input's encoded value.
        unsigned short int tvtMinuteOfMonth;// The minute of the month as read from the tvt file.
        try {                               // Try to read this inputs temperature.
            ttFile.seekg(offset, ios_base::beg);// Move the file pointer to the time(minute of month) for this recording.
            ttFile.read((char*)&tvtMinuteOfMonth, 2);// Read the minute of the month when this recording happened.
            ttFile.seekg(chnlOffset, ios_base::beg);// Move the file pointer to the specified input's temperature for this recording.
            ttFile.read((char*)&nValue, 2); // Read the input's value.
        } catch (...) {                     // Was there a problem?
            if (ttFile./*ios_base::*/eof() == true) {// Was the end of the file been reached?
                values += "-502.0,";        // Yes. -502 indicates the end.
                ttFile.close();             // Close the tt file.
                return values;              // Return the temperatures that were retrieved.
            } else {                        // There was an error.
                values += "500.2,";         // Yes. 500.2 indicates an error.
            }
        }                                   // The encoded temperature value has been set.
        if(nValue >= 32768) {               // Is the encoded value negative?
            nValue = 32768 - nValue & 32767;// Yes. Don't know why, but need to AND 32768 to result.
            nValue = -nValue;               // Convert the number to a negative value.
        }                                   // The negative part of a number, if it exists, has been processed.
        value = (float)nValue / 10;         // Convert to a float value (temperature).
        value += .01;                       // Take care of round off error.
        if (tvtMinuteOfMonth < minuteNo + minutesPast || tvtMinuteOfMonth > minuteNo + span) {// Is the tvt minute of the month valid?
            value = -501.0;                 // No. Set the temperature to -501 to signal nothing was recorded.
        }                                   // If required, the value has been modified to indicate no tempearture was recorded.
        string v = to_string(value);        // Convert the value to a string.
        size_t periodLocation = v.find_first_of('.');// Find the location of the first period.
        v = v.substr(0, periodLocation + 2);// Get the part of the value plus one decimal place.
        values += v;                        // Add the value to the end of the return string.
        values += ",";                      // Follow this value with a comma.
        minutesPast += storageInterval_;    // Increment the amount of the span that has been evaluated.
        offset += recordLength;             // Adjust the offset to point to the minute of the month for the next record.
        chnlOffset += recordLength;         // Adjust the offset to point to the input's temperature for the next record.
    }                                       // The entire span has been evaluated.
    ttFile.close();                         // Close the tt file.
    return values;
}

bool Manager::isValidTemeprature(float temperature, float setpoint) {
    if (setpoint <= -500.0f) {      // Is the chnl off?
        return false;               // Yes. Invalid.
    }
    if (setpoint < theChnls_.getLowerTemperatureLimit()) {// Is the setpoint too cold?
        return false;               // Yes. Invalid.
    }
    if (setpoint > theChnls_.getUpperTemperatureLimit()) {// if the setpoint too hot?
        return false;               // Yes. Invalid.
    }
    // As of this line, the setPoint is valid and within limits.
    if (temperature < theChnls_.getLowerTemperatureLimit()) {// Is the temperature too cold?
        return false;               // Yes. Invalid.
    }
    if (temperature > theChnls_.getUpperTemperatureLimit()) {// Is the temperature too hot?
        return false;               // Yes. Invalid.
    }

    return true;
}

void Manager::startAlarmProcess(SCChnl* scChnl) {
    int sysNo = theInputs_.at(scChnl->getInputIndex())->getSystemNo();// Get this chnl's sub-system number.
    // Remember the system number is ones based. But, theSystems_ are zero based.
    switch (theSystems_.at(sysNo - 1)->getType()) {// Turn on the sub-system's alarm based on its type.
        case SCSystem::SYSTYPE_ONEWIRE : {  // ONE_WIRE is sub-system's type.
            // Go through each one wire sub-system.
            for_each(oneWireSystems_.begin(), oneWireSystems_.end(), [&] (OneWireEquip* owSys) {
                // Is this the one wire sub-system with same index as the input?
                if (owSys->getSubSystem().getSubSystemNo() == sysNo) {
                    owSys->shouldAlarmBeOn = true;// Yes. The alarm should be on.
                }// Done testing the sub-system index.
            });// Done going through the one wire sub-systems.
            break;          // Done setting the one wire alarm.
        }// Done with a one wire sub-system.
        case SCSystem::SYSTYPE_MINI : {    // Adlink or ADAM is this sub-system's type.
            // Go through each mini sub-system.
            for_each(miniSystems_.begin(), miniSystems_.end(), [&] (MiniEquip* miniSys) {
                // Is this the mini sub-system with same index as the input?
                if (miniSys->getSubSystem().getSubSystemNo() == sysNo) {
                    miniSys->shouldAlarmBeOn = true;// Yes. The alarm should be on.
                }// Done testing the sub-system index.
            });// Done going through the mini sub-systems.
            break;          // Done setting the mini's alarm.
        }// Done with a mini sub-system.
        default: {                          // This has to be some type of lawson sub-system.
            // Go through each lawson sub-system.
            for_each(lawsonSystems_.begin(), lawsonSystems_.end(), [&] (LawsonEquip* lawSys) {
                // Is this the lawson sub-system with same index as the input?
                if (lawSys->getSubSystem().getSubSystemNo() == sysNo) {
                    lawSys->shouldAlarmBeOn = true;// Yes. The alarm should be on.
                }// Done testing the sub-system index.
            });// Done going through the lawson sub-systems.
            break;          // Done setting the lawson alarm.
        }// Done with a default sub-system (should be standard/lawson or micro/lawson).
    }// Done switching for the sub-system type.
}

void Manager::adjustMimicList(SCChnl* scChnl) {
    if (scChnl->program.getControlType() != SCProgram::TYPE_MIMIC) {// Double check, is this chnl programmed as a mimic?
        // No. Create a message.
        errorMessage_ = "adjustMimicList() was called when a channel was not programmed as a mimic!!!";
        toLog(errorMessage_);                       // Display the error on the console.
        return;                                     // There is nothing to adjust on the mimic list.
    }                                               // The chnl was programmed as a mimic.
    int mimicRelayIndex = scChnl->getRelayIndex();  // Get the index of this chnl's relay.
    if (mimicRelayIndex == -1) {                    // Does this chnl have a relay?
        // No. Create a message.
        errorMessage_ = "adjustMimicList() was called for a channel which doesn't have a realy!!!";
        toLog(errorMessage_);                       // Display the error on the console.
        return;                                     // There is nothing to adjust on the mimic list.
    }                                               // The chnl has a relay.
    int indexToRefChnl = scChnl->program.getRefChnl();// Get the index of the chnl this program is referenced to.
    if (indexToRefChnl < 0 || indexToRefChnl >= theChnls_.size()) {// Is this a valid chnl index?
        // No. Create a message.
        errorMessage_ = "adjustMimicList() was called for a channel with a program which doesn't have a reference!!!";
        toLog(errorMessage_);                       // Display the error on the console.
        return;                                     // There is nothing to adjust in the mimic list.
    }                                               // The reference chnl index is valid.
    SCChnl* refChnl = theChnls_.at(indexToRefChnl); // Get access to the reference chnl.
    int refRelayIndex = refChnl->getRelayIndex();   // Get the reference's relay index.
    if (refRelayIndex < 0 || refRelayIndex >= theRelays_.size()) {// Does the reference have a valid relay?
        // No. Create a message.
        errorMessage_ = "adjustMimicList() was called for a reference channel with an invalid relay index!!!";
        toLog(errorMessage_);                       // Display the error on the console.
        return;                                     // There is nothing to adjust on the mimic list.
    }                                               // The reference has a valid relay.
    Relay* refRelay = theRelays_.at(refRelayIndex); // Get the reference's relay.
    if (scChnl->program.isActive == true) {         // Is the mimic chnl's program still active?
        bool isMimicOnList = false;                 // Yes. Check to see if this chnl's relay is already on the mimic list.
        for (int nDex = 0; nDex < refRelay->mimicList.size(); nDex++) {// Go through each relay on the mimic list.
            Relay* mimic = refRelay->mimicList.at(nDex);// Get this relay on the mimic list.
            if (mimic->indexNo == theRelays_.at(mimicRelayIndex)->indexNo) {// Is this relay the chnl's relay?
                isMimicOnList = true;               // Yes. The chnl's relay is already on the reference's mimic list.
                break;                              // Done looking.
            }// Done testing for the chnl's relay on the reference's mimic list.
        }// Done going through the mimic list.
        if (isMimicOnList == false) {               // Was the chnl's relay on the mimic list?
            refRelay->mimicList.push_back(theRelays_.at(mimicRelayIndex));// No, add it to the list.
        }// Done testing if the chnl's relay was missing from the reference's mimic list.
    } else {                                        // This SCChnl's program is no longer active.
        Relay* mimicRelay = theRelays_.at(mimicRelayIndex);// Get acces to the mimic relay.
        if (mimicRelay->isOn == true) {             // Is the mimic relay on right now?
            mimicRelay->requestToTurnOff();         // Yes. Start the process of turning it off.
        }// Done testing if the mimic realy is on.
        // Remove this chnl's relay from the reference relay's mimic list.
        refRelay->removeMimic(theRelays_.at(mimicRelayIndex));
    }// This SCChnl's relay has been added or removed from the reference relay's mimic list.
}

void Manager::dbReply(string result) {
    sqlite3 *db;                        // Create a location for the database.
    string sql;                         // Create a string for SQL commands.
    string tableName = "pm_replies";    // Create the table name for the plant manager replys.
    sqlite3_stmt *stmt;                 // Will contain the results of the querrys.
    string dbName = getFilePath() + "SCConfig.db";// Create the path with name to SCConfig.db.
    int rc = sqlite3_open(dbName.c_str(), &db);// Open the SCConfig database to store plant manager reply.
    if( rc ){                           // Was the database opened?
        string eMessage = "Can't open pm_replies in SCConfig.db: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);           // Display the error on the console.
        return;                         // Return to the caller.
    }                                   // The database is open.
    sqlite3_busy_timeout(db, 50);       // If the DB is locked wait up to 50 milliseconds before giving up.
    sql = "UPDATE " + tableName + " set reply = ?";// Set the plant manager's parametric reply in the reply column.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Send the command to the database.
    if (rc != SQLITE_OK) {              // Did the database's readings table accept the querry?
        errorMessage_ = "pm_replies table will not UPDATE the reply field: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);           // Display the error on the console.
        sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt".
        sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
        return;                         // Return to the caller.
    }                                   // The database pm_replies table accepted the querry.
    sqlite3_bind_text(stmt, 1, result.c_str(), (int)result.length(), 0);// Set the reply parameter.
    toLog("The result text is " + result);
    rc = sqlite3_step(stmt);            // Send the parameter to set the plant manager's reply.
    if (rc != SQLITE_ROW && rc != SQLITE_OK && rc != SQLITE_DONE) {// Was the reply set?
        errorMessage_ = "Can't UPDATE replies: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);           // Display the error on the console.
    }                                   // The plant manager's reply has been sent to the databaseis.
    sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
    sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
    return;                             // Done setting the reply.
}

void Manager::dbUpdateReadings() {
    dbUpdateRunTime();                      // Set the last time the PM was running.
    sqlite3 *db;                            // Create a location for the database.
    int  rc;                                // Create a return variable.
    string sql;                             // Create a string for SQL commands.
    char *zErrMsg = 0;                      // Create a location for an error message.
    string tableName = "readings";          // Create the table name for the chnl readings table.
    sqlite3_stmt *stmt;                     // Will contain the results of the querrys.
    string dbName = getFilePath() + "SCConfig.db";// Set the configuration database name.
    if (isReadingsTableConfigured == false) {// Has the readings table been checked?
    ///////////////////// Create the readings table in SCConfig.db/////////////////////////////////////////////////////////////////////
        toLog("Readings table is being configured!");
        rc = sqlite3_open(dbName.c_str(), &db);// No. Open the database named SCConfig.db.
        if( rc ){                           // Was the database opened?
            errorMessage_ = "Can't open SCConfig.db: " + string(sqlite3_errmsg(db));// No. create an error message.
            toLog(errorMessage_);           // Display the error on the console.
            return;                         // Return to the enless while loop. Perhaps the database will open next time.
        }                                   // The database is open.
        sqlite3_busy_timeout(db, 50);       // If the DB is locked wait up to 50 milliseconds before giving up.
        sql = "SELECT COUNT(*) from " + tableName + ";"; // Create an SQL statement to get the number of rows in the readings table.
        rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the querry to the database.
        if (rc != SQLITE_OK) {              // Was it successful?
            errorMessage_ = "Can't prepare channel count query for readings table: " + string(sqlite3_errmsg(db));// No. create an error message.
            toLog(errorMessage_);           // Display the error on the console.
            sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt".
            // In this case sqlite3_finalize will return an error because the previous statement caused a database error.
            sqlite3_close(db);              // Close the database. There is no need to check for errors because finalize was just called.
            return;                         // Return to the enless while loop. Perhaps the database will accept the count command next time.
        }                                   // The query to return the row count (number of channels) was accepted by the database.
        rc = sqlite3_step(stmt);            // Cause the database to return the first row. This contains the count.
        if (rc != SQLITE_OK && rc != SQLITE_ROW && rc != SQLITE_DONE) {// Was there an error getting this table's number of rows?
            // Yes. Create an error message.
            errorMessage_ = "Channel count query failed for readings table: " + string(sqlite3_errmsg(db));
            toLog(errorMessage_);           // Display the error on the console.
            sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt".
            // In this case sqlite3_finalize will return an error because the previous statement caused a database error.
            sqlite3_close(db);              // Close the database. There is no need to check for errors because finalize was just called.
            return;                         // Return to the enless while loop. Perhaps the database will accept the count command next time.
        }                                   // No errors.
        int chnlCount = sqlite3_column_int(stmt, 0);// Get the number of chnls. There is no error checking.
        rc = sqlite3_step(stmt);            // The database returns a second column when it is asked for the count.
        if (rc != SQLITE_DONE) {            // Was there an error?
            errorMessage_ = "Can't get the channel count from readings: " + string(sqlite3_errmsg(db));// Yes. create an error message.
            toLog(errorMessage_);           // Display the error on the console.
            sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt".
            sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
            return;                         // Return to the enless while loop. Perhaps the database will provide the count next time.
        }                                   // We have the count.
        sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
        sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
        if (chnlCount < theChnls_.size()) { // Are there enough rows to hold the readings from each channel?
            toLog("The readings table is missing channels.");
            ///////////////////// Open SCConfig.db to add missing channel rows for channel readings. /////////////////////////////////
            rc = sqlite3_open(dbName.c_str(), &db);// Open the database named SCConfig.db.
            if( rc ){                       // Was the database opened?
                errorMessage_ = "Can't open SCConfig.db: " + string(sqlite3_errmsg(db));// No. create an error message.
                toLog(errorMessage_);       // Display the error on the console.
                return;                     // Return to the enless while loop. Perhaps the database will open next time.
            }                               // The database is open.
            ///////////////////// Create an SQL statement to set the ones based channel id in the readings table.///////////////////
            sqlite3_busy_timeout(db, 50);       // If the DB is locked wait up to 50 milliseconds before giving up.
            sql = "INSERT INTO " + tableName + "(id) values (?1);";
            rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Send the sql commmand.
            if (rc != SQLITE_OK) {          // Was it successful?
                errorMessage_ = "SCConfig.db will not accept inserting channels into readings: ";//No.
                errorMessage_ += string(sqlite3_errmsg(db));// Create an error message.
                toLog(errorMessage_);       // Display the error on the console.
                sqlite3_finalize(stmt);     // Free any resources associated with the prepared statement "stmt".
                sqlite3_close(db);          // Close SCConfig.db. Since finalize was just called there should be no errors.
                return;                     // Return to the enless while loop. Perhaps the database will provide the count next time.
            }                               // The command to insert a row was accepted by the database.
            ///////////////////// Add a row for each missing channel in the readings table. /////////////////////////////////////////////
            for (int chnlDex = chnlCount; chnlDex < theChnls_.size(); chnlDex++) {// Add a row for each missing chnl.
                //////////////////Bind the channel index to the INSERT statement//////////////////////////////////////////////////////////
                if (sqlite3_bind_int(stmt, 1, chnlDex + 1) != SQLITE_OK) {// Cause the chnl index to be inserted. Did it work?
                    // No. Create an error message and discontinue talking to the SCConfig.db.
                    errorMessage_ = "Cannot bind the channel index to insert a channel into the readings table: ";
                    errorMessage_ += string(sqlite3_errmsg(db));// Add the database error to the message.
                    toLog(errorMessage_);   // Display the error on the console.
                    sqlite3_finalize(stmt); // Free any resources associated with the prepared statement "stmt".
                    sqlite3_close(db);      // Close SCConfig.db. Since finalize was just called there should be no errors.
                    return;                 // Return to the enless while loop. Perhaps the database will provide the count next time.
                }                           // Set the readings table first column to this chnl's id.
                //////////////////Implement the INSERT statement////////////////////////////////////////////////////////////////////////
                rc = sqlite3_step(stmt);    // Cause a row for this channel to be created in the readings table.
                if (rc != SQLITE_DONE) {    // Was there an error?
                    errorMessage_ = "Can't insert a channel into readings: " + string(sqlite3_errmsg(db));// Yes. create an error message.
                    toLog(errorMessage_);   // Display the error on the console.
                    sqlite3_finalize(stmt); // Free any resources associated with the prepared statement "stmt".
                    sqlite3_close(db);      // Close SCConfig.db. Since finalize was just called there should be no errors.
                    return;                 // Return to the enless while loop. Perhaps the database will create the row next time.
                }                           // The required row was created.
            }                               // The last required additional row has been created.
            //////////////////Free the resources allocated to the INSERT statement//////////////////////////////////////////////////////////
            sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt".
            //////////////////Close SCConfig.db and free its resources.////////////////////////////////////////////////////////////////////
            sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
        }                                   // The readings table contains at least enough rows to hold each chnl's readings.
        isReadingsTableConfigured = true;   // The readings table is configured to hold data from each chnl.
    } ///////////////////////////////////////////////////////////////////////    The readings table is configured correctly.
    rc = sqlite3_open(dbName.c_str(), &db); // Open the SCConfig database to store current chnl readings
    if( rc ){                               // Was the database opened?
        errorMessage_ = "Can't open SCConfig.db: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);               // Display the error on the console.
        return;                             // Return to the enless while loop. Perhaps the database will open next time.
    }                                       // The database is open.
    sqlite3_busy_timeout(db, 50);           // If the DB is locked wait up to 50 milliseconds before giving up.
    for (int nDex = 0; nDex < theChnls_.size(); nDex++) {// Go through each chnl to store its readings.
        SCChnl* theChnl = theChnls_.at(nDex);// Get the chnl whose readings are to be stored.
        int inputIndex = theChnl->getInputIndex();// Get the index of the input associated with this chnl.
        double temperature = theInputs_.at(inputIndex)->getMeasuredValue();// Get this chnl's temperature.
        stringstream stream;                // Create a string stream.
        stream << fixed << setprecision(1) << temperature;// Pass the chnl's temperature(F) with one decimal point to the stream.
        string valueF = stream.str();       // Convert the string stream to a string.
        stream.str("");                     // Clear the string stream.
        // Pass the chnl's temperature(C) with one decimal point to the stream.
        stream << fixed << setprecision(1) << (temperature - 32.0) * 5 / 9;
        string valueC = stream.str();       // Convert the string stream to a string.
        if (temperature >= 500.0) {         // Is the thermocouple up scale?
            valueF = "Open";                // Yes, set the F value to open.
            valueC = "Open";                // Set the C value to open.
            if(temperature == 502.0) {      // Does the temperature indicate the input is off-line?
                valueF = "Off-line";        // Yes, set the F value to Off-line.
                valueC = "Off-line";        // Set the C value to Off-line.
            } // End of condition: The input is off-line.
        } // End of condition: The input's temperature is up scale.
        float chnlSetPoint = Manager::presentSetPoint(theChnl);// Get the chnl's present setpoint.
        stream.str("");                     // Clear the string stream.
        stream << fixed << setprecision(1) << chnlSetPoint;// Pass the chnl's present setpoint(F) with one decimal point to the stream.
        string setPointF = stream.str();    // Convert the string stream to a string.
        stream.str("");                     // Clear the string stream.
        // Pass the chnl's present setpoint(C) with one decimal point to the stream.
        stream << fixed << setprecision(1) << (chnlSetPoint - 32.0) * 5 / 9;
        string setPointC = stream.str();    // Convert the string stream to a string.
        if(theChnl->program.presentSegmentNo() == 0 && theChnl->program.isActive == true) {// Has the program started?
            setPointF = "Set";              // No. Display Set for a setpoint(F).
            setPointC = "Set";              // Display Set for a setpoint(C).
        } else {                            // The program is not Set.
            if (chnlSetPoint == -500.0) {   // Does this chnl have a present setpoint?
                setPointF = "None";         // No. Display None for a setpoint(F).
                setPointC = "None";         // Display None for a setpoint(C).
            } else if (chnlSetPoint == -501.0) {// Does this chnl's reference have a bad sensor?
                setPointF = "Bad";          // No. Display None for a setpoint(F).
                setPointC = "Bad";          // Display None for a setpoint(C).
            }                               // The chnl's setpoint has been set.
            if(theChnl->program.getControlType() == SCProgram::TYPE_MIMIC && theChnl->program.isActive == true) {
                int refDex = theChnl->program.getRefChnl();// Get the reference chnl's index.
                SCChnl* refChnl = theChnls_.at(refDex);// Get the reference chnl.
                int relayIndex = refChnl->getRelayIndex();// Get the reference chnl's relay.
                if(relayIndex == -1) {      // Does this reference have a relay?
                    setPointF = "Bad";      // No. Display Bad for a setpoint(F) to indicate this mimic has nothing to imitate.
                    setPointC = "Bad";      // Display Bad for a setpoint(C) to indicate this mimic has nothing to imitate.
                } else {                    // This reference chnl has a relay.
                    Relay* refRelay = theRelays_.at(relayIndex);// Yes. Get the reference's relay.
                    if(refRelay->isOn == true) {// Is the reference's relay turned on?
                        setPointF = "On";   // Yes. Display On for a setpoint(F).
                        setPointC = "On";   // Display On for a setpoint(C).
                    } else {                // The reference's relay is turned off.
                        setPointF = "Off";  // No. Display Off for a setpoint(F).
                        setPointC = "Off";  // Display Off for a setpoint(C).
                    }                       // The setpoint for this mimic is set to either On or Off.
                } // End of this reference has a relay.
            } // End of this is an active mimic chnl.
        } // End of this chnl has an active program. The chnl's setpoint is ready for the readings table.
        string ctrlType = theChnl->program.getTypeString();// Get the chnl's control type.
        int refDex = theChnl->program.getRefChnl();// Get this chnl's reference chnl index.
        string refName = "n/a";             // By default, there is no refenece chnl.
        if( refDex < theChnls_.size()) {    // Is the reference index valid?
            refName = theChnls_.at(refDex)->getName();// Yes. Get the reference chnl's name.
        }                                   // The reference's name has been set.
        int relayDex = theChnl->getRelayIndex();// Get this channel's relay index.
        string timeOn = "n/a";              // Time on is not applicable for a channel without a relay.
        string relayState = "Off";          // The default relay state is off.
        if(relayDex >= 0) {                 // Does this chnl have a relay?
            timeOn = theRelays_.at(relayDex)->getTimeOnString();// Yes. Get the time on as a string.
            if (theRelays_.at(relayDex)->isOn == true) {// Is the relay currently on (activated)?
                relayState = "On";          // Yes. Set its state to on.
            }                               // The relay state has been set.
        }                                   // The time on has been set.
        double chnlMaturity = theChnl->maturity.getMaturityF();// Get this chnl's maturity.
        stream.str("");                     // Clear the string stream.
        stream << fixed << setprecision(1) << chnlMaturity;// Pass the chnl's maturity(F) with one decimal point to the stream.
        string maturityF = stream.str();    // Convert the string stream to a string.
        chnlMaturity = theChnl->maturity.getMaturityC();// Get this chnl's maturity in Celsius.
        stream.str("");                     // Clear the string stream.
        stream << fixed << setprecision(1) << chnlMaturity;// Pass the chnl's maturity(C) with one decimal point to the stream.
        string maturityC = stream.str();    // Convert the string stream to a string.
        float chnlDev = theChnl->deviation; // Get the allowable deviation before an alarm is triggered.
        stream.str("");                     // Clear the string stream.
        stream << fixed << setprecision(1) << chnlDev;// Pass the chnl's allowable deviation(F) with one decimal point to the stream.
        string deviationF = stream.str();   // Convert the string stream to a string.
        stream.str("");                     // Clear the string stream.
        stream << fixed << setprecision(1) << chnlDev * 5 / 9;// Pass the chnl's allowable deviation(C) with one decimal point to the stream.
        string deviationC = stream.str();   // Convert the string stream to a string.
        if (chnlDev < 0) {                  // Has the deviation alarm valu been set?
            deviationF = "none";            // No. There is no value
            deviationC = "none";            // in either F or C.
        }                                   // The deviation has been set.
        sql = "UPDATE " + tableName + " set valueF='" + valueF + "'";// Set this chnl's temperature in F.
        sql += ", valueC='" + valueC + "'";         // Set this chnl's temperature in C.
        sql += ", controlType='" + ctrlType + "'";  // Set this chnl's control type.
        sql += ", deviationC='" + deviationC + "'"; // Set this chnl's deviation alarm in C.
        sql += ", deviationF='" + deviationF + "'"; // Set this chnl's deviation alarm in F
        sql += ", maturityC='" + maturityC + "'";   // Set this chnl's maturity in C.
        sql += ", maturityF='" + maturityF + "'";   // Set this chnl's maturity in F.
        sql += ", reference='" + refName + "'";     // Set this chnl's reference.
        sql += ", relayState='" + relayState + "'"; // Set this chnl's relay state.
        sql += ", setPointC='" + setPointC + "'";   // Set this chnl's setpoint in C.
        sql += ", setPointF='" + setPointF + "'";   // Set this chnl's setpoint in F.
        sql += ", timeOn='" + timeOn + "'";         // Set this chnl's time on.
        sql += " where id=" + to_string(nDex + 1) + ";";// where the row is for this chnl's data.
        rc = sqlite3_exec(db, sql.c_str(), 0, 0, &zErrMsg);// Set this chnl's data in the DB table.
        if (rc == SQLITE_BUSY) {            // Was the database's readings table free to accept the querry?
            errorMessage_ = "Readings table was busy and did not accept the update command.";// No. Create an error message.
            toLog(errorMessage_);           // Display the error on the console.
            errorMessage_ = "SQL: " + sql;
            toLog(errorMessage_);
            errorMessage_ = "This channel's data will be updated next time.";
            toLog(errorMessage_);
            if(zErrMsg != NULL) {           // Was memory allocated for an error message?
                sqlite3_free(zErrMsg);      // Yes. Free the memory allocated for the error message.
            } // End of condition: Memory was allocated for an error message.
            this_thread::sleep_for(chrono::milliseconds(10));// Yes. Wait 10 milliSecs to allow the other application to finish.
            continue;                       // Go onto the next chnl.
        } // End of condition: The DB was busy.
        if (rc != SQLITE_OK) {              // Did the database's readings table accept the querry?
            errorMessage_ = "Readings table will not accept update command: " + string(zErrMsg);// No. create an error message.
            toLog(errorMessage_);           // Display the error on the console.
            errorMessage_ = "SQL: " + sql;
            toLog(errorMessage_);
            sqlite3_free(zErrMsg);          // Free the memory allocated for the error message.
            sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
            return;                         // Return to the enless while loop. Perhaps the database accept the querry next time.
        }                                   // The database readings table accepted the querry.
    }                                       // All the chnls current data has been updated in the readings table.
    sqlite3_close(db);                      // Close SCConfig.db. Since finalize was just called there should be no errors.
} // End of function dbUpdateReadings.

void Manager::dbUpdateRunTime()                  // Let any user know the PM is not running.
{
    sqlite3 *db;                        // Create a location for the database.
    string sql;                         // Create a string for SQL commands.
    string tableName = "pmSettings";    // Create the table name for the plant manager settings.
    sqlite3_stmt *stmt;                 // Will contain the results of the querrys.
    string dbName = getFilePath() + "SCConfig.db";// Create the path with name to SCConfig.db.
    int rc = sqlite3_open(dbName.c_str(), &db);// Open the SCConfig database to set the runTime field.
    if( rc ){                           // Was the database opened?
        errorMessage_ = "Can't open pmSettings in SCConfig.db: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);           // Display the error on the console.
        return;                         // Return to the caller.
    }                                   // The database is open.
    sqlite3_busy_timeout(db, 50);       // If the DB is locked wait up to 50 milliseconds before giving up.
    sql = "UPDATE " + tableName + " set runTime = ?";// Set the plant manager's parametric reply in the runTime column.
    sql += ", version = ?";             // and the version colume.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Send the command to the database.
    if (rc != SQLITE_OK) {              // Did the database's readings table accept the querry?
        errorMessage_ = "pm_replies table will not UPDATE the runTime field: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);           // Display the error on the console.
        sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt".
        sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
        return;                         // Return to the caller.
    }                                   // The database pm_replies table accepted the querry.
    string utcNow = utcTimeNowString(); // Get the current UTC time.
    rc = sqlite3_bind_text(stmt, 1, utcNow.c_str(), (int)utcNow.length(), 0);// Set the time the PM was running.
    if (rc != SQLITE_OK) {              // Did the database's readings table accept the querry?
        errorMessage_ = "The current time cannot be bound for the runTime field: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);           // Display the error on the console.
        sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt".
        sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
        return;                         // Return to the caller.
    }                                   // The current time was bound.
    rc = sqlite3_bind_text(stmt, 2, version_.c_str(), (int)version_.length(), 0);// Set the version number.
    if (rc != SQLITE_OK) {              // Did the database's readings table accept the querry?
        errorMessage_ = "The version cannot be bound for the runTime field: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);           // Display the error on the console.
        sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt".
        sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
        return;                         // Return to the caller.
    }                                   // The version was bound.
    rc = sqlite3_step(stmt);            // Send the parameter to set the plant manager's reply.
    if (rc != SQLITE_ROW && rc != SQLITE_OK && rc != SQLITE_DONE) {// Was the reply set?
        errorMessage_ = "Can't UPDATE runTime: " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);           // Display the error on the console.
    }                                   // The plant manager's reply has been sent to the databaseis.
    sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
    sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
    return;                             // Done setting runTime in the DB (the last time the PM was running).
}

void Manager::checkAlerts() {
    // Check for requests from Sure Cure applications.
    int number = -1;                    // Will contain the number associated with the error.
    string alert = dbReadAlert(&number);// Read the alerts table.
    if( alert.substr(0, 5) == "Error" ){// Was there an error reading the alerts table?
        // Yes. The error has already been inserted into the error log.
        return;                         // Return to the enless while loop. Perhaps the database will open next time.
    }                                   // The database is open.
    if (alert == "SetProgram") {        // Should a program be started or stopped?
        toLog("The program for scChnlDex " + to_string(number) + " is being read.");
        SCChnl* aChnl = theChnls_.at(number);// Access the channel whose program is to be started.
        bool result = aChnl->program.loadFromDB(aChnl->getIndex());// Load the program for the channel with the given index.
        if (result == true) {
            toLog("The program for scChnlDex " + to_string(number) + " was read.");
            if (aChnl->program.isActive == true) { // Is the progrma being activated?
                toLog("The program for channel " + aChnl->getName() + " is active.");
                aChnl->maturity.setMaturityF(0);// Yes. Set the maturity to zero. A new program is now running or going to start.
                aChnl->maturity.setMaturityC(0);// Set the maturity to zero. A new program is now running or going to start.
                int rDex = aChnl->getRelayIndex();// Get this chnl's relay index.
                if (rDex >= 0) {                // Does this chnl have a relay?
                    Relay* aRelay = theRelays_.at(rDex);// Yes. Get the relay.
                    aRelay->setTimeOn(0);       // Reset the time this relay has been on.
                }                               // The time on has been set.
            }                                   // The maturity and time on have been reset for programs being activated.
            clearAlert();                       // Clear the tables associated with alerts in the DB.
        } else {
            dbReply("PM_ERROR");
            errorMessage_ = "Can't load program for channel " + aChnl->getName();// No. create an error message.
            toLog(errorMessage_);               // Display the error on the console.
            return;                             // Return to the enless while loop. Perhaps the database will open next time.
        }
    } else if (alert == "RelayTest") {      // Should a relay be tested?
        relayTest(number);                  // Yes. Turn on the given relay.
        toLog("Relay " + to_string(number) + " is being activated.");
        clearAlert();                       // Clear the tables associated with alerts in the DB.
    } else if (alert == "RelayTestTerminate") {// Should a relay test be terminated?
        relayTestTerminate();               // Yes. Turn off all realys.
        toLog("Relay " + to_string(number) + " is being turned off.");
        clearAlert();                       // Clear the tables associated with alerts in the DB.
    } else if (alert == "isRunning") {      // Someone is asking if the Plant Manager is running.
        clearAlert();                       // Clear the tables associated with alerts in the DB.
        dbUpdateRunTime();                  // Update the run time.
    } else if (alert == "Stop") {           // Someone is calling to stop the Plant Manager.
        isToContinueRunning = false;        // This stops the Plant Manager.
        toLog("Plant Manager is being shut down.");
        clearAlert();                       // Clear the tables associated with alerts in the DB.
        dbUpdateRunTime();                  // Update the run time.
    } // End of condition "Stop" to shut down the Plant Manager.
} // End of funtion checkAlerts.

void Manager::clearAlert() {
    int number = 0;                     // Will contain the number associated with the alert.
    dbReply("PM_SUCCESS");              // Set the plant manger replies table to indicate the command was received.
    this_thread::sleep_for(chrono::milliseconds(500));// Wait 500 mSecs to allow the application to read the reply.
    int count = 0;                      // Set the time out counter to zero.
    while (count < 30) {                // Allow up to 30 counts.
        string alert = dbReadAlert(&number);// Read the alerts table.
        if( alert.substr(0, 5) == "Error" ){// Was there an error reading the alerts table?
            // Yes. The error has already been inserted into the error log.
            return;                     // Leave the error and don't set the PM replies table.
        }                               // The alert was read.
        if (alert == "PM_NONE") {       // Is the application done clearing the alert?
            dbReply("PM_NONE");         // Yes. Clear the plant manager reply.
            break;                      // The application is done.
        } // End of condition checking that the alert is cleared.
        this_thread::sleep_for(chrono::milliseconds(500));// Wait 500 mSecs to allow the application to respond to the reply.
        count++;                        // Increment the time out count.
    } // End of looping to wait for the user app to clear the alert.
} // End of function clearAlert.

void Manager::showRelaysOff() { // This causes the browser to show the relays as off.
    for (int nDex = 0; nDex < chnlsWithRelays_.size(); nDex++) {// Go through ever chnl with a relay.
        SCChnl* scChnl = chnlsWithRelays_.at(nDex);// Get access to this chnl's relay.
        Relay* relay = theRelays_.at(scChnl->getRelayIndex());// Get this chnl's relay.
        relay->incrementTimeOn();       // Update its time-on.
        relay->isWantingOn = false;     // Set the relay to want off.
        relay->isOn = false;            // Set the relay as off (it might not be yet, but it will be momentarily).
    } // End of loop to show each relay as off (so customers don't panic when they turn off the PM).
}

string Manager::dbReadAlert(int* number) {
    sqlite3 *db;                        // Create a location for the database.
    string sql;                         // Create a string for SQL commands.
    string tableName = "pm_alerts";     // Create the table name for the plant manager alerts.
    sqlite3_stmt *stmt;                 // Will contain the results of the query.
    string dbName = getFilePath() + "SCConfig.db";// Create the path plus name to SCConfig.db.
    int rc = sqlite3_open(dbName.c_str(), &db);// Open the SCConfig database to read a Sure Cure application alert.
    if( rc ){                           // Was the database opened?
        errorMessage_ = "Error: Can't open database in dbReadAlert, " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);           // Display the error on the console.
        return errorMessage_;           // Return to the caller.
    }                                   // The database is open.
    sqlite3_busy_timeout(db, 50);       // If the DB is locked wait up to 50 milliseconds before giving up.
    sql = "SELECT * from " + tableName + ";"; // Create an SQL statement to get application alerts.
    rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);// Apply the query to the database table.
    if (rc != SQLITE_OK) {              // Was the query accepted?
        errorMessage_ = "Error: Can't request application alerts from pm_alerts, " + string(sqlite3_errmsg(db));// No. create an error message.
        toLog(errorMessage_);           // Display the error on the console.
        sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt".
        sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
        return errorMessage_;           // Return to the caller.
    }                                   // The pm_alerts table accepted the query for application alerts.
    rc = sqlite3_step(stmt);            // Cause the database to return the first row. This contains the alrert.
    if (rc != SQLITE_ROW && rc != SQLITE_OK && rc != SQLITE_DONE) {// Was the alert read?
        errorMessage_ = "Error: Can't get the application alert: " + string(sqlite3_errmsg(db));// No. Create an error message.
        toLog(errorMessage_);           // Display the error on the console.
        sqlite3_finalize(stmt);         // Free any resources associated with the prepared statement "stmt".
        sqlite3_close(db);              // Close SCConfig.db. Since finalize was just called there should be no errors.
        return errorMessage_;           // Return to the caller.
    }                                   // The plant manager's reply has been sent to the databaseis.
    char buffer [50];                   // Create a buffer to hold the alert.
    int n = sprintf (buffer, "%s", sqlite3_column_text(stmt, 1));// Put the alert in the buffer.
    string alert(buffer, n);            // Convert the alert in the buffer to a string.
    *number = sqlite3_column_int(stmt, 2);// Get the number associated with this alert.
    sqlite3_finalize(stmt);             // Free any resources associated with the prepared statement "stmt".
    sqlite3_close(db);                  // Close SCConfig.db. Since finalize was just called there should be no errors.
    return alert;                       // Return the alert to the caller.
}

float Manager::presentSetPoint(SCChnl* scChnl) {
    float setpoint = scChnl->program.presentSetPoint();// Get the chnl's setpoint.
    if(setpoint == -500.0f) {           // Is the program running?
        return setpoint;                // No. Return -500 to indicate the program is done.
    }                                   // the program is running.
    float refTemp = 0;                  // Will contain the reference's temperature.
    switch (scChnl->program.getControlType()) {// The program's setpoint is modified based on the type of program.
        case SCProgram::TYPE_MASTER: {  // The program type is MASTER (also know as parent).
            break;                      // No modifications of the setpoint are required.
        } // TYPE_MASTER complete.
        case SCProgram::TYPE_SLAVE: {   // The program type is SLAVE (also know as child).
            unsigned int refChnlDex = scChnl->program.getRefChnl();// Get the slave's reference.
            SCChnl* refChnl = theChnls_.at(refChnlDex);// Get access to the reference chnl.
            int inputDex = refChnl->getInputIndex();// Get this chnl's input index.
            if (inputDex >= 0) {                // Does this chnl have an input?
                refTemp = (float)theInputs_.at(inputDex)->getMeasuredValue();// Yes. Get its temperature.
                if(refTemp >= 500.0f) { // Does the reference have an open thermocouple?
                    setpoint = -501.0f; // Yes. A slave which is referenced to a chnl with an open sensor no setpoint.
                } else {                // The reference's temperature is valid.
                    setpoint += refTemp;// The program setpoint was the amount to offset the reference's temperature.
                }                       // The setpoint has been adjusted for the reference's temperature.
            } else {                    // There is no input.
                setpoint = -501.0f;     // A slave which is not referenced to a chnl with an input has no setpoint.
            }                           //
            break;                      // The slave's setpoint has been adjusted using the reference's temperature.
        } // TYPE_SLAVE complete.
        case SCProgram::TYPE_MIMIC: {   // The program type is MIMIC.
            break;                      // No modifications of the setpoint are required.
        } // TYPE_MIMIC complete.
        case SCProgram::TYPE_CUTOFF: {  // The program type is CUTOFF.
            // This channel follows a programmed curve until its reference exceeds the cutoff temperature.
            // Once the reference exceeds the cutoff, the channels follows a maintenance temperature.
            unsigned int refChnlDex = scChnl->program.getRefChnl();// Get the cutoff's reference.
            SCChnl* refChnl = theChnls_.at(refChnlDex);// Get access to the reference chnl.
            int inputDex = refChnl->getInputIndex();// Get this chnl's input index.
            if (inputDex >= 0) {        // Does the reference chnl have an input?
                refTemp = (float)theInputs_.at(inputDex)->getMeasuredValue();// Yes. Get its temperature.
                if(refTemp >= 500.0f) { // Does the reference have an open thermocouple?
                    break;              // Yes. Just use the chnls program to calculate a setpoint.
                }                       //
            } else {                    // There is no input.
                setpoint = -500.0f;     // A cutoff which is not referenced to a chnl with an input has no setpoint.
                break;                  // A low value has been assigned to the setpoint (cause whatever to turn off).
            }                           //
            if(refTemp > scChnl->program.getCutoffTemperature()) {// Has the reference's temperature exceeded the cutoff setting?
                setpoint = scChnl->program.getMaintenanceTemperature();// Yes. Change the setpoint to the maintenance temperature.
            }                           //
            break;                      // The cutoff's setpoint has been adjusted based on the reference's temperature.
        } // TYPE_CUTOFF complete.
        default: {                      // This should never happen.
            setpoint = -500.0;          // Set an invalid low setpoint.
            break;                      //
        }                               //
    }                                   // All control types have had their setpoint's calculated.
    return setpoint;                    // Return this chnl's temperature.
}

bool Manager::connectToSocket(ServerSocket server, Socket& miniSocket) {
    if (SCEquipment::isInArtificialMode == true) {// Is the PM in artificial mode?
        return true;                            // Yes. There is nothing wrong. The socket should not be opened.
    }                                           // The plant manager is in real mode.
    try {                                       // Try to connect to the NetCOM device acting as a client for a sub-system.
        int result = 0;                         // Contains the number of connections obtained from polling the socket.
        int connectCount = 0;                   // Contains the number of times a request has been looked for.
        while (connectCount < 13) {             // Is the connection process supposed to continue?
            result = server.poll(50);           // Yes. Wait up to 50 milliseconds for a connection request. Most of the time, the
                                                // request will be waiting from the previous 5 second pause.
            toLog("The poll returned: " + to_string(result) + "-" + to_string(connectCount++));
            if (result > 0) {                   // Are there any connection requests?
                break;                          // Yes. There is a connection. Done trying to connect.
            }                                   // There isn't a connection yet.
            this_thread::sleep_for(chrono::milliseconds(5000));// Wait five seconds to check for a connection again.
            // The reason to wait five seconds is so that the network will not be flooded with polling.
        }                                       // Either a connection was made or the Plant Manager is being shut down.
        if (connectCount == 13) {               // Is this sub-system supposed to keep running?
            toLog("Exiting the Manager::connectToSocket without a connection.");// No.
            server.closeServerSocket();         // Close the server socket.
            return false;                       // Let the caller know the socket didn't connect to the NetCom device.
        } // End of condition to see if there was a connection. The NetCOM connected.
// Since there is more than one miniSystem, it is necessary to distinguish between miniSystems.
// miniSystems are distiguish by their IP address and the server name which Products Engineering configures
// into each serial device (A serial device provides network communications to a mini sub-system).
// On 2020-06-10 a miniSystem is connected to a NetCom Ethernet to serial converter.
// To connect a miniSystem to the Plant Manager, the Plant Manager waits to receive a request
// to connect from the miniSystem. Once that signal is received, the Plant Manager has to determine which
// NetCom/miniSystem sent the signal. This is done in other parts of the program. This function is only resposible
// for creating a connection from a request by a serial device. When the accept function returns, its socket
// contains the miniSystem's (NetCom's) IP address.
        server.accept(miniSocket);              // Accept an incoming connection request from NetCOM serial client.
        toLog("The miniSocket handle is " + to_string(miniSocket.getdescriptor()));
        toLog("The miniSocket address is " + to_string(miniSocket.m_addr.sin_addr.s_addr));
    } // End of the try block for a socket requesting a connection.
    catch ( SocketException& e ) {              // Were there any errors?
        errorMessage_ = "ServerSocket connection exception: ";// There was an error. Create an error message.
        errorMessage_ += e.description();       // Add the exception descriotion.
        toLog(errorMessage_);                   // Display the error on the console.
        server.closeServerSocket();             // Close the server socket.
        return false;                           // The connection failed.
    } // End of the catch block for trying to make a connection.
    return true;                                // A connection was completed.
} // End of function connectToSocket.

string Manager::convertIntToIPString(unsigned long add) {
    unsigned long w[4];                     // Will contain the four words that make up the IP address.
    w[0] = add % 256;                       // The first word is the lower order two bytes.
    unsigned long shiftedAdd = add;         // Store the IP long after the low order word is removed.
    string ipStr = to_string(w[0]);         // Start the IP address string.
    for(int dex = 1; dex < 4; dex++) {      // Loop through to create the three other members (words) of the IP address string.
        w[dex] = shiftedAdd - w[dex - 1];   // Remove the previous word.
        w[dex] = w[dex] / 256;              // Shift the address by 8 bits.
        shiftedAdd = w[dex];                // Store the new long IP address once the previos word has been removed.
        w[dex] = w[dex] % 256;              // Get the low order 8 bits for the next word.
        ipStr += ".";                       // Add a period to seperate the previous word from the word just obtained.
        ipStr += to_string(w[dex]);         // Add the word just obtained.
    } // End of the loop to create the last three words in the IP address string.
    return ipStr;                           // Return the ip Address string to the caller.
}

int Manager::serverNameToSubSystemDex(string name){
    for (int sysDex = 0; sysDex < theSystems_.size(); sysDex++) { // Go through each sub-system.
        SCSystem* scSys = theSystems_.at(sysDex);// Get access to the sub-system.
        string portName = scSys->getPortName();// Get the sub-system's portName which is also used to store its server name.
        name = name.substr(0, portName.length());// Only compare the number of characters found in the portName.
        if(portName == name) {              // Does this sub-system's portName (serverName) match the given name?
            return sysDex;                  // Yes. Return the sub-system's index to the caller.
        } // End of the condition to check for a name match.
    } // End of the loop to go through each sub-system.
    return -1;                              // There was no sub-system found with the given server name.
}
                                        
bool Manager::miniSystemRequestingShutDown() {
    bool shutDown = false;              // By default, the mini sub-systems are not requesting a shut down.
    if (miniSystems_.size() > 0) {      // Are there any mini sub-systems?
        for_each(miniSystems_.begin(), miniSystems_.end(), [&] (MiniEquip* miniSys) {// Yes. Go through each one.
            if(miniSys->isOperationAllowed == false) {// If isOperationAllowed is false, then a shut down should occur.
                shutDown = true;        // A shut down should happen.
            } // End of condition checking for shut down.
        }); // End of the loop to shut down all the mini sub-systems.
    } // End of condition checking if mini sub-systems exist.
    return shutDown;                    // Return the desired shut down condition.
}

string Manager::getSerialServerName(string ipAddress) {
    FILE *fp;                               // A file pointer used to open a pipe stream to get the server name.
    int status;                             // A flag to determine if the pipe stream closed correctly.
    char buff[512];                         // The server name sent by the NetCom device.
    string command = "snmpwalk -c public -v1 ";// The start of the unix system command to get the server name.
    command += ipAddress;                   // The IP address already provided by the NetCom device.
    command += " SNMPv2-MIB::sysName.0";    // The end of the command to get the server name.
    fp = popen(command.c_str(), "r");       // Open the pipe stream and issue the command to get the server name.
    if (fp == NULL) {                       // Did the pipe stream fail to open?
        errorMessage_ = "Error issuing the command in Manager::getSerialServerName()!";// Failure. Create an error.
        toLog(errorMessage_);               // Display the error on the console.
        return "";                          // Returning an empty string indicates failure.
    } // End of condition testing for an open pipe.
    if(fgets(buff, 512, fp) == NULL) {      // Was the server name obtained from the pipe stream?
        errorMessage_ = "Error receiving the response in Manager::getSerialServerName()!";// No. Create an error.
        toLog(errorMessage_);               // Display the error on the console.
        pclose(fp);                         // Close the pipe stream.
        return "";                          // Returning an empty string indicates failure.
    } // End of the condition testing server name retrieval.
    status = pclose(fp);                    // Close the pipe stream.
    if (status == -1) {                     // Did the pipe stream close successfully?
        errorMessage_ = "Error closing the pipe in Manager::getSerialServerName()!";// No. Create an error.
        toLog(errorMessage_);               // Display the error on the console.
        return "";                          // Returning an empty string indicates failure.
    } // End of the condition testing the pipe closure.
    string reply(buff);                     // Convert the charachters received from the pipe to a string.
    size_t found = reply.find_last_of(":"); // Find the last : in the string.
    if (found == string::npos) {            // Was a : found?
        errorMessage_ = "The server name was invalid in Manager::getSerialServerName()!";// No. Create an error.
        toLog(errorMessage_);               // Display the error on the console.
        return "";                          // Returning an empty string indicates failure.
    } // End of the condition testing the pipe closure.
    string serverName = reply.substr(found + 2);// Parse the server name from the pipe reply.
    return serverName;                      // Return the server name to the caller.
} // End of function getSerialServerName().

int Manager::getSysDexFromServerName(string serverName) {
    for(int sysDex = 0; sysDex < theSystems_.size(); sysDex++) {// Go through each sub-system.
        toLog("Checking if sub-system " + to_string(sysDex) + " is associated with the server name.");
        SCSystem* aSys = theSystems_.at(sysDex);// Get access to this sub-system.
        string portName = aSys->getPortName();// Get the port name (the server name is stored as the port name in the DB).
        string name = serverName.substr(0, portName.length());// Only compare the number of characters found in the portName.
        // The serverName my have extra characters like line feeds on the end of it.
        toLog("The server name is " + name + "!!!!");
        toLog("The port name is " + portName + "!!!!");
        if(portName == name) {              // Does this sub-system match this server name?
            return sysDex;                  // Yes. Provide the call with the index of the matching sub-system.
        } // End of conditional test matching the sub-system port name with the given server name.
    } // End of the loop to go through all the sub-systems.
    return -1;                              // Returning -1 indicates no sub-system is associated with this server name.
}

void Manager::restoreMinis() {
    if (SCEquipment::isInArtificialMode == true) {// Is the plant manager in artificial mode?
        return;                                 // Yes. This mini should not be reconnected.
    } // End of condition: The PM is in artificial mode.
    int noSubSysToReconnect = 0;                // There are no sub-systems to reconnect until they have been evaluated.
    for(int miniDex = 0; miniDex < miniSystems_.size(); miniDex++) {// Go through each mini sub-system.
        MiniEquip* miniSys = miniSystems_.at(miniDex);// Access a mini sub-system.
        if(miniSys->isRequestedToSignOn == true) {// Is this mini sub-system requesting to reconnect?
            time_t tTime = chrono::system_clock::to_time_t(miniSys->getTimeInitialized() + chrono::minutes(2));
            string ts = ctime(&tTime);
            ts.resize(ts.size() - 1);
            toLog("Reinitialization will be attempted after " + ts);
            if (chrono::system_clock::now() > miniSys->getTimeInitialized() + chrono::minutes(2)) {
                // Yes. Has it been at least 2 minutes since the last time initialization took place?
                noSubSysToReconnect++;          // Increment the number of mini sub-systems to reconnect.
            } // End of condition checking if this mini sub-system should be allowed to reconnect.
        } // End of condition checking if this mini sub-system is requesting to reconnect.
    } // Done looping through each mini sub-system.
    if(noSubSysToReconnect > 0) {
        toLog(to_string(noSubSysToReconnect) + " mini(s) need to be reconnected.");
    }
    if(noSubSysToReconnect > 0) {
        try {                                   // Protect the server constructor with a try block.
            ServerSocket pmServer(socketPort_); // Create the server socket.
            Socket* aSocket = new Socket();                     // Create a communication socket.
            toLog("Sockets created in restoreMinis.");
            if(connectToSocket(pmServer, *aSocket) == false) {  // Was a connection established?
                // No. Create error message.
                errorMessage_ = "Failed to detect any mini clients in restoreMinis.";
                toLog(errorMessage_);           // Display the error on the console.
//                continue;                       // Try again to connect to another serial device if there is an approved one.
// Added the next three lines to take the place of continue.
                pmServer.closeServerSocket();       // Done with the server.
                delete aSocket;
                return;
            } else {                            // A connection was created.
                toLog("The Plant Manager Server descriptor is " + to_string(pmServer.getDescriptor()) + ".");
                unsigned long add = aSocket->m_addr.sin_addr.s_addr;// Get the integer form of the IP address.
                string ipAdd = convertIntToIPString(add);// Convert the integer IP address to a string IP address.
                string serverName = getSerialServerName(ipAdd);// Get the server name from the serial device.
                toLog("The new socket's handle is " + to_string(aSocket->getdescriptor()) + ".");
                toLog("The new socket's int address is " + to_string(add) + ".");
                toLog("The new socket's string address is " + ipAdd + ".");
                toLog("The new socket's server name is " + serverName + ".");
                int aSysDex = getSysDexFromServerName(serverName);// Get the sub-system index with this server name.
                // Access the mini sub-system with this sub-system index.
                MiniEquip* miniSys = nullptr;   // Create a variable for a mini sub-system.
                if(aSysDex >= 0) {              // Was there a matching sub-system?
                    int miniDex = 0;            // Yes. We need to go through each mini sub-system and check for a matching system wide
                                                // sub-system index (the system wide index may very well be different than the mini index).
                    for(; miniDex < miniSystems_.size(); miniDex++) {// Go through each mini sub-system.
                        miniSys = miniSystems_.at(miniDex);// Retrieve one of the mini sub-system(s).
                        int miniSubSysDex = miniSys->getSubSystem().getSubSystemNo() - 1;// An index is required, not a sub-system number.
                        if(aSysDex == miniSubSysDex) {// Does the sub-system index match this mini's sub-system index?
                            break;              // Yes. This miniSys points to the mini which is associated with this connection.
                        } // End of condition checking for a matching sub-system index.
                    } // End of looping through all the mini sub-systems.
                    if(miniDex >= miniSystems_.size()) {// Was a matching sub-system found?
//                        continue;               // No. Go on and try the next approved connection if there is one..
// Added the next four lines to take the place of continue.
                        pmServer.closeServerSocket();       // Done with the server.
                        aSocket->closeSocket();
                        delete aSocket;
                        return;
                    } // End of condition checking if a matching sub-system was found.
                } else {                        // There wasn't a serial device with the acquired server name.
/* aSocket must exist because connectToSocket returned true. The next lines are not needed.
                    if(aSocket != nullptr) {    // Was there som kind of connection request from something unknown or mis-configured?
                        aSocket->closeSocket(); // Yes. Close the communication socket that was acquired.
                        delete aSocket;         // Free the resources allocated to this socket.
                    } // End of condition checking for an invalid connection request.
*/
                    errorMessage_ = "There wasn't a sub-system with the server name " + serverName;// Create an error
                    errorMessage_ += " in restoreMinis!";// message.
                    toLog(errorMessage_);               // Display the error on the console.
                    // Added the next four lines to take the place of continue.
                    pmServer.closeServerSocket();       // Done with the server.
                    aSocket->closeSocket();
                    delete aSocket;
                    return;
                } // End of the condition to check for a connection to a serial device.
                toLog("Assigning the new connection to mini" + to_string(miniSys->getSubSystem().getSubSystemNo()) + ".");
                miniSys->miniSocket = aSocket;  // Associate the acquired connection to this mini sub-system.
                miniSys->connectedIPAddress = ipAdd;// Remember the ip address returned by the connecion.
                miniSys->resetCommunicationsOnAllModules();// Prevent previous failure from causing this sub-system from reconnecting again.
                miniSys->isRequestedToSignOn = false;// This mini sub-system has been reconnected.
                toLog("isRequestedToSignOn was just set to false.");
                miniSys->setTimeInitialized();  // Set the initialization time to now.
            } // End of the condition to check the creation of a connection to a mini sub-system.
            pmServer.closeServerSocket();       // Done with the server.
            toLog("End of connection loop in restoreMinis.");
        } // End of the try block to protect the constructor of the server.
        catch ( SocketException& e ) {          // Were there any errors?
            // Yes. This is a major error. If the construction of the server fails, there is no way to connect to
            errorMessage_ = "Error connecting to mini sub-system serial device in restoreMinis. Error: ";// a mini sub-system.
            errorMessage_ += e.description();   // Create an error message.
            toLog(errorMessage_);               // Display the error on the console.
        } // End of catch protecting the server constructor.
    } // Done looping through each approved connection.
} // End of function restoreMinis.

void Manager::toLog(string note) {
//    chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// Get the present time.
//    time_t tt = chrono::system_clock::to_time_t(now);// Convert the time to time_t format.
//    string time = ctime(&tt);                   // Convert the time from time_t format to string format.
//    time = time.substr(0, time.length() - 1);   // The conversion leaves a \n at the end of the string. Remove the \n.
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

