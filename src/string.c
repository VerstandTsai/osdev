#include <string.h>

char *strcpy(char *dest, const char *src) {
    int i;
    for (i=0; src[i]; i++)
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;
}

char *strncpy(char *dest, const char *src, size_t count) {
    size_t i;
    for (i=0; src[i] && i<count; i++)
        dest[i] = src[i];
    dest[i] = '\0';
    return dest;
}

char *strcat(char *dest, const char *src) {
    strcpy(strchr(dest, '\0'), src);
    return dest;
}

char *strncat(char *dest, const char *src, size_t count) {
    strncpy(strchr(dest, '\0'), src, count);
    return dest;
}

size_t strlen(const char *str) {
    return strchr(str, '\0') - str;
}

char *strchr(const char* str, int ch) {
    for (; *str || !ch; str++)
        if (*str == ch)
            return (char*)str;
    return NULL;
}

void *memset(void *dest, int ch, size_t count) {
    while (count--)
        ((char*)dest)[count] = ch;
    return dest;
}

void *memcpy(void *dest, const void *src, size_t count) {
    while (count--)
        ((char*)dest)[count] = ((char*)src)[count];
    return dest;
}

void *memmove(void *dest, const void *src, size_t count) {
    if (dest > src)
        while (count--)
            ((char*)dest)[count] = ((char*)src)[count];
    else
        for (size_t i=0; i<count; i++)
            ((char*)dest)[i] = ((char*)src)[i];
    return dest;
}

