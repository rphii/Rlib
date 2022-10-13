#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../src/map.h"

#define MAX_STR_LEN 31

size_t hash_function(uintptr_t value)
{
    size_t hash = 5381;
    unsigned char c;
    unsigned char *str = (unsigned char *)value;
    while((c = *str++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}


int main(void)
{
    /* initialize a map with both keys and values of type integer. 
     * since those are basic types, most of the settings can be zero
     */
    Map m = MAP_INIT(0, 0, hash_function, strcmp, sizeof(char [MAX_STR_LEN+1]), strcpy, 0, 0, 0, 0);

    int amount = 1000;
    char key[MAX_STR_LEN+1] = {0};
    // first of all, set some keys
    for(int i = 0; i < amount; i++)
    {
        memset(key, 0, MAX_STR_LEN);
        sprintf(key, "Key %d", i);
        map_set(&m, (uintptr_t)key, i);
    }
    // then, remove other keys
    for(int i = 0; i < amount; i++)
    {
        if(i % 5 == 0) continue;
        memset(key, 0, MAX_STR_LEN);
        sprintf(key, "Key %d", i);
        map_del(&m, (uintptr_t)key);
    }
    // maybe add back other keys
    for(int i = amount; i < 2 * amount; i++)
    {
        if(i % 5 != 0) continue;
        memset(key, 0, MAX_STR_LEN);
        sprintf(key, "Key %d", i);
        map_set(&m, (uintptr_t)key, i);
    }
    // finally, print the map
    for(int i = 0; i < 2 * amount; i++)
    {
        uintptr_t got = 0;
        memset(key, 0, MAX_STR_LEN);
        sprintf(key, "Key %d", i);
        bool exist = map_get(&m, (uintptr_t)key, &got);
        if(exist) printf("m['%s'] = %d\n", key, (int)got);
        else printf("m['%s'] = (null)\n", key);
    }
    // at last, iterate over the map
    MapIter mi = MAP_ITER(&m);
    uintptr_t val;
    while(map_iter(&mi, (uintptr_t *)key, &val))
    {
        printf("iter: m['%s'] = %d\n", key, (int)val);
    }

    map_free(&m);
}
