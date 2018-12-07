//
// Created by nada on 12/4/18.
//

#ifndef RDT_DATAPACKAGE_H
#define RDT_DATAPACKAGE_H


#include "Packet.h"

class DataPacket : public Packet{

public:
    DataPacket();
    char data[500];


};


#endif //RDT_DATAPACKAGE_H