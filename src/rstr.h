#ifndef RSTR_H

/////////////
// HEADERS //
/////////////

#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

/////////////
// DEFINES //
/////////////

#define RSTR_DEFAULT_BLOCKSIZE  ((size_t)0x10)

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
    /* length of string (without '\0') */
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


#define RSTR_H
#endif
