//
//  SCSerialPort.hpp
//  PlantManager
//
//  Created by Mark Davis on 4/28/16.
//  Copyright © 2016 Products Engineering. All rights reserved.
//

#ifndef SCSerialPort_hpp
#define SCSerialPort_hpp

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <paths.h>
#include <termios.h>
#include <sysexits.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/serial/ioss.h>
#include <IOKit/IOBSD.h>


class SCSerialPort
{
public:
    SCSerialPort();
    virtual ~SCSerialPort();

    int openSerialPort(const char *bsdPath);
    struct termios gOriginalTTYAttrs;
    kern_return_t findSerialPorts(io_iterator_t *matchingServices);
    kern_return_t getSerialPortPath(io_iterator_t serialPortIterator, char *bsdPath, CFIndex maxPathSize);
};

    
#endif /* SCSerialPort_hpp */
