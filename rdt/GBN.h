//
// Created by zook on 09/12/18.
//

#ifndef TCP_NETWORK_GBN_H
#define TCP_NETWORK_GBN_H


#include "RDT.h"

class GBN : public RDT{
public:
    int const N = 50; // window size
    Packet *packets[N];
    void handleReciever(int soc, struct sockaddr_in addr, string fileName) override;

    void handleSender(int soc, struct sockaddr_in addr, string fileName) override;

    void sendPKT(Packet *pkt);

private:
    void sendPKT(char data[], uint16_t len, int seqNo, int soc, struct sockaddr_in addr);
};


#endif //TCP_NETWORK_GBN_H
