//
//  CountModule.hpp
//  PlantManager
//
//  Created by Mark Davis on 12/2/15.
//  Copyright © 2015 Products Engineering. All rights reserved.
//

#ifndef CountModule_hpp
#define CountModule_hpp

#include <stdio.h>

class CountModule {
public:
    /*! The default constructor. */
    CountModule();
    /*! The default destructor. */
    ~CountModule();
    /*! \returns The address for this counter module. */
    int getAddress();
    /*! Sets the address for this counter module. Valid values are between 0 and 255.
     * \param value The desired address for this counter module. */
    void setAddress(unsigned int value);
    /*! \returns The counter value for this counter module. */
    int getCounter();
    /*! Increments the counter by one. */
    void incrementCounter();
    /*! Sets the counter to zero. */
    void resetCounter();
    
    
private:
    /*! Contains the address for this counter module. */
    unsigned int address_;
    /*! Contains the counter value for this module. */
    unsigned int counter_;
};

#endif /* CountModule_hpp */
