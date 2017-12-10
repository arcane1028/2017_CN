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

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("sockfd socket error\n");
        return -1;
    }

    memset(&addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORTNUM);

    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
    {
        printf("bind error\n");
        return -1;
    }


    if (listen(sockfd, 100) != 0)
    {
        printf("listen error\n");
    }

    while (1)
    {
        clilen = sizeof(addr);
        cli_sockfd = accept(sockfd, NULL, (socklen_t *) &clilen);
        if (cli_sockfd < 0)
            exit(0);
        pid = fork();
        if (pid == 0)
        {
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

// upload 받
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

    // send file
    if ((fd = open(filename, O_WRONLY|O_CREAT)) == -1)
    {
        printf("upload open error\n");

        return -1;
    }

    memset(buf, 0x00, MAXLINE);

    while ((readn = recv(sockfd, buf, MAXLINE, 0)) > 0)
    {
        writen = write(fd, buf, readn);
        if (writen < 0)
        {
            printf("upload write error\n");
        }
        memset(buf, 0x00, MAXLINE);
    }
    //list add

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

//file send
int file_download(int sockfd, char *filename)
{
    int fd;
    int readn;
    int sendn;
    char buf[MAXLINE];
    struct sockaddr_in addr;
    int addrlen;

    printf("file_download\n");

    // send file
    if ((fd = open(filename, O_RDONLY|O_CREAT)) == -1)
    {
        printf("download open error\n");
        return -1;
    }

    memset(buf, 0x00, MAXLINE);

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

int file_list(int sockfd)
{
    int sendn;
    char temp[MAXLINE];
    char *filep;

    printf("file_list\n");

    FILE *fp = fopen("list.txt", "r");

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

int process(int sockfd)
{
    struct Cquery query;

    while (1)
    {
        if (recv(sockfd, &query, sizeof(struct Cquery), 0) <= 0)
        {
            return -1;
        }
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
