//
//  SerialPort.hpp
//  PlantManager
//
//  Created by Mark Davis on 10/24/17.
//  Copyright © 2017 Products Engineering. All rights reserved.
//

#ifndef SerialPort_hpp
#define SerialPort_hpp

#include <stdio.h>
#include <string>
#include <string.h>
#include <termios.h>            // Contains POSIX terminal control definitions.
#include <fcntl.h>              // Contains file controls like O_RDWR.
#include <errno.h>              // Error integer and strerror() function.
#include <unistd.h>             // write(), read(), close()

class SerialPort {
private:
    int fileDescriptor;
    std::string name_;
    tcflag_t bRate_;
    
public:
    SerialPort();
    int connect ();
    /*! Connect to the serial port using linux manual method. */
    int connect2();
    /*! Connect to the serial port using linux manual method. */
    int connect3();
    /*! Connect to the serial port using https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/ method. */
    int connect4();
//    int connect (char * device);
    void disconnect(void);
    
    int sendArray(unsigned char *buffer, int len);
    int getArray (unsigned char *buffer, int len);
    
    /*! /returns The number of bytes to be read. */
    int bytesToRead();
    /*! /return The number of bytes to be sent. */
    int bytesToSend();
    void clear();
    bool enableDTR();
    /*! Sets the ports name.
     * \param name The port's name. */
    void setName(std::string name);
    /*! Sets the port's baud rate.
     * \param bRate The port's baud rate. */
    void setBaudRate(int bRate);
    void changeBaudRate();
};

#endif /* SerialPort_hpp */
