//
// Created by jsh on 17. 12. 9.
//

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define PORTNUM 9090
#define MAXLINE 1024

#define Q_UPLOAD 1
#define Q_DOWNLOAD 2
#define Q_LIST 3

struct Cquery
{
    int command;
    char filename[256];
};

int process(int sockfd);

int file_upload(int sockfd, char *filename);

int file_download(int sockfd, char *filename);

int file_list(int sockfd);


int main(int argc, char **argv)
{
    struct sockaddr_in addr;
    int cli_sockfd;
    int clilen;
    int sockfd;
    pid_t pid;

    printf("FTP SERVER START\n");
    //소켓 생성
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("sockfd socket error\n");
        return -1;
    }
    //모든 IP에서 받도록 소켓의 주소 설정
    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORTNUM);
    // 소켓에 IP와 포트번호 지정
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        printf("bind error\n");
        return -1;
    }
    //클라이언트 접속 요청 기다림
    if (listen(sockfd, 100) != 0)
    {
        printf("listen error\n");
    }

    while (1)
    {
        clilen = sizeof(addr);
        //클라이언트 접속 요청 수락
        cli_sockfd = accept(sockfd, NULL, (socklen_t *) &clilen);
        if (cli_sockfd < 0)
            exit(0);
        // 프로세스 생성
        pid = fork();
        // 자식프로세스인 경우
        if (pid == 0)
        {
            //클라이언트로 온 명령어 처리 하고 종료
            process(cli_sockfd);
            close(cli_sockfd);
            sleep(1);
            exit(0);
        }
        close(cli_sockfd);

    }
    close(sockfd);

    return 0;
}

// upload 받는 함수
int file_upload(int sockfd, char *filename)
{
    int fd;
    int readn;
    int writen;
    char buf[MAXLINE];
    struct sockaddr_in addr;
    int addrlen;
    char writebuf[MAXLINE];

    printf("file_upload\n");

    // 저장할 파일을 연다. 이때 없으면 생성하도록 O_CREAT 옵션을 주었다.
    if ((fd = open(filename, O_WRONLY|O_CREAT)) == -1)
    {
        printf("upload open error\n");

        return -1;
    }

    memset(buf, 0x00, MAXLINE);
    //클라이언트로 온 정보를 파일에 적는다.
    while ((readn = recv(sockfd, buf, MAXLINE, 0)) > 0)
    {
        writen = write(fd, buf, readn);
        if (writen < 0)
        {
            printf("upload write error\n");
        }
        memset(buf, 0x00, MAXLINE);
    }
    //list 파일에 파일정보 입력
    FILE *fp = fopen("list.txt", "w");
    if (fp == NULL)
    {
        printf("upload fopen error\n");
        return 0;
    }

    addrlen = sizeof(addr);
    getpeername(sockfd, (struct sockaddr *) &addr, &addrlen);

    sprintf(writebuf, "name = %s\nip = %s\n ", filename, inet_ntoa(addr.sin_addr));
    fwrite(writebuf, 1, strlen(writebuf), fp);
    fclose(fp);

    printf("File Upload %s\n", inet_ntoa(addr.sin_addr));
    close(fd);

    return 1;
}

//file 보내는 함수
int file_download(int sockfd, char *filename)
{
    int fd;
    int readn;
    int sendn;
    char buf[MAXLINE];
    struct sockaddr_in addr;
    int addrlen;

    printf("file_download\n");

    // 보내고자할 파일을 연다.
    if ((fd = open(filename, O_RDONLY|O_CREAT)) == -1)
    {
        printf("download open error\n");
        return -1;
    }

    memset(buf, 0x00, MAXLINE);
    //읽은 데이터를 소켓을 통해 전송한다.
    while ((readn = read(fd, buf, MAXLINE)) > 0)
    {
        sendn = send(sockfd, buf, readn, 0);
        if (sendn < 0)
        {
            printf("download sendn error\n");
        }
        memset(buf, 0x00, MAXLINE);
    }
    close(fd);
    addrlen = sizeof(addr);
    getpeername(sockfd, (struct sockaddr *) &addr, &addrlen);
    printf("File Download %s\n", inet_ntoa(addr.sin_addr));

    return 1;
}
//리스트 전송하는 함수
int file_list(int sockfd)
{
    int sendn;
    char temp[MAXLINE];
    char *filep;

    printf("file_list\n");
    //리스트 파일을 연다
    FILE *fp = fopen("list.txt", "r");
    //읽은 내용을 소켓으로 전송한다.
    while (!feof(fp))
    {
        filep=fgets(temp, MAXLINE, fp);
        // list send
        sendn = send(sockfd, temp, MAXLINE, 0);
        if (sendn < 0)
        {
            printf("list sendn error\n");
        }

    }
    fclose(fp);

    printf("File Open Success %s\n",temp);


    return 1;
}
//명령어를 확인하는 함수이다.
int process(int sockfd)
{
    struct Cquery query;

    while (1)
    {
        //소켓으로 명령어를 받는다.
        if (recv(sockfd, &query, sizeof(struct Cquery), 0) <= 0)
        {
            return -1;
        }
        //명령어의 타입에 따라 해당되는 함수를 실행한다.
        query.command = ntohl(query.command);
        switch (query.command)
        {
            case (Q_UPLOAD):
                file_upload(sockfd, query.filename);
                break;
            case (Q_DOWNLOAD):
                file_download(sockfd, query.filename);
                break;
            case (Q_LIST):
                file_list(sockfd);
                break;
            default:
                printf("process switch error\n");
        }
        break;
    }
    return 1;
}
