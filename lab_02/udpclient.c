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
    char* op;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    struct cal_data sdata;
    char *hostname;
    char msg[BUFSIZE];
    //실행시 인자수가 맞는지 검사 argv[0]은 실행파일 이름
    if (argc != 3) {
        fprintf(stderr, "usage : %s <hostname>\n", argv[0]);
        exit(0);
    }

    hostname = argv[1]; //입력받은 서버주소 저장
    portno = atoi(argv[2]);  //입력받은 포트번호 저장 atoi는 char* 숫자는 int형으로 전환

    //소켓 생성 AF_INET은 IPv4, SOCK_DGRAM은 UDP 사용을 의미
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd<0)
        error("ERROR opening socket"); //-1인 경우 에러

    //호스트 정보 구함
    server = gethostbyname(hostname);
    if (server == NULL){
        fprintf(stderr,"ERROR, no such host ad %s\n", hostname);
        exit(0);
    }
    //유닉스 IPv4에 사용되는 주소 구조체 초기화
    bzero((char *)&serveraddr, sizeof(serveraddr));//
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,(char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);
    while(1) {
        printf("> ");
        fgets(msg, BUFSIZE, stdin); //수식 입력

        sscanf(msg, "%d%[^0-9]%d", &left_num, op, &right_num); //입력받은 수식은 변수에 저장

        memset((void *) &sdata, 0x00, sizeof(sdata));
        sdata.left_num = htonl(left_num); //네트워크 바이트 순서로 변환하여 저장
        sdata.right_num = htonl(right_num);
        sdata.op = op[0];

        printf(" %d %c %d =", ntohl(sdata.left_num), sdata.op, ntohl(sdata.right_num));

        serverlen = sizeof(serveraddr);
        //server로 전송
        n = sendto(sockfd, (void *) &sdata, sizeof(sdata), 0, &serveraddr, serverlen);
        //-1이면 실패
        if (n < 0)
            error("ERROR in sendto");
        // 서버에서 응답받음
        n = recvfrom(sockfd, (void *) &sdata, sizeof(sdata), 0, &serveraddr, &serverlen);
        // -1이면 실패
        if (n < 0)
            error("ERROR in recvform");
        printf(" %d\n", ntohl(sdata.result));  //결과 출력

    }

}