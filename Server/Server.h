//
// Created by Zook on 12/4/18.
//


#include <sys/socket.h>
#include <stdio.h>
#include<iostream>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "../Packet/Packet.h"
using namespace std;
class RDT;
#ifndef TCP_NETWORK_SERVER_H
#define TCP_NETWORK_SERVER_H

#define MAX_LINE 500
#define PORT_NUMBER 8080
class Server {
public:
    int sockfd;
//    uint32_t in_seqNum = 1;
    struct sockaddr_in serv_addr;

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
    /**
     * receive message from the client
     * @return the received message
     */
    string rcvFileName();
    RDT *rdt;
    Server();
/**
     * Start server
     * @param port, the port that server should listen to
     */
    void startServer(int port);

    void setRDT(RDT *rdt);
};


#endif //TCP_NETWORK_SERVER_H