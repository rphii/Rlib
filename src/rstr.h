#ifndef RSTR_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#define RSTR_DEFAULT_BLOCKSIZE  ((size_t)0x10)

/**
 * @brief Rstr is a basic, quite versatile string structure
 * 
 */
typedef struct Rstr
{
    /* the string */
    char *s;
    /* length in bytes */
    size_t len;
    /* how many bytes allocated so far */
    size_t allocd;
    /* blocksize of 'allocd' */
    size_t blocksize;
}
Rstr;

void rstr_free(Rstr *rstr);
void rstr_recycle(Rstr *rstr);
bool rstr_append(Rstr *rstr, char *format, ...);
bool rstr_append_va(Rstr *rstr, char *format, va_list argp);

#define RSTR_H
#endif