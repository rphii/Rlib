#ifndef RBAR_H

#include <stddef.h>

#define RBAR_DEFAULT_DIGITS 78  /* 80 chars default, but remove 2 because of '[]' */
#define RBAR(digs)    (Rbar){.digits = digs ? digs : RBAR_DEFAULT_DIGITS, .last = -1}

typedef struct Rbar
{
    int digits;
    int last;
}
Rbar;

/******************************/
/* PUBLIC FUNCTION PROTOTYPES */
/******************************/

void rbar_init(Rbar *bar, size_t n, size_t m);
void rbar_show(Rbar *bar, size_t n, size_t m);

#define RBAR_H
#endif
