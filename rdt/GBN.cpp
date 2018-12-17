//
// Created by zook on 09/12/18.
//

#include <mutex>
#include <cstring>
#include <DataPacket.h>
#include <AckPacket.h>
#include "GBN.h"

void GBN::handleReciever(int soc, struct sockaddr_in addr, string fileName) {

    FILE *fp = fopen(("../Client/" + fileName).c_str(), "wb");
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


    FILE *fp = fopen(("../Server/" + fileName).c_str(), "rb");
    char buff[MAX_DATA_LEN];
    memset(buff, 0, sizeof(buff));
    uint16_t read;

    // send first N packets
    while (nextSeqNo < base + N && (read = fread(buff, 1, sizeof(buff), fp)) >= 0) {
        sendPKT(buff, read, nextSeqNo, soc, addr);
        nextSeqNo++;
    }

    // if there are other packets to send
    while ((read = fread(buff, 1, sizeof(buff), fp)) > 0) {
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

    cout << "sending empty packet" << endl;
    Packet *dataPkt = new DataPacket();
    dataPkt->len = 0;
    sendUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN);


}


void GBN::sendPKT(char data[], uint16_t len, int seqNo, int soc, struct sockaddr_in addr) {
    Packet *dataPkt = new DataPacket();
    dataPkt->len = len;
    dataPkt->seqno = seqNo;
    strcpy(((DataPacket *) dataPkt)->data, data);
    packets[seqNo % N] = dataPkt;
    sendUDP(dataPkt, soc, addr, MAX_DATA_PACKET_LEN);
}
