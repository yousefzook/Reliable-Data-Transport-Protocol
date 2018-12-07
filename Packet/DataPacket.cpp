//
// Created by nada on 12/4/18.
//

#include <cstring>
#include "DataPacket.h"


DataPacket::DataPacket() {
    memset(data, 0, sizeof(data));
}