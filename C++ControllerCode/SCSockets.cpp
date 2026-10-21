
//
//  SCSockets.cpp
//  PlantManager
//
//  Created by Mark Davis on 4/14/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//


#include "SCSockets.h"
#include <string>


namespace SCClasses {
    std::mutex SCServerSocket::coutMutex;

    SCServerSocket::SCServerSocket() {
        sockFileDescrip_ = socket(AF_INET, SOCK_STREAM, 0);     // Create a socket for TCP/IP communication.
        memset(&serverAddress_, 0, sizeof(serverAddress_));     // Zero out the server address.
        portNo_ = 30000;                                        // The default port number will be 65000.
        serverAddress_.sin_family = AF_INET;                    // Use the normal settings
        serverAddress_.sin_addr.s_addr = INADDR_ANY;            // for TCP/IP communications to
        serverAddress_.sin_port = htons(portNo_);               // initialize the server address.
    }
    
    SCServerSocket::~SCServerSocket() {
        
    }
    
    void SCServerSocket::setPortNo(int value) {
        if (value < 0) {                                        // Was the port number greater than or equal to zero?
            return;                                             // No. Don't set the port number.
        }                                                       // The port number is legal. This does not mean it will work.
        portNo_ = value;                                        // Set the port number.
        serverAddress_.sin_port = htons(portNo_);               // Set server address's port number.
    }
    
    int SCServerSocket::getPortNo() {
        return portNo_;                                         // Provide the caller with the port number.
    }

    void SCServerSocket::setIPAddress(std::string strAddress) {
         inet_pton(AF_INET, strAddress.c_str(), &(serverAddress_.sin_addr));// Set the server's IPv4 IP address.
    }

    sockaddr_in  SCServerSocket::getIPAddress() {
        return serverAddress_;                                  // Provide the caller with this server's IP address.
    }

    void SCServerSocket::startServer() {
        coutMutex.lock();
        std::cout << "Starting the server socket in SCSockets." << std::endl;
        coutMutex.unlock();

    }
    
    std::string SCServerSocket::receive() {
        if(bind(sockFileDescrip_, (struct sockaddr *) &serverAddress_, sizeof(serverAddress_)) < 0) {
            return "error binding";
        }
        listen(sockFileDescrip_, 5);
        clientLength_ = sizeof(clientAddress_);
        clientSockFileDescrip_ = accept(sockFileDescrip_, (struct sockaddr *)&clientAddress_, &clientLength_);
        if (clientSockFileDescrip_ < 0) {
            return "error accepting";
        }
        memset(buffer, 0, 256);
        int count = (int)read(clientSockFileDescrip_, buffer, 255);
        if (count < 0) {
            return "error reading";
        }
        std::string reply = std::string(buffer);
        return reply;
    }
    
    bool SCServerSocket::send(const std::string s) const {
        int status = (int)::send ( sockFileDescrip_, s.c_str(), s.size(), /*MSG_NOSIGNAL*/ /*MSG_SEND*/ MSG_DONTWAIT );
        if ( status == -1 )
        {
            return false;
        }
        else
        {
            return true;
        }
    }

    
//////////////////////////////////////////////////////////////////////////////////////////////////////
    
    SCClientSocket::SCClientSocket() {
        sockFileDescrip_ = -1;                                  // The socket file descriptor is -1 until the socket is created.
        memset(&serverAddress_, 0, sizeof(serverAddress_));     // Zero out the server address.
        portNo_ = 65000;                                        // The default port number will be 65000.
        setServerIPAddress("192.168.0.1");                      // Put in something for the IP address.
        errorMessage = "";                                      // Nothing has failed yet, so there is no error message.
    }
    
    SCClientSocket::~SCClientSocket() {
        
    }

    void SCClientSocket::setPortNo(int value) {
        if (value < 0) {                                        // Was the port number greater than or equal to zero?
            return;                                             // No. Don't set the port number.
        }                                                       // The port number is legal. This does not mean it will work.
        portNo_ = value;                                        // Set the port number.
        serverAddress_.sin_port = htons(portNo_);               // Set clients address's port number.
    }
    
    int SCClientSocket::getPortNo() {
        return portNo_;                                         // Provide the caller with the port number.
    }

    bool SCClientSocket::setFileDescriptor() {
        int sockFD = socket(AF_INET, SOCK_STREAM, 0);           // Set the socket file descriptor.
        if (sockFD < 0) {                                       // Was the file descriptor set?
            return false;                                       // No something went wrong.
        }                                                       // The socket file descriptor is set.
        sockFileDescrip_ = sockFD;                              // Save the socket file descriptor for use.
        return true;                                            // Let the caller know the socket file desriptor was set.
    }

    bool SCClientSocket::setServerIPAddress(std::string strAddress) {
        struct sockaddr_in testAdd;                             // Create an address to test.
        if (inet_pton(AF_INET, strAddress.c_str(), &(testAdd.sin_addr)) == -1) {// Could the string convert to a network address?
            return false;                                       // No. Let the caller no of the failure.
        }                                                       // The given string could be converted to a network address.
        inet_pton(AF_INET, strAddress.c_str(), &(serverAddress_.sin_addr));// Set the server's IPv4 IP address.
        return true;                                            // Let the caller know the IP address was set successfully.
    }
    
    sockaddr_in  SCClientSocket::getServerIPAddress() {
        return serverAddress_;                                  // Provide the caller with this client's IP address.
    }
    
    std::string SCClientSocket::getIPAddressString() {
        char str[INET_ADDRSTRLEN];                              // Will contain the string version of the server's IP address.
        inet_ntop(AF_INET, &(serverAddress_.sin_addr), str, INET_ADDRSTRLEN);// Convert the network version to a string.
        return str;                                             // Provide the caller with the string version of the IP address.
    }
    
    bool SCClientSocket::connectToServer() {
        errorMessage = "";                                      // Nothing has failed yet, so there is no error message.
        if (sockFileDescrip_ == -1) {                           // Has this server socket been created yet?
            sockFileDescrip_ = socket(AF_INET, SOCK_STREAM, 0); // No. Create a socket for TCP/IP communication.
            if (sockFileDescrip_ == -1) {                       // Was the socket established?
                errorMessage = "The socket file descriptor couldn't be created.";// No. Create an error message.
                return false;                                   // Failure, the socket couldn't be created.
            }                                                   // The socket file descriptor was successfully created,
        }                                                       // The server socket exists.
        if(fcntl(sockFileDescrip_, F_SETFL, O_NONBLOCK) == -1) {// Set the socket to non-blocking?
            errorMessage = "Failed to set the socket to non blocking for connection.";// No. Create error message.
            return false;                                       // Let the caller known of the failure.
        }                                                       // The socket is non-blocking.
        if (connect(sockFileDescrip_ , (struct sockaddr *)&serverAddress_ , sizeof(serverAddress_)) < 0) {// Did the socket connect?
            errorMessage = "The connection with the server couldn't be established.";// No. Create an error message.
            errorMessage += " Error number: " + std::to_string(errno);
            if (errno == EACCES) { errorMessage += " EACCES."; }
            if (errno == EPERM) { errorMessage += " EPERM."; }
            if (errno == EADDRINUSE) { errorMessage += " EADDRINUSE."; }
            if (errno == EADDRNOTAVAIL) { errorMessage += " EADDRNOTAVAIL."; }
            if (errno == EAFNOSUPPORT) { errorMessage += " EAFNOSUPPORT."; }
            if (errno == EAGAIN) { errorMessage += " EAGAIN."; }
            if (errno == EALREADY) { errorMessage += " EALREADY."; }
            if (errno == EBADF) { errorMessage += " EBADF."; }
            if (errno == ECONNREFUSED) { errorMessage += " ECONNREFUSED."; }
            if (errno == EFAULT) { errorMessage += " EFAULT."; }
            if (errno == EINPROGRESS) {
                fd_set fds;                                     // Create the file descriptor set.
                struct timeval tv;                              // Create a timeval for the time out.
                tv.tv_sec = 2;                                  // Set the time out to
                tv.tv_usec = 500000;                            // 2.5 seconds.
                FD_ZERO(&fds);                                  // Zero out the file descriptor set.
                FD_SET(sockFileDescrip_, &fds);                 // Ad the socket file desriptor to the file descriptor set.
                select(sockFileDescrip_ + 1, &fds, NULL, NULL, &tv);// Wait up to 2.5 seconds for connection.
                if (FD_ISSET(sockFileDescrip_, &fds)) {         // Did the socket connect?
                    return true;                                // Yes. Let the caller know the socket is connected.
                } else {                                        // No. Couldn't connect to the socket.
                    errorMessage = "Time out trying to connect to the given server socket.";// Create error.
                    return false;                               // Let the caller know about the failure.
                }
            }
            if (errno == EINTR) { errorMessage += " EINTR."; }
            if (errno == EISCONN) { errorMessage += " EISCONN."; }
            if (errno == ENETUNREACH) { errorMessage += " ENETUNREACH."; }
            if (errno == ENOTSOCK) { errorMessage += " ENOTSOCK."; }
            if (errno == EPROTOTYPE) { errorMessage += " EPROTOTYPE."; }
            if (errno == ETIMEDOUT) { errorMessage += " ETIMEDOUT."; }
            return false;                                       // Let the caller know the connection failed.
        }                                                       // The connection suceeded.
        return true;                                            // Let the caller know the server is connected.
    }
    
    bool SCClientSocket::isReady() {
        errorMessage = "";                              // Nothing has failed yet, so there is no error message.
        fd_set readFDS;                                 // Create the read file descriptor set.
        fd_set writeFDS;                                // Create the write file descriptor set.
        struct timeval tv;                              // Create a timeval for the time out.
        tv.tv_sec = 0;                                  // Set the time out to
        tv.tv_usec = 500000;                            // 0.5 seconds.
        FD_ZERO(&readFDS);                              // Zero out the read file descriptor set.
        FD_ZERO(&writeFDS);                             // Zero out the write file descriptor set.
        FD_SET(sockFileDescrip_, &readFDS);             // Add the socket file desriptor to the read file descriptor set.
        FD_SET(sockFileDescrip_, &writeFDS);            // Add the socket file desriptor to the write file descriptor set.
        int result = select(sockFileDescrip_ + 1, &readFDS, &writeFDS, NULL, &tv);// Wait up to .5 seconds to verify the connection.
        std::cout << "select() completed with result = " << std::to_string(result) << std::endl;
        if (result < 0) {                               // Did the call to select fail?
            errorMessage = "The call to the select function failed.";// Yes. Create an error.
            return false;                               // Let the caller know the socket is not connected.
        }                                               // The call to select suceeded.
        if (result == 0) {                              // Is there anything available to read or write?
            errorMessage = "The call to select succeeded, but no socket has data to read or write.";// No. Create an error.
        }                                               // There is something available to read or write. Is it this socket?
        if (FD_ISSET(sockFileDescrip_, &readFDS) != 0) {// Was this socket connected for reading?
            return true;                                // Yes. Let the caller know the socket is connected.
        } else if (FD_ISSET(sockFileDescrip_, &writeFDS) != 0) {// Was this socket connected for writing?
            return true;                                // Yes. Let the caller know the socket is connected.
        } else {                                        // No. The socket is not connected.
            return false;                               // Let the caller know about the failure.
        }
    }

    
    bool SCClientSocket::sendData(std::string data) {
        errorMessage = "";                                      // Nothing has failed yet, so there is no error message.
        if( send(sockFileDescrip_, data.c_str(), strlen(data.c_str()), 0) < 0) {// Did the send fail?
            errorMessage = "Failed to send data to the server.";// Yes. Failed. Create an error message.
            return false;                                       // Let the caller know the data wasn't sent.
        }                                                       // The data was sent.
        return true;                                            // Let the caller know the data sent successfully.
    }

    std::string SCClientSocket::receiveData(int size = 512) {
        int timeout = 300000;                                   // The number of micro seconds after which receiving times out.
        long cReceived = 0;                                     // The number of characters received from the socket presently.
        long total_size = 0;                                    // The total number of characters received from the socket.
        struct timeval begin;                                   // The time just before receiving characters from the socket.
        struct timeval now;                                     // The time now.
        double timediff;                                        // The number of seconds since data was last received.
        errorMessage = "";                                      // Nothing has failed yet, so there is no error message.
        char buffer[size + 512];                                // Will contain the characters received from the server.
        std::string reply = "";                                 // Will contain the string of characters received from the server.
        gettimeofday(&begin, NULL);                             // Get the time prior to receiving from the socket.
        while (1) {                                             // Keep receiving data from the socket until there is a timeout.
            gettimeofday(&now, NULL);                           // Get the time now.
            timediff = 1000000 * (now.tv_sec - begin.tv_sec) + now.tv_usec - begin.tv_usec;// Calculate the time spent.
            if (total_size >= size || timediff > timeout) {     // Has data been received and the time out exceeded?
                break;                                          // Yes. Done receiving from the socket.
            } else if (timediff > timeout * 4) {                // If there is no data, wait until twice the time out.
                errorMessage = "Timeout! Failed to receive data.";// Create an error message.
                return reply;                                   // Provide the caller with the empty response.
            }                                                   // Try to receive more data.
            memset(buffer, 0, size + 4);                            // Clear the buffer to receive more data.
            cReceived = recv(sockFileDescrip_, buffer, sizeof(buffer), MSG_DONTWAIT);// Get any data available from the socket.
            if( cReceived < 0) {                                // Was a data available?
                usleep(100000);                                 // Wait for .1 seconds.
            } else {                                            // Data was received.
                total_size += cReceived;                        // Increment the total number of characters rececived.
                reply += buffer;                                // Add data to the reply.
            }                                                   // The reply is updated.
        }                                                       // The reply is complete.
        return reply;                                           // Provide the caller with the response.
    }
    
    void SCClientSocket::close() {
        usleep(100000);                                 // Wait for .1 seconds.
        ::close(sockFileDescrip_);
        sockFileDescrip_ = -1;
    }

}

