//
// Created by zook on 09/12/18.
//

#include <mutex>
#include <cstring>
#include <DataPacket.h>
#include <AckPacket.h>
#include "GBN.h"

void GBN::handleReciever(int soc, struct sockaddr_in addr, string fileName) {

    FILE *fp = fopen(("../Client/" + fileName).c_str(), "w");
    uint32_t expectedSeqNo = 0; // expected seqno

    do {
        Packet *dataPkt = new DataPacket();
        rcvUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN, false);
        if (dataPkt->len == 0) // end of file
            break;
        if (dataPkt->seqno == expectedSeqNo) { // send ack and write packet
            Packet *ackPkt = new AckPacket();
            ackPkt->len = 6;
            ackPkt->seqno = expectedSeqNo;
            sendUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN);
            expectedSeqNo++;
            fwrite(((DataPacket *) dataPkt)->data, sizeof(char), dataPkt->len, fp);
        } else { // resend ack for last received packet
            Packet *ackPkt = new AckPacket();
            ackPkt->len = 6;
            ackPkt->seqno = expectedSeqNo - 1;
            sendUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN);
        }
    } while (1);
    fflush(fp);
    fclose(fp);

}

void GBN::handleSender(int soc, struct sockaddr_in addr, string fileName) {
    uint32_t nextSeqNo = 0;
    uint32_t base = 0;
    uint32_t lastAcked = -1;


    FILE *fp = fopen(("../Server/" + fileName).c_str(), "r");
    char buff[MAX_DATA_LEN];
    memset(buff, 0, sizeof(buff));
    uint16_t read;

    // send first N packets
    while (nextSeqNo < base + N && (read = fread(buff, 1, sizeof(buff), fp)) >= 0) {
        sendPKT(buff, read, nextSeqNo, soc, addr);
        nextSeqNo++;
    }
//    timer = clock(); // start timer

    // if there are other packets to send
    while ((read = fread(buff, 1, sizeof(buff), fp)) >= 0) {
        while (1) {
            Packet *ackPkt = new AckPacket();
            int res = rcvUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN, true);
            if (res == -1) {  // if timeout, resend all
                for (int i = lastAcked + 1; i < nextSeqNo; i++)
                    sendUDP(packets[i % N], soc, addr, sizeof(*packets[i % N]));
                continue; // receive again
            }
            if (ackPkt->seqno != base)
                continue; // ignore the pkt and receive again

            // shift base and send new packet
            sendPKT(buff, read, nextSeqNo, soc, addr); // send next packet
            nextSeqNo++;
            lastAcked = base; // set last acked packet = base
            base++;
            break;
        }
    }

    // if there are other packets not acked yet
    while (base != nextSeqNo) {
        Packet *ackPkt = new AckPacket();
        int res = rcvUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN, false);
        if (res == -1) {  // if timeout, resend all
            for (int i = lastAcked + 1; i < nextSeqNo; i++)
                sendUDP(packets[i % N], soc, addr, sizeof(*packets[i % N]));
            continue; // receive again
        }
        if (ackPkt->seqno != base)
            continue; // ignore the pkt and receive again
        lastAcked = base; // set last acked packet = base
        base++;
    }


}


void GBN::sendPKT(char data[], uint16_t len, int seqNo, int soc, struct sockaddr_in addr) {
    Packet *dataPkt = new DataPacket();
    dataPkt->len = len;
    dataPkt->seqno = seqNo;
    strcpy(((DataPacket *) dataPkt)->data, data);
    packets[seqNo % N] = dataPkt;
    sendUDP(dataPkt, soc, addr, sizeof(*dataPkt));
}

//
//void timerFn(GBN *gbn) {
//    while (1) {
//        bool isFinished = true;
//        for (int i = 0; i < gbn->N; i++) {
//            mtx1.lock();
//            if (sr->timer[i] == -1) {
//                mtx1.unlock();
//                continue;
//            }
//            isFinished = false; // there are some pkts not acked yet
//            double time = (clock() - sr->timer[i]) / (CLOCKS_PER_SEC / 1000);
//            if (!sr->acked[i] && time > sr->timeout) { // timeout, resend
//                mtx2.lock();
//                sr->sendPKT(sr->packets[i]);
//                mtx2.unlock();
//                sr->timer[i] = clock(); // restart timer
//                sr->decreaseN();
//                goto RECHECK;
////                isFinished = true; // exit the thread
////                break;
//            }
//            mtx1.unlock();
//        }
//        if (isFinished)
//            break;
//    }
//}