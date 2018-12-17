//
// Created by nada on 12/4/18.
//

#ifndef RDT_STOPNWAIT_H
#define RDT_STOPNWAIT_H

#include <cstring>
#include "DataPacket.h"
#include "AckPacket.h"
#include "RDT.h"

class StopNWait : public RDT {

public:
    StopNWait();
    int RTT = 10000000; //todo change
    void handleReciever(int soc, struct sockaddr_in addr, string fileName) override;
    void handleSender(int soc, struct sockaddr_in addr, string fileName) override;

//    char checkSum(char data[]);
};


#endif //RDT_STOPNWAIT_H