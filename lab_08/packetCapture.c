//
// Created by jsh on 17. 11. 7.
//

#include <netinet/in.h>
#include <stdio.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PACKET_LENGTH 65536

void PrintPacket(unsigned char * , int);
void PrintTcp(unsigned char *, int size);
void PrintData(unsigned char *, int size);

int main(int argc, char **argv)
{
    int readn;
    socklen_t addrlen;
    int sock_raw;
    struct sockaddr_in saddr;

    unsigned char *buffer = (unsigned char *)malloc(PACKET_LENGTH);
    //소켓 설정
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock_raw < 0)
    {
        return 1;
    }
    while (1)
    {
        addrlen = sizeof(saddr);
        memset(buffer, 0x00, PACKET_LENGTH);
        // 데이터를 읽음
        readn = recvfrom(sock_raw, buffer, PACKET_LENGTH, 0, (struct sockaddr * )&saddr, &addrlen);
        if (readn<0)
        {
            return 1;
        }
        // 패킷 출력
        PrintPacket(buffer, readn);
    }
    close(sock_raw);
    return 0;
}

void PrintPacket(unsigned char * buffer, int size)
{
    // 패킷헤더 추출
    struct iphdr *iph = (struct iphdr*)buffer;
    printf("protocol : %d\n",iph->protocol);
    // 프로토콜 추출
    switch (iph->protocol)
    {
        case IPPROTO_TCP: // TCP인 경우
            PrintTcp(buffer, size);
            break;
        default:
            break;
    }
}
void PrintTcp(unsigned char * buf, int size)
{
    unsigned short iphdrlen;
    unsigned char *data;

    struct iphdr *iph = (struct iphdr *)buf;
    iphdrlen = iph->ihl*4; //IP 헤더 길이
    struct tcphdr *tcph = (struct tcphdr*)(buf + iphdrlen); //IP의 데이터 추출
    // TCP 데이터 추출
    data = (unsigned char * )(buf + (iph->ihl*4) + (tcph->doff*4) );
    printf("%s", data);

}

