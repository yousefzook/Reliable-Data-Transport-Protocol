//
// Created by nada on 12/4/18.
//

#include <cstring>
#include <DataPacket.h>
#include <AckPacket.h>
#include <thread>
#include <mutex>
#include "SR.h"

mutex mtx1; // locked while accessing sr arrays
mutex mtx2; // locked while sending packets

SR::SR() {

}

void SR::handleReciever(int soc, struct sockaddr_in addr, string fileName) {
    this->soc = soc;
    this->addr = addr;
    FILE *fp = fopen(("../Client/" + fileName).c_str(), "w");
    uint32_t inSeqNo;
    int base = 0;

    bool rec[N];
    Packet *datawnd[N]; // received data window
    for (int i = 0; i < N; i++)
        rec[i] = false;
    do {
        Packet *dataPkt = new DataPacket();
        rcvUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN, false);
        if (dataPkt->len == 0) // end of file
            break;
        inSeqNo = dataPkt->seqno;
        if (inSeqNo >= base && inSeqNo < base + N) { // send ack and store it
            rec[inSeqNo % N] = true;
            datawnd[inSeqNo % N] = dataPkt;
            Packet *ackPkt = new AckPacket();
            ackPkt->len = 6;
            ackPkt->seqno = inSeqNo;
            sendUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN);
        } else if (inSeqNo < base && inSeqNo >= base - N) { // resend ack
            Packet *ackPkt = new AckPacket();
            ackPkt->len = 6;
            ackPkt->seqno = inSeqNo;
            sendUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN);
        } else { // ignore
            continue;
        }
        while (rec[base % N]) { // while base is received
            fwrite(((DataPacket*)datawnd[base%N])->data, sizeof(char), dataPkt->len, fp);
            rec[base%N] = false;
            base++;
        }
    } while (1);
    fflush(fp);
    fclose(fp);
}

void SR::handleSender(int soc, struct sockaddr_in addr, string fileName) {

    this->soc = soc;
    this->addr = addr;
    char buff[MAX_DATA_LEN];
    memset(buff, 0, sizeof(buff));
    for (int i = 0; i < N; i++)
        acked[i] = false;


    FILE *fp = fopen(("../Server/" + fileName).c_str(), "r");
    uint16_t read;
    int base = 0;
    uint32_t nextSeqNo = 0;
    
    // send first N packets
    while (nextSeqNo < base + N && (read = fread(buff, 1, sizeof(buff), fp)) >= 0) {
        sendPKT(buff, read, nextSeqNo);
        timer[nextSeqNo] = clock();
        nextSeqNo++;
    }

    // start timers thread
    thread timerThread(timerFn, this);


    // if there are other packets to send
    while ((read = fread(buff, 1, sizeof(buff), fp)) >= 0) {
        while (1) {
            Packet *ackPkt = new AckPacket();
            rcvUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN, false);
            mtx1.lock();
            acked[ackPkt->seqno % N] = true;
            timer[ackPkt->seqno % N] = -1; // reset timer
            // shift base and send new packet
            if (acked[base % N]) {
                timer[nextSeqNo % N] = clock();
                acked[nextSeqNo % N] = false;
                mtx1.unlock();
                sendPKT(buff, read, nextSeqNo); // send next packet
                nextSeqNo++;
                base++;
                break;
            } else
                mtx1.unlock();
        }
    }

    // if there are other packets not acked yet
    while (base != nextSeqNo) {
        Packet *ackPkt = new AckPacket();
        rcvUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN, false);
        mtx1.lock();
        acked[ackPkt->seqno % N] = true;
        timer[ackPkt->seqno % N] = -1; // reset timer
        while (acked[base % N]) {
            base++;
        }
        mtx1.unlock();
    }

    timerThread.join();


}


void SR::sendPKT(char data[], uint16_t len, int seqNo) {
    Packet *dataPkt = new DataPacket();
    dataPkt->len = len;
    dataPkt->seqno = seqNo;
    strcpy(((DataPacket *) dataPkt)->data, data);
    packets[seqNo % N] = dataPkt;
    mtx2.lock();
    sendUDP(dataPkt, soc, addr, sizeof(*dataPkt));
    mtx2.unlock();
}

void SR::sendPKT(Packet *pkt) {
    mtx2.lock();
    sendUDP(pkt, soc, addr, sizeof(*pkt));
    mtx2.unlock();
}

void timerFn(SR *sr) {
    while (1) {
        bool isFinished = true;
        for (int i = 0; i < sr->N; i++) {
            mtx1.lock();
            if (sr->timer[i] == -1) {
                mtx1.unlock();
                continue;
            }
            isFinished = false; // there are some pkts not acked yet
            double time = (clock() - sr->timer[i]) / (CLOCKS_PER_SEC / 1000);
            if (!sr->acked[i] && time > sr->timeout) { //resend
                mtx2.lock();
                sr->sendPKT(sr->packets[i]);
                mtx2.unlock();
                sr->timer[i] = clock(); // restart timer
            }
            mtx1.unlock();
        }
        if (isFinished)
            break;
    }
}