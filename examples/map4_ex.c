#include <stdlib.h>
#include <stdio.h>
#include "../src/rstr.h"
#include "../src/map.h"

int main(void)
{
    /* initialize a map with keys of type Rstr and values of type integer. 
     * since those are basic types, some of the settings can be zero,
     * and also provide info for Rstr.
     */
    Map m = MAP_INIT(0, 0, rstr_djb2, rstr_cmp, sizeof(Rstr), rstr_cpy, rstr_free, 0, 0, 0);

    Rstr key = {0};
    int amount = 1000;
    // first of all, set some keys
    for(int i = 0; i < amount; i++)
    {
        rstr_recycle(&key);
        rstr_append(&key, "Key %d", i);
        map_set(&m, (uintptr_t)&key, i);
    }
    // then, remove other keys
    for(int i = 0; i < amount; i++)
    {
        if(i % 5 == 0) continue;
        rstr_recycle(&key);
        rstr_append(&key, "Key %d", i);
        map_del(&m, (uintptr_t)&key);
    }
    // maybe add back other keys
    for(int i = amount; i < 2 * amount; i++)
    {
        if(i % 5 != 0) continue;
        rstr_recycle(&key);
        rstr_append(&key, "Key %d", i);
        map_set(&m, (uintptr_t)&key, i);
    }
    // finally, print the map
    for(int i = 0; i < 2 * amount; i++)
    {
        uintptr_t got = 0;
        bool exist = map_get(&m, (uintptr_t)&key, &got);
        if(exist) printf("m['%.*s'] = %d\n", (int)key.len, key.s, i);
        else printf("m['%.*s'] = (null)\n", (int)key.len, key.s);
    }
    // at last, iterate over the map
    MapIter mi = MAP_ITER(&m);
    uintptr_t val;
    while(map_iter(&mi, (uintptr_t *)&key, &val))
    {
        printf("iter: m['%.*s'] = %d\n", (int)key.len, key.s, (int)val);
    }

    rstr_free(&key);
    map_free(&m);
}
