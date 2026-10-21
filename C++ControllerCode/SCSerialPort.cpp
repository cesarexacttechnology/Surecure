//
//  SCSerialPort.cpp
//  PlantManager
//
//  Created by Mark Davis on 4/28/16.
//  Copyright © 2016 Products Engineering. All rights reserved.
//

#include "SCSerialPort.hpp"

SCSerialPort::SCSerialPort() {
    ;
}

SCSerialPort::~SCSerialPort() {
    ;
}

int SCSerialPort::openSerialPort(const char *bsdPath) {
    int				fileDescriptor = -1;
    //    int				handshake;
    struct termios options;
    
    // Open the serial port read/write, with no controlling terminal, and don't wait for a connection.
    // The O_NONBLOCK flag also causes subsequent I/O on the device to be non-blocking.
    // See open(2) <x-man-page://2/open> for details.
    
    fileDescriptor = open(bsdPath, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fileDescriptor == -1) {
        printf("Error opening serial port %s - %s(%d).\n",
               bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1) {
            close(fileDescriptor);
        }
        
        return -1;
    }
    
    // Note that open() follows POSIX semantics: multiple open() calls to the same file will succeed
    // unless the TIOCEXCL ioctl is issued. This will prevent additional opens except by root-owned
    // processes.
    // See tty(4) <x-man-page//4/tty> and ioctl(2) <x-man-page//2/ioctl> for details.
    
    if (ioctl(fileDescriptor, TIOCEXCL) == -1) {
        printf("Error setting TIOCEXCL on %s - %s(%d).\n",
               bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1) {
            close(fileDescriptor);
        }
        
        return -1;
    }
    
    // Now that the device is open, clear the O_NONBLOCK flag so subsequent I/O will block.
    // See fcntl(2) <x-man-page//2/fcntl> for details.
    
    if (fcntl(fileDescriptor, F_SETFL, 0) == -1) {
        printf("Error clearing O_NONBLOCK %s - %s(%d).\n",
               bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1) {
            close(fileDescriptor);
        }
        
        return -1;
    }
    
    // Get the current options and save them so we can restore the default settings later.
    if (tcgetattr(fileDescriptor, &gOriginalTTYAttrs) == -1) {
        printf("Error getting tty attributes %s - %s(%d).\n",
               bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1) {
            close(fileDescriptor);
        }
        
        return -1;
    }
    
    // The serial port attributes such as timeouts and baud rate are set by modifying the termios
    // structure and then calling tcsetattr() to cause the changes to take effect. Note that the
    // changes will not become effective without the tcsetattr() call.
    // See tcsetattr(4) <x-man-page://4/tcsetattr> for details.
    
    options = gOriginalTTYAttrs;
    
    // Print the current input and output baud rates.
    // See tcsetattr(4) <x-man-page://4/tcsetattr> for details.
    
    printf("Current input baud rate is %d\n", (int) cfgetispeed(&options));
    printf("Current output baud rate is %d\n", (int) cfgetospeed(&options));
    
    // Set raw input (non-canonical) mode, with reads blocking until either a single character
    // has been received or a one second timeout expires.
    // See tcsetattr(4) <x-man-page://4/tcsetattr> and termios(4) <x-man-page://4/termios> for details.
    
    cfmakeraw(&options);
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 10;
    
    // The baud rate, word length, and handshake options can be set as follows:
    
    /*
     cfsetspeed(&options, B19200);		// Set 19200 baud
     options.c_cflag |= (CS7 	   | 	// Use 7 bit words
     PARENB	   | 	// Parity enable (even parity if PARODD not also set)
     CCTS_OFLOW | 	// CTS flow control of output
     CRTS_IFLOW);	// RTS flow control of input
     */
    // The IOSSIOSPEED ioctl can be used to set arbitrary baud rates
    // other than those specified by POSIX. The driver for the underlying serial hardware
    // ultimately determines which baud rates can be used. This ioctl sets both the input
    // and output speed.
    /*
     speed_t speed = 14400; // Set 14400 baud
     if (ioctl(fileDescriptor, IOSSIOSPEED, &speed) == -1) {
     printf("Error calling ioctl(..., IOSSIOSPEED, ...) %s - %s(%d).\n",
     bsdPath, strerror(errno), errno);
     }
     */
    // Print the new input and output baud rates. Note that the IOSSIOSPEED ioctl interacts with the serial driver
    // directly bypassing the termios struct. This means that the following two calls will not be able to read
    // the current baud rate if the IOSSIOSPEED ioctl was used but will instead return the speed set by the last call
    // to cfsetspeed.
    
    printf("Input baud rate changed to %d\n", (int) cfgetispeed(&options));
    printf("Output baud rate changed to %d\n", (int) cfgetospeed(&options));
    
    // Cause the new options to take effect immediately.
    if (tcsetattr(fileDescriptor, TCSANOW, &options) == -1) {
        printf("Error setting tty attributes %s - %s(%d).\n",
               bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1) {
            close(fileDescriptor);
        }
        
        return -1;
    }
    
    
    /*
     // To set the modem handshake lines, use the following ioctls.
     // See tty(4) <x-man-page//4/tty> and ioctl(2) <x-man-page//2/ioctl> for details.
     
     // Assert Data Terminal Ready (DTR)
     if (ioctl(fileDescriptor, TIOCSDTR) == -1) {
     printf("Error asserting DTR %s - %s(%d).\n",
     bsdPath, strerror(errno), errno);
     }
     
     // Clear Data Terminal Ready (DTR)
     if (ioctl(fileDescriptor, TIOCCDTR) == -1) {
     printf("Error clearing DTR %s - %s(%d).\n",
     bsdPath, strerror(errno), errno);
     }
     
     // Set the modem lines depending on the bits set in handshake
     handshake = TIOCM_DTR | TIOCM_RTS | TIOCM_CTS | TIOCM_DSR;
     if (ioctl(fileDescriptor, TIOCMSET, &handshake) == -1) {
     printf("Error setting handshake lines %s - %s(%d).\n",
     bsdPath, strerror(errno), errno);
     }
     
     // To read the state of the modem lines, use the following ioctl.
     // See tty(4) <x-man-page//4/tty> and ioctl(2) <x-man-page//2/ioctl> for details.
     
     // Store the state of the modem lines in handshake
     if (ioctl(fileDescriptor, TIOCMGET, &handshake) == -1) {
     printf("Error getting handshake lines %s - %s(%d).\n",
     bsdPath, strerror(errno), errno);
     }
     
     printf("Handshake lines currently set to %d\n", handshake);
     */
    unsigned long mics = 1UL;
    
    // Set the receive latency in microseconds. Serial drivers use this value to determine how often to
    // dequeue characters received by the hardware. Most applications don't need to set this value: if an
    // app reads lines of characters, the app can't do anything until the line termination character has been
    // received anyway. The most common applications which are sensitive to read latency are MIDI and IrDA
    // applications.
    
    if (ioctl(fileDescriptor, IOSSDATALAT, &mics) == -1) {
        // set latency to 1 microsecond
        printf("Error setting read latency %s - %s(%d).\n",
               bsdPath, strerror(errno), errno);
        if (fileDescriptor != -1) {
            close(fileDescriptor);
        }
        return -1;
    }
    
    // Success
    return fileDescriptor;
}

kern_return_t SCSerialPort::findSerialPorts(io_iterator_t *matchingServices) {
    kern_return_t			kernResult;
    CFMutableDictionaryRef	classesToMatch;
    
    // Serial devices are instances of class IOSerialBSDClient.
    // Create a matching dictionary to find those instances.
    classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
 /*
    if (classesToMatch == NULL) {
        printf("IOServiceMatching returned a NULL dictionary.\n");
    }
    else {
        // Look for devices that claim to be modems.
        CFDictionarySetValue(classesToMatch,
                             CFSTR(kIOSerialBSDTypeKey),
                             //                             CFSTR(kIOSerialBSDModemType));
                             CFSTR(kIOSerialBSDAllTypes));
        
        // Each serial device object has a property with key
        // kIOSerialBSDTypeKey and a value that is one of kIOSerialBSDAllTypes,
        // kIOSerialBSDModemType, or kIOSerialBSDRS232Type. You can experiment with the
        // matching by changing the last parameter in the above call to CFDictionarySetValue.
        
        // As shipped, this sample is only interested in modems,
        // so add this property to the CFDictionary we're matching on.
        // This will find devices that advertise themselves as modems,
        // such as built-in and USB modems. However, this match won't find serial modems.
    }
    
    // Get an iterator across all matching devices.
    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, matchingServices);
    if (KERN_SUCCESS != kernResult) {
        printf("IOServiceGetMatchingServices returned %d\n", kernResult);
    }
  */
    return kernResult;
}

