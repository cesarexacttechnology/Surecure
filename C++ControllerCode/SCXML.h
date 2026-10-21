//
//  SCXML.h
//  PlantManager
//
//  Created by Mark Davis on 7/14/14.
//  Copyright (c) 2014 Products Engineering. All rights reserved.
//

#ifndef PlantManager_SCXML_h
#define PlantManager_SCXML_h

#include <string>
#include <vector>
#include <chrono>

using namespace std;

namespace SCClasses {

    class XMLNode {
    public:
        /*! Default constructor. */
        XMLNode();
        /*! Alternate constructor. */
        XMLNode(XMLNode* theParent, string theName, string theData);
        /*! Default destructor. */
        virtual ~XMLNode();
        /*! Deletes this node and any of its sub-nodes.
         * \param nodeToDelete A pointer to the node to delete. */
        virtual void deleteNode(XMLNode* nodeToDelete);
        /*! A list of the sub-nodes below this node.  */
        vector<XMLNode*> subNodes;
        /*! The nodes parent if any. Only the root has no parent. */
        XMLNode* parent;
        /*! The name associated with this node. */
        string name;
        /*! The data associated with this node. */
        string data;
        /*! Find the sub-node with the given name. 
         * \returns A pointer to the sub-node.
         * \param theName The name of the sub-node. */
        XMLNode* findSubNode(string theName);
    };

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    class SCXML {
    public:
        /*! Default constructor. */
        SCXML();
        /*! Default destructor. */
        virtual ~SCXML();
        /*! Deletes this node and any of its sub-nodes. 
         * \param nodeToDelete A pointer to the node to delete. */
        virtual void deleteNode(XMLNode* nodeToDelete);
        /*! The root node. Every xml structure requires a root node. */
        XMLNode root;
        /*! The file name used to store this xml data. */
        string fileName;
        /*! Creates the lines which make up an xml stream(file). 
         * Uses the existing nodes for this SCXML object to create the lines. */
        void createLines();
        /*! Creates a string which starts with '<' then contains a name and finally finishes with '>'. 
         * \param node The node(tag) which contains the name. 
         * \returns A string with the format "<name>". */
        string createOpeningTag(XMLNode* node);
        /*! Creates a string which starts with "</" then contains a name and finally finishes with '>'.
         * \param node The node(tag) which contains the name.
         * \returns A string with the format "</name>". */
        string createClosingTag(XMLNode* node);
        /*! Creates a string which starts with '<name>' then contains a data string and finally finishes with '</name>'.
         * \param node The node(tag) which contains the name.
         * \returns A string with the format "<name>data</name>". */
        string createDataLine(XMLNode* node);
        /*! Adds a series of lines to the lines_ collection of lines.
         * The lines are either opening tags, closing tags, or data tags.
         * Lines are added for the given node and all of its sub-nodes. 
         * \param nodeToAdd The node which is used to add xml lines to the list of lines(lines_). */
        void addNodeToLines(XMLNode* nodeToAdd);
        /*! Displays the list of lines(lines_) to the standard output device.
         * Is used for testing. */
        void display();
        /*! Writes the list of lines(lines_) to the file loacted at filePath_ and named fileName. 
         * \returns True if the xml file was successfully written.*/
        bool writeXMLFile();
        /*! Reads the xml file loacted at filePath_ and named fileName. It puts the data read into
         * the root XMLNode. 
         * \returns True if the xml file was read correctly. */
        bool readXMLFile();
        /*! Reads an opening tag(node) from the given line.
         * Expects the line to look like <name>.
         * \param line Normally a line which was read from a stream.
         * \returns The name associated with the opening tag(node). */
        string getOpeningTag(const string& line);
        /*! Reads an closing tag(node) from the given line.
         * Expects the line to look like </name>.
         * \param line Normally a line which was read from a stream.
         * \returns The name associated with the closing tag(node). */
        string getClosingTag(const string& line);
        /*! Reads a data tag(node) from the given line.
         * Expects the line to look like <name>data</name>.
         * \param line Normally a line which was read from a stream.
         * \returns The data associated with the data tag(node). */
        string getDataFromLine(const string& line);
        /*! Used to set the xml files location. 
         * It may add a seperator to the end of the path.
         * \param path The location for the xml file. */
        void setFilePath(string path);
        /*! Remove the root's sub-node with the given index(zero based).
         * \param nodeNo The index(zero based) of the node to be removed. */
        void removeRootSubNode(unsigned int nodeNo);
        /*! Appends a node to the end of the root's list of sub-nodes. 
         * \param node The node to be appended to the root's list of sub-nodes. */
        void appendRootSubNode(XMLNode* node);
        /*! Contains the maximum number of sub-nodes for the root. 
         * If nSubNodes Allowed is set to zero, then an infinite number of sub-nodes are allowed. */
        int nSubNodesAllowed;
        /*! Appends an "error" node to this xml data structure.
         * Normally this is only called to add to an error file.
         * \param message The message associated with the error. */
        void appendErrorNode(string message);
        /*! Get the data for the given tag.
         * \param tagName The name of the tag containing the data.
         * \return The data associated with the given tag. */
        string getDataFrom(string tagName);
        /*! Returns a pointer to the node with the given name from the array of sub-nodes.
         * \param node The node whose sub-nodes will be searched for a sub-node with the given name.
         * \param theName The name of the node being looked for.
         * \returns A pointer to the node. */
        XMLNode* findNode(XMLNode* node, string theName);
        /*! A list of lines which make up the contents of an xml file. */
        vector<string> lines_;
        
    private:
        /*! The standard string for the start of an xml file. */
        string xml_;
        /*! The location of the xml file. */
        string filePath_;
        
    };


}


#endif
