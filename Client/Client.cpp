//
// Created by Zook on 12/4/18.
//


#include "Client.h"
#include "../rdt/StopNWait.h"


Client::Client() {}

void Client::setRDT(RDT *rdt){
    this->rdt = rdt;
}
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

int Client::sendFileName(string fileName) {
    int result = sendto(sockfd, fileName.c_str(), fileName.size(),
                        MSG_CONFIRM, (const struct sockaddr *) &server_add, sizeof(server_add));
    if (result < 0)
        cout << "Sending file failed!" << endl;
    else
        cout << "fileName sent successfully" << endl;
    return result;
}


void Client::startClient(string hostName, int portNumber, string fileName) {
    conToserver(hostName, portNumber);
    if(sendFileName(fileName) >= 0){
        rdt->handleReciever(sockfd, server_add, fileName);
    }

}

int main(){
    Client *c = new Client();
    c->setRDT(new StopNWait());
    string fileName = "home.html";
    c->startClient(HOST_NAME, CLIENT_PORT_NUM, fileName);
}