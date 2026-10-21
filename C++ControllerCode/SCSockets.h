//
//  SCSockets.h
//  PlantManager
//
//  Created by Mark Davis on 4/14/15.
//  Copyright (c) 2015 Products Engineering. All rights reserved.
//

#ifndef __PlantManager__SCSockets__
#define __PlantManager__SCSockets__
/*
#include <stdio.h>
#include <string>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
*/
#include <iostream>
/* A simple server in the internet domain using TCP
 The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/fcntl.h>
#include <sys/select.h>

//using namespace std;

namespace SCClasses {
    
    class SCServerSocket {
    private:
        /*! The size of the clients address. */
        socklen_t clientLength_;
        /*! The file descriptor for the server socket. */
        int sockFileDescrip_;
        /*! The file desriptor of the client socket. */
        int clientSockFileDescrip_;
        /*! The port number used for this socket. */
        int portNo_;
        /*! The buufer used to receive data from the client. */
        char buffer[256];
        /*! The address of the socket which listens for requests from clients. */
        struct sockaddr_in serverAddress_;
        /*! The address of the socket which is created to talk to a client. */
        struct sockaddr_in clientAddress_;
        
    public:
        /*! The default server socket constructor. */
        SCServerSocket();
        /*! The default server socket destructor. */
        virtual ~SCServerSocket();
        /*! Sets the port number.
         * \param value The desired port number.*/
        void setPortNo(int value);
        /*! \returns The port number. */
        int getPortNo();
        /*! Sets this socket's IP address.
         * \param strAddress The string version of the IPv4 internet address.
         * for example: "192.168.0.10" */
        void setIPAddress(std::string strAddress);
        /*! \returns The server socket's IP address in network format. */
        sockaddr_in getIPAddress();
        /*! \returns The server socket's IP address in string format. */
        std::string getIPAddressString();
        /*! \returns The server socket's file descriptor. It returns -1 if a socket has not been defined.  */
        int getDescriptor();
        /*! Start a server to listen for clients. */
        static void startServer();
        /*! Control ouput to cout. */
        static std::mutex coutMutex;
        /*! Returns a command from a caller. */
        std::string receive();
        /*! Sends the given string to the calling client. */
        bool send ( const std::string ) const;

    };

    class SCClientSocket {
    private:
        /*! The file desriptor for the client socket. */
        int sockFileDescrip_;
        /*! The port number used for this socket. */
        int portNo_;
        /*! The address of the server socket this socket connected to. */
        struct sockaddr_in serverAddress_;
        
    public:
        /*! The default server socket constructor. */
        SCClientSocket();
        /*! The default server socket destructor. */
        virtual ~SCClientSocket();
        /*! Sets the port number.
         * \param value The desired port number.*/
        void setPortNo(int value);
        /*! \returns The port number. */
        int getPortNo();
        /*! Sets this server's IP address.
         * \returns true if the IP address was legal.
         * \param strAddress The string version of the IPv4 internet address.
         * for example: "192.168.0.10" */
        bool setServerIPAddress(std::string strAddress);
        /*! Sets the socket file descriptor. */
        bool setFileDescriptor();
        /*! \returns The server's IP address in network format. */
        sockaddr_in getServerIPAddress();
        /*! \returns The client socket's IP address in string format. */
        std::string getIPAddressString();
        /*! Connects to a server. 
         * \returns false if the connection cannot be established. */
        bool connectToServer();
        /*! Sends data to the connected server.
         * \returns true if the data was successfully sent.
         * \param data A string containing the characters to send. */
        bool sendData(std::string data);
        /*! Receives data from the connected server.
         * \returns The data received from the server.
         * \param size The estimated number of characters in the response. */
        std::string receiveData(int size);
        /*! Used to close the socket. */
        void close();
        /*! \returns true if this socket is ready to read or write. */
        bool isReady();
        /*! Contains an error message which provides additional information if a function fails. */
        std::string errorMessage;
    };

/*
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
 
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
            error("ERROR on binding");

    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    bzero(buffer,256);
    n = (int)read(newsockfd,buffer,255);
    if (n < 0) {
        error("ERROR reading from socket");
    }
    printf("Here is the message: %s\n",buffer);
    n = (int)write(newsockfd,"I got your message",18);
    if (n < 0) {
        error("ERROR writing to socket");
    }
    close(newsockfd);
    close(sockfd);    
    std::cout << "Hello, World!\n";
};
*/
    
}
#endif /* defined(__PlantManager__SCSockets__) */

