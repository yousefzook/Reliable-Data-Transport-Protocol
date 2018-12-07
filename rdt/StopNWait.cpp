//
// Created by nada on 12/4/18.
//

#include "StopNWait.h"

StopNWait::StopNWait() {
}

void StopNWait::handleReciever(int soc, struct sockaddr_in addr, string fileName){
    FILE *fp = fopen(("../Client/"+fileName).c_str(), "w");
    uint16_t in_seqNum = 1;
    Packet *dataPkt;
    do {
        dataPkt = new DataPacket();
        rcvUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN);
        if(dataPkt->seqno == in_seqNum) {
            fwrite(((DataPacket*) dataPkt)->data, sizeof(char), dataPkt->len, fp);
            Packet *ackPkt = new AckPacket();
            ackPkt->len = 6;
            ackPkt->seqno = in_seqNum;
            sendUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN);
            in_seqNum++;
        }
    }while (dataPkt->len > 0);
    fflush(fp);
    fclose(fp);
}

void StopNWait::handleSender(   int soc, struct sockaddr_in addr, string fileName) {
    char buff[MAX_DATA_LEN];
    memset(buff, 0, sizeof(buff));
    FILE *fp = fopen(("../Server/"+fileName).c_str(), "r");
    uint16_t read = 0;
    uint32_t in_seqNum = 1;

    while ((read = fread(buff, 1, sizeof(buff), fp)) >= 0) {
        Packet *dataPkt = new DataPacket();
        dataPkt->len = read;
        dataPkt->seqno = in_seqNum;
        strcpy(((DataPacket*) dataPkt)->data, buff);
        sendUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN); //todo send with probablity
        clock_t start = clock();
        Packet *ackPkt = new AckPacket();
        rcvUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN);
        if(ackPkt->seqno == in_seqNum){
            in_seqNum++;
        }
        memset(buff, 0, sizeof(buff));
        if(read == 0)
            break;
    }
    fclose(fp);
}