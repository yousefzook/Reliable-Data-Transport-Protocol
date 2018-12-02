//
// Created by zook on 02/12/18.
//

#include <sys/socket.h>
#include <stdio.h>
#include<iostream>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>

using namespace std;

#ifndef TCP_NETWORK_SERVER_H
#define TCP_NETWORK_SERVER_H


class Server {

private:
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;

    /**
     * create socket file descriptor, if failed it exits the process
     */
    void createSocketFD();

    /**
     * bind the socket to the current IP address on port "portno"
     * @param portno server port number
     */
    void bindServer(int portno);

    /**
     * send message to the client
     * * @param message message to be send
     */
    void sendMessage(string message);

    /**
     * receive message from the client
     * @return the received message
     */
    string recvMessage();

//    /**
//     * send header
//     * @param data
//     * @param socket
//     * @return
//     */
//    bool sendHeader(int socket, string data);
//
//    /**
//     *
//     * @param size
//     * @param fileName
//     * @return the data or empty string if error occured
//     */
//    string recieveData(int socket, int len, string fileName);
//
//    /**
//     *
//     * @param socket
//     */
//    void closeCon(int socket);


public:
    /**
     * Start server
     * @param port, the port that server should listen to
     */
    void startServer(int port);
};


#endif //TCP_NETWORK_SERVER_H
