//
// Created by nada on 12/4/18.
//

#ifndef RDT_PACKET_H
#define RDT_PACKET_H


#include <cstdint>

class Packet {

public:
    uint16_t len;
    uint32_t seqno;
};


#endif //RDT_PACKET_H