#pragma once

#include <stddef.h>
#include <stdint.h>

#include <memory.h>

// Structs for stdlib
typedef struct {
    short quot, rem;
} div_t;

typedef struct {
    long quot, rem;
} ldiv_t;

 
void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
void operator delete[](void *p);

// "String" functions
size_t strlen(const char*);
char *strcat(char *dest, const char *source);
int strncmp(const char *s1, const char *s2, size_t n);

int putchar(int);

int printf(const char* __restrict, ...);
int sprintf(char* buffer, const char* fmt, ...);

// Long-functions
ldiv_t ldiv (unsigned long numerator, unsigned long denominator );
char* ltoa(unsigned long long value, char* buffer, int radix );

// Integer functions
char* itoa (int value, char* result, int base);
