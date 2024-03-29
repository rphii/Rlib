#ifndef RSTR_H

/////////////
// HEADERS //
/////////////

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/////////////
// DEFINES //
/////////////

#define RSTR_DEFAULT_BLOCKSIZE  ((size_t)0x10)
#define RSTR_STATIC(str)        (Rstr){.s = str, .len = sizeof(str)-1}

////////////////
// STRUCTURES //
////////////////

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

////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES //
////////////////////////////////

void rstr_free(Rstr *rstr);
void rstr_recycle(Rstr *rstr);
bool rstr_append(Rstr *rstr, char *format, ...);
bool rstr_append_va(Rstr *rstr, char *format, va_list argp);
size_t rstr_djb2(Rstr *rstr);
int rstr_cmp(Rstr *a, Rstr *b);
bool rstr_cpy(Rstr *a, Rstr *b);
bool rstr_ncpy(Rstr *a, Rstr *b, size_t n);
ssize_t rstr_find(const Rstr *str, const Rstr *sub);

#define RSTR_H
#endif
