#ifndef UTILS_H
#define UTILS_H 

#include "sha256/sha256.h"

int cmp(const char * _Str1, const char * _Str2);
int stremp(const char * _Str);
void sha256_string(char *input, char output[65]);

#endif