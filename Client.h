//
// Created by zook on 02/12/18.
//

#include<string>
#include<iostream>
#include <stdlib.h>
#include <stdio.h>

#ifndef TCP_NETWORK_CLIENT_H
#define TCP_NETWORK_CLIENT_H

using namespace std;

class Client {

private:
    int sockfd;

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
    void sendMessage(string message);
    /**
     * receive message from server
     * @return the received message
     */
    string recvMessage();

public:
    /**
     * Start server
     * @param portNumber server port number
     * @param hostName host name
     */
    void startClient(string hostName, int portNumber);
};


#endif //TCP_NETWORK_CLIENT_H
