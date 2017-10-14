#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

//Clion 2017, MinGW로 작업했습니다.

int myaddress[4] = {192, 168, 0, 5};
int myport = 6712;
int routeraddress[4] = {168, 188, 123, 60};
int length = 0; //NAT 테이블의 길이를 저장할 변수

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

void printTable(struct NAT table[10]);

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
        ch = (char) getchar();
        if (ch == 'N' || ch == 'n')
            return 1;
    }
}

// 패킷을 보내는 함수, ip와 포트를 입력받은 뒤 패킷을 생성하고 출력한다.
void sender_packet(struct packet *mypacket) {
    int dstAddress[4];
    int dstPort;
    // 목적지 IP, port 입력받음
    fflush(stdin); //에러를 방지하기위해 표준입력을 초기화
    printf("Enter destination address\n");
    scanf("%d %d %d %d", &dstAddress[0], &dstAddress[1], &dstAddress[2], &dstAddress[3]);
    fflush(stdin);
    printf("Enter destination port\n");
    scanf("%d", &dstPort);
    fflush(stdin);
    //패킷 생성
    memcpy(mypacket->dip, dstAddress, sizeof(dstAddress));
    mypacket->dport = dstPort;
    memcpy(mypacket->sip, myaddress, sizeof(myaddress));
    mypacket->sport = myport;
    //패킷 정보 출력
    printf("\nSender Sends\n\n");
    printPacket(mypacket);

}

//NAT에 도착한 패킷 처리
void router_private(int *entry, struct NAT table[10], struct packet *mypacket) {

    //NAT 받은 패킷 표시
    printf("NAT Receives Packet from Private Networks\n\n");
    printPacket(mypacket);

    //테이블에 있는지 검사
    for (int i = 0; i < length; ++i) {
        // memecmp는 같으면 0을 출력한다.
        //그래서 0이나오면 다음 엔트리를 검사한다.
        if (memcmp(mypacket->sip, table[i].priip, sizeof(mypacket->sip)) != 0) {
            continue;// 다음 엔트리를 검사
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
        //위를 통과하면 같은 엔트리가 존재 할경우이다.
        printTable(table);
        //패킷변경
        memcpy(mypacket->sip, routeraddress, sizeof(routeraddress));
        // 변경된 패킷 출력
        printf("NAT Sends Packet to External Network\n\n");
        printPacket(mypacket);
        return;
    }

    //테이블에 없을 경우 테이블에 추가
    printf("New Entry\n\n");
    //테이블이 꽉찬 경우 처음부터 덮어씌운다.
    if (*entry == 10) {
        *entry = 0;
    }
    //테이블에 추가
    memcpy(table[*entry].priip, mypacket->sip, sizeof(mypacket->sip));
    table[*entry].priport = mypacket->sport;
    memcpy(table[*entry].extip, mypacket->dip, sizeof(mypacket->dip));
    table[*entry].extport = mypacket->dport;
    *entry = *entry + 1;
    // 길이 증가 10보다 크면 증가하지 않게 한다.
    if (length < 10)
        length = length + 1;

    printTable(table); //테이블 출력

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
    memcpy(mypacket->sip, mypacket->dip, sizeof(tmpIP));
    memcpy(mypacket->dip, tmpIP, sizeof(tmpIP));

    tmpPort = mypacket->sport;
    mypacket->sport = mypacket->dport;
    mypacket->dport = tmpPort;
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
    for (int i = 0; i < length; ++i) {
        //패킷비교
        if (memcmp(mypacket->sip, table[i].extip, sizeof(mypacket->sip)) != 0) {
            continue;
        }
        if (memcmp(&mypacket->sport, &table[i].extport, sizeof(mypacket->sport)) != 0) {
            continue;
        }
        if (memcmp(&mypacket->dip, &routeraddress, sizeof(mypacket->dip)) != 0) {
            continue;
        }
        if (memcmp(&mypacket->dport, &table[i].priport, sizeof(mypacket->dport)) != 0) {
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
void printTable(struct NAT table[10]) {
    printf("====Current NAT Table Entry====\n");
    for (int i = 0; i < length; ++i) {

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