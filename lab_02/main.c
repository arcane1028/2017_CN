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
    exit(1);
}
int main(int argc, char **argv) {
    int sockfd;
    int portno;
    int clientlen;
    int left_num;
    int right_num;
    int result;
    struct sockaddr_in serveraddr;
    struct sockaddr_in clientaddr;
    struct hostent *hostp;
    char  buf[BUFSIZE];
    char *hostaddrp;
    int optval;
    int n;
    struct cal_data rdata;

    if (argc != 2){
        fprintf(stderr, "usage : %s <port>\n",argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);

    sockfd = socket(AF_INET, SOCK_DGRAM,0);
    if (sockfd<0)
        error("ERROR opening socket");

    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    if (bind(sockfd,(struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
        error("ERROR on binding");
    printf("Start to run server!\n");
    clientlen = sizeof(clientaddr);
    while (1) {

        //bzero(buf, BUFSIZE);
        memset((void *)&rdata, 0x00, sizeof(rdata));
        n= recvfrom(sockfd, (void *)&rdata, sizeof(rdata), 0, (
        struct sockaddr *)&clientaddr, &clientlen);

        if (n<0)
            error("ERROR in recvfrom");

        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr),AF_INET);

        if (hostp == NULL)
            error("ERROR on gethostbyaddr");

        hostaddrp = inet_ntoa(clientaddr.sin_addr);

        if (hostaddrp == NULL)
            error("ERROR on inet_ntoa\n");

        printf("Client Info : %s (%d)\n",hostaddrp, clientaddr.sin_port );
        printf("Input : %d %c %d \n", ntohl(rdata.left_num), rdata.op, ntohl(rdata.right_num));

        left_num=ntohl(rdata.left_num);
        right_num=ntohl(rdata.right_num);

        switch (rdata.op)
        {
            case '+':
                result = left_num+right_num;
                break;
            case '-':
                result = left_num-right_num;
                break;
            case '*':
                result = left_num*right_num;
                break;
            case '/':
                if (right_num == 0){
                    rdata.error = htons(2);
                    break;
                }
                result = left_num/right_num;
                break;
        }
        printf("Result : %d\n", result );
        rdata.result = htonl(result);
        n=sendto(sockfd,(void *)&rdata, sizeof(rdata),0, (struct sockaddr *)&clientaddr, clientlen);

        if (n<0)
            error("ERROR in sendto");

    }

}