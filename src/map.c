//////////////
// INCLUDES //
//////////////

#include <stdio.h>
#include <string.h>

#include "map.h"

//////////////////////////////////////
// PRIVATE FUNCTION IMPLEMENTATIONS //
//////////////////////////////////////

/**
 * @brief 
 * 
 * @param map 
 * @param key 
 * @param bucket 
 * @param index 
 * @return true 
 * @return false 
 */
static bool static_map_indexing(Map *map, uintptr_t key, MapBucket **bucket, size_t *index8)
{
    // calculate hash for the key
    size_t hash = map->hash(key) % map->slots;
    // set up bucket and exist variable
    size_t index = 0;
    *bucket = &map->b[hash];
    bool exist = false;
    // calculate size_tot of one element
    size_t size_key = map->size.key ? map->size.key : sizeof(map->b->node->key);
    size_t size_val = map->size.val ? map->size.val : sizeof(map->b->node->val);
    size_t size_tot = size_key + size_val;
    // check if the key exists
    if(map->size.key)
    {
        void *key_p = (void *)key;
        if(!key_p) return false;
        if(map->cmp)
        {
            for(index = 0; index < (*bucket)->used; (index)++)
            {
                MapNode *node = (MapNode *)((uint8_t *)(*bucket)->node + index * size_tot);
                if(map->cmp(&node->key_p, key_p)) continue;
                exist = true;
                break;
            }
        }
        else
        {
            for(index = 0; index < (*bucket)->used; (index)++)
            {
                MapNode *node = (MapNode *)((uint8_t *)(*bucket)->node + index * size_tot);
                if(memcmp(&node->key_p, key_p, map->size.key)) continue;
                exist = true;
                break;
            }
        }
    }
    else for(index = 0; index < (*bucket)->used; (index)++)
    {
        MapNode *node = (MapNode *)((uint8_t *)(*bucket)->node + index * size_tot);
        if(node->key != key) continue;
        exist = true;
        break;
    }
    *index8 = index * size_tot;
    return exist;
}

static inline size_t static_map_size_key(Map *map)
{
    return map->size.key ? map->size.key : sizeof(map->b->node->key);
}

static inline size_t static_map_size_val(Map *map)
{
    return map->size.val ? map->size.val : sizeof(map->b->node->val);
}

/////////////////////////////////////
// GLOBAL FUNCTION IMPLEMENTATIONS //
/////////////////////////////////////

size_t map_hash_djb2(uintptr_t *str)
{
    size_t hash = 5381;
    char *str_p = (char *)str;
    unsigned char c;
    while((c = *str_p++))
    {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

/**
 * @brief free up memory when done using the map
 * 
 * @param map 
 */
void map_free(Map *map)
{
    if(!map) return;
    // calculate size_tot of one element
    size_t size_key = static_map_size_key(map);
    size_t size_val = static_map_size_val(map);
    size_t size_tot = size_key + size_val;
    // go over each slot
    for(size_t i = 0; i < map->slots; i++)
    {
        MapBucket *bucket = &map->b[i];
        for(size_t j = 0; j < bucket->used; j++)
        {
            // set key to zero
            MapNode *node = (MapNode *)((uint8_t *)bucket->node + j * size_tot);
            if(map->fk) map->fk(&node->key_p);
            node->key_p = 0;
            // set val to zero
            node = (MapNode *)((uint8_t *)node + size_key);
            if(map->fv) map->fv(&node->val_p);
            node->val_p = 0;
        }
        free(bucket->node);
    }
    memset(map->b, 0, sizeof(MapBucket) * map->slots);
}

/**
 * @brief within the given map, set the specified key to the value. 
 *        values are overwritten, when they already exist.
 * 
 * @param map the given map
 * @param key the key
 * @param val the value
 * @return true success
 * @return false an error occured
 */
bool map_set(Map *map, uintptr_t key, uintptr_t val)
{
    if(!map) return false;
    bool result = true;
    // check if the key exists
    size_t index8 = 0;
    MapBucket *bucket = 0;
    bool exist = static_map_indexing(map, key, &bucket, &index8);
    // calculate size_tot of one element
    size_t size_key = static_map_size_key(map);
    size_t size_val = static_map_size_val(map);
    size_t size_tot = size_key + size_val;
    // add node if not exist
    if(!exist)
    {
        size_t is_count = ((bucket->used + map->batch - 1) / map->batch) * map->batch;
        size_t new_count = (bucket->used / map->batch + 1) * map->batch;
        if(new_count > is_count)
        {
            // allocate memory
            void *temp = realloc(bucket->node, new_count * size_tot);
            if(!temp) return false;
            // initialize new memory
            bucket->node = temp;
            memset((uint8_t *)bucket->node + bucket->used * size_tot, 0, map->batch * size_tot);
        }
        // increment used
        bucket->used++;
    }
    MapNode *node = (MapNode *)((uint8_t *)bucket->node + index8);
    // assign key
    if(map->size.key)
    {
        void *key_p = (void *)key;
        if(!key_p) return false;
        if(map->av) result &= map->av(&node->key_p, key_p);
        else memcpy(&node->key_p, key_p, map->size.key);
    }
    else node->key = key;
    // assign value
    if(map->size.val)
    {
        node = (MapNode *)((uint8_t *)node + size_key);
        void *val_p = (void *)val;
        if(!val_p) return false;
        if(map->ak) result &= map->ak(&node->val_p, val_p);
        else memcpy(&node->val_p, val_p, map->size.val);
    }
    else node->val = val;
    return result;
}

/**
 * @brief within the given map, retrieve the value matching the specified key.
 * 
 * @param map the given map
 * @param key specified key
 * @param got_p the retrieved value
 * @return true success
 * @return false an error occured
 */
bool map_get(Map *map, uintptr_t key, uintptr_t *val_p)
{
    if(!map || !val_p) return false;
    // check if the key exists
    bool result = true;
    size_t index8 = 0;
    MapBucket *bucket = 0;
    bool exist = static_map_indexing(map, key, &bucket, &index8);
    if(!exist) return false;
    size_t size_key = static_map_size_key(map);
    MapNode *node = (MapNode *)((uint8_t *)bucket->node + index8);
    // calculate size_tot of one element
    // assign to val_p if it does
    if(map->size.val)
    {
        node = (MapNode *)((uint8_t *)node + size_key);
        // maybe don't copy the value, but just return the address??
        if(map->av) result &= map->av(val_p, &node->val_p);
        else memcpy(val_p, &node->val_p, map->size.val);
    }
    else *val_p = node->val;
    return result;
}

/**
 * @brief within the given map, delete the value matching the specified key.
 * 
 * @param map the given map
 * @param key specified key
 * @return true success
 * @return false an error occured
 */
bool map_del(Map *map, uintptr_t key)
{
    if(!map) return false;
    // check if the key exists
    size_t index8 = 0;
    MapBucket *bucket = 0;
    bool exist = static_map_indexing(map, key, &bucket, &index8);
    if(!exist) return false;
    // calculate size_tot of one element
    size_t size_key = static_map_size_key(map);
    size_t size_val = static_map_size_val(map);
    size_t size_tot = size_key + size_val;
    // free val if it exists
    MapNode *node = (MapNode *)((uint8_t *)bucket->node + index8 + size_key);
    if(map->fv) map->fv(&node->val_p);
    // free key if it exists (make this later, because then the node address is right)
    node = (MapNode *)((uint8_t *)node - size_key);
    if(map->fk) map->fk(&node->key_p);
    // this should never happen, but do check for it...
    if(!bucket->used) return false;
    // decrease used and calculate how many elements remain
    size_t move_count = --bucket->used * size_tot - index8;
    // move memory in place
    if(move_count) memmove(node, (uint8_t *)node + size_tot, move_count);
    // set old memory to zero
    memset((uint8_t *)bucket->node + bucket->used * size_tot, 0, size_tot);
    // possibly reallocate
    size_t was_count = ((bucket->used + map->batch) / map->batch) * map->batch;
    size_t new_count = ((bucket->used + map->batch - 1) / map->batch) * map->batch;
    // don't do realloc with size of 0 because it may be undefined
    if(new_count && new_count < was_count)
    {
        void *temp = realloc(bucket->node, new_count * size_tot);
        if(!temp) return false;
        bucket->node = temp;
    }
    return true;
}

/**
 * @brief iterate over elements within a map
 * 
 * @param map_it the map iterator
 * @param key_p the retrieved key
 * @param val_p the retrieved value
 * @return true 
 * @return false 
 */
bool map_iter(MapIter *map_it, uintptr_t *key_p, uintptr_t *val_p) // maybe make a macro to replace the while (??)
{
    if(!map_it) return false;
    bool result = true;
    Map *map = map_it->m;
    MapBucket *bucket = 0;
    // search for valid index
    while(map_it->i < map->slots)
    {
        bucket = &map->b[map_it->i];
        // increment indices accordingly
        if(map_it->j < bucket->used) break;
        map_it->j = 0;
        map_it->i++;
    }
    // calculate size_tot of one element
    size_t size_key = static_map_size_key(map);
    size_t size_val = static_map_size_val(map);
    size_t size_tot = size_key + size_val;
    // make sure we're still in the map
    if(map_it->i >= map->slots) return false;
    // assign the key
    if(key_p)
    {
        MapNode *node = (MapNode *)((uint8_t *)bucket->node + map_it->j * size_tot);
        if(map->size.key)
        {
            if(map->ak) result &= map->ak(key_p, &node->key_p);
            else memcpy(key_p, &node->key_p, map->size.key);
        }
        else *key_p = node->key;
    }
    // assign the value
    if(val_p)
    {
        MapNode *node = (MapNode *)((uint8_t *)bucket->node + map_it->j * size_tot);
        if(map->size.val)
        {
            node = (MapNode *)((uint8_t *)node + size_key);
            if(map->av) result &= map->av(val_p, &node->val_p);
            else memcpy(val_p, &node->val_p, map->size.val);
        }
        else *val_p = node->val;
    }
    // next index
    map_it->j++;
    return result;
}

/**
 * @brief print some map statistics
 * 
 * @param map 
 */
void map_stats(Map *map)
{
    size_t min_used = SIZE_MAX;
    size_t max_used = 0;
    size_t total_used = 0;
    // size_t allocs_made = 0;
    double avg_used = 0;
    for(size_t i = 0; i < map->slots; i++)
    {
        size_t used = map->b[i].used;
        total_used += used;
        if(used < min_used) min_used = used;
        if(used > max_used) max_used = used;
        // allocs_made += (used / map->batch);
    }
    avg_used = (double)total_used / (double)map->slots;
    // size_t alloced_bytes = allocs_made * map->batch;
    // output
    printf("map stats:\n");
    printf("- slots: %zu\n", map->slots);
    printf("- total used subslots: %zu\n", total_used);
    printf("- max use of subslots: %zu\n", max_used);
    printf("- avg use of subslots: %.4f\n", avg_used);
    printf("- min use of subslots: %zu\n", min_used);
}

