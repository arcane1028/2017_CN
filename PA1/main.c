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
void router_private(int *entry, struct NAT table[10], struct packet *mypacket);
void receiver_packet(struct packet *mypacket);
void router_public(const int *entry, struct NAT table[10], struct packet *mypacket);
void printPacket(struct packet *mypacket);
void printTable(struct NAT table[10], int entry);

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
        ch = (char)getchar();
        if (ch == 'N' || ch == 'n')
            return 1;
    }
}

// 패킷을 보내는 함수
void sender_packet(struct packet *mypacket) {
    int dstAddress[4];
    int dstPort;
    // 목적지 IP, port 입력받음
    fflush(stdin);
    printf("Enter destination address\n");
    scanf("%d.%d.%d.%d", &dstAddress[0], &dstAddress[1], &dstAddress[2], &dstAddress[3]);
    fflush(stdin);
    printf("Enter destination port\n");
    scanf("%d", &dstPort);
    fflush(stdin);
    //패킷 생성
    memcpy(mypacket->dip, dstAddress, sizeof(dstAddress));
    mypacket->dport = dstPort;
    memcpy(mypacket->sip, myaddress, sizeof(myaddress));
    mypacket->sport = myport;

    printf("\nSender Sends\n\n");
    printPacket(mypacket);

}
//NAT에 도착한 패킷 처리
void router_private(int *entry, struct NAT table[10], struct packet *mypacket) {

    int addPoint;
    //NAT 받은 패킷 표시
    printf("NAT Receives Packet from Private Networks\n\n");
    printPacket(mypacket);

    //테이블에 있는지 검사
    for (int i = 0; i < *entry; ++i) {
        if (memcmp(mypacket->sip, table[i].priip, sizeof(mypacket->sip)) != 0) {
            continue;// 다른경우 다음 엔트리를 검사
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
        //엔트리가 존재 할경우
        printTable(table, *entry);

        memcpy(mypacket->sip, routeraddress, sizeof(routeraddress)); //패킷변경
        // 변경된 패킷 출력
        printf("NAT Sends Packet to External Network\n\n");
        printPacket(mypacket);
        return;
    }

    //테이블에 없을 경우 테이블에 추가
    printf("New Entry\n\n");
    addPoint = *entry;
    //테이블이 꽉찬경우
    if (*entry==9) {
        addPoint=0;
        *entry= *entry-1;
    }

    memcpy(table[addPoint].priip, mypacket->sip, sizeof(mypacket->sip));
    table[addPoint].priport = mypacket->sport;
    memcpy(table[addPoint].extip, mypacket->dip, sizeof(mypacket->dip));
    table[addPoint].extport = mypacket->dport;
    *entry = *entry + 1;

    printTable(table, *entry); //테이블 출력

    // 패킷 변경
    memcpy(mypacket->sip, routeraddress, sizeof(routeraddress));
    //변경된 패킷 출력
    printf("NAT Sends Packet to External Network\n\n");
    printPacket(mypacket);

}
// 목적지 도착한 패킷 처리
void receiver_packet(struct packet *mypacket) {
    int tmpIP[4];
    int tmpPort;

    //도착한 패킷 출력
    printf("Receiver Receives Packet\n\n");
    printPacket(mypacket);

    //목적지와 출발지를 바꿈
    memcpy(tmpIP, mypacket->sip, sizeof(tmpIP));
    memcpy(mypacket->sip,mypacket->dip, sizeof(tmpIP));
    memcpy(mypacket->dip,tmpIP, sizeof(tmpIP));

    tmpPort=mypacket->sport;
    mypacket->sport=mypacket->dport;
    mypacket->dport=tmpPort;
    //보낼 패킷 출력
    printf("Receiver Sends Packet\n\n");
    printPacket(mypacket);
}
//NAT에 도착한 패킷 처리
void router_public(const int *entry, struct NAT table[10], struct packet *mypacket) {
    //도착한 패킷 출력
    printf("NAT Receives Packet from External Networks\n\n");
    printPacket(mypacket);
    //테이블에 도착한 패킷이 있는지 검사
    for (int i = 0; i < *entry; ++i) {
        if (memcmp(mypacket->sip, table[i].extip, sizeof(mypacket->sip)) != 0) {        //IP검사
            continue;
        }
        if (memcmp(&mypacket->sport, &table[i].extport, sizeof(mypacket->sport)) != 0) { //port검사
            continue;
        }
        //패킷 IP 변경
        memcpy(mypacket->dip, table[i].priip, sizeof(mypacket->dip));
        printf("NAT Sends Packet to Private Network\n\n");
        printPacket(mypacket);
        return;
    }
}
// NAT테이블 출력하는 함수
void printTable(struct NAT table[10], int entry) {
    printf("====Current NAT Table Entry====\n");
    for (int i = 0; i< entry; ++i) {

        printf("%d.%d.%d.%d %d %d.%d.%d.%d %d\n",
               table[i].priip[0], table[i].priip[1], table[i].priip[2], table[i].priip[3],
               table[i].priport,
               table[i].extip[0], table[i].extip[1], table[i].extip[2], table[i].extip[3],
               table[i].extport);
    }
    printf("===============================\n\n");
}
// 패킷을 출력하는 함수
void printPacket(struct packet *mypacket) {
    printf("====Packet Information====\n");
    printf("Source IP : %d. %d. %d. %d\n", mypacket->sip[0], mypacket->sip[1], mypacket->sip[2], mypacket->sip[3]);
    printf("Dest   IP : %d. %d. %d. %d\n", mypacket->dip[0], mypacket->dip[1], mypacket->dip[2], mypacket->dip[3]);
    printf("Source PORT : %d\n", mypacket->sport);
    printf("Dest   PORT : %d\n", mypacket->dport);
    printf("==========================\n");
    printf("\n");
}