//
// Created by nada on 12/4/18.
//

#ifndef RDT_SR_H
#define RDT_SR_H


#include "RDT.h"

class SR : public RDT {
public:
    SR();

    int const timeout = 500; //time out after 500 ms
    int const N = 50; // window size

    clock_t timer[N];
    Packet *packets[N];
    bool acked[N];

    void handleReciever(int soc, struct sockaddr_in addr, string fileName) override;

    void handleSender(int soc, struct sockaddr_in addr, string fileName) override;

    void sendPKT(Packet *pkt);

private:
    void sendPKT(char data[], uint16_t len, int seqNo);
    int soc;
    struct sockaddr_in addr;
};

void timerFn(SR *sr);

#endif //RDT_SR_H