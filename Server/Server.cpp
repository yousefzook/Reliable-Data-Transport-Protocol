//
// Created by zook on 02/12/18.
//

#include "Server.h"
#include <arpa/inet.h>
#include <cstring>

void Server::createSocketFD() {
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        cout << "Socket creation failed!" << endl;
        exit(1);
    }
    cout << "socket has been created" << endl;
}


void Server::bindServer(int portno) {

    // Filling server information
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Binding failed" << endl;
        exit(1);
    }
    cout << "binding finished \nwaiting for client..." << endl;

}


void Server::sendMessage(string message) {

    int result = sendto(sockfd, message.c_str(), message.size(),
                        MSG_CONFIRM, (const struct sockaddr *) &cli_addr, sizeof(cli_addr));
    if (result < 0)
        cout << "Sending message failed!" << endl;
    else
        cout << "Message sent successfully" << endl;
}

#define MAX_LINE 1024

string Server::recvMessage() {

    memset(&cli_addr, 0, sizeof(cli_addr));
    char buffer[MAX_LINE];
    int n;
    n = recvfrom(sockfd, (char *) buffer, MAX_LINE,
                 MSG_WAITALL, (struct sockaddr *) &cli_addr, 0);
    buffer[n] = '\0';
    string message(buffer);

    return message;
}

void Server::startServer(int port) {

    createSocketFD();
    bindServer(port);
    string mess = recvMessage();
    cout << mess << endl;
    sendMessage("this is server");

}


#define PORT_NUMBER 8080

int main() {
    Server *server = new Server();
    server->startServer(PORT_NUMBER);
}