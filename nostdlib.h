#ifndef NOSTDLIB_H
#define NOSTDLIB_H

void* memset(void* ptr, int value, size_t num );

void* memcpy(void* destination, const void* source, size_t num );

char* strcpy (char* destination, const char* source);

char* strncpy(char *s1, const char *s2, size_t n);

int strncmp(const char *s1, const char *s2, size_t n);

#endif // NOSTDLIB_H

