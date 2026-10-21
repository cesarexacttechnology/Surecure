//
//  ServerSocket.h
//  SimpleServer
//
//  Created by Mark Davis on 8/17/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#ifndef __SimpleServer__ServerSocket__
#define __SimpleServer__ServerSocket__

#include <stdio.h>
#include "Socket.h"


class ServerSocket : private Socket
{
public:
    
    ServerSocket ( int port );
    ServerSocket (){};
    /*! Sets the left server socket object equal to the right server socket object. */
//    ServerSocket& operator=(ServerSocket otherSocket);
    virtual ~ServerSocket();
    
    const ServerSocket& operator << ( const std::string& ) const;
    const ServerSocket& send( const unsigned char*, unsigned int count ) const;
    const ServerSocket& operator >> ( std::string& ) const;
    
    int receive(std::string& s);
    int receive(unsigned char* reply, unsigned int count);
    int poll(int ms);
    /*! Poll the socket for ms milliseconds to determine if it is OK to send data. */
    int pollSend(int ms);
    
    void accept ( Socket& );
    void closeServerSocket();
    /*! The socket's ip address. */
    unsigned long connectedAddress;
    
    void setConnectedAddress(unsigned long address);
    
    unsigned long getConnectedAddress();
    
    /*! \returns true if the socket exists. */
    bool exists();
    
    /*! \returns The descriptor for this socket.*/
    int getDescriptor();
    
};

#endif /* defined(__SimpleServer__ServerSocket__) */
