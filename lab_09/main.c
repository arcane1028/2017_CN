//
// Created by jsh on 17. 11. 7.
//

#include <netinet/in.h>
#include <stdio.h>
#include <netinet/ip_icmp.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PACKET_LENGTH 65536

void PrintPacket(unsigned char *, int);

void PrintTcp(unsigned char *, int size);

void PrintData(unsigned char *, int size);

void PrintIcmp(unsigned char *buffer, int size);

void PrintUcp(unsigned char *buffer, int size);

void PrintEthernet(unsigned char *buffer, int size);

void PrintIp(unsigned char *buffer, int size);

FILE *file;


int main(int argc, char **argv)
{
    int readn;
    socklen_t addrlen;
    int sock_raw;
    struct sockaddr_in saddr;
    unsigned char *buffer = (unsigned char *) malloc(PACKET_LENGTH);

    file = fopen("log.txt", "w");
    if (file == NULL)
    {
        printf("file");
    }

    sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sock_raw < 0)
    {
        printf("socket");

        return 1;
    }
    while (1)
    {
        addrlen = sizeof(saddr);
        memset(buffer, 0x00, PACKET_LENGTH);
        readn = recvfrom(sock_raw, buffer, PACKET_LENGTH, 0, (struct sockaddr *) &saddr, &addrlen);
        if (readn < 0)
        {
            printf("readn");
            return 1;
        }
        PrintPacket(buffer, readn);
    }
    close(sock_raw);
    return 0;
}

void PrintPacket(unsigned char *buffer, int size)
{
    struct iphdr *iph = (struct iphdr *) (buffer + sizeof(struct ethhdr));
    printf("protocol : %d\n", iph->protocol);
    switch (iph->protocol)
    {
        case IPPROTO_TCP:
            printf("tcp");
            PrintTcp(buffer, size);
            break;
        case IPPROTO_UDP:
            printf("udp");

            PrintUcp(buffer, size);
            break;
        case IPPROTO_ICMP:
            printf("icmp");

            PrintIcmp(buffer, size);
        default:
            printf("protocol : %d\n", iph->protocol);
            break;
    }
}

void PrintUcp(unsigned char *buffer, int size)
{
    unsigned int iphdrlen;
    unsigned char *data;
    struct iphdr *iph = (struct iphdr *) buffer;
    iphdrlen = iph->ihl * 4;
    struct udphdr *udph = (struct udphdr*)(buffer + iphdrlen  + sizeof(struct ethhdr));

    fprintf(file, "\n============UDP Packet=============\n");

    PrintEthernet(buffer, size);
    PrintIp(buffer, size);

    fprintf(file, "\n");
    fprintf(file , "UDP Header\n");
    fprintf(file , " |-Source Port      : %d\n" , ntohs(udph->source));
    fprintf(file , " |-Destination Port : %d\n" , ntohs(udph->dest));
    fprintf(file , " |-UDP Length       : %d\n" , ntohs(udph->len));
    fprintf(file , " |-UDP Checksum     : %d\n" , ntohs(udph->check));

    fprintf(file, "\n===================================\n");
}

void PrintIcmp(unsigned char *buffer, int size)
{
    unsigned int iphdrlen;
    unsigned char *data;
    struct iphdr *iph = (struct iphdr *) buffer;
    iphdrlen = iph->ihl * 4;
    struct icmphdr *icmph = (struct icmphdr *)(buffer + iphdrlen  + sizeof(struct ethhdr));

    fprintf(file, "\n============ICMP Packet=============\n");

    PrintEthernet(buffer, size);
    PrintIp(buffer, size);

    fprintf(file, "\n");
    fprintf(file , "ICMP Header\n");
    fprintf(file , " |-Type : %d  ",icmph->type);
    fprintf(file , " |-Code : %d\n",icmph->code);
    fprintf(file , " |-Checksum : %d\n",ntohs(icmph->checksum));

    fprintf(file, "\n===================================\n");
}

void PrintTcp(unsigned char *buffer, int size)
{
    unsigned int iphdrlen;
    struct iphdr *iph = (struct iphdr *) buffer;
    iphdrlen = iph->ihl * 4;
    struct tcphdr *tcph = (struct tcphdr *) (buffer + iphdrlen + sizeof(struct ethhdr));

    fprintf(file, "\n============TCP Packet=============\n");

    PrintEthernet(buffer, size);
    PrintIp(buffer, size);

    fprintf(file, "\n");
    fprintf(file, "TCP Header\n");
    fprintf(file, " |-Source Port          : %d\n", ntohs(tcph->source));
    fprintf(file, " |-Destination Port     : %d\n", ntohs(tcph->dest));
    fprintf(file, " |-Sequence Number      : %u\n", ntohl(tcph->seq));
    fprintf(file, " |-Acknowledge Number   : %u\n", ntohl(tcph->ack_seq));
    fprintf(file, " |-Header Length        : %d DWORDS or %d BYTES \n", tcph->doff, tcph->doff * 4);
    fprintf(file, " |-Urgent Flag          : %d\n", tcph->urg);
    fprintf(file, " |-Acknowledgement Flag : %d\n", tcph->ack);
    fprintf(file, " |-Push Flag            : %d\n", tcph->psh);
    fprintf(file, " |-Reset Flag           : %d\n", tcph->rst);
    fprintf(file, " |-Synchronise Flag     : %d\n", tcph->syn);
    fprintf(file, " |-Finish Flag          : %d\n", tcph->fin);
    fprintf(file, " |-Window               : %d\n", ntohs(tcph->window));
    fprintf(file, " |-Checksum             : %d\n", ntohs(tcph->check));
    fprintf(file, " |-Urgent Pointer       : %d\n", tcph->urg_ptr);

    fprintf(file, "\n===================================\n");
}

void PrintIp(unsigned char *buffer, int size)
{
    struct sockaddr_in src;
    struct sockaddr_in dest;

    struct iphdr *iph = (struct iphdr *) (buffer + sizeof(struct ethhdr));

    memset(&src, 0, sizeof(src));
    src.sin_addr.s_addr = iph->saddr;

    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    fprintf(file, "\n");
    fprintf(file, "IP Header\n");
    fprintf(file, " |-IP Version        : %d\n", iph->version);
    fprintf(file, " |-IP Header Length  : %d DWORDS or %d Bytes\n", iph->ihl, ((iph->ihl)) * 4);
    fprintf(file, " |-Type Of Service   : %d\n", iph->tos);
    fprintf(file, " |-IP Total Length   : %d  Bytes(Size of Packet)\n", ntohs(iph->tot_len));
    fprintf(file, " |-Identification    : %d\n", ntohs(iph->id));
    fprintf(file, " |-TTL               : %d\n", iph->ttl);
    fprintf(file, " |-Protocol          : %d\n", iph->protocol);
    fprintf(file, " |-Checksum          : %d\n", ntohs(iph->check));
    fprintf(file, " |-Source IP         : %s\n", inet_ntoa(src.sin_addr));
    fprintf(file, " |-Destination IP    : %s\n", inet_ntoa(dest.sin_addr));

}

void PrintEthernet(unsigned char *buffer, int size)
{
    struct ethhdr *eth = (struct ethhdr *) buffer;

    fprintf(file, "\n");
    fprintf(file, "Ethernet Header\n");
    fprintf(file, " |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0], eth->h_dest[1],
            eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    fprintf(file, " |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0], eth->h_source[1],
            eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
    fprintf(file, " |-Protocol            : %d \n", eth->h_proto);

}

