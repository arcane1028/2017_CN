#include <stdio.h>
#include <mem.h>
#include <malloc.h>

char getEncryptArange(char temp, int diff, int startChar) {
    int endChar = startChar + 27;
    int divChar = startChar + 1;
    int newDiff = diff%26;
    int store=0;
    if (temp > startChar && temp < endChar) {
        store = temp+ newDiff;
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
        if ((temp - divChar) < 0) temp += 26;
        if (temp <= startChar) temp = (((temp - divChar) % 26) + divChar);
    }
    return temp;
}

void getEncrypt1(const char *parse, char *encryptparse1, int parselen) {
    int i = 0;
    for (i = 0; i < parselen; ++i) {
        char temp = *(parse + i);
        temp = getEncryptArange(temp, 1, 64);
        temp = getEncryptArange(temp, 1, 96);

        *(encryptparse1 + i) = temp;
    }
    *(encryptparse1 + parselen) = '\0';
}

void getDecrypt1(const char *encryptparse1, char *decryptparse1, int parselen) {
    int i = 0;
    for (i = 0; i < parselen; ++i) {
        char temp = *(encryptparse1 + i);
        temp = getDecryptArange(temp, 1, 64);
        temp = getDecryptArange(temp, 1, 96);

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

    char *parse = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    //char *parse = "zZ";
    char *encryptparse1;
    char *decryptparse1;
    char *encryptparse2;
    char *decryptparse2;
    char *encryptparse3;
    char *decryptparse3;
    int parselen = strlen(parse);
    int i = 0;

    printf("parse length %d\n", parselen);

    printf("A : %d, Z : %d\n", 'A', 'Z');
    printf("a : %d, z : %d\n", 'a', 'z');

    printf("q : %d, z : %d\n", 'q', 'z');


    encryptparse1 = malloc((size_t) parselen + 1);
    memset(encryptparse1, 0x00, (size_t) parselen + 1);
    encryptparse2 = malloc((size_t) parselen + 1);
    memset(encryptparse2, 0x00, (size_t) parselen + 1);
    encryptparse3 = malloc((size_t) parselen + 1);
    memset(encryptparse3, 0x00, (size_t) parselen + 1);

    decryptparse1 = malloc((size_t) parselen + 1);
    memset(decryptparse1, 0x00, (size_t) parselen + 1);
    decryptparse2 = malloc((size_t) parselen + 1);
    memset(decryptparse2, 0x00, (size_t) parselen + 1);
    decryptparse3 = malloc((size_t) parselen + 1);
    memset(decryptparse3, 0x00, (size_t) parselen + 1);

    // Test1
    printf("test1\n");
    getEncrypt1(parse, encryptparse1, parselen);

    printf("%s\n", parse);
    printf("%s\n", encryptparse1);

    getDecrypt1(encryptparse1, decryptparse1, parselen);

    printf("%s\n", decryptparse1);


    // Test2
    printf("test2\n");
    getEncrypt2(parse, encryptparse2, parselen);

    printf("%s\n", parse);
    printf("%s\n", encryptparse2);

    getDecrypt2(encryptparse2, decryptparse2, parselen);

    printf("%s\n", decryptparse2);


    // Test3
    printf("test3\n");
    getEncrypt3(parse, encryptparse3, parselen);

    printf("%s\n", parse);
    printf("%s\n", encryptparse3);

    getDecrypt3(encryptparse3, decryptparse3, parselen);

    printf("%s\n", decryptparse3);

    return 0;
}