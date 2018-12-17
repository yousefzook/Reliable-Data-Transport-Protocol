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
    }else{
        cout << "packet with sequence number is lost:"<< p->seqno <<endl;
    }
}

int RDT::rcvUDP(Packet *p, int soc, struct sockaddr_in addr, int len, bool wait) {
    socklen_t lenAddr = sizeof(struct sockaddr_in);

    if (wait) {
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;
        setsockopt(soc, SOL_SOCKET, SO_RCVTIMEO, (char *) &timeout, sizeof(timeout));
    }
    return recvfrom(soc, p, len, MSG_WAITALL, (struct sockaddr *) &addr, &lenAddr);
}

bool RDT::isLoss() {
    srand(time(0));
    int randNum = rand() % 100 + 1;
    int prob = 1; // this number is the probability to make loss, from 0 to 100
    if (randNum <= prob){
        cout << "here" << endl;
        return true;
    }
    else
        return false;
}