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
    //실행시 인자수가 맞는지 검사 argv[0]은 실행파일 이름
    if (argc != 2){
        fprintf(stderr, "usage : %s <port>\n",argv[0]);
        exit(1);
    }
    portno = atoi(argv[1]);  //입력받은 포트번호 저장 atoi는 char* 숫자는 int형으로 전환
    //소켓 생성 AF_INET은 IPv4, SOCK_DGRAM은 UDP 사용을 의미
    sockfd = socket(AF_INET, SOCK_DGRAM,0);
    if (sockfd<0)
        error("ERROR opening socket");

    optval = 1;
    //소켓 옵션값을 설정하는 함수
    //SOL_SOCKET은 소켓 레벨에서 처리함을 의미, SO_REUSEADDR 주소를 재사용을 의미
    //optval  옵션정보를 저장할 버퍼의 위치
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    //유닉스 IPv4에 사용되는 주소 구조체 초기화
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);

    //소켓에 포트와 IP주소 지정
    if (bind(sockfd,(struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0)
        error("ERROR on binding");
    printf("Start to run server!\n");
    clientlen = sizeof(clientaddr);
    while (1) {

        //bzero(buf, BUFSIZE);
        memset((void *)&rdata, 0x00, sizeof(rdata));
        //client에서 정보 받음
        n= recvfrom(sockfd, (void *)&rdata, sizeof(rdata), 0, (struct sockaddr *)&clientaddr, &clientlen);

        if (n<0)
            error("ERROR in recvfrom");
        //주소로 host정보 얻음
        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr),AF_INET);

        if (hostp == NULL)
            error("ERROR on gethostbyaddr");
        //네트워크 바이트를 char*로 변환
        hostaddrp = inet_ntoa(clientaddr.sin_addr);

        if (hostaddrp == NULL)
            error("ERROR on inet_ntoa\n");

        printf("Client Info : %s (%d)\n",hostaddrp, clientaddr.sin_port );
        printf("Input : %d %c %d \n", ntohl(rdata.left_num), rdata.op, ntohl(rdata.right_num));

        //네트워크 바이트를 int형으로 변환
        left_num=ntohl(rdata.left_num);
        right_num=ntohl(rdata.right_num);
        //값 계산
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
        // 전송
        n=sendto(sockfd,(void *)&rdata, sizeof(rdata),0, (struct sockaddr *)&clientaddr, clientlen);

        if (n<0)
            error("ERROR in sendto");

    }

}