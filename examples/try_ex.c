#include <stdlib.h>
#include <stdint.h>

#include "../src/try.h"

int main(void)
{
    TRY {
        char *buf = malloc(INT64_MAX);
        if(!buf) THROW("could not allocate all the memory");
    } CATCH {
        /* handle potential error */
        WARN("error reached!");
    }

    TRY {
        if(0) THROW("the value is 0");
    } CATCH {
        /* handle potential error */
        printf("error reached!\n");
    }

    return 0;
}
