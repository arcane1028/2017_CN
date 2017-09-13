//
// Created by jsh on 17. 9. 12.
//
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024

struct  cal_data
{
    int left_num;
    int right_num;
    char op;
    int result;
    short int error;
};

void error(char *msg){
    perror(msg);
    exit(0);
}
int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen;
    int left_num, right_num;
    char op;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    struct cal_data sdata;
    char *hostname;
    char buf[BUFSIZE];
    char msg[BUFSIZE];


    if (argc != 3) {
        fprintf(stderr, "usage : %s <hostname>\n", argv[0]);
        exit(0);
    }

    hostname = argv[1];
    portno = atoi(argv[2]);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd<0)
        error("ERROR opening socket");

    server = gethostbyname(hostname);
    if (server == NULL){
        fprintf(stderr,"ERROR, no such host ad %s\n", hostname);
        exit(0);
    }
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);
    while(1) {
        printf("Please enter msg : ");
        fgets(msg, BUFSIZE, stdin);

        sscanf(msg, "%d%c%d", &left_num, &op, &right_num);

        memset((void *) &sdata, 0x00, sizeof(sdata));
        //sdata.left_num = htonl(left_num);
        //sdata.right_num = htonl(right_num);
        sdata.left_num = left_num;
        sdata.right_num = right_num;
        sdata.op = op;

        printf(" > %d %c %d\n", sdata.left_num, sdata.op, sdata.right_num);

        serverlen = sizeof(serveraddr);
        n = sendto(sockfd, (void *) &sdata, sizeof(sdata), 0, &serveraddr, serverlen);

        if (n < 0)
            error("ERROR in sendto");

        n = recvfrom(sockfd, (void *) &sdata, sizeof(sdata), 0, &serveraddr, &serverlen);

        if (n < 0)
            error("ERROR in recvform");
        printf("Echo from server Result : %d\n", sdata.result);

    }

}