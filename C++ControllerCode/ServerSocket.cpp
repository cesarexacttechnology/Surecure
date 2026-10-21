//
//  ServerSocket.cpp
//  SimpleServer
//
//  Created by Mark Davis on 8/17/15.
//

// Implementation of the ServerSocket class

#include "ServerSocket.h"
#include "SocketException.h"
#include "string.h"


ServerSocket::ServerSocket ( int port )
{
    if ( ! Socket::create() )
    {
        throw SocketException ( "Could not create server socket." );
    }
    
    if ( ! Socket::bind ( port ) )
    {
        throw SocketException ( "Could not bind to port." );
    }
    
    if ( ! Socket::listen() )
    {
        throw SocketException ( "Could not listen to socket." );
    }
    
    //Socket::set_non_blocking(true);
    
}

/*
ServerSocket& ServerSocket::operator=(ServerSocket otherSocket) {
    setConnectedAddress(otherSocket.getConnectedAddress());
    return *this;
}
*/

ServerSocket::~ServerSocket()
{
    Socket::closeSocket();
}

int ServerSocket::poll(int ms) {
    return Socket::poll(ms);
}

int ServerSocket::pollSend(int ms) {
    return Socket::pollSend(ms);
}

const ServerSocket& ServerSocket::operator << ( const std::string& s ) const
{
    if ( ! Socket::send ( s ) )
    {
        throw SocketException ( "Could not write to socket." );
    }
    
    return *this;
    
}

const ServerSocket& ServerSocket::send(const unsigned char* s, unsigned int count ) const
{
    if ( ! Socket::send ( s, count ) )
    {
        throw SocketException ( "Could not write to socket." );
    }
    
    return *this;
    
}



const ServerSocket& ServerSocket::operator >> ( std::string& s ) const
{
    int result = Socket::recv(s);                           // Modified, receive the string from the socket.
    if ( result == -1 || result == 0 )                      // -1 indicates there was an error and 0 indicates there was no string.
    {                                                       // An exception is thrown for either an error or nothing to receive.
        throw SocketException ( "Could not read from socket." );
    }
    
    return *this;
}

int ServerSocket::receive(std::string& s )
{
    return Socket::recv(s);
}

int ServerSocket::receive(unsigned char* reply, unsigned int count )
{
    return Socket::recv(reply, count);
}


void ServerSocket::accept ( Socket& sock )
{
    if ( ! Socket::accept ( sock ) )            // Get the first connection request from the que on the listening socket.
//    if ( ! Socket::accept (m_sock, sock ) )            // Get the first connection request from the que on the listening socket.
    {                                           // There was an error.
        throw SocketException ( "Could not accept socket." );
    }
    std::cout << "The ServerSocket::accept handle is " << std::to_string(sock.getdescriptor()) << std::endl;
    std::cout << "The ServerSocket::accept address is " << std::to_string(sock.m_addr.sin_addr.s_addr) << std::endl;
}

void ServerSocket::setConnectedAddress(unsigned long address) {
    connectedAddress = address;
}

unsigned long ServerSocket::getConnectedAddress() {
    return connectedAddress;
}

void ServerSocket::closeServerSocket() {
    Socket::closeSocket();
}

bool ServerSocket::exists() {
    if(this->getdescriptor() == -1) {
        return false;
    }
    return true;
}

int ServerSocket::getDescriptor() {
    return this->Socket::getdescriptor();
}
