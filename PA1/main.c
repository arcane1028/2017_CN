#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

int myaddress[4] = {192, 168, 0, 5};
int myport = 6712;
int routeraddress[4] = {168, 188, 123, 60};

struct packet {
    int sip[4];
    int dip[4];
    int sport;
    int dport;
};

struct NAT {
    int priip[4];
    int priport;
    int extip[4];
    int extport;
};

void sender_packet(struct packet *mypacket);
void router_private(int *pInt, struct NAT pNAT[10], struct packet *pPacket);
void receiver_packet(struct packet *pPacket);
void router_public(int *pInt, struct NAT pNAT[10], struct packet *pPacket);

void printPacket(struct packet *mypacket);
void printTable(struct NAT table[10], int i);

int main() {
    struct packet *mypacket = (struct packet *) malloc(sizeof(struct packet));
    struct NAT table[10];
    int entry = 0;
    char ch = 'Y';
    while (1) {
        sender_packet(mypacket);
        router_private(&entry, table, mypacket);
        receiver_packet(mypacket);
        router_public(&entry, table, mypacket);
        printf("Continue (Y/N)\n");
        fflush(stdin);
        ch = getchar();
        if (ch == 'N' || ch == 'n')
            return 1;
    }
}

void router_public(int *entry, struct NAT table[10], struct packet *mypacket) {
    printf("NAT Receives Packet from External Networks\n\n");

    printPacket(mypacket);

    for (int i = 0; i < *entry; ++i) {
        if (memcmp(mypacket->sip, table[i].extip, sizeof(mypacket->sip)) != 0) {
            continue;
        }
        if (memcmp(&mypacket->sport, &table[i].extport, sizeof(mypacket->sport)) != 0) {
            continue;
        }
        //packet change
        memcpy(mypacket->dip, table[i].priip, sizeof(mypacket->dip));
        printf("NAT Sends Packet to Private Network\n\n");
        printPacket(mypacket);
        return;
    }
    return;
}

void receiver_packet(struct packet *mypacket) {
    int tmpIP[4];
    int tmpPort;
    printf("Receiver Receives Packet\n\n");

    printPacket(mypacket);

    printf("Receiver Sends Packet\n\n");

    memcpy(tmpIP, mypacket->sip, sizeof(tmpIP));
    memcpy(mypacket->sip,mypacket->dip, sizeof(tmpIP));
    memcpy(mypacket->dip,tmpIP, sizeof(tmpIP));

    tmpPort=mypacket->sport;
    mypacket->sport=mypacket->dport;
    mypacket->dport=tmpPort;

    printPacket(mypacket);
}

void router_private(int *entry, struct NAT table[10], struct packet *mypacket) {
    printf("NAT Receives Packet from Private Networks\n\n");

    printPacket(mypacket);

    for (int i = 0; i < *entry; ++i) {
        if (memcmp(mypacket->sip, table[i].priip, sizeof(mypacket->sip)) != 0) {
            continue;
        }
        if (memcmp(mypacket->dip, table[i].extip, sizeof(mypacket->dip)) != 0) {
            continue;
        }
        if (memcmp(&mypacket->sport, &table[i].priport, sizeof(mypacket->sport)) != 0) {
            continue;
        }
        if (memcmp(&mypacket->dport, &table[i].extport, sizeof(mypacket->dport)) != 0) {
            continue;
        }
        //packet change
        printTable(table, *entry);
        memcpy(mypacket->sip, routeraddress, sizeof(routeraddress));
        printf("NAT Sends Packet to External Network\n\n");
        printPacket(mypacket);
        return;
    }

    //entry add
    printf("New Entry\n\n");
    memcpy(table[*entry].priip, mypacket->sip, sizeof(mypacket->sip));
    table[*entry].priport = mypacket->sport;
    memcpy(table[*entry].extip, mypacket->dip, sizeof(mypacket->dip));
    table[*entry].extport = mypacket->dport;
    *entry = *entry + 1;

    printTable(table, *entry);

    // packet change
    memcpy(mypacket->sip, routeraddress, sizeof(routeraddress));
    printf("NAT Sends Packet to External Network\n\n");
    printPacket(mypacket);

    return;


}

void printTable(struct NAT table[10], int entry) {
    printf("====Current NAT Table Entry====\n");
    for (int i = 0; i < entry; ++i) {

        printf("%d.%d.%d.%d %d %d.%d.%d.%d %d\n",
               table[i].priip[0], table[i].priip[1], table[i].priip[2], table[i].priip[3],
               table[i].priport,
               table[i].extip[0], table[i].extip[1], table[i].extip[2], table[i].extip[3],
               table[i].extport);
    }
    printf("===============================\n\n");
}

void sender_packet(struct packet *mypacket) {
    int dstAddress[4];
    int dstPort;

    printf("Enter destination address\n");
    scanf("%d.%d.%d.%d", &dstAddress[0], &dstAddress[1], &dstAddress[2], &dstAddress[3]);
    printf("Enter destination port\n");
    scanf("%d", &dstPort);

    printf("\nSender Sends\n\n");
    memcpy(mypacket->dip, dstAddress, sizeof(dstAddress));
    mypacket->dport = dstPort;
    memcpy(mypacket->sip, myaddress, sizeof(myaddress));
    mypacket->sport = myport;

    printPacket(mypacket);

}

void printPacket(struct packet *mypacket) {
    printf("====Packet Information====\n");
    printf("Source IP : %d. %d. %d. %d\n", mypacket->sip[0], mypacket->sip[1], mypacket->sip[2], mypacket->sip[3]);
    printf("Dest   IP : %d. %d. %d. %d\n", mypacket->dip[0], mypacket->dip[1], mypacket->dip[2], mypacket->dip[3]);
    printf("Source PORT : %d\n", mypacket->sport);
    printf("Dest   PORT : %d\n", mypacket->dport);
    printf("==========================\n");
    printf("\n");
}