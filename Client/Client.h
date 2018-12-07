//
// Created by Zook on 12/4/18.
//
#include<string>
#include<iostream>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include "../Packet/Packet.h"


#ifndef RDT_CLIENT_H
#define RDT_CLIENT_H

#define HOST_NAME "localhost"
#define CLIENT_PORT_NUM 8080
#define MAX_LINE 500


class RDT;

using namespace std;

class Client {

public:
    Client();
    int sockfd;
    void setRDT(RDT *rdt);
    RDT *rdt;

    uint32_t in_seqNum = 1;

    struct sockaddr_in server_add;

    /**
     * Connect to server
     * @param hostName the host name
     * @param port port of the server
     */
    void conToserver(string hostName, int port);
    /**
     * Get the IP of a host given by hostName
     * @param hostName the host name
     * @return the IP of the given host name
     */
    struct in_addr getHostIP(string hostName);
    /**
     * send message to the server
     * @param message message to be send
     */
    int sendFileName(string fileName);
    /**
     * Start server
     * @param portNumber server port number
     * @param hostName host name
     */
    void startClient(string hostName, int portNumber, string fileName);


};


#endif //RDT_CLIENT_H