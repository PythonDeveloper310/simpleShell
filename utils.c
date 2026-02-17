#include "sha256/sha256.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

int cmp(const char * _Str1, const char * _Str2) {
    return strcmp(_Str1, _Str2) == 0;
}

int stremp(const char * _Str) {
    return strlen(_Str) == 0;
}

void sha256_string(char *input, char output[65]) {
    unsigned char hash[SHA256_DIGEST_SIZE];

    SHA256((unsigned char*)input, strlen(input), hash);

    for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
        sprintf(output + (i * 2), "%02x", hash[i]);
    }

    output[64] = 0;
}