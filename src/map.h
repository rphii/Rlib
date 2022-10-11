#ifndef MAP_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


/////////////
// DEFINES //
/////////////

#define MAP_DEFAULT_SLOTS 64    // default slot size
#define MAP_DEFAULT_BATCH 4     // default batch sizes

/**
 * @brief shorthand macro to create a Map struct.
 * @param n_slots number of buckets/slots (static). 0=default
 * @param n_batch only each n-th subslot is re-allocated (dynamic). 0=default
 * @param hash_f (required) hash function (operates on key-type)
 * @param cmp_f (optional*) compare function (operates on key-type)
 * @param size_k (optional*) size in bytes of key-type
 * @param assign_k (optional**) function to copy/assign key-type
 * @param free_k (optional**) function to free key-type
 * @param size_v (optional*) size in bytes fo value-type
 * @param assign_v (optional**) function to copy/assign value-type
 * @param free_v (optional**) function to free value-type
 * 
 * @note optional* if you operate on basic types, such as int or char, you don't need it.
 * @note optional** if your custom structure requires extra steps to copy/free, basic types don't need it
 */
#define MAP_INIT(n_slots,n_batch,hash_f,cmp_f,size_k,assign_k,free_k,size_v,assign_v,free_v) (Map){\
    .batch = n_batch ? n_batch : MAP_DEFAULT_BATCH,\
    .slots = n_slots ? n_slots : MAP_DEFAULT_SLOTS,\
    .b = (MapBucket[n_slots ? n_slots : MAP_DEFAULT_SLOTS]){{0}},\
    .size.key = size_k,\
    .size.val = size_v,\
    .hash = (MapHash)hash_f,\
    .cmp = (MapCmp)cmp_f,\
    .ak = (MapAssign)assign_k,\
    .av = (MapAssign)assign_v,\
    .fk = (MapFree)free_k,\
    .fv = (MapFree)free_v}

#define MAP_ITER(map)   (MapIter){.m = map}

//////////////////////////////
// PUBLIC FUNCTION TYPEDEFS //
////////////////////////////// 

typedef size_t (*MapHash)(uintptr_t a);         // takes one parameter and returns the calculated hash for that
typedef bool (*MapCmp)(void *a, void *b);       // compares two values for equality (true if equal)
typedef bool (*MapAssign)(void *a, void *b);    // assigns one value to another; c equivalent: a=b;
typedef void (*MapFree)(void *a);               // free a value; c equivalend: free(a);

/////////////
// STRUCTS //
/////////////

typedef struct MapNode
{
    union
    {
        uintptr_t key;
        void *key_p;
    };
    union
    {
        uintptr_t val;
        void *val_p;
    };
}
MapNode;

typedef struct MapBucket
{
    size_t used;    // used nodes
    MapNode *node;  // actual nodes
}
MapBucket;

typedef const struct Map
{
    MapBucket *b;           // buckets/slots
    MapNode size;           // size of key/val in bytes
    size_t slots, batch;    // nb of slots/batch
    MapHash hash;           // required; hashing function
    MapCmp cmp;             // optional; compare function
    MapAssign ak, av;       // optional; assign key/value
    MapFree fk, fv;         // optional; free key/value
}
Map;

typedef struct MapIter
{
    Map *m;
    size_t i,j;
}
MapIter;

////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES //
////////////////////////////////

size_t map_hash_djb2(uintptr_t *str);

void map_free(Map *map);
bool map_set(Map *map, uintptr_t key, uintptr_t val);
bool map_get(Map *map, uintptr_t key, uintptr_t *val_p);
bool map_del(Map *map, uintptr_t key);

bool map_iter(MapIter *map_it, uintptr_t *key_p, uintptr_t *val_p);

void map_stats(Map *map);

#define MAP_H
#endif
