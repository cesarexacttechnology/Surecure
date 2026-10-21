//
//  Beds.cpp
//  PlantManager
//
//  Created by Mark Davis on 3/13/17.
//  Copyright © 2017 Products Engineering. All rights reserved.
//

#include "Beds.hpp"

namespace SCClasses {
    
SCBedChnl::SCBedChnl() {
    chnlDex_ = -1;                              // The SCChnl index associated with this bedChnl has not been set.
    refDex_ = -1;                               // There is no reference associated with this bedChnl by default.
    assignedChnlDex_ = -1;                      // There is no assigned chnl associated with this bedChnl by default.
    assignedRefDex_ = -1;                       // There is no assigned reference associated with this bedChnl by default.
    isAssigned = false;                         // By default an SCChnl is not assigned at runtime.
    controlType = SCProgram::TYPE_MASTER;       // the default way of controlling a bed chnl is as a master.
    errorMessage = "";                          // there hasn't been an error yet.
}

SCBedChnl::~SCBedChnl() {
        
}

int SCBedChnl::getSCChnlDex() {
    return chnlDex_;                            // Provide the caller with the SCChnl index associated with this bedChnl.
}
    
void SCBedChnl::setSCChnlDex(int scChnlDex) {
    if (scChnlDex > -3) {                       // Is the desired SCChnl index greater than -3(is it legal)?
        chnlDex_ = scChnlDex;                   // Yes. Set the SCChnl index associated with this bedChnl.
    } else {                                    // No. Illegal SCChnl index.
        chnlDex_ = -1;                          // Set the SCChnl index to not assigned.
    }
}
    
int SCBedChnl::getRefSCChnlDex() {
    return refDex_;                             // Provide the caller with the bedChnl index this bedChnl is referenced to.
}
    
void SCBedChnl::setRefSCChnlDex(int scChnlDex) {
    if (scChnlDex > -2) {                       // Is the desired SCChnl index greater than -2(is it legal)?
        refDex_ = scChnlDex;                    // Yes. Set the SCChnl index associated with this bedChnl's reference.
    }
}
    
int SCBedChnl::getAssignedSCChnlDex() {
    return assignedChnlDex_;                    // Provide the assigned SCChnl index for this bedChnl.
}

void SCBedChnl::setAssignedSCChnlDex(int scChnlDex) {
    if (scChnlDex > -2) {                       // Is the desired SCChnl index greater than -2(is it legal)?
        assignedChnlDex_ = scChnlDex;           // Yes. Set the SCChnl index assigned to the bedChnl at runtime.
    }
}
    
int SCBedChnl::getAssignedRefDex() {
    return assignedRefDex_;                     // Provide the channel on the bed this channel will be referenced to.
}

void SCBedChnl::setAssignedRefDex(int bedChnlDex) {
    if (bedChnlDex > -2) {                      // Is the desired SCBedChnl index greater than -2(is it legal)?
        assignedRefDex_ = bedChnlDex;           // Yes. Set the SCBedChnl index this bedChnl will reference.
    }
}
    
int SCBedChnl::getBedChnlIndex() {
    return index_;                              // Provide the caller with this bed chnl's index into the bedChnl list.
}
    
void SCBedChnl::setIndex(int nDex) {
    index_ = nDex;                              // Set the desired index. Don't worry about it being legal.
}                                               // It is only finalized when this bedChnl is added to the bedChnl list.

bool SCBedChnl::load(XMLNode* xmlBedChnl) {
    XMLNode* xmlData = xmlBedChnl->findSubNode("SCChnlDex");// Get the node containing this bedChnl's SCChnl index.
    string bedChnl = to_string(getBedChnlIndex());// Create a string version of this bedChnl's index for error messages.
    if (xmlData == nullptr) {                   // Was the node retrieved?
        errorMessage = "The SCChnl index for bed Chnl " + bedChnl + " was not found.";// No. Create error.
        return false;                           // Let the caller know there was an error.
    }                                           // The SCChnl index exists.
    chnlDex_ = stoi(xmlData->data.c_str());     // Convert the data string to an integer containing the SCChnl index.
    xmlData = xmlBedChnl->findSubNode("SCRefDex");// Get the node containing this bedChnl's SCChnl ref index.
    if (xmlData == nullptr) {                   // Was the node retrieved?
        errorMessage = "The SCChnl reference index for bed Chnl " + bedChnl + " was not found.";// No. Create error.
        return false;                           // Let the caller know there was an error.
    }                                           // The SC reference index exists.
    refDex_ = stoi(xmlData->data.c_str());      // Convert the data string to an integer for this bedChnl's reference index.
    xmlData = xmlBedChnl->findSubNode("IsAssigned");// Get the node containing this bedChnl's assignment flag.
    if (xmlData == nullptr) {                   // Was the node retrieved?
        errorMessage = "The assignment flag for bed Chnl " + bedChnl + " was not found.";// No. Create error.
        return false;                           // Let the caller know there was an error.
    }                                           // The assignment flag exists.
    isAssigned = false;                         // By default, this bedChnl is not assigned at runtime.
    if (xmlData->data == "1") {                 // Is the flag set to true?
        isAssigned = true;                      // Yes. This bedChnl will be assigned at runtime.
    }                                           // The Assigned flag has been set.
    xmlData = xmlBedChnl->findSubNode("AssignedChnlDex");// Get the node containing this bedChnl's assigned SCChnl index.
    if (xmlData == nullptr) {                   // Was the node retrieved?
        errorMessage = "The assigned SCChnl index for bed Chnl " + bedChnl + " was not found.";// No. Create error.
        return false;                           // Let the caller know there was an error.
    }                                           // The assigned SCChnl index exists.
    // Convert the data string to an integer for this bedChnl's runtime
    assignedChnlDex_ = stoi(xmlData->data.c_str());// assigned SCChnl index.
    // Get the node containing this bedChnl's runtime assigned
    xmlData = xmlBedChnl->findSubNode("AssignedRefChnlDex");// SCChnl reference index.
    if (xmlData == nullptr) {                   // Was the node retrieved?
        errorMessage = "The assigned SCChnl reference index for bed Chnl " + bedChnl + " was not found.";// No. Create error.
        return false;                           // Let the caller know there was an error.
    }                                           // The assigned SCChnl reference index exists.
    // Convert the data string to an integer for this bedChnl's reference chnl which is an SCBedChnl index
    assignedRefDex_ = stoi(xmlData->data.c_str());// to another channel on this chnl's bed.
    xmlData = xmlBedChnl->findSubNode("ControlType");// Get the node containing this bedChnl's control type.
    if (xmlData == nullptr) {                   // Was the node retrieved?
        errorMessage = "The control type for bed Chnl " + bedChnl + " was not found.";// No. Create error.
        return false;                           // Let the caller know there was an error.
    }                                           // The control type exists.
    controlType = (SCProgram::Type)stol(xmlData->data.c_str());// Convert the data string to this bedChnl's control type.
    return true;                                // Success.
}
    
bool SCBedChnl::save(XMLNode* xmlBedChnl) {
    // Create a node containing this bed chnl's SCChnl index.
    XMLNode* xmlChnlDex = new XMLNode(xmlBedChnl, "SCChnlDex", to_string(chnlDex_));
    xmlBedChnl->subNodes.push_back(xmlChnlDex); // Add this node to the array of subnodes.
    // Create a node containing this bed chnl's SCChnl reference index.
    XMLNode* xmlRefDex = new XMLNode(xmlBedChnl, "SCRefDex", to_string(refDex_));
    xmlBedChnl->subNodes.push_back(xmlRefDex);  // Add this node to the array of subnodes.
    string assigned = "0";                      // By default the SCChnl is not assigned at runtime.
    if (isAssigned == true) {                   // Is this bed channel supposed to be assigned at runtime?
        assigned = "1";                         // Yes. Set the assigned tag to true.
    }                                           // The assigned tag is set.
    // Create a node containing whether this bed chnl's SCChnl index is assigned at runtime.
    XMLNode* xmlAssigned = new XMLNode(xmlBedChnl, "IsAssigned", assigned);
    xmlBedChnl->subNodes.push_back(xmlAssigned);// Add this node to the array of subnodes.
    // Create a node containing this bed chnl's assigned SCChnl index(if any).
    XMLNode* xmlAssDex = new XMLNode(xmlBedChnl, "AssignedChnlDex", to_string(assignedChnlDex_));
    xmlBedChnl->subNodes.push_back(xmlAssDex);  // Add this node to the array of subnodes.
    // Create a node containing the SCBedChnl index this bed chnl references(if any).
    XMLNode* xmlAssRefDex = new XMLNode(xmlBedChnl, "AssignedRefChnlDex", to_string(assignedRefDex_));
    xmlBedChnl->subNodes.push_back(xmlAssRefDex);// Add this node to the array of subnodes.
    // Create a node containing this bed chnl's control type.
    XMLNode* xmlCtrlTypeDex = new XMLNode(xmlBedChnl, "ControlType", to_string(controlType));
    xmlBedChnl->subNodes.push_back(xmlCtrlTypeDex);// Add this node to the array of subnodes.
    return true;
    // All the memory allocated to the subNodes of xmlBedChnl is freed when it goes out of scope. Its detructor is called.
}
    
#pragma mark - Bed class implementation
/////////////////////////////////Bed//////////////////////////////////////////////////////////////
    
SCBed::SCBed() {
    name_ = "none";                             // This new bed does not have a name yet.
    index_ = -1;                                // The bed doesn't have an index until it is added to the bed list.
    errorMessage = "";                          // There hasn't been an error yet.
}
    
SCBed::SCBed(const SCBed& other) {
    name_ = other.name_;
    index_ = other.index_;
    for (int chnlDex = 0; chnlDex < other.bedChnls_.size(); chnlDex++) {
        SCBedChnl* aBedChnl = new SCBedChnl();
        *aBedChnl = *other.bedChnls_.at(chnlDex);
        this->addBedChnl(aBedChnl);
    }
}
    
SCBed::~SCBed() {
    // Delete the SCBedChnls in the SCBedChnl.
    for (int bedChnlDex = 0; bedChnlDex < bedChnls_.size(); bedChnlDex++) {// Go through each bedChnl in the list.
        SCBedChnl* theBedChnl = bedChnls_.at(bedChnlDex);// Get access to this bed chnl.
        delete theBedChnl;                      // Free the memory reserved for this bed channel.
        theBedChnl = nullptr;                   // Set the bed chnl pointer to null so that it won't be used.
    }                                           // All memeory reserved for bed channels has been deleted.
    bedChnls_.clear();                          // Clear all the enries in the bed channel list.
}

SCBed& SCBed::operator=(const SCClasses::SCBed &other){
    SCBed tmp(other);
    std::swap(name_, tmp.name_);
    std::swap(index_, tmp.index_);
    for (int chnlDex = 0; chnlDex < other.bedChnls_.size(); chnlDex++) {
        SCBedChnl* aChnl = new SCBedChnl();
        aChnl->setIndex(999);                   // Put this chnl at the end of the chnl list.
        addBedChnl(aChnl);
        std::swap(bedChnls_.at(chnlDex), tmp.bedChnls_.at(chnlDex));
        }
        return *this;
    }
    
    
string SCBed::getName() {
    return name_;                               // Provide the caller with this bed's name.
}
    
void SCBed::setName(string name) {
    size_t loc = name.find("-");                // Make sure the desired name doesn't contain '-'.
    while (loc != string::npos) {               // Is '-' in the desired name?
        name.at(loc) = '_';                     // Yes. Replace it with '_'.
        loc = name.find("-");                   // See if '-' occures again.
    }                                           // The desired name doesn't contain '-';
    name_ = name;                               // Set this bed's new name.
}
    
void SCBed::addBedChnl(SCBedChnl* bedChnl) {
    int bedChnlDex = bedChnl->getBedChnlIndex();// Get the desired index for this  bedChnl.
    for (std::vector<SCBedChnl*>::iterator it = bedChnls_.begin(); it != bedChnls_.end(); ++it)
    {   // Go through each of the bedChnls to see if this bedChnl is already on the list.
        // Is the SCChnl index of the bedChnl to add the same as the SCChnl index of one of the bedChnls already
        if ((*it)->getSCChnlDex() == bedChnl->getSCChnlDex()) {// on the list?
            return;                             // Yes. Do not add this bedChnl.
        }                                       // This bedChnl doesn't has the same SCChnl index as the one to add.
    }                                           // The bedChnl to add does not have the same SCChnl index as one on the list.
    if(bedChnlDex >= bedChnls_.size()) {        // Is the desired position past the end?
        bedChnlDex = (int)bedChnls_.size();     // Yes. Put the bedChnl at the end of the vector.
    } else if (bedChnlDex < 1) {                // Is the desired position before the start?
        bedChnlDex = 0;                         // Yes. Put the bedChnl at the list's start.
    }                                           // Otherwise the desired position was legal.
    bedChnl->setIndex(bedChnlDex);              // Set the actual index for this bedChnl.
    bedChnls_.insert(bedChnls_.begin() + bedChnlDex, bedChnl);// Insert the new bedChnl into the list of bedChnls.
    for(unsigned int j = bedChnlDex + 1; j < bedChnls_.size(); j++) {// Adjust each of the following
        bedChnls_.at(j)->setIndex(j);           // bedChnl indexes.
    }                                           // Done adjusting subsequent bedChnl indexes.
}

void SCBed::removeBedChnl(int nDex) {
    if (nDex < 0 || nDex > bedChnls_.size() - 1) {  // Is this a valid bedChnl index?
        return;                                     // No, there is nothing to remove.
    }                                               // Done.
    delete bedChnls_.at(nDex);                      // Free the memory allocated to the given bedChnl.
    bedChnls_.erase(bedChnls_.begin() + nDex);		// Remove the bedChnl from the vector of bedChnls.
    int bedChnlDex = 0;                             // The first bedChnl index is zero.
    for (std::vector<SCBedChnl*>::iterator it = bedChnls_.begin(); it != bedChnls_.end(); ++it)
    {   // Go through each of the bedChnls and reassign its index.
        (*it)->setIndex(bedChnlDex++);              // Reassign this bedChnl's index.
    }                                               // Done going through the remaining bedChnls.
}
    
SCBedChnl* SCBed::getBedChnl(unsigned int bedChnlDex) {
    if (bedChnlDex < bedChnls_.size()) {        // Does the given index point to a bedChnl?
        return bedChnls_.at(bedChnlDex);        // Yes. Return the bedChnl it references.
    }                                           // It doesn't reference a bedChnl.
    return nullptr;                             // REturn a nullptr.
}

int SCBed::findBedChnl(int scDexToFind) {
    for (std::vector<SCBedChnl*>::iterator it = bedChnls_.begin(); it != bedChnls_.end(); ++it)
    {   // Go through each of the bedChnls and exmine its SCChnl index.
        int scDex = (*it)->getSCChnlDex();          // Get the index of the SCChnl associated with this bedChnl.
        if (scDex == scDexToFind) {                 // Is this the SCChnl index being looked for?
            return (*it)->getBedChnlIndex();        // Yes. Return this bedChnl index.
        }                                           // This was not the SCChnl index being looked for.
    }                                               // Done going through the remaining bedChnls.
    return -1;                                      // There were no SCBedChnls found using SCChnl index scDexToFind.
}
    
int SCBed::getIndex() {
    return index_;                                  // Provide the caller with this bed's index.
}
    
void SCBed::setIndex(int value) {
    index_ = value;
}
   
int SCBed::getNoOfBedChnls() {
    return (int)bedChnls_.size();                   // Provide the caller with the number of chnls on this bed.
}
    
bool SCBed::save(XMLNode* xmlBed) {
    XMLNode* xmlName = new XMLNode(xmlBed, "Name", name_);// Create a node containing this bed's name.
    xmlBed->subNodes.push_back(xmlName);            // Add this node to the array of subnodes.
    // Create a node containing this bed's number of channels.
    XMLNode* xmlNoChnls = new XMLNode(xmlBed, "NoOfChannels", to_string(bedChnls_.size()));
    xmlBed->subNodes.push_back(xmlNoChnls);         // Add this node to the array of subnodes.
    for (unsigned int chDex = 0; chDex < bedChnls_.size(); chDex++) {// Go through each bed channel.
        XMLNode* xmlBedChnl = new XMLNode(xmlBed, "BedChnl_" + to_string(chDex), "");// Create a node for this bed.
        bedChnls_.at(chDex)->save(xmlBedChnl);      // Add xml data for this bed channel.
        xmlBed->subNodes.push_back(xmlBedChnl);     // Add this bed channel to the XMLNode containing this bed's data.
    }                                               // Each bed chnl's data has been put in xml format.
    return true;                                    // The bed configuration was successfully transfered to xmlBed.
    // All the memory allocated to the subNodes of xmlBed is freed when it goes out of scope. Its detructor is called.
}

bool SCBed::load(XMLNode* xmlBed) {
    XMLNode* xmlName = xmlBed->findSubNode("Name");// Get the node containing this bed's name.
    if (xmlName == nullptr) {               // Was the node retrieved?
        errorMessage = "The name of bed# " + to_string(getIndex()) + " was not found.";// No. Create error.
        return false;                       // Let the caller know there was an error.
    }                                       // The name exists.
    name_ = xmlName->data.c_str();          // Get the name from the node's data.
    XMLNode* xmlNoOfChnls = xmlBed->findSubNode("NoOfChannels");// Get the node containing this bed's number of channels.
    if (xmlName == nullptr) {               // Was the node retrieved?
        errorMessage = "The number of channels on bed# " + to_string(getIndex()) + " was not found.";// No. Create error.
        return false;                       // Let the caller know there was an error.
    }                                       // The number of channels exists.
    int noBedChnls = stoi(xmlNoOfChnls->data.c_str());// Get the number of channels from the node's data.
    for (int chnlDex = 0; chnlDex < noBedChnls; chnlDex++) {// Read configuration data for each bedChnl.
        string chnlNo = to_string(chnlDex); // Create a string version of the bedChnl index.
        XMLNode* xmlBedChnl = xmlBed->findSubNode("BedChnl_" + chnlNo);//Get the node containing this bedChnl.
        if (xmlBedChnl == nullptr) {        // Was the node retrieved?
            errorMessage = "BedChnl_" + chnlNo + " wasn't found.";// No. Create error.
            return false;                   // Let the caller know there was an error.
        }                                   // The bedChnl exists.
        SCBedChnl* aBedChnl = new SCBedChnl();// Create a new bedChnl to contain the configuration being read.
        aBedChnl->setIndex(chnlDex);        // Set the desired bedChnl index.
        if (aBedChnl->load(xmlBedChnl) == false) {// Was the bedChnl loaded?
            errorMessage = "BedChnl_" + chnlNo + " error: " + aBedChnl->errorMessage;// No. Create error.
            return false;                   // Let the caller know there was an error.
        }                                   // The bedChnl loaded.
        addBedChnl(aBedChnl);               // Add this bedChnl to the list of bed chnls.
    }                                       // All bed configurations have been read.
    return true;                            // Success.
}

/*
bool SCBed::setFromXML(string data) {
    size_t pos = data.find("Bed");                  // Find the position of the Bed tag.
    if (pos == string::npos) {                      // Is the position valid?
        return false;                               // No. Failure.
    }                                               // The position is valid.
    string str = "";                                // Will contain a prtion of the data string.
    try {                                           // Try to remove the portion
        str = data.substr(pos + 4);                 // of the data string before the bed name.
    } catch (...) {                                 // Was the portion before the bed name removed?
        return false;                               // No. Failure.
    }                                               // Yes. The bed name is at the start of the data string.
    pos = str.find("/Bed");                         // Find the position of the closing tag.
    if (pos == string::npos) {                      // Is the position valid?
        return false;                               // No. Failure.
    }                                               // The position is valid.
    string bedName = "";                            // Will contain the bed's anme.
    try {                                           // Try to get
        bedName= str.substr(0, pos - 1);            // the bed's name.
    } catch (...) {                                 // Was the bed name retreived?
        return false;                               // No. Failure.
    }                                               // The bed name was retrieved.
    try {                                           // Try to remove the
        str = str.substr(pos + 6);                  // remaining part of the closing bed tag.
    } catch (...) {                                 // Was the closing tag removed?
        return false;                               // No. Failure.
    }                                               // Yes. The bed closing tage was removed.
    pos = str.find(">");                            // Get the end of the Version opening tag.
    if (pos == string::npos) {                      // Is the position valid?
        return false;                               // No. Failure.
    }                                               // The position is valid.
    try {                                           // Try to remove the
        str = str.substr(pos + 1);                  // Version opening tag.
    } catch (...) {                                 // Was the Version opening tag removed?
        return false;                               // No. Failure.
    }                                               // The Version opening tag was removed.
    pos = str.find("<");                            // Get the start of the Version closing tag.
    if (pos == string::npos) {                      // Is the position valid?
        return false;                               // No. Failure.
    }                                               // The position is valid.
    string val;                                     // Will contain the string representation of numeric values.
    try {                                           // Try to get the
        val = str.substr(0, pos);                   // string representation of the Version.
    } catch (...) {                                 // Was it obtained?
        return false;                               // No. Failure.
    }                                               // Yes. There is a string of the version.
    int version = 1;                                // Set the default version.
    try {                                           // Try to convert the string representation
        version = stoi(val);                        // to the Version.
    } catch (...) {                                 // Did the conversion work?
        return false;                               // No. Failure.
    }                                               // The Version has been set.
    try {                                           // Try to remove the
        str = str.substr(pos + 10);                 // remaining part of the Version tag.
    } catch (...) {                                 // Was it removed?
        return false;                               // No. Failure.
    }                                               // Yes. The closing Version tag was removed.
    string tagName = "";                            // Will contain the tag name for the data being evaluated.
    int noBedChnls = 0;                             // Contains the number of bed chnls.
    try {                                           // Try to get the number fo bed chnls.
        tagName = "NoOfChnls";                      // Create the tag name for the number of bed chnls.
        pos = str.find(">");                        // Get the end of the opening tag.
        if (pos == string::npos) {                  // Is the position valid?
            return false;                           // No. Failure.
        }                                           // The position is valid.
        str = str.substr(pos + 1);                  // Remove the opening tag.
        pos = str.find("<");                        // Get the start of the closing tag.
        if (pos == string::npos) {                  // Is the position valid?
            return false;                           // No. Failure.
        }                                           // The position is valid.
        val = str.substr(0, pos);                   // Get the string representation of this tag's data.
        noBedChnls = stoi(val);                     // of this tag's data to a number.
        str = str.substr(pos + tagName.length() + 3);   // Remove the remaining part of the closing tag.
    } catch (...) {                                 // Were any exceptions thrown trying to get the number of chnls?
        return false;                               // Yes. Failure.
    }                                               // The number of bed chnls was obtained.
    for (int chnlDex = 0; chnlDex < noBedChnls; chnlDex++) {// Go through each bed chnl.
        string chnlStr = "Chnl_" + to_string(chnlDex) + "_";// Create the chnl name part of the tag.
        SCBedChnl* theBedChnl = bedChnls_.at(chnlDex);// Access this bed chnl.
        try {                                       // Try to get this bed chnl's configuration.
            tagName = chnlStr + "SCChnlDex";        // Create the tag name for this bed chnl's SCChnl index.
            pos = str.find(">");                    // Get the end of the opening tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            str = str.substr(pos + 1);              // Remove the opening tag.
            pos = str.find("<");                    // Get the start of the closing tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            val = str.substr(0, pos);               // Get the string representation of this tag's data.
            theBedChnl->setSCChnlDex(stoi(val));    // Set this bed chnl's SCChnl index.
            str = str.substr(pos + tagName.length() + 3);// Remove the remaining part of the closing tag.
            tagName = chnlStr + "SCRefDex";         // Create the tag name for this bed chnl's SCChnl reference index.
            pos = str.find(">");                    // Get the end of the opening tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            str = str.substr(pos + 1);              // Remove the opening tag.
            pos = str.find("<");                    // Get the start of the closing tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            val = str.substr(0, pos);               // Get the string representation of this tag's data.
            theBedChnl->setRefSCChnlDex(stoi(val)); // Set this bed chnl's SCChnl reference index.
            str = str.substr(pos + tagName.length() + 3);// Remove the remaining part of the closing tag.
            tagName = chnlStr + "IsAssigned";       // Create the tag name for this bed chnl's assigned at runtime flag.
            pos = str.find(">");                    // Get the end of the opening tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            str = str.substr(pos + 1);              // Remove the opening tag.
            pos = str.find("<");                    // Get the start of the closing tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            val = str.substr(0, pos);               // Get the string representation of this tag's data.
            if (val == "1") {                       // One mean this bed chnl's SCChnl index is set at runtime.
                theBedChnl->isAssigned = true;      // It is one. Thus, it is assigned.
            } else {                                // It is not one.
                theBedChnl->isAssigned = false;     // Thus, this one is not assigned at runtime.
            }                                       // This bed chnl's assigned at runtime flag has been obtained.
            str = str.substr(pos + tagName.length() + 3);// Remove the remaining part of the closing tag.
            tagName = chnlStr + "AssignedChnlDex";  // Create the tag name for this bed chnl's assigned SCChnl index.
            pos = str.find(">");                    // Get the end of the opening tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            str = str.substr(pos + 1);              // Remove the opening tag.
            pos = str.find("<");                    // Get the start of the closing tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            val = str.substr(0, pos);               // Get the string representation of this tag's data.
            theBedChnl->setAssignedSCChnlDex(stoi(val));// Set this bed chnl's assigned SCChnl index.
            str = str.substr(pos + tagName.length() + 3);// Remove the remaining part of the closing tag.
            tagName = chnlStr + "AssignedRefChnlDex";// Create the tag name for this bed chnl's assigned SCChnl ref index.
            pos = str.find(">");                    // Get the end of the opening tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            str = str.substr(pos + 1);              // Remove the opening tag.
            pos = str.find("<");                    // Get the start of the closing tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            val = str.substr(0, pos);               // Get the string representation of this tag's data.
            theBedChnl->setAssignedRefDex(stoi(val));// Set this bed chnl's assigned SCChnl reference index.
            str = str.substr(pos + tagName.length() + 3);// Remove the remaining part of the closing tag.
            tagName = chnlStr + "ControlType";      // Create the tag name for this bed chnl's control type.
            pos = str.find(">");                    // Get the end of the opening tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            str = str.substr(pos + 1);              // Remove the opening tag.
            pos = str.find("<");                    // Get the start of the closing tag.
            if (pos == string::npos) {              // Is the position valid?
                return false;                       // No. Failure.
            }                                       // The position is valid.
            val = str.substr(0, pos);               // Get the string representation of this tag's data.
            theBedChnl->controlType = (SCProgram::Type)stoi(val);// Set this bed chnl's control type.
            str = str.substr(pos + tagName.length() + 3);// Remove the remaining part of the closing tag.
        } catch (...) {                             // Was the configuration for this SCBedChnl obtained?
            return false;                           // No. Failure.
        }                                           // The SCBedChnl configuration was obtained.
    }                                               // All the SCBedChnl configurations have been obtained.
    return true;                                    // Success.
}
*/

#pragma mark - Beds class implementation
/////////////////////////////////Beds//////////////////////////////////////////////////////////////
    
SCBeds::SCBeds() {
    version_ = 1;                   // As of 2017-03-16 the version is 1.
    bedChnlVersion_ = 1;            // As of 2017-03-16 the bed chnl version is 1.
    noBeds_ = 0;                    // There are no beds until they are loaded.
    filePath = "";                  // There is no file path until it is assigned.
    cylinderCureTime = 240;         // By default, the cylinder is controlled for 4 hours after the bed's master finishes.
    xmlErrors = nullptr;            // There is no access to the Plant Manager error data until it is set.
    errorMessage = "";              // There hasn't been an error yet.
}
    
SCBeds::~SCBeds() {
    clearBeds();                    // Free all the memory allocated to bed.
}
   
void SCBeds::clearBeds() {
    for (unsigned int bedDex = 0; bedDex < this->size(); bedDex++) {// Go through each bed.
        delete this->at(bedDex);    // Free the memory allocated to it.
    }                               // All the memory allocated to beds has been feed.
    this->clear();                  // Remove all the beds from the bed list.
    noBeds_ = 0;                    // There are no beds now.
}
    
void SCBeds::add() {
    SCBed* aBed = new SCBed();                  // Create a new SCBed.
    long nameNumber = this->size();             // Create a number that represents the name.
    string testName = to_string(++nameNumber);  // Convert that number to a string.
    while (nameExists(testName) == true) {      // Does another SCBed have that name?
        nameNumber++;                           // Yes. Increment the number in order to try a new name.
        testName = to_string(nameNumber);       // Convert that number to a string.
    }                                           // A unique name has been found.
    aBed->setName(testName);                   // Set the SCBed's name.
    this->push_back(aBed);                     // Add the new bed to SCBeds.
}
    
void SCBeds::add(SCBed* aBed) {
    int bedDex = aBed->getIndex();              // Get the desired index for this bed.
    for (std::vector<SCBed*>::iterator it = this->begin(); it != this->end(); ++it)
    {   // Go through each of the bedCs to see if this bed is already on the list.
        // Is the bed name of the bed to add the same as a bed already
        if ((*it)->getName() == aBed->getName()) {// in the list?
            return;                             // Yes. Do not add this bed.
        }                                       // This bed doesn't has the same name as the one to add.
    }                                           // The bed to add does not have the same name as one on the list.
    if(bedDex >= this->size()) {                // Is the desired position past the end?
        bedDex = (int)this->size();             // Yes. Put the bed at the end of the vector.
    } else if (bedDex < 1) {                    // Is the desired position before the start?
        bedDex = 0;                             // Yes. Put the bed at the list's start.
    }                                           // Otherwise the desired position was legal.
    aBed->setIndex(bedDex);                     // Set the actual index for this bed.
    this->insert(this->begin() + bedDex, aBed); // Insert the new bed into the list of beds.
    for(unsigned int j = bedDex + 1; j < this->size(); j++) {// Adjust each of the following
        this->at(j)->setIndex(j);               // bed indexes.
    }                                           // Done adjusting subsequent bed indexes.
}
  
bool SCBeds::nameExists(string value) {
    for (int z = 0; z < size(); z++) {          // Go through each SCBed.
        if (at(z)->getName() == value) {        // Does this SCBed's name match the given name?
            errorMessage = value + " matches the name used by SCChnl[" + to_string(z) + "].";
            return true;                        // The name already exists. Let the caller know there is a match.
        }                                       // This SCBed's name does not match the given name.
    }                                           // The name doesn't exist.
    errorMessage = "";                         // Clear the error message.
    return false;                               // Let the caller know the name doesn't exist.
}
    
string SCBeds::createXMLVersion() {
    string xmlBedsString = "";                  // The XML string representing the bed configurations.
    SCXML xmlBeds;                              // Create an XML object to hold the SCBeds.
    xmlBeds.root.name = "SCBeds";               // Give it a name to reflect SCBeds.
    xmlBeds.root.data = "";                     // There is no data in the root.
    XMLNode* parent = &xmlBeds.root;            // Contains the address of the root node.
    XMLNode* xmlVersion = new XMLNode(parent, "Version", to_string(version_));// Create a node for the version.
    xmlBeds.root.subNodes.push_back(xmlVersion);// Add this node to the array of subnodes.
    // Create a node for the  bed channel version.
    XMLNode* xmlBedChnlVer = new XMLNode(parent, "BedChnlVersion", to_string(bedChnlVersion_));
    xmlBeds.root.subNodes.push_back(xmlBedChnlVer);// Add this node to the array of subnodes.
    XMLNode* xmlCount = new XMLNode(parent, "NumberOfBeds", to_string(this->size()));// Create node for number of SCBeds.
    xmlBeds.root.subNodes.push_back(xmlCount);  // Add this node to the array of subnodes.
    // Create a node for the number of minutes the cylinder should continue to follow the master after it finishes.
    XMLNode* xmlCylCureTime = new XMLNode(parent, "CylinderCureTime", to_string(cylinderCureTime));
    xmlBeds.root.subNodes.push_back(xmlCylCureTime);// Add this node to the array of subnodes.
    for (unsigned int bedDex = 0; bedDex < size(); bedDex++) {// Go through each SCBed.
        XMLNode* xmlBed = new XMLNode(parent, "SCBed_" + to_string(bedDex), "");// Create a node for this SCBed.
        at(bedDex)->save(xmlBed);               // Add xml data for this SCBed.
        xmlBeds.root.subNodes.push_back(xmlBed);// Add this SCBed to the vector containing xml SCBed data.
    }                                           // Each SCBed's data has been put in xml format and stored.
    xmlBeds.createLines();                      // Put the XML nodes in line format.
    for_each(xmlBeds.lines_.begin(), xmlBeds.lines_.end(), [&] (string bedLine) {// Go through each line.
        xmlBedsString += bedLine;               // Append the line to the bed's string.
        xmlBedsString += '\n';                  // Put a new line between each line.
    });                                         // The beds are in xml string format.
    return xmlBedsString;                       // Return the beds in xml string format to the caller.
}


}// End of namespace SCClasses.
