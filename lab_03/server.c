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
#define OPSZ 4

void error_handling(char* message);
int calculate(int opnum, int opnds[], char operator);

int main(int argc, char* argv[]){
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr; //유닉스 IPv4에 사용되는 주소 구조체
    socklen_t clnt_adr_sz;
    char opinfo[BUF_SIZE];
    int result, opnd_cnt, i;
    int recv_cnt, recv_len;

    //실행시 인자수를 검사해 포트번호를 입력했는지 검사 argv[0]은 실행파일 이름
    if(argc!=2){
        printf("Usage : %s <port>\n",argv[0]);
        exit(1);
    }
    //소켓 생성 PF_INET은 IPv4, SOCK_DGRAM은 TCP 사용을 의미
    serv_sock=socket(PF_INET, SOCK_STREAM, 0);
    if(serv_sock==-1)
        error_handling("socket() error");

    //서버 주소 구조체 초기화 및 값 입력
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET; //주소체계
    serv_addr.sin_addr.s_addr=htonl(INADDR_ANY); //IP 주소, INADDR_ANY는 자동을 의미
    serv_addr.sin_port=htons(atoi(argv[1])); //포트정보, 실행시 입력받은 포트로 설정

    //소켓에 포트와 IP주소 지정
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling("bind() error");
    // 소켓을 통해 클라이언트의 접속 요청을 기다리도록 설정
    if (listen(serv_sock, 5)==-1)
        error_handling("listen() error");

    clnt_adr_sz = sizeof(clnt_addr);
    //최대 5번 클라이언트와 접속
    for(i=0;i<5;i++)
    {
        opnd_cnt=0;
        //클라이언트와 접속, 자동으로 클라이언트 소켓이 생성됨
        clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_adr_sz);
        read(clnt_sock,&opnd_cnt,1); //숫자 개수 수신

        recv_len=0;
        while((opnd_cnt*OPSZ+1)>recv_len) //전송된 길이를 검사
        {
            //클라이언트에서 데이터 수신, 리턴값은 수신된 데이터 길이
            recv_cnt=read(clnt_sock, &opinfo[recv_len], BUF_SIZE);
            recv_len+=recv_cnt;
        }
        result=calculate(opnd_cnt,(int *)opinfo, opinfo[recv_len-1]); //결과 값계산
        write(clnt_sock, (char*)&result, sizeof(result)); //클라이언트로 전송
        close(clnt_sock); //클라이언트 소켓 종료
    }
    close(serv_sock); //서버소켓 종료
    return 0;

}
//결과값 계산하는 함수
int calculate(int opnum, int opnds[], char op)
{
    int result=opnds[0], i;
    switch (op)
    {
        case '+':
            for (i = 1; i <opnum ; i++) result+=opnds[i];
            break;
        case '-':
            for (i = 1; i <opnum ; i++) result-=opnds[i];
            break;
        case '*':
            for (i = 1; i <opnum ; i++) result*=opnds[i];
            break;
    }
    return result;
}
//에러출력 함수
void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n',stderr);
    exit(1);
}

























