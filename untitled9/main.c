#include <stdio.h>
#include <mem.h>
#include <malloc.h>

char getEncryptArange(char temp, int diff, int startChar) {
    int endChar = startChar + 27; //문자열 범위 구함
    int divChar = startChar + 1;
    int newDiff = diff%26; // 불필요한 add 크기 제거
    int store=0;
    if (temp > startChar && temp < endChar) {
        store = temp+ newDiff; //문자열에 크기만큼 더함
        // 범위 처리
        if (store >= endChar) {store = (((store - divChar) % 26) + divChar);}

        return (char)store;
    }

    return temp;
}

char getDecryptArange(char temp, int diff, int startChar) {
    int endChar = startChar + 27;
    int divChar = startChar + 1;
    int newDiff = diff%26;
    if (temp > startChar && temp < endChar) {
        temp = temp - newDiff;
        if ((temp - divChar) < 0) temp += 26; //음수가 될경우 예외처리
        if (temp <= startChar) temp = (((temp - divChar) % 26) + divChar);
    }
    return temp;
}

void getEncrypt1(const char *parse, char *encryptparse1, int parselen) {
    int i = 0;
    for (i = 0; i < parselen; ++i) {
        char temp = *(parse + i); // 한 단어 추출
        temp = getEncryptArange(temp, 1, 64); // 대문자 암호화
        temp = getEncryptArange(temp, 1, 96); // 소문자 암호화

        *(encryptparse1 + i) = temp;  // 암호화 문자열에 저장
    }
    *(encryptparse1 + parselen) = '\0'; // 문자열 끝 표시
}

void getDecrypt1(const char *encryptparse1, char *decryptparse1, int parselen) {
    int i = 0;
    for (i = 0; i < parselen; ++i) {
        char temp = *(encryptparse1 + i);
        temp = getDecryptArange(temp, 1, 64); // 대문자 부호화
        temp = getDecryptArange(temp, 1, 96); // 소문자 부호화

        *(decryptparse1 + i) = temp;
    }
    *(decryptparse1 + parselen) = '\0';
}

void getEncrypt2(const char *parse, char *encryptparse1, int parselen) {
    int i = 0;
    for (i = 0; i < parselen; ++i) {
        char temp = *(parse + i);
        temp = getEncryptArange(temp, i, 64);
        temp = getEncryptArange(temp, i, 96);

        *(encryptparse1 + i) = temp;
    }
    *(encryptparse1 + parselen) = '\0';
}

void getDecrypt2(const char *encryptparse1, char *decryptparse1, int parselen) {
    int i = 0;
    for (i = 0; i < parselen; ++i) {
        char temp = *(encryptparse1 + i);
        temp = getDecryptArange(temp, i, 64);
        temp = getDecryptArange(temp, i, 96);

        *(decryptparse1 + i) = temp;
    }
    *(decryptparse1 + parselen) = '\0';
}

void getEncrypt3(const char *parse, char *encryptparse1, int parselen) {
    int i = 0;
    for (i = 0; i < parselen; ++i) {
        char temp = *(parse + i);
        temp = getEncryptArange(temp, i*i, 64);
        temp = getEncryptArange(temp, i*i, 96);

        *(encryptparse1 + i) = temp;
    }
    *(encryptparse1 + parselen) = '\0';
}

void getDecrypt3(const char *encryptparse1, char *decryptparse1, int parselen) {
    int i = 0;
    for (i = 0; i < parselen; ++i) {
        char temp = *(encryptparse1 + i);
        temp = getDecryptArange(temp, i*i, 64);
        temp = getDecryptArange(temp, i*i, 96);

        *(decryptparse1 + i) = temp;
    }
    *(decryptparse1 + parselen) = '\0';
}

int main() {

    //char *parse = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char *parse = "HelloWorld";
    char *encryptparse1;
    char *decryptparse1;
    char *encryptparse2;
    char *decryptparse2;
    char *encryptparse3;
    char *decryptparse3;
    int parselen = strlen(parse);

    printf("parse length %d\n", parselen);
    // 암호화한 문자열 초기화
    encryptparse1 = malloc((size_t) parselen + 1);
    memset(encryptparse1, 0x00, (size_t) parselen + 1);
    encryptparse2 = malloc((size_t) parselen + 1);
    memset(encryptparse2, 0x00, (size_t) parselen + 1);
    encryptparse3 = malloc((size_t) parselen + 1);
    memset(encryptparse3, 0x00, (size_t) parselen + 1);
    // 부호화한 문자열 초기화
    decryptparse1 = malloc((size_t) parselen + 1);
    memset(decryptparse1, 0x00, (size_t) parselen + 1);
    decryptparse2 = malloc((size_t) parselen + 1);
    memset(decryptparse2, 0x00, (size_t) parselen + 1);
    decryptparse3 = malloc((size_t) parselen + 1);
    memset(decryptparse3, 0x00, (size_t) parselen + 1);

    // Case a 실행
    printf("Case a.\n");
    getEncrypt1(parse, encryptparse1, parselen); // 암호화 실행

    printf("%s\n", parse);
    printf("%s\n", encryptparse1);

    getDecrypt1(encryptparse1, decryptparse1, parselen); //부호화 실행

    printf("%s\n", decryptparse1);

    printf("\n");

    // Case b 실행
    printf("Case b.\n");
    getEncrypt2(parse, encryptparse2, parselen);

    printf("%s\n", parse);
    printf("%s\n", encryptparse2);

    getDecrypt2(encryptparse2, decryptparse2, parselen);

    printf("%s\n", decryptparse2);

    printf("\n");

    // Case c 실행
    printf("Case c.\n");
    getEncrypt3(parse, encryptparse3, parselen);

    printf("%s\n", parse);
    printf("%s\n", encryptparse3);

    getDecrypt3(encryptparse3, decryptparse3, parselen);

    printf("%s\n", decryptparse3);

    return 0;
}