//
// Created by nada on 12/4/18.
//

#ifndef RDT_SR_H
#define RDT_SR_H


#include <vector>
#include "RDT.h"

class SR : public RDT {
public:
    SR();

    int const timeout = 500; //time out after 500 ms

    int N = 1; // window size

    clock_t *timer;
    Packet **packets;
    bool *acked;

    void handleReciever(int soc, struct sockaddr_in addr, string fileName) override;

    void handleSender(int soc, struct sockaddr_in addr, string fileName) override;

    void sendPKT(Packet *pkt);

    /**
     * next 3 functions are for congestion control purpose
     */
    void increaseN();
    void decreaseN();
    void rebuildArrs(int n);

private:
    void sendPKT(char data[], uint16_t len, int seqNo);
    int soc;
    struct sockaddr_in addr;
};

void timerFn(SR *sr);

#endif //RDT_SR_H