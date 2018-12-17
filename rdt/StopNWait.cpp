//
// Created by nada on 12/4/18.
//

#include "StopNWait.h"
#include <ctime>
#include <ratio>
#include <chrono>
using namespace std::chrono;


StopNWait::StopNWait() {
}

//char StopNWait::checkSum(char data[]){
//    for(int i =  0 ; i < sizeof(data); i++){
//
//    }
//}

void StopNWait::handleReciever(int soc, struct sockaddr_in addr, string fileName) {
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    FILE *fp = fopen(("../Client/" + fileName).c_str(), "wb");
    uint16_t in_seqNum = 1;
    Packet *dataPkt;
    do {
        dataPkt = new DataPacket();
        rcvUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN, false);
        if (dataPkt->seqno == in_seqNum) {
            fwrite(((DataPacket *) dataPkt)->data, sizeof(char), dataPkt->len, fp);
            Packet *ackPkt = new AckPacket();
            ackPkt->len = 6;
            ackPkt->seqno = in_seqNum;
            sendUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN);
            in_seqNum++;
        }
    } while (dataPkt->len > 0);
    fflush(fp);
    fclose(fp);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    cout << "Recieving time: " << time_span.count() << endl;
}

void StopNWait::handleSender(int soc, struct sockaddr_in addr, string fileName) {
    char buff[MAX_DATA_LEN];
    memset(buff, 0, sizeof(buff));
    FILE *fp = fopen(("../Server/" + fileName).c_str(), "rb");
    uint16_t read = 0;
    uint32_t in_seqNum = 1;

    while ((read = fread(buff, 1, sizeof(buff), fp)) >= 0) {
        Packet *dataPkt = new DataPacket();
        dataPkt->len = read;
        dataPkt->seqno = in_seqNum;
        strcpy(((DataPacket *) dataPkt)->data, buff);
        RESEND:
        sendUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN);
        Packet *ackPkt = new AckPacket();
        if (rcvUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN, true) == -1) {
            goto RESEND; // to handle time out;
        }
        if (ackPkt->seqno == in_seqNum) {
            in_seqNum++;
        }
        memset(buff, 0, sizeof(buff));
        if (read == 0)
            break;
    }
    fclose(fp);
}