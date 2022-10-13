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
    // first of all, set some keys
    for(int i = 0; i < amount; i++)
    {
        map_set(&m, i, i);
    }
    // then, remove other keys
    for(int i = 0; i < amount; i++)
    {
        if(i % 5 == 0) continue;
        map_del(&m, i);
    }
    // maybe add back other keys
    for(int i = amount; i < 2 * amount; i++)
    {
        if(i % 5 != 0) continue;
        map_set(&m, i, i);
    }
    // finally, print the map
    for(int i = 0; i < 2 * amount; i++)
    {
        uintptr_t got = 0;
        bool exist = map_get(&m, i, &got);
        if(exist) printf("m[%d] = %d\n", i, (int)got);
        else printf("m[%d] = (null)\n", i);
    }
    // at last, iterate over the map
    MapIter mi = MAP_ITER(&m);
    uintptr_t key, val;
    while(map_iter(&mi, &key, &val))
    {
        printf("iter: m[%d] = %d\n", (int)key, (int)val);
    }

    map_free(&m);
}
