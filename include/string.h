#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t count);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t count);
size_t strlen(const char *str);
int strcmp(const char *lhs, const char *rhs);
int strncmp(const char *lhs, const char *rhs, size_t count);
char *strchr(const char* str, int ch);
char *strrchr(const char* str, int ch);

int memcmp(const void *lhs, const void *rhs, size_t count);
void *memset(void *dest, int ch, size_t count);
void *memcpy(void *dest, const void *src, size_t count);
void *memmove(void *dest, const void *src, size_t count);

#endif // _STRING_H

