//
//  SerialPort.cpp
//  PlantManager
//
//  Created by Mark Davis on 10/24/17.
//  Copyright © 2017 Products Engineering. All rights reserved.
//

#include "SerialPort.hpp"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>

/*
int SerialPort::connect() {
//    char device[] = "//dev//ttyS0";
    char device[] = "//dev//cu.usbserial-A6004vGT";
    return connect(device);
}
*/
SerialPort::SerialPort() {
    name_ = "";
    bRate_ = B9600;
}

int SerialPort::connect() {
    int n = (int)name_.length();        // Get the length of the device name to open.
 
   // declaring character array
   char device[n + 1];
 
   // copying the contents of the
   // string to char array
   strcpy(device, name_.c_str());

    
    struct termios terminalAttributes;
    
    /*
     * http://linux.die.net/man/2/open
     *
     * Open the serial port
     * read/write
     * not become the process's controlling terminal
     * When possible, the file is opened in nonblocking mode
     *
     */
    fileDescriptor = open(device, O_RDWR | O_NOCTTY | O_NDELAY | O_FSYNC );
    
    // clear terminalAttributes data
    memset(&terminalAttributes, 0, sizeof(struct termios));
    
    /*    http://linux.die.net/man/3/termios
     *
     *  control modes: c_cflag flag constants:
     *
     * 57600 bauds
     * We are going to use 9600 baud
     * 8 bits per word
     * Ignore modem control lines.
     * Enable receiver.
     */
    
//    terminalAttributes.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    terminalAttributes.c_cflag = bRate_ | CS8 | CLOCAL | CREAD;
    std::cout << "The baud rate to connect at is: " << std::to_string(bRate_) << std::endl;

    /*
     * input modes: c_iflag flag constants:
     *
     * Ignore framing errors and parity errors.
     * (XSI) Map NL to CR-NL on output.
     */
    terminalAttributes.c_iflag = IGNPAR/* |  ONLCR*/;
    
    /*
     * output modes: flag constants defined in POSIX.1
     *
     * Enable implementation-defined output processing.
     */
    
    terminalAttributes.c_oflag = OPOST;
    
    /*
     * Canonical and noncanonical mode
     *
     * min time
     * min bytes to read
     */
    
    //terminalAttributes.c_lflag = ICANON;
    terminalAttributes.c_cc[VTIME] = 0;
    terminalAttributes.c_cc[VMIN] = 1;
    
    /*
     * http://linux.die.net/man/3/tcsetattr
     * Set the port to our state
     *
     * the change occurs immediately
     */
    
    tcsetattr(fileDescriptor, TCSANOW, &terminalAttributes);
    
    /*
     * http://linux.die.net/man/3/tcflush
     *
     * flushes data written but not transmitted.
     * flushes data received but not read.
     */
    
    tcflush(fileDescriptor, TCOFLUSH);
    tcflush(fileDescriptor, TCIFLUSH);
    
    return fileDescriptor;
}

int SerialPort::connect2() {
    int n = (int)name_.length();    // Get the length of the device name to open.
    char device[n + 1];             // declaring character array
    strcpy(device, name_.c_str());  // Copying the contents of the name to a char array.
    int fileDescriptor = open(
             device,                // the name of the serial port as a c-string (char *) eg. /dev/ttys0
             // configuration options
             // O_RDWR - we need read
             //     and write access
             // O_CTTY - prevent other
             //     input (like keyboard)
             //     from affecting what we read
             // O_NDELAY - We don't care if
             //     the other side is
             //     connected (some devices
             //     don't explicitly connect)
             O_RDWR | O_NOCTTY | O_NDELAY
        );
    if(fileDescriptor == -1) {
        return fileDescriptor;
    }
    // create the struct
    struct termios options;

    // get the current settings of the
    // serial port
    tcgetattr(fileDescriptor, &options);

    // set the read and write speed to
    // 19200 BAUD
    // All speeds can be prefixed with B
    // as a settings.
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    // now to set the other settings
    // The first will be no parity,
    // Assume 8-bit words

    /**********************************
    *
    *     no parity example
    *
    **********************************/

    // PARENB is enable parity bit
    // so this disables the parity bit
    options.c_cflag &= ~PARENB;

    // CSTOPB means 2 stop bits
    // otherwise (in this case)
    // only one stop bit
    options.c_cflag &= ~CSTOPB;

    // CSIZE is a mask for all the
    // data size bits, so anding
    // with the negation clears out
    // the current data size setting
    options.c_cflag &= ~CSIZE;

    // CS8 means 8-bits per work
    options.c_cflag |= CS8;

    // Set the timeouts
    // VMIN is the minimum amount
    // of characters to read.
    options.c_cc[VMIN] = 0;

    // The amount of time to wait
    // for the amount of data
    // specified by VMIN in tenths
    // of a second.
    options.c_cc[VTIME] = 1;

    // CLOCAL means don't allow
    // control of the port to be changed
    // CREAD says to enable the receiver
    options.c_cflag |= (CLOCAL | CREAD);

    // apply the settings to the serial port
    // TCSNOW means apply the changes now
    // other valid options include:
    //    TCSADRAIN - wait until every
    //        thing has been transmitted
    //    TCSAFLUSH - flush buffers
    //        and apply changes
    if(tcsetattr(fileDescriptor, TCSANOW, &options)!= 0) {
        return -1;
       // error code goes here

    }
    return fileDescriptor;
}

int SerialPort::connect3() {
    int n = (int)name_.length();    // Get the length of the device name to open.
    char device[n + 1];             // declaring character array
    strcpy(device, name_.c_str());  // Copying the contents of the name to a char array.
    fileDescriptor = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if(fileDescriptor == -1) // if open is unsucessful
    {
        return fileDescriptor;
    }
    else
    {
        fcntl(fileDescriptor, F_SETFL, 0);
    }
    struct termios port_settings;      // structure to store the port settings in

    cfsetispeed(&port_settings, B9600);    // set baud rates
    cfsetospeed(&port_settings, B9600);

    port_settings.c_cflag &= ~PARENB;    // set no parity, stop bits, data bits
    port_settings.c_cflag &= ~CSTOPB;
    port_settings.c_cflag &= ~CSIZE;
    port_settings.c_cflag |= CS8;
    
    tcsetattr(fileDescriptor, TCSANOW, &port_settings);    // apply the settings to the port
    return(fileDescriptor);
}

int SerialPort::connect4() {
    // Open the serial port.
    int n = (int)name_.length();            // Get the length of the device name to open.
    char device[n + 1];                     // declaring character array
    strcpy(device, name_.c_str());          // Copying the contents of the name to a char array.
    fileDescriptor = open(device, O_RDWR);  // Open the serial port.
    
    struct termios tty;                     // Create new termios struc, we call it 'tty' for convention.
    memset(&tty, 0, sizeof tty);            // Set each byte of memory allocated to tty to 0.
    if(tcgetattr(fileDescriptor, &tty) != 0) {// Read in existing settings, and handle any error.
        std::cout << "Error get existing serial port setting: " << strerror(errno) << std::endl;
        return -1;                          // The caller is allerted to the error by returning -1.
    } // End of condition: The serial port attributes couldn't be obtained.
    tty.c_cflag &= ~PARENB;                 // Clear parity bit, disabling parity (most common).
    tty.c_cflag &= ~CSTOPB;                 // Clear stop field, only one stop bit used in communication (most common).
    tty.c_cflag |= CS8;                     // 8 bits per byte (most common).
    tty.c_cflag &= ~CRTSCTS;                // Disable RTS/CTS hardware flow control (most common).
    tty.c_cflag |= CREAD | CLOCAL;          // Turn on READ & ignore ctrl lines (CLOCAL = 1)
    tty.c_lflag &= ~ICANON;                 // Disable canonical mode.
    tty.c_lflag &= ~ECHO;                   // Disable echo.
    tty.c_lflag &= ~ECHOE;                  // Disable erasure.
    tty.c_lflag &= ~ECHONL;                 // Disable new-line echo.
    tty.c_lflag &= ~ISIG;                   // Disable interpretation of INTR, QUIT and SUSP.
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl.
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes.
    tty.c_oflag &= ~OPOST;                  // Prevent special interpretation of output bytes (e.g. newline chars).
    tty.c_oflag &= ~ONLCR;                  // Prevent conversion of newline to carriage return/line feed.
    tty.c_cc[VTIME] = 10;                   // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;                     // Start accumulating VTIME for a time out as soon as read is called.
    // Set in/out baud rates.
    if(bRate_ == B300) {                    // Is the baud rate supposed to be 300?
        cfsetispeed(&tty, B300);            // Set the receive speed to 300.
        cfsetospeed(&tty, B300);            // Set the send speed to 300.
    } else if (bRate_ == B4800) {           // Is the baud rate supposed to be 4800?
        cfsetispeed(&tty, B4800);           // Set the receive speed to 4800.
        cfsetospeed(&tty, B4800);           // Set the send speed to 4800.
    } else {                                // Is the baud rate supposed to be 9600?
        cfsetispeed(&tty, B9600);           // Set the receive speed to 9600.
        cfsetospeed(&tty, B9600);           // Set the send speed to 9600.
    } // End of condition: Set the poarts baud rate to the desired rate.
    // Save tty settings.
    if (tcsetattr(fileDescriptor, TCSANOW, &tty) != 0) {// Was there an error?
        std::cout << "Error get setting serial port configuration: " << strerror(errno) << std::endl;// Yes. Display it.
        return -1;                          // The caller is alerted to the error by returning -1.
    } // End of condition: The serial port attributes couldn't be set.
    return fileDescriptor;                  // Return the file descriptor, a positive.
}

void SerialPort::disconnect(void)
{
    close(fileDescriptor);
    printf("Serial port has been CLOSED and %d is the file description.\n", fileDescriptor);
}

int SerialPort::sendArray(unsigned char *buffer, int len) {
    int n = (int)write(fileDescriptor, buffer, len);
    return n;
}

int SerialPort::getArray (unsigned char *buffer, int len)
{
//    int n1 = bytesToRead();
    int n = (int)read(fileDescriptor, buffer, len);
    return n;
}

void SerialPort::clear()
{
    tcflush(fileDescriptor, TCIFLUSH);
    tcflush(fileDescriptor, TCOFLUSH);
}

int SerialPort::bytesToRead()
{
    int bytes=0;
    ioctl(fileDescriptor, FIONREAD, &bytes);
    
    return bytes;
}

int SerialPort::bytesToSend() {
    int bytes = 0;
    ioctl(fileDescriptor, TIOCOUTQ, &bytes);
    
    return bytes;
}

bool SerialPort::enableDTR() {
    int status;
    if(ioctl(fileDescriptor, TIOCMGET, &status) == -1) {
        return false;
    }
    status |= TIOCM_DTR;    /* turn on DTR */
    status &= ~TIOCM_RTS;    /* turn off RTS */

    if(ioctl(fileDescriptor, TIOCMSET, &status) == -1) {
        return false;
    }
    return true;
}

void SerialPort::setName(std::string name) {
    name_ = name;           // Set the port's name.
}

void SerialPort::setBaudRate(int bRate) {
    if(bRate == 300) {
        bRate_ = B300;
    } else if (bRate == 4800) {
        bRate_ = B4800;
    } else {
        bRate_ = B9600;
    }
}

void SerialPort::changeBaudRate() {
    struct termios options;

    /*
     * Get the current options for the port...
     */

    tcgetattr(fileDescriptor, &options);

    /*
     * Set the baud rates to 19200...
     */

    cfsetispeed(&options, bRate_);
    cfsetospeed(&options, bRate_);

    /*
     * Enable the receiver and set local mode...
     */

    options.c_cflag |= (CLOCAL | CREAD);

    /*
     * Set the new options for the port...
     */

    tcsetattr(fileDescriptor, TCSANOW, &options);

}

