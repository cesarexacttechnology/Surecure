//
//  SCXML.cpp
//  PlantManager
//
//  Created by Mark Davis on 7/14/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#include "SCXML.h"

#include <iostream>
#include <fstream>

#define MEMORYLEAKTEST false    // Set to true to display memory leak diagnostics.

using namespace std;

namespace SCClasses {

    XMLNode::XMLNode() {
        name = "name";          // The default name is name.
        data = "data";          // The default data is "dat".
        subNodes.clear();       // This node has no sub-nodes until they are added.
        parent = nullptr;       // There is no parent until it is set.
        if(MEMORYLEAKTEST == true) {                    // Is a memory leak test being performed?
            cout << "Created: XMLNode " << to_string((long)this) << endl;// Yes, display creation data.
        }                                               // Memory leak test complete.
    }
    
    XMLNode::XMLNode(XMLNode* theParent, string theName, string theData) {
        name = theName;         // Set the name to the given name.
        data = theData;         // Set the data to the given data.
        parent = theParent;     // Set the parent to the given parent.
        subNodes.clear();       // This node has no sub-nodes until they are added.
        if(MEMORYLEAKTEST == true) {                    // Is a memory leak test being performed?
            cout << "Created: XMLNode " << to_string((long)this) << endl;// Yes, display creation data.
        }                                               // Memory leak test complete.
    }
    
    
    XMLNode::~XMLNode() {
        if(MEMORYLEAKTEST == true) {                    // Is a memory leak test being performed?
            string test = "Deleting: XMLNode " + name + ", address " + to_string((long)this) + ".";// Yes.
            cout << test << endl;                       // Display deletion data.
        }                                               // Tiem to delete the node.
        for(unsigned int i = 0; i < subNodes.size(); i++){	// Go through each xmlNode.
            XMLNode* theNode = subNodes.at(i);          // Get the node to delete
            delete theNode;                             // Delete the node.
            theNode = nullptr;                          // This is a safe guard against double deleteing.
        }                                               // Done deleting nodes.
    }
    
    
    void XMLNode::deleteNode(XMLNode* nodeToDelete) {
        for (unsigned int i = 0; i < nodeToDelete->subNodes.size(); i++) {// Go through each subnode for this node.
            XMLNode* subNode = nodeToDelete->subNodes.at(i);// Get the subnode to delete.
            deleteNode(subNode);                        // Delete the subnode.
        }                                               // All subnodes are deleted.
        nodeToDelete->subNodes.clear();                 // Clear the subNode vector.
        cout << "Deleted XMLNode " << to_string((long)nodeToDelete) << endl;
        delete nodeToDelete;                            // Delete this node.
        nodeToDelete = nullptr;                         // This is a safe guard against double deleteing.
    }

    XMLNode* XMLNode::findSubNode(string theName) {
        XMLNode* theNode = nullptr;                     // Contains the address of the node that was found.
        for (int q = 0; q < subNodes.size(); q++) {     // Go through each sub-node which is a part of this node.
            if (subNodes.at(q)->name == theName) {      // Does this sub-node's name match the given name?
                theNode = subNodes.at(q);               // This is the sub-node that is being searched for.
                break;                                  // Done looking for a match.
            }                                           // Check the next sub-node.
        }                                               // Done going through the sub-nodes.
        return theNode;                         // Return the address of the matching sub-node(nullptr if none was found).
    }

    
    
    
    
    
    
    SCXML::SCXML() {
        xml_ = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";// Contains the standard xml first line.
        root.name = "Root";                             // XML must have a root. The default name is root.
        root.data = "Root Data";                        // The default data is Root Data.
        root.subNodes.clear();                          // There are no subNodes until they are added by a caller.
        fileName = "";                                  // There is no file name until it is set.
        filePath_ = "";                                 // There is no file path until it is set.
        nSubNodesAllowed = 0;                           // There can be as many sub-nodes off the root as desired.
        if(MEMORYLEAKTEST == true) {                    // Is a memory leak test being performed?
            cout << "Created: SCXML " << to_string((long)this) << endl;// Yes, display creation data.
        }                                               // Memory leak test complete.
    }
    
    SCXML::~SCXML() {
        if (MEMORYLEAKTEST == true) {                   // Is a memory leak test being performed?
            string test = "Deleteing: SCXML " + to_string((long)this) + " containing " + to_string(root.subNodes.size()) + " subNodes.";                       // Create a deletion message.
            cout << test << endl;                       // Display the deletion message.
        }                                               // Get on to deleting the SCXML object.
        for(unsigned int i = 0;  i < root.subNodes.size(); i++){	// Go through each xmlNode.
            XMLNode* theNode = root.subNodes.at(i);     // Get the node to delete
            delete theNode;                             // Delete the node.
            theNode = nullptr;                          // This is a safe guard against double deleteing.
        }                                               // Done deleting nodes.
        root.subNodes.clear();                          // Empty the subNode vector.
    }

    void SCXML::deleteNode(XMLNode* nodeToDelete) {
        int nSubNodes = (int)nodeToDelete->subNodes.size();// Get the number of subnodes this node has.
        int i = nSubNodes - 1;                          // Start with the last subnode of the node to delete.
        for (; i >= 0; i--) {                           // Go through each subnode for the node to delete.
            XMLNode* subNode = nodeToDelete->subNodes.at(i);// Get the subnode to delete.
            deleteNode(subNode);                        // Delete the subnode.
        }                                               // All subnodes are deleted.
        nodeToDelete->subNodes.clear();                 // Remove all the subnodes from the vector(they have already been deleted).
        delete nodeToDelete;                            // Delete this node.
        nodeToDelete = nullptr;                         // This is a safe guard against double deleteing.
    }
    
    void SCXML::createLines() {
        lines_.clear();                                 // Clear the list of xml lines.
        lines_.push_back(xml_);                         // Add the generic header line.
        if (root.subNodes.size() == 0) {                // Is the root node a data node?
            addNodeToLines(&root);                       // Yes.
        } else {                                        // The root node is not a data node.
            lines_.push_back(createOpeningTag(&root));  // Add the opening tag for the root.
            for (int nDex = 0; nDex < root.subNodes.size(); nDex++) {// Go through each of the roots sub-nodes.
                addNodeToLines(root.subNodes.at(nDex)); // Add a line for this sub-node.
            }                                           // Lines for all sub-nodes have been added.
            lines_.push_back(createClosingTag(&root));  // Add the closing tag for the root.
        }                                               // All lines have been created.
    }
    
    void SCXML::addNodeToLines(XMLNode* nodeToAdd) {
        if (nodeToAdd->subNodes.size() > 0) {            // Are there sub-nodes?
            lines_.push_back(createOpeningTag(nodeToAdd));// Yes, create an opening tag for this node.
            for (int n = 0; n < nodeToAdd->subNodes.size(); n++) {// Go through each of the sub-nodes.
                addNodeToLines(nodeToAdd->subNodes.at(n));// Add the required lines for this sub-node.
            }                                           // The required lines for all sub-nodes have been added.
            lines_.push_back(createClosingTag(nodeToAdd));// create a closing tag for this node.
        } else {                                        // There were no sub-nodes.
            lines_.push_back(createDataLine(nodeToAdd));// This is a data node. Create and add a line for this data node.
        }                                               // All lines have been added for this node.
    }
    
    string SCXML::createOpeningTag(XMLNode* node){
        string line = "<";                              // Start building a tag with the less than character.
        line += node->name;                             // Append the name.
        line += ">";                                    // Finish the tag with the greater than character.
        return line;                                    // Provide the caller with the opening tag.
    }
    
    string SCXML::createClosingTag(XMLNode* node){
        string line = "</";                             // Start building the closing tag with less than slash.
        line += node->name;                             // Append the name.
        line += ">";                                    // Finish the tag with the greater than character.
        return line;                                    // Provide the caller with the closing tag.
    }
    
    string SCXML::createDataLine(XMLNode* node){
        string line = createOpeningTag(node);           // Start building a data line with a opening tag.
        line.append(node->data);
        line += createClosingTag(node);                 // Finish the data line with the closing tag.
        return line;                                    // Provide the caller with the data line.
    }
    
    void SCXML::display() {
        for (int i = 0; i < lines_.size(); i++) {       // Go through each line.
            cout << lines_.at(i) << endl;               // Send it to the standard output stream.
        }                                               // All lines have been displayed.
    }
    
    bool SCXML::writeXMLFile() {
        string name = filePath_;                    // Start the file name with the path.
        name += fileName;                           // Add the actual name to the path to get the file name.
        ofstream xmlFile(name);                     // Create a stream to the xml file.
        if(xmlFile.is_open() == false) {            // Did the xml file open?
            return false;                           // No, let the user know the xml file was not written.
        }                                           // The xml file is open to be written to.
        for (int i = 0; i < lines_.size(); i++) {   // Go through each xml line.
            xmlFile << lines_.at(i) << endl;;       // Write the line to the file.
        }                                           // All line have been written.
        xmlFile.close();                            // Close the xml File.
        return true;                                // Let the caller know the file was written successfully.
    }
    
    bool SCXML::readXMLFile() {
        string xmlFileName = filePath_;             // Start the file name with the path.
        xmlFileName += fileName;                    // Add the actual name to the path to get the file name.
        ifstream xmlFile(xmlFileName);              // Create a stream to the xml file.
        if(xmlFile.is_open() == false) {            // Did the xml file file open?
            return false;                           // No, let the user know the xml file was not loaded.
        }                                           //
        string line;                                // Will contain a line from the xml file.
        getline(xmlFile, line);                     // Get the first line.
        size_t pos = line.find("?xml");             // Find the position of the comma in the line.
        if (pos == 1) {                             // Did the first line contain the xml info?
            getline(xmlFile, line);                 // Yes. Read the next line to get the root.
        } else {                                    // No. There is an error.
            xmlFile.close();                        // Close whatever opened.
            return false;                           // Let the caller know the file couldn't be read.
        }                                           // Line contains the root. It may also contain data.
        size_t firstLTPos = line.find_first_of("<");// Get the position of the first less than.
        size_t lastLTPos = line.find_last_of("<");  // Get the position of the last less than.
        size_t firstGTPos = line.find_first_of(">");// Get the position of the first greater than.
        size_t lastGTPos = line.find_last_of(">");  // Get the position of the last greater than.
        if (firstGTPos == 0) {                      // Is there no greater than or is it at the first position?
            return false;                           // This is illegal in xml.
        }                                           // The line might be legal xml.
        if (firstLTPos == lastLTPos && firstGTPos == lastGTPos) {// Is there only one "< >" pair?
            if (line.at(firstLTPos + 1) != '/') {   // Yes. Is the second character something other than a forward slash?
                root.name = getOpeningTag(line.substr(firstLTPos));// Yes. This is an opening tag. Get its name.
            } else {                                // This is a closing tag.
                string tagName = getClosingTag(line.substr(firstLTPos));// Get itd name.
            }                                       // A tag has been processed and its name has been obtained.
        } else if (firstLTPos != lastLTPos && firstGTPos != lastGTPos) {// The root contains data.
            // Get the part of the line which contains the opening tag.
            string openingTag = line.substr(firstLTPos, firstGTPos - firstLTPos + 1);
            root.name = getOpeningTag(openingTag);  // Get the tag name.
            root.data = getDataFromLine(line);      // Get data from the line.
        }                                           //
        else {                                      // There was something illegal.
            return false;                           // Let the caller know.
        }                                           // The root line has been processed.
        if (root.subNodes.size() > 0) {             // If the root is being reassigned, does it have sub-nodes?
            for (int n = 0; n < root.subNodes.size(); n++) {// Go through all the sub-nodes.
                deleteNode(root.subNodes.at(n));    // Delete this sub-node.
            }                                       // Done deleteing sub-nodes.
        }                                           // The root's sub-nodes have been deletted.
        XMLNode* parentNode = &root;                // Use parentNode to keep track of which node to assign sub-nodes.
        while (getline(xmlFile, line)) {            // Go through the remaining lines in the file.
            firstLTPos = line.find_first_of("<");   // Get the position of the first less than.
            lastLTPos = line.find_last_of("<");     // Get the position of the last less than.
            firstGTPos = line.find_first_of(">");   // Get the position of the first greater than.
            lastGTPos = line.find_last_of(">");     // Get the position of the last greater than.
            if (firstGTPos == 0) {                  // Is there a greater than after the first character?
                return false;                       // No. This is an invalid xml line.
            }                                       // This is a valid xml line.
            if (firstLTPos == lastLTPos && firstGTPos == lastGTPos) {// Is there only one set of "< >"?
                if (line.at(firstLTPos + 1) != '/') {// Yes. Is the second character a forward slash?
                    XMLNode* node = new XMLNode();  // No, this is an opening tag. Create a new tag(node).
                    node->name = getOpeningTag(line.substr(firstLTPos));// Set the name.
                    node->data = "";                // An opening tag has no data.
                    node->parent = parentNode;      // Set the parent for this node.
                    parentNode->subNodes.push_back(node);// Add the new sub-node.
                    parentNode = node;              // The parent node becomes this node.
                } else {                            // This is a closing tag.
                    string tagName = getClosingTag(line.substr(firstLTPos));// Nothing to do right now.
                    parentNode = parentNode->parent;// Move the parent node up a level.
                }                                   // Lines containing just opening or closing tags have been processed.
            } else if (firstLTPos != lastLTPos && firstGTPos != lastGTPos) {// Are there two sets of "< >"?
                XMLNode* node = new XMLNode();      // Yes, this is a data tag. Create a new tag(node).
                // Get the part of the line which contains the opening tag.
                string openingTag = line.substr(firstLTPos, firstGTPos - firstLTPos + 1);
                node->name = getOpeningTag(openingTag);// Get the tag name.
                node->data = getDataFromLine(line); // Get data from the line.
                node->parent = parentNode;          // Set this data node's parent.
                parentNode->subNodes.push_back(node);// Set this node as a sub-node to its parent.
            }                                       // A data node has been processed.
            else {                                  // This is not a legal xml line.
                return false;                       // Let the caller know.
            }                                       // This line has been processed.
        }                                           // All the lines have been processed.
        xmlFile.close();                            // Close the xml file.
        return true;                                // Let the caller know the xml file was read.
    }
    
    string SCXML::getOpeningTag(const string& line) {
        string tag = "";                        // Start with an empty tag.
        if (line.at(0) != '<') {                // Is the first character a less than?
            string error = "The less than character is missing from the opening tag.";// No. That is not right.
            cout << error << endl;              // Display an error message.
            return tag;                         // Return the empty tag.
        }                                       // The first character is a less than.
        size_t pos = line.find(">");            // Find the location of the greater than.
        if (pos == string::npos) {              // Is there a location?
            string error = "The greater than character is missing from the opening tag.";// No. That is invalid.
            cout << error << endl;              // Display an error message.
            return tag;                         // Return the empty tag.
        }                                       // The "< >" exists.
        tag = line.substr(1, pos - 1);          // The characters in between < and > are the tag name.
        return tag;                             // Return the tag name.
    }

    string SCXML::getClosingTag(const string& line) {
        string tag = "";                        // Start with an empty tag.
        if (line.at(0) != '<') {                // Is the first character a less than?
            string error = "The less than character is missing from the closing tag.";// No. This is wrong.
            cout << error << endl;              // Display an error message.
            return tag;                         // Return an empty tag.
        }                                       // The first character is a less than.
        if (line.at(1) != '/') {                // Is the second character a forward slash?
            string error = "The forward slash character is missing from the closing tag.";// No. This is wrong.
            cout << error << endl;              // Display an error message.
            return tag;                         // Return an empty tag.
        }                                       // The line start with </. (Valid closing tag characters)
        size_t pos = line.find(">");            // Find the position of the greater than.
        if (pos == string::npos) {              // Does a position exist after the first character?
            string error = "The greater than character is missing from the closing tag.";// No. This is wrong.
            cout << error << endl;              // Display an error message.
            return tag;                         // Return an empty tag.
        }                                       // "</ >" exists.
        tag = line.substr(2, pos - 2);          // The character in between </ and > are the tag name.
        return tag;                             // Return the tag name.
    }
    
    string SCXML::getDataFromLine(const string& line) {
        string data = "";                           // Start with an empty tag.
        size_t lastLTPos = line.find_last_of("<");  // Get the position of the last less than.
        size_t firstGTPos = line.find_first_of(">");// Get the position of the first greater than.
        data = line.substr(firstGTPos + 1, lastLTPos - firstGTPos - 1);// The data is in between these two positions.
        return data;                                // Return the data.
    }
    
    void SCXML::setFilePath(string path) {
        filePath_ = path;                       // Set the file path for the XML file.
        if (path.size() == 0) {                 // Is the file path the current directory?
            return;                             // Yes. No need to add a slash.
        }                                       // The desired path exists.
        char slash = path.at(path.size() - 1);  // Get the last character from the path.
        if (slash != '/') {                     // Is the last character a slash?
            filePath_.append("/");              // No, make the last character a slash.
        }                                       // The file path is set.
    }
    
    void SCXML::removeRootSubNode(unsigned int nodeNo) {
        if (nodeNo >= root.subNodes.size()) {   // Is the index valid?
            string error = "The index of the node to be removed in removeRootSubNode() is greater than the number of sub-nodes.";
            cout << error << endl;              // Invalid index, display error message.
        }                                       // The index is valid.
        deleteNode(root.subNodes.at(nodeNo));   // Delete any memory allocated to this sub-node or its sub-nodes.
        root.subNodes.erase(root.subNodes.begin() + nodeNo);// Remove this sub-node from the list.
    }

    void SCXML::appendRootSubNode(XMLNode* node) {
        node->parent = &root;                   // Make sure this sub-node's parent is set to the root.
        root.subNodes.push_back(node);          // Append a sub-node to the root's list of sub-nodes.
        if (nSubNodesAllowed > 0) {             // Are the number of sub-nodes restricted?
            if (root.subNodes.size() > nSubNodesAllowed) {// Yes. Has the number of sub-nodes allowed been exceeded?
                removeRootSubNode(0);           // Remove the first sub-node.
            }                                   // The first sub-node has been removed.
        }                                       // Sub-nodes were not restricted.
    }
    
    void SCXML::appendErrorNode(string message) {
        cout << message << endl;
        XMLNode* eNode = new XMLNode();         // Create an XMLNode to hold this error message.
        eNode->name = "SCNote";                 // The node will have the name SCNote.
        XMLNode* tNode = new XMLNode();         // Create an XMLNode to hold the time the error occurred.
        tNode->name = "Time";                   // The node will have the name Time.
        chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();// Get the present time.
        time_t tt = chrono::system_clock::to_time_t(now);// Convert the time to time_t format.
        string time = ctime(&tt);               // Convert the time from time_t format to string format.
        time = time.substr(0, time.length() - 1);// The conversion leaves a \n at the end of the string. Remove the \n.
        tNode->data = time;                     // The time node will have the present time as its data.
        eNode->subNodes.push_back(tNode);       // The time XMLNode is a sub-node to the error XMLNode.
        XMLNode* mNode = new XMLNode();         // Create an XMLNode for the message.
        mNode->name = "Message";                // The node will have name Message.
        mNode->data = message;                  // The nodes data will be the massage passed by the caller.
        eNode->subNodes.push_back(mNode);       // The message XMLNode is a sub-node to the error XMLNode.
        root.subNodes.push_back(eNode);         // The error XMLNode is a sub-node to the root.
        if (nSubNodesAllowed > 0) {             // Are the number of sub-nodes restricted?
            if (root.subNodes.size() > nSubNodesAllowed) {// Yes. Has the number of sub-nodes allowed been exceeded?
                removeRootSubNode(0);           // Yes. Remove the first sub-node.
            }                                   // The first sub-node has been removed.
        }                                       // Sub-nodes were not restricted.
        createLines();                          // Create a new set of text lines for the XML file.
        if (writeXMLFile() == false) {          // Rewrite the error file. Did it succeed?
            cout << "Error writting to " << filePath_ << fileName << "." << endl;;// No. Display an error message.
        }                                       // The error file was rewritten successfully.
    }

    string SCXML::getDataFrom(string tagName) {
        XMLNode* node = findNode(&root, tagName);// Find the node with the given tag name.
        if (node == nullptr) {                  // Was a node found that contains the given name?
            return "";                          // No. Therefore, there is no data to return.
        }                                       // A node was found.
        return node->data;                      // Return that node's data.
    }

    XMLNode* SCXML::findNode(XMLNode* node, string theName) {
        for (int nDex = 0; nDex < node->subNodes.size(); nDex++) {// Go through each sub-node looking for the name.
            XMLNode* aNode = node->subNodes.at(nDex);// Get the sub-node at nDex.
            if (aNode->name == theName) {          // Does this sub-node have the name we are looking for?
                return aNode;                   // Yes. Return the sub-node.
            }                                   // This sub-node does not have the name we are looking for.
            if (aNode->subNodes.size() != 0) {  // Does this sub-node have sub-nodes?
                XMLNode* fNode = aNode->findSubNode(theName);// Yes. See if any of its sub-nodes have the name we are looking for.
                if (fNode != nullptr) {         // Was a sub-node returned?
                    return fNode;               // Yes. Return the sub-node.
                }                               // None of this sub-node's sub-nodes had the name.
            }                                   // This sub-node did not match the name or have a sub-node that matched the name.
        }                                       // All the sub-nodes have been checked.
        return nullptr;                         // Let the caller know the name was not found.
    }
    
}
