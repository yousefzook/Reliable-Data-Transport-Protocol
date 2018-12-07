//
// Created by nada on 12/4/18.
//

#include "RDT.h"

RDT::RDT(){}
void RDT::handleReciever(int soc, struct sockaddr_in addr, string fileName) {

}

void RDT::handleSender(int soc, struct sockaddr_in addr, string fileName) {

}

void RDT::sendUDP(Packet *p, int soc, struct sockaddr_in addr, int len){
    if (!isLoss()) {
        sendto(soc, p, len, MSG_CONFIRM, (const struct sockaddr *) &addr, sizeof(addr));
    }
}

int RDT::rcvUDP(Packet *p, int soc, struct sockaddr_in addr, int len, bool wait) {
    socklen_t lenAddr = sizeof(struct sockaddr_in);

    if (wait) {
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt(soc, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
    }
    return recvfrom(soc, p, len, MSG_WAITALL, (struct sockaddr *) &addr, &lenAddr);
}

bool RDT::isLoss() { // prob = 0.1
    int min = 1, max = 10;
    srand(time(0));
    int randNum = rand() % (max - min + 1) + min;
    return randNum % 10 == 0;
}