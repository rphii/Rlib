#include <stdlib.h>
#include <stdio.h>
#include "../src/rstr.h"
#include "../src/map.h"

size_t hash_function(uintptr_t value)
{
    return value*2654435761;
}

int main(void)
{
    /* initialize a map with keys of type integer and values of type Rstr. 
     * since those are basic types, most of the settings can be zero,
     * but do provide info for Rstr.
     */
    Map m = MAP_INIT(0, 0, hash_function, 0, 0, 0, 0, sizeof(Rstr), rstr_cpy, rstr_free);

    Rstr val = {0};
    int amount = 1000;
    // first of all, set some keys
    for(int i = 0; i < amount; i++)
    {
        rstr_recycle(&val);
        rstr_append(&val, "The value is %d", i);
        map_set(&m, i, (uintptr_t)&val);
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
        rstr_recycle(&val);
        rstr_append(&val, "The value is %d", i);
        map_set(&m, i, (uintptr_t)&val);
    }
    // finally, print the map
    for(int i = 0; i < 2 * amount; i++)
    {
        bool exist = map_get(&m, i, (uintptr_t *)&val);
        if(exist) printf("m[%d] = %.*s\n", i, (int)val.len, val.s);
        else printf("m[%d] = (null)\n", i);
    }
    // at last, iterate over the map
    MapIter mi = MAP_ITER(&m);
    uintptr_t key;
    while(map_iter(&mi, &key, (uintptr_t *)&val))
    {
        printf("iter: m[%d] = %.*s\n", (int)key, (int)val.len, val.s);
    }

    rstr_free(&val);
    map_free(&m);
}
