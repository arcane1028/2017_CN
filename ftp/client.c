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

int file_download(int sockfd, char *file)
{
    struct Cquery query;
    int fd;
    int readn, writen;
    char buf[MAXLINE];

    if ((fd = open(file, O_WRONLY | O_CREAT)) == -1)
    {
        printf("download open error\n");
        return -1;
    }

    memset(buf, 0x00, MAXLINE);

    memset(&query, 0x00, sizeof(query));
    query.command = htonl(Q_DOWNLOAD);
    sprintf(query.filename, "%s", file);

    if (send(sockfd, (void *) &query, sizeof(query), 0) <= 0)
    {
        return -1;
    }
    while ((readn = recv(sockfd, buf, MAXLINE, 0)) > 0)
    {
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

int file_upload(int sockfd, char *file)
{
    struct Cquery query;
    int fd;
    int readn;
    int sendn;
    char buf[MAXLINE];

    if ((fd = open(file, O_RDONLY|O_CREAT)) == -1)
    {
        printf("upload open error\n");
        return -1;
    }

    memset(&query, 0x00, sizeof(query));
    query.command = htonl(Q_UPLOAD);
    sprintf(query.filename, "%s", file);

    if (send(sockfd, (void *) &query, sizeof(query), 0) <= 0)
    {
        return -1;
    }
    while ((readn = read(fd, buf, MAXLINE)) > 0)
    {
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

int file_list(int sockfd)
{
    struct Cquery query;
    char buf[MAXLINE];
    int len;

    memset(&query, 0x00, sizeof(query));
    query.command = htonl(Q_LIST);

    if (send(sockfd, (void *) &query, sizeof(query), 0) <= 0)
    {
        perror("Send Error\n");
        return -1;
    }
    memset(buf, 0x00, MAXLINE);
    while (1)
    {
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

    while ((opt = getopt(argc, argv, "hli:u:d:")) != -1)
    {
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

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("socket error\n");
        return 1;
    }

    memset((void *) &addr, 0x00, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ipaddr);
    addr.sin_port = htons(PORTNUM);
    clilen = sizeof(addr);

    if (connect(sockfd, (struct sockaddr *) &addr, clilen) < 0)
    {
        printf("connect error\n");
        return 0;
    }

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