//
//  Socket.cpp
//  SimpleClient
//
//  Created by Mark Davis on 8/7/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#include "Socket.h"
#include "string.h"
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
using namespace std;

Socket::Socket() :
m_sock ( -1 )
{
    memset ( &m_addr,
            0,
            sizeof ( m_addr ) );
    
}

Socket::~Socket()
{
    if ( is_valid() )
        ::close ( m_sock );
}

bool Socket::create()
{
    m_sock = socket ( AF_INET,
                     SOCK_STREAM,
                     0 );
    
    
    if ( ! is_valid() )
        return false;
    
    
    // TIME_WAIT - argh
    int on = 1;
    if ( setsockopt ( m_sock, SOL_SOCKET, SO_REUSEADDR, ( const char* ) &on, sizeof ( on ) ) == -1 )
        return false;

    int flag = 1;
    if ( setsockopt ( m_sock, IPPROTO_TCP, TCP_NODELAY, ( const char* ) &flag, sizeof ( flag ) ) == -1 )
        return false;

    
    return true;
    
}



bool Socket::bind ( const int port )
{
    
    if ( ! is_valid() )
    {
        return false;
    }
    
    
    
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = INADDR_ANY;
    m_addr.sin_port = htons ( port );
    
    int bind_return = ::bind ( m_sock,
                              ( struct sockaddr * ) &m_addr,
                              sizeof ( m_addr ) );
    
    
    if ( bind_return == -1 )
    {
        return false;
    }
    
    return true;
}


bool Socket::listen() const
{
    if ( ! is_valid() )
    {
        return false;
    }
    
//    int listen_return = ::listen ( m_sock, MAXCONNECTIONS );
    int listen_return = ::listen ( m_sock, 5 );

    
    if ( listen_return == -1 )
    {
        return false;
    }
    
    return true;
}


bool Socket::accept ( Socket& new_socket ) const
{
    int addr_length = sizeof ( m_addr );    // Get the size of the soon to be connected socekt address.
    cout << "The handle being accepted on is " << to_string(m_sock) << endl;
//    new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &m_addr, ( socklen_t * ) &addr_length );// Accept the new conenction.
    new_socket.m_sock = ::accept ( m_sock, ( sockaddr * ) &new_socket.m_addr, ( socklen_t * ) &addr_length );// Accept the new conenction.
                                            // new_socket.m_sock contains the descriptor of the newly connected socket.
//    new_socket.m_addr = m_addr;             // The new socket must have its IP address assigned.
//    cout << "The accept handle is " << to_string(new_socket.m_sock) << endl;
//    cout << "The accept address is " << to_string(new_socket.m_addr.sin_addr.s_addr) << endl;
                                            // It will be used by the caller to determine what is trying to connect.
    if ( new_socket.m_sock <= 0 )           // Has an error occured?
        return false;                       // Yes. Return false(failure).
    else                                    // No. The new connection was successfully established.
        return true;                        // Return true(success).
}

int Socket::accept(int server, Socket& new_socket) {
    int addr_length = sizeof ( new_socket.m_addr );    // Get the size of the soon to be connected socekt address.
    cout << "The accept address length is " << to_string(addr_length) << endl;
    new_socket.m_sock = ::accept ( server, ( sockaddr * ) &new_socket.m_addr, ( socklen_t * ) &addr_length );// Accept the new conenction.
                                            // new_socket.m_sock contains the descriptor of the newly connected socket.
//    new_socket.m_addr = m_addr;             // The new socket must have its IP address assigned.
                                            // It will be used by the caller to determine what is trying to connect.
    return new_socket.m_sock;
}


bool Socket::send ( const std::string s ) const
{
    int status = (int)::send ( m_sock, s.c_str(), s.size(), /*MSG_NOSIGNAL*/ /*MSG_SEND*/ MSG_DONTWAIT );
    if ( status == -1 )
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool Socket::send ( const unsigned char* s, unsigned int count ) const
{
    int status = (int)::send ( m_sock, s, count, /*MSG_NOSIGNAL*/ /*MSG_SEND*/ MSG_DONTWAIT );
    if ( status == -1 )
    {
        return false;
    }
    else
    {
        return true;
    }
}



int Socket::recv ( std::string& s ) const
{
    char buf [ MAXRECV + 1 ];
    
    s = "";
    
    memset ( buf, 0, MAXRECV + 1 );
    
    int status = (int)::recv ( m_sock, buf, MAXRECV, 0 );
    
    if ( status == -1 )
    {
        std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
        return -1;                      // Modified, returns -1 if there is an error. Previously, returned 0.
    }
    else if ( status == 0 )
    {
        return 0;
    }
    else
    {
        s = buf;
        return status;
    }
}


int Socket::recv ( unsigned char* reply, unsigned int count ) const
{
    int status = (int)::recv ( m_sock, reply, count, 0 );
    
    if ( status == -1 )
    {
        std::cout << "status == -1   errno == " << errno << "  in Socket::recv\n";
        return -1;                      // Modified, returns -1 if there is an error. Previously, returned 0.
    }
    else if ( status == 0 )
    {
        return 0;
    }
    else
    {
        return status;
    }
}



bool Socket::connect ( const std::string host, const int port )
{
    if ( ! is_valid() ) return false;
    
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons ( port );
    
    int status = inet_pton ( AF_INET, host.c_str(), &m_addr.sin_addr );
    
    if ( errno == EAFNOSUPPORT || status < 1) return false;
    
    status = ::connect ( m_sock, ( sockaddr * ) &m_addr, sizeof ( m_addr ) );
    
    if ( status == 0 )
        return true;
    else
        return false;
}

void Socket::set_non_blocking ( const bool b )
{
    
    int opts;
    
    opts = fcntl ( m_sock,
                  F_GETFL );
    
    if ( opts < 0 )
    {
        return;
    }
    
    if ( b )
        opts = ( opts | O_NONBLOCK );
    else
        opts = ( opts & ~O_NONBLOCK );
    
    fcntl ( m_sock,
           F_SETFL,opts );
    
}

int Socket::poll(int timeout)
{
    int rv = 0;
    struct pollfd ufds;
    ufds.fd = m_sock;
    ufds.events = POLLIN;
    // wait for events on the sockets, timeout milliseconds
    rv = ::poll(&ufds, 1, timeout);
    
    return rv;
}

int Socket::pollSend(int timeout)
{
    int rv = 0;
    struct pollfd ufds;
    ufds.fd = m_sock;
    ufds.events = POLLOUT;
    // wait for events on the sockets, timeout milliseconds
    rv = ::poll(&ufds, 1, timeout);
    
    return rv;
}


void Socket::closeSocket() {
    if ( is_valid() )
        close(m_sock);
}

int Socket::getdescriptor() {
    return m_sock;
}

