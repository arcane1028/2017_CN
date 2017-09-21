//
// Created by jsh on 17. 9. 19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
#define RLT_SIZE 4
#define OPSZ 4

void error_handling(char* message);  //에러출력 함수

int main(int argc, char* argv[])
{
    int sock;
    struct sockaddr_in serv_addr;//유닉스 IPv4에 사용되는 주소 구조체
    char opmsg[BUF_SIZE];
    int result, opnd_cnt, i;

    //실행시 인자수를 검사해 포트번호를 입력했는지 검사 argv[0]은 실행파일 이름
    if(argc!=3)
    {
        printf("Usage : %s\n <IP> <port>\n", argv[0]);
        exit(1);
    }
    //소켓 생성 PF_INET은 IPv4, SOCK_DGRAM은 TCP 사용을 의미
    sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock==-1)
        error_handling("socket() error");
    //서버 주소 구조체 초기화 및 값 입력
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;//주소체계
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]); //IP 주소, 실행시 입력받은 주소로 설정
    serv_addr.sin_port=htons(atoi(argv[2]));//포트정보, 실행시 입력받은 포트로 설정

    //소켓을 통해 서버 주소로 접속을 요청
    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("connect() error");

    // 숫자 개수 입력
    fputs("oper count : ",stdout);
    scanf("%d", &opnd_cnt);
    opmsg[0]=(char)opnd_cnt;//메시지의 첫 원소를 연산 갯수로 설정

    // 계산할 숫자 입력
    for (i = 0; i < opnd_cnt; ++i) {
        printf("operand %d : ", i+1);
        scanf("%d", (int*)&opmsg[i*OPSZ+1]);//메시지에 숫자 저장 int형이므로 4씩 건너띔
    }
    //연산자 입력 받음
    fgetc(stdin);
    fputs("operator : ", stdout);
    scanf("%c", &opmsg[opnd_cnt*OPSZ+1]); //숫자 마지막에 연산자 저장
    write(sock, opmsg, opnd_cnt*OPSZ+2); //데이터 전송
    read(sock, &result, RLT_SIZE); // 데이터 수신

    printf("result = %d\n\n", result);  //화면에 출력

    close(sock); //소켓 종료
    return 0;

}
//에러출력 함수
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}