#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "../src/rstr.h"
#include "../src/map.h"

#define RAND_STR_BYTES    (1+16)

void get_rand_str(char out[RAND_STR_BYTES])
{
    for(int i = 0; i + 1 < RAND_STR_BYTES; i++)
    {
        out[i] = rand() % ('z'-'a') + 'a';
    }
    out[RAND_STR_BYTES - 1] = 0;
}

int main(void)
{
    // initialize srand
    srand(time(0));

    /* initialize a map with both keys and values of type Rstr.
     * since those are custom structs, also provide their size,
     * copy and free function.
     */
    Map m = MAP_INIT(0, 0, rstr_djb2, rstr_cmp, sizeof(Rstr), rstr_cpy, rstr_free, sizeof(Rstr), rstr_cpy, rstr_free);

    Rstr key = {0};
    Rstr val = {0};
    int amount = 10000;
    for(int i = 0; i < amount; i++)
    {
        char rand_str[RAND_STR_BYTES] = {0};
        get_rand_str(rand_str);
        rstr_recycle(&key);
        rstr_recycle(&val);
        rstr_append(&key, "Key %d", i);
        rstr_append(&val, "Value %s", rand_str);
        map_set(&m, (uintptr_t)&key, (uintptr_t)&val);
    }
    for(int i = 0; i < amount; i++)
    {
        rstr_recycle(&key);
        rstr_append(&key, "Key %d", i);
        bool exist = map_get(&m, (uintptr_t)&key, (uintptr_t *)&val);
        if(exist) printf("m['%.*s'] = %.*s\n", (int)key.len, key.s, (int)val.len, val.s);
        else printf("m[%.*s] = (null)\n", (int)key.len, key.s);
    }

    rstr_free(&key);
    rstr_free(&val);
    map_free(&m);
}
