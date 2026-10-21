//
//  Beds.hpp
//  PlantManager
//
//  Created by Mark Davis on 3/13/17.
//  Copyright © 2017 Products Engineering. All rights reserved.
//

#ifndef Beds_hpp
#define Beds_hpp

#include <stdio.h>
#include <vector>
#include <string>
#include "SCProgram.h"
#include "SCXML.h"

namespace SCClasses {

    class SCBedChnl {
    public:
        /*! The default constructor. */
        SCBedChnl();
        /*! The default destructor. */
        virtual ~SCBedChnl();
        /*! Set to true if this bedChnl is assigned at runtime.*/
        bool isAssigned;
        /*! \returns The SCChnl index associated with this bedChnl. -1 indicates the bedChnl is not associated with
         * an SCChnl. -2 indicates this bedChnl will be associated to an SCChnl at runtime. */
        int getSCChnlDex();
        /*! Sets the desired scChnlDex for this bedChnl.
         * \param scChnlDex The SCChnl index this bedChnl is going to be associated with.
         * -1 means this bedChnl is not associated to an SCChnl. -2 means this bedChnl will be associated at runtime. */
        void setSCChnlDex(int scChnlDex);
        /*! \returns The SCChnl index this bedChnl is referenced to. -1 indicates this bedChnl is not referenced
         * to another SCChnl. */
        int getRefSCChnlDex();
        /*! Sets the desired scChnlDex for this bedChnl's reference.
         * \param scChnlDex The SCChnl index this bedChnl is going to be referenced to.
         * -1 means this bedChnl is not going to be refenced to another SCChnl. */
        void setRefSCChnlDex(int scChnlDex);
        /*! \returns The SCChnl index associated with this bedChnl at runtime. 
         * -1 indicates this bedChnl has not been associated with an SCChnl yet. */
        int getAssignedSCChnlDex();
        /*! \returns true if the desired scChnlDex can legaly be associated with this bedChnl.
         * \param scChnlDex The SCChnl index this bedChnl is going to be associated with.
         * -1 means this bedChnl is not associated to an SCChnl yet. */
        void setAssignedSCChnlDex(int scChnlDex);
        /*! \returns The SCChnl index referenced by this bedChnl at runtime.
         * -1 indicates this bedChnl has not been assigned a reference yet. */
        int getAssignedRefDex();
        /*! Set the SCBedChnl index this assigned channel will be referenced to.
         * \param bedChnlDex The SCBedChnl index this bedChnl is going to be referenced to.
         * -1 means this bedChnl is not referenced to another channel on its bed. */
        void setAssignedRefDex(int bedChnlDex);
        /*! The type of control to be applied to this bedChnl. */
        SCProgram::Type controlType;
        /*! \returns This bedChnl's index into the list of BedChnls for the SCBed this bedChnl belongs. */
        int getBedChnlIndex();
        /*! Sets the desired index into the list of BedChnls for the SCBed this bedChnl belongs.
         * The index is only set when this bedChnl is added to bedChnl list. */
        void setIndex(int nDex);
        /*! Gets this bedChnl's configuration from the given XMLNode.
         * \param xmlBedChnl The xmlNode contianing this bedChnl's configuration. */
        bool load(XMLNode* xmlBedChnl);
        /*! Puts this bedChnl's configuration into the xmlBed XMLNode object.
         *  \returns true If the configuration is successfully put into xmlBedChnl.
         *  \param xmlBedChnl An XMLNode object containg the configuration of this bed channel.*/
        bool save(XMLNode* xmlBedChnl);
        /*! Contains an error message which provides additional information if a function fails. */
        string errorMessage;
    private:
        /*! The SCChnl index of associated with the bedChnl. */
        int chnlDex_;
        /*! The index of the bedChnl to which this bedChnl is referenced. */
        int refDex_;
        /*! The SCChnl index assigned to this bedChnl at runtime. */
        int assignedChnlDex_;
        /*! The index of another channel on this chnl's bed which this chnl will be referenced to.
         *  It is in the form of an SCBedChnl index(see index_). 
         *  For example: If the bed has three channels and the third one is a cylinder which is assigned 
         *  at runtime to follow the first channel on this bed, the assignedRefDex_ would be zero. */
        int assignedRefDex_;
        /*! The zero based index of this SCBedChnl in the bedChnl list. 
         *  The index may be changed by adding other bedChnls to the list of BedChnls for the SCBed this bedChnl belongs. */
        int index_;
    };
    
    class SCBed {
    public:
        /*! The default constructor. */
        SCBed();
        /*! The copy constructor. */
        SCBed(const SCBed& other);
        /*! The default destructor. */
        virtual ~SCBed();
        /*! Sets the left bed object equal to the right bed object. */
        SCBed& operator=(const SCBed& other);
        /*! \returns The name of this bed. */
        string getName();
        /*! Sets the name of this bed.
         * \param name The desired name for this bed. The Character '-' may not be part of the name. */
        void setName(string name);
        /*! Adds the given SCBedChnl to the vector of SCBedChnls provided the bedChnl to add does not have
         *  the same SCChnl index as a bedChnl which is already on the list.
         * \param bedChnl* A pointer to the bedChnl to be added. */
        void addBedChnl(SCBedChnl* bedChnl);
        /*! Removes the given bedChnl with the given index from the vector of SCBedChnls.
         * \param nDex The index of the SCBedChnl to remove(zero based). */
        void removeBedChnl(int nDex);
        /*! \returns The SCBedChnl index of the SCBedChnl using the given SCChnl index. 
         *   If an SCBedChnl doesn't exist, -1 is returned.
         * \param scDex The SCChnl index used by the SCBedChnl which is being searched for. */
        int findBedChnl(int scDex);
        /*! \returns This bed's index. (zero based) */
        int getIndex();
        /*! Sets the desired index for this bed. It may be changed when the bed is added. 
         *  \param This bed's desired index. */
        void setIndex(int value);
        /*! Puts this SCBed's configuration in the given XMLNode.
         * \param xmlBed The xmlNode contianing this bed's configuration. */
        bool save(XMLNode* xmlBed);
        /*! Gets this SCBed's configuration from the given XMLNode.
         * \param xmlBed The xmlNode contianing this bed's configuration. */
        bool load(XMLNode* xmlBed);
        /*! \returns The number of chnls associated with this bed. */
        int getNoOfBedChnls();
        /*! Contains an error message which provides additional information if a function fails. */
        string errorMessage;
        /*! \returns The bed chnl with the given index.
         *  If the index is invalid, the function returns nullptr. */
        SCBedChnl* getBedChnl(unsigned int bedChnlDex);
        /*! Sets the bed configuration from the given XML string.
         *  The string contains <Bed>Name<\Bed> at the start to indicate which bed this is.
         * \returns true if the operation is successful.
         *  \param dat Contains the bed name as well as the bed configuration in standard XML format. */
        //bool setFromXML(string data);
    private:
        /*! This bed's name. */
        string name_;
        /*! A vector contains the SCBedChnls which make up this bed. */
        vector<SCBedChnl*> bedChnls_;
        /*! This beds index, zero based. */
        int index_;
        
    };
    
    class SCBeds: public std::vector<SCBed*> {
    public:
        /*! The default constructor. */
        SCBeds();
        /*! The default destructor. */
        virtual ~SCBeds();
        /*! The path where the bed configurations file is located. */
        string filePath;
        /*! The number of minutes the cylinder continues to be controlled after the bed's master chnl finishes. */
        unsigned int cylinderCureTime;
        /*! */
        void add();
        /*! */
        void add(SCBed* aBed);
        /*! */
        void remove(unsigned int bedDex);
        /*! */
        void remove(string bedName);
        /*! */
        int getBedDex(string bedName);
        /*! Removes all the beds and frees the memory allocated to them. */
        void clearBeds();
        /*! A pointer to the SCXML object containing recent error data for the Plant Manager. */
        SCXML* xmlErrors;
        /*! \returns true If the given name is used by any one of the SCBeds.*/
        bool nameExists(string name);
        /*! Contains an error message which provides additional information if a function fails. */
        string errorMessage;
        /*! \returns an XML version of the SCBeds. */
        string createXMLVersion();
    private:
        /*! The number of beds. */
        unsigned int noBeds_;
        /*! The version of how the beds are stored. */
        unsigned int version_;
        /*! The version of how the bed channels are stored. */
        unsigned int bedChnlVersion_;
    };
}

#endif /* Beds_hpp */
