//
//  Socket.h
//  SimpleClient
//
//  Created by Mark Davis on 8/7/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#ifndef __SimpleClient__Socket__
#define __SimpleClient__Socket__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <sys/poll.h>
#include <netinet/tcp.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 4000;

class Socket
{
public:
    Socket();
    virtual ~Socket();
    
    // Server initialization
    bool create();
    bool bind ( const int port );
    bool listen() const;
    bool accept ( Socket& ) const;
    int accept (int server, Socket& newServer);
    int poll(int timeout);
    int pollSend(int timeout);
    
    // Client initialization
    bool connect ( const std::string host, const int port );
    
    // Data Transimission
    bool send ( const std::string ) const;
    bool send ( const unsigned char* s, unsigned int count ) const;
    int recv ( std::string& ) const;
    int recv ( unsigned char* reply, unsigned int count ) const;
    
    
    void set_non_blocking ( const bool );
    
    bool is_valid() const { return m_sock != -1; }
    
    /*! The file descriptor for the server socket. */
    int sockFileDescrip_;
    
    /*! Closes this socket. */
    void closeSocket();
    sockaddr_in m_addr;
    /*! \returns m_sock. m_sock is -1 if there is no socket. */
    int getdescriptor();
    int m_sock;

private:
    
    
};

#endif /* defined(__SimpleClient__Socket__) */
