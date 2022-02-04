#include <stdio.h>
#include <stdlib.h>

#include "../src/rstr.h"

int main(void)
{
    // initialize a new Rstr
    Rstr rstr = {0};
    // create a decently long string
    for(size_t i = 0; i < 1000; i++)
    {
        rstr_append(&rstr, "Number %llu.\n", i);
    }
    // print that string
    printf("%s\n", rstr.s);

    // sometimes you want to 'recycle' the string (keeping the memory) but write a new string to it...
    rstr_recycle(&rstr);
    for(size_t i = 0; i < 1000; i++)
    {
        rstr_append(&rstr, "%llu is a number!\n", i);
    }
    // print that string
    printf("%s\n", rstr.s);
    
    // now we can finally free the string
    rstr_free(&rstr);

    return 0;
}

