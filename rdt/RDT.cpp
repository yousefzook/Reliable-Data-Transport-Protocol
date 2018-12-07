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
    sendto(soc,  p, len,MSG_CONFIRM, (const struct sockaddr *) &addr, sizeof(addr));
}

int RDT::rcvUDP(Packet *p, int soc, struct sockaddr_in addr, int len){
    socklen_t lenAddr = sizeof(struct sockaddr_in);
    return recvfrom(soc, p, len,MSG_WAITALL, (struct sockaddr *) &addr, &lenAddr);
}