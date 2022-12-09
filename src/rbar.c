#include <stdio.h>
#include "rbar.h"

/***********************************/
/* PUBLIC FUNCTION IMPLEMENTATIONS */
/***********************************/

/**
 * @brief set the bar progress + show it
 * 
 * @param bar 
 * @param n 
 * @param m 
 */
void rbar_init(Rbar *bar, size_t n, size_t m)
{
    if(!bar) return;
    float already_f = (float)n / (float)m * 100.0f;
    int already = (size_t)bar->digits * n / m;
    if(already == bar->last) return;
    bar->last = already;
    const int paddingright = 7;
    if(already + paddingright >= bar->digits) already = bar->digits - paddingright;
    int digits = bar->digits - already + 1 - paddingright;
    printf("[%.*d%*d\b%*.1f%%]\r", already, 0, digits, 0, paddingright - 1, already_f);
    fflush(stdout);
}

/**
 * @brief show how much is done
 * 
 * @param bar 
 * @param n 
 * @param m 
 */
void rbar_show(Rbar *bar, size_t n, size_t m)
{
    rbar_init(bar, n + 1, m);
}

