#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

int in_cksum(u_short *p, int n);

int main(int argc, char **argv) {
    int icmp_socket;
    int ret;
    struct icmp *p, *rp;
    struct sockaddr_in addr, from;
    struct ip *ip;
    char buffer[1024];
    socklen_t sl;
    int hlen;
    // 소켓 생성 IPv4 사용, RAW 소켓 타입, ICMP 프로토콜 사용
    icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    // 예외 처리
    if (icmp_socket < 0) {
        perror("socket error : ");
        exit(0);
    }

    memset(buffer, 0x00, 1024);
    // ICMP 구조체 설정
    p = (struct icmp *) buffer;
    p->icmp_type = ICMP_ECHO; //목적지 호스트에 응답 요청
    p->icmp_code = 0;
    p->icmp_cksum = 0;
    p->icmp_seq = 15;
    p->icmp_id = getpid();
    p->icmp_cksum = in_cksum((u_short *) p, 1000); //체크섬 설정

    // 주소 설정
    memset(&addr, 0, sizeof(addr));
    addr.sin_addr.s_addr = inet_addr(argv[1]);
    addr.sin_family = AF_INET;
    // 주소로 데이터 전송
    // 소켓, 데이터, 데이터의 길이, 전송을 위한 옵션, 목적지 주소, 주소의 크기
    // MSG_DONTWAIT None Blocking 통신
   ret = sendto(icmp_socket, p, sizeof(*p), MSG_DONTWAIT, (struct sockaddr *) &addr, sizeof(addr));

    if (ret < 0) {
        perror("sendto error : ");
    }

    sl = sizeof(from);
    // 데이터를 수신
    ret = recvfrom(icmp_socket, buffer, 1024, 0, (struct sockaddr *) &from, &sl);

    if (ret < 0) {
        printf("%d %d %d\n", ret, errno, EAGAIN);
        perror("recvfrom error : ");
    }
    //
    ip = (struct ip *) buffer;
    // IP의 헤더길이 저장
    hlen = ip->ip_hl * 4;
    // ICMP 패킷 추출
    rp = (struct icmp *) (buffer + hlen);
    // 출력
    printf("reply from %s\n", inet_ntoa(from.sin_addr));
    printf("Type : %d \n", rp->icmp_type);
    printf("Code : %d \n", rp->icmp_code);
    printf("Seq : %d \n", rp->icmp_seq);
    printf("Iden : %d \n", rp->icmp_id);

    return 1;
}

int in_cksum(u_short *p, int n) {
    register u_short answer;
    register long sum = 0;
    u_short odd_byte = 0;
    //모든 값 더함
    while (n > 1) {
        sum += *p++;
        n -= 2;
    }
    if (n == 1) {
        *(u_char *)(&odd_byte) = *(u_char *) p;
        sum += odd_byte;
    }
    //앞자리와 뒷자리를 더함
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16); // 캐리 비트 더함
    answer = ~sum; // 보수를 구함
    return (answer);
}