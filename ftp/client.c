#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

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
// 다운로드 받는 함수
int file_download(int sockfd, char *file)
{
    struct Cquery query;
    int fd;
    int readn, writen;
    char buf[MAXLINE];
    //저장할 파일 연결, 없으면 생성
    if ((fd = open(file, O_WRONLY | O_CREAT)) == -1)
    {
        printf("download open error\n");
        return -1;
    }

    memset(buf, 0x00, MAXLINE);

    memset(&query, 0x00, sizeof(query));
    query.command = htonl(Q_DOWNLOAD);
    sprintf(query.filename, "%s", file);
    // 서버로 명령문 전송
    if (send(sockfd, (void *) &query, sizeof(query), 0) <= 0)
    {
        return -1;
    }
    //서버로 데이터 읽음
    while ((readn = recv(sockfd, buf, MAXLINE, 0)) > 0)
    {
        //파일에 쓰기
        writen = write(fd, buf, readn);
        if (writen != readn)
        {
            printf("download error\n");
            return -1;
        }
        memset(buf, 0x00, MAXLINE);
    }
    close(fd);
    return 1;
}
//파일을 업로드 하는 함수
int file_upload(int sockfd, char *file)
{
    struct Cquery query;
    int fd;
    int readn;
    int sendn;
    char buf[MAXLINE];
    //없로드할 파일 연결
    if ((fd = open(file, O_RDONLY|O_CREAT)) == -1)
    {
        printf("upload open error\n");
        return -1;
    }

    memset(&query, 0x00, sizeof(query));
    query.command = htonl(Q_UPLOAD);
    sprintf(query.filename, "%s", file);
    //명령문 전송
    if (send(sockfd, (void *) &query, sizeof(query), 0) <= 0)
    {
        return -1;
    }
    //서버에서 데이터 읽음
    while ((readn = read(fd, buf, MAXLINE)) > 0)
    {
        // 파일에 데이터 쓰기
        sendn = send(sockfd, buf, readn, 0);
        if (sendn != readn)
        {
            printf("upload error\n");
            return -1;
        }
    }
    close(fd);
    return 1;
}
//서버 파일 리스트 출력
int file_list(int sockfd)
{
    struct Cquery query;
    char buf[MAXLINE];
    int len;

    memset(&query, 0x00, sizeof(query));
    query.command = htonl(Q_LIST);
    //명령문 전송
    if (send(sockfd, (void *) &query, sizeof(query), 0) <= 0)
    {
        perror("Send Error\n");
        return -1;
    }
    memset(buf, 0x00, MAXLINE);
    while (1)
    {
        //전송 받은 정보 출력
        len = recv(sockfd, buf, MAXLINE, 0);
        if (len <= 0) {
            printf("========================\n");
            break;
        }
        printf("%s", buf);
        memset(buf, 0x00, MAXLINE);
    }
    printf("End!\n");
}
// 도움말 출력 함수
void help(char *progname)
{
    printf("Usage : %s -h -i [ip] -u [upload filename] -d [download filename] -l\n", progname);
}

int main(int argc, char **argv)
{
    struct sockaddr_in addr;
    int sockfd;
    int clilen;
    int opt;
    char *ipaddr;
    int command_type = 0;
    char fname[256];
    char buf[MAXLINE];

    memset(ipaddr, 0x00, sizeof(ipaddr));
    // 실행시 입력받은 옵션을 구한다.
    while ((opt = getopt(argc, argv, "hli:u:d:")) != -1)
    {
        //옵션에 따라 설정
        switch (opt)
        {
            case 'h':
                help(argv[0]);
                return 1;
            case 'i':
                sprintf(ipaddr, "%s", optarg);
                break;
            case 'u':
                command_type = Q_UPLOAD;
                sprintf(fname, "%s", optarg);
                break;
            case 'd':
                command_type = Q_DOWNLOAD;
                sprintf(fname, "%s", optarg);
                break;
            case 'l':
                command_type = Q_LIST;
                break;
            default:
                help(argv[0]);
                return 1;
        }
    }
    //소켓 생성
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error\n");
        return 1;
    }
    //소켓에 IP와 포트 지정
    memset((void *) &addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ipaddr);
    addr.sin_port = htons(PORTNUM);
    clilen = sizeof(addr);
    // 접속요청
    if (connect(sockfd, (struct sockaddr *) &addr, clilen) < 0)
    {
        printf("connect error\n");
        return 0;
    }
    // 해당 함수 실행
    while (1)
    {
        switch (command_type)
        {
            case (Q_LIST):
                file_list(sockfd);
                break;
            case (Q_DOWNLOAD):
                file_download(sockfd, fname);
                break;
            case (Q_UPLOAD):
                file_upload(sockfd, fname);
                break;
            default:
                printf("command error\n");
                break;
        }
        break;
    }
    close(sockfd);
}