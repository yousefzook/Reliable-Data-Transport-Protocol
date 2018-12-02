//
// Created by zook on 02/12/18.
//

#include <netinet/in.h>
#include <netdb.h>
#include <cstring>
#include "Client.h"


struct in_addr Client::getHostIP(string hostName) {
    struct hostent *host;
    struct in_addr **in_list;
    if ((host = gethostbyname(hostName.c_str())) != NULL) {
        in_list = (struct in_addr **) host->h_addr_list;
        return **in_list;
    }
}


void Client::conToserver(string hostName, int port) {

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        cout << "Error while connecting to the server!" << endl;
        exit(1);
    }
    cout << "connection succeed" << endl;

    // Filling server information
    memset(&server_add, 0, sizeof(server_add));
    server_add.sin_family = AF_INET;
    server_add.sin_port = htons(port);
    server_add.sin_addr = getHostIP(hostName);
}


void Client::sendMessage(string message) {
    int result = sendto(sockfd, message.c_str(), message.size(),
                        MSG_CONFIRM, (const struct sockaddr *) &server_add, sizeof(server_add));

    if (result < 0)
        cout << "Sending message failed!" << endl;
    else
        cout << "Message sent successfully" << endl;
}

#define MAX_LINE 1024

string Client::recvMessage() {
    char buffer[MAX_LINE];
    int n;
    n = recvfrom(sockfd, buffer, sizeof(buffer),
                 MSG_WAITALL, (struct sockaddr *) &server_add, 0);
    buffer[n] = '\0';
    string message(buffer);

    return message;
}


void Client::startClient(string hostName, int portNumber) {
    conToserver(hostName, portNumber);
    string mess = "this is client!";
    sendMessage(mess);
    string mess2 = recvMessage();
    cout << mess2 << endl;
}

#define HOST_NAME "localhost"
#define SERVER_PORT_NUM 8080

int main() {

    Client c;
    c.startClient(HOST_NAME, SERVER_PORT_NUM);
}