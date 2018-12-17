//
// Created by nada on 12/4/18.
//

#include <cstring>
#include <DataPacket.h>
#include <AckPacket.h>
#include <thread>
#include <mutex>
#include "SR.h"
#include <ctime>
#include <chrono>
#include <ratio>

using namespace std::chrono;

mutex mtx1; // locked while accessing sr arrays
mutex mtx2; // locked while sending packets

SR::SR() {

}

void SR::handleReciever(int soc, struct sockaddr_in addr, string fileName) {
    high_resolution_clock::time_point t1 = high_resolution_clock::now();

    this->soc = soc;
    this->addr = addr;
    FILE *fp = fopen(("../Client/" + fileName).c_str(), "w");
    uint32_t inSeqNo;
    int base = 0;
    char temp[3];
    bool rec[N];
    Packet *datawnd[N]; // received data window
    for (int i = 0; i < N; i++)
        rec[i] = false;
    Packet *dataPkt;
    do {
        dataPkt = new DataPacket();
        rcvUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN, false);
        if (dataPkt->len == 0) // end of file
            break;
        inSeqNo = dataPkt->seqno;
        cout << "message with seqno: " << inSeqNo << " is received" << endl;
        if (inSeqNo >= base && inSeqNo < base + N) { // send ack and store it
            rec[inSeqNo % N] = true;
            datawnd[inSeqNo % N] = dataPkt;
            Packet *ackPkt = new AckPacket();
            ackPkt->len = 6;
            ackPkt->seqno = inSeqNo;
            sendUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN);
            cout << "message: " << ackPkt->seqno << " ack sent" << endl;
        } else if (inSeqNo < base && inSeqNo >= base - N) { // resend ack
            Packet *ackPkt = new AckPacket();
            ackPkt->len = 6;
            ackPkt->seqno = inSeqNo;
            sendUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN);
            cout << "message: " << ackPkt->seqno << " ack resent" << endl;
        } else { // ignore
            continue;
        }
        while (rec[base % N]) { // while base is received
            fwrite(((DataPacket *) datawnd[base % N])->data, sizeof(char), dataPkt->len, fp);
            rec[base % N] = false;
            base++;
        }
    } while (1);
    fflush(fp);
    fclose(fp);
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
    cout << "Recieving time: " << time_span.count() << endl;
}

void SR::increaseN() {
    mtx1.lock();
    rebuildArrs(N + 1);
    mtx1.unlock();
}

void SR::decreaseN() {

    mtx1.lock();
    if (N == 1) {
        mtx1.unlock();
        return;
    }
    rebuildArrs(N / 2);
    mtx1.unlock();
}

void SR::rebuildArrs(int n) {
    Packet **pktTemp = packets;
    bool *ackTemp = acked;
    clock_t *tTemp = timer;
    // reinitialize arrays
    packets = new Packet *[n];
    acked = new bool[n];
    timer = new clock_t[n];
    // copy old values
    for (int i = 0; i < N; i++) {
        packets[i] = pktTemp[i];
        acked[i] = ackTemp[i];
        timer[i] = tTemp[i];
    }
    acked[n - 1] = false;
    timer[n - 1] = -1;
}

void SR::handleSender(int soc, struct sockaddr_in addr, string fileName) {

    this->soc = soc;
    this->addr = addr;
    char buff[MAX_DATA_LEN];

    // initialize arrays
    this->packets = new Packet *[N];
    this->acked = new bool[N];
    this->timer = new clock_t[N];

    FILE *fp = fopen(("../Server/" + fileName).c_str(), "r");
    uint32_t nextSeqNo = 0;
    int base = 0;
    SEND: // for congs control purpose, when N is increased
    memset(buff, 0, sizeof(buff));
    for (int i = 0; i < N; i++)
        acked[i] = false;

    uint16_t read;

    // send first N packets
    while (nextSeqNo < base + N && (read = fread(buff, 1, sizeof(buff), fp)) >= 0) {
        sendPKT(buff, read, nextSeqNo);
        cout << "packett #" << nextSeqNo << " sent!" << endl;
        timer[nextSeqNo] = clock();
        nextSeqNo++;
    }

    // start timers thread
    thread timerThread(timerFn, this);


    // if there are other packets to send
    while ((read = fread(buff, 1, sizeof(buff), fp)) >= 0) {
        if (read == 0) // end of file
            break;
        while (1) {
            RECV: // for congs control purpose, when N is decreased
            Packet *ackPkt = new AckPacket();
            rcvUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN, false);
            mtx1.lock();
            if (ackPkt->seqno >= base + N) { // for congs control purpose
                mtx1.unlock();
                continue; // skip this packet
            }
            cout << "message: " << ackPkt->seqno << " is acked" << endl;
            acked[ackPkt->seqno % N] = true;
            timer[ackPkt->seqno % N] = -1; // reset timer
            // shift base and send new packet
            if (acked[base % N]) {
                timer[nextSeqNo % N] = clock();
                acked[nextSeqNo % N] = false;
                mtx1.unlock();
                cout << "packet #" << nextSeqNo << " sent!" << endl;
                sendPKT(buff, read, nextSeqNo); // send next packet
                nextSeqNo++;
                base++;
                break;
            } else
                mtx1.unlock();

            increaseN(); // additive increase window size, for congs control purpose
            goto SEND; // there is new slot in window, so go to send
        }

    }

    // if there are other packets not acked yet
    while (base != nextSeqNo) {
        Packet *ackPkt = new AckPacket();
        rcvUDP(ackPkt, soc, addr, MAX_ACK_PACKET_LEN, false);
        cout << "message: " << ackPkt->seqno << " is acked" << endl;
        mtx1.lock();
        acked[ackPkt->seqno % N] = true;
        timer[ackPkt->seqno % N] = -1; // reset timer
        while (acked[base % N]) {
            base++;
            if (base == nextSeqNo)
                break;
        }
        mtx1.unlock();
//        increaseN(); // additive increase window size, for congs control purpose
//        goto SEND; // there is new slot in window, so go to send
    }

    timerThread.join();

    cout << "sending empty packet" << endl;
    Packet *dataPkt = new DataPacket();
    dataPkt->len = 0;
    sendUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN);
}


void SR::sendPKT(char data[], uint16_t len, int seqNo) {
    Packet *dataPkt = new DataPacket();
    dataPkt->len = len;
    dataPkt->seqno = seqNo;
    strcpy(((DataPacket *) dataPkt)->data, data);
    packets[seqNo % N] = dataPkt;
    mtx2.lock();
    sendUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN);
    mtx2.unlock();
}

void SR::sendPKT(Packet *pkt) {
    mtx2.lock();
    sendUDP(pkt, soc, addr, sizeof(*pkt));
    mtx2.unlock();
}

void timerFn(SR *sr) {
    while (1) {
        RECHECK:
        bool isFinished = true;
        for (int i = 0; i < sr->N; i++) {
            mtx1.lock();
            if (sr->timer[i] == -1) {
                mtx1.unlock();
                continue;
            }
            isFinished = false; // there are some pkts not acked yet
            double time = (clock() - sr->timer[i]) / (CLOCKS_PER_SEC / 1000);
            if (!sr->acked[i] && time > sr->timeout) { // timeout, resend
                cout << "resending " <<  sr->packets[i]->seqno << " .." << endl;
                mtx2.lock();
                sr->sendPKT(sr->packets[i]);
                mtx2.unlock();
                sr->timer[i] = clock(); // restart timer
                sr->decreaseN();
                goto RECHECK;
            }
            mtx1.unlock();
        }
        if (isFinished)
            break;
    }
}