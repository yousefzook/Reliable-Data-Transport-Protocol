//
// Created by nada on 12/4/18.
//

#ifndef RDT_RDT_H
#define RDT_RDT_H
#define MAX_DATA_LEN 500
#define MAX_DATA_PACKET_LEN 508
#define MAX_ACK_PACKET_LEN 6

#include <stdio.h>
#include<iostream>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include "../Packet/Packet.h"

using namespace std;
class RDT {

public:
    RDT();
    virtual  void handleReciever(int soc, struct sockaddr_in addr, string fileName);
    virtual void handleSender(int soc, struct sockaddr_in addr, string fileName);
    virtual void sendUDP(Packet *p, int soc, struct sockaddr_in addr, int len);
    virtual int rcvUDP(Packet *p, int soc, struct sockaddr_in addr, int len);

};


#endif //RDT_RDT_H