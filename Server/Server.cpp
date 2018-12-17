//
// Created by Zook on 12/4/18.
//


#include <SR.h>
#include <GBN.h>
#include "Server.h"
#include "../rdt/RDT.h"
#include "../rdt/StopNWait.h"

Server::Server() {}

void Server::setRDT(RDT *rdt) {
    this->rdt = rdt;
}

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

string Server::rcvFileName() {
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    char buffer[MAX_LINE];
    memset(buffer, 0, sizeof(buffer));
    size_t n;
    socklen_t len = sizeof(struct sockaddr_in);
    n = recvfrom(sockfd, (char *) buffer, MAX_LINE,MSG_WAITALL, (struct sockaddr *) &cli_addr, &len);
    buffer[n] = '\0';
    string message = buffer;
    cout << "received file name: " << message << endl;
    rdt->handleSender(sockfd, cli_addr, message);
    return message;
}

void Server::startServer(int port) {
    createSocketFD();
    bindServer(port);
    rcvFileName();

}


int main() {
    Server *server = new Server();
    server->setRDT(new SR());
    server->startServer(PORT_NUMBER);
}