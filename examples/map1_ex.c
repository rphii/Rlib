#include <stdlib.h>
#include <stdio.h>
#include "../src/map.h"

size_t hash_function(uintptr_t value)
{
    return value*2654435761;
}

int main(void)
{
    /* initialize a map with both keys and values of type integer. 
     * since those are basic types, most of the settings can be zero
     */
    Map m = MAP_INIT(0, 0, hash_function, 0, 0, 0, 0, 0, 0, 0);

    int amount = 1000;
    for(int i = 0; i < amount; i++)
    {
        map_set(&m, i, i * 33);
    }
    for(int i = 0; i < amount; i++)
    {
        int got = 0;
        bool exist = map_get(&m, i, (uintptr_t *)&got);
        if(exist) printf("m[%d] = %d\n", i, got);
        else printf("m[%d] = (null)\n", i);
    }

    map_free(&m);
}
