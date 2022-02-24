#ifndef RSON_H

#define USE_RSTR    false
#define USE_R2STR   false

#if USE_RSTR
#include "rstr.h"
#endif
#if USE_R2STR
#include "r2str.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define RSON_MAX_DEPTH  0x10
#define RSON_MAP_SIZE   0x10

typedef struct Stack Stack;

typedef enum RsonList
{
    RSON_OK,
    // keep above the first or you'll mess up my error system
    RSON_ERROR, // general error -> adjust later lmao
    RSON_ERROR_ARGS,
    RSON_ERROR_REALLOC,
    RSON_ERROR_REGISTER_KEY,
    RSON_ERROR_MAX_DEPTH,
    RSON_ERROR_STRING_BEGIN,
    RSON_ERROR_INVALID_ESCAPE_SEQUENCE,
    RSON_ERROR_INVALID_HEX,
    RSON_ERROR_EXPECT_COLON,
    RSON_ERROR_OBJECT_END,
}
RsonList;

typedef enum
{
    RSON_NONE,
    RSON_NULL,
    RSON_OBJECT,
    RSON_ARRAY,
    RSON_VALUE,
    RSON_STRING,
    RSON_INT,
    RSON_DOUBLE,
    RSON_BOOL,
}
RsonValueList;

typedef struct RsonFormat
{
    /* true: format will be in the same order as in the source */
    bool ordered;
    /* true: indent with tabs; false: indent with spaces */
    bool indent_tab;
    /* max. indent depth */
    int indent_depth;
    /* max. newline depth for objects */
    int newline_depth_obj;
    /* max. newline depth for arrays */
    int newline_depth_arr;
}
RsonFormat;

typedef struct RsonObject
{
    /* the object map (hash table) */
    struct
    {
        /* the key */
        char *key;
        /* length of the key */
        size_t len;
        /* index within actual data */
        size_t pos;
        /* the value */
        struct Rson
        {
            // union for unifying memory
            union
            {
                /* integers, null */
                int64_t i;
                /* floats */
                double d;
                /* bools */
                bool b;
                /* string */
                char *s;
                /* array */
                struct Rson *a;
                /* object */
                struct RsonObject *o;
            }
            content;
            /* count:
            number, bool, null = 1
            string = length of string
            array = number of elements
            object = number of read elements
            */
            size_t count;
            /* type of value */
            RsonValueList id;
        }
        val;
    }
    *map[RSON_MAP_SIZE];
    /* use count at index */
    size_t used[RSON_MAP_SIZE];
    /* count = sum of all used */
    size_t count;
}
RsonObject;

typedef struct Rson Rson;

typedef void RsonPairCallback(char *key, Rson *val);

////////////////////////////////
// PUBLIC FUNCTION PROTOTYPES //
////////////////////////////////

/* from JSON to Rson */

RsonList rson_from_json_n(Rson *rson, char *path, char *json, size_t len, bool flexible);
RsonList rson_from_json(Rson *rson, char *path, char *json, bool flexible);


/* from Rson to formatting */

void rson_print(Rson *rson, bool ordered);
RsonList rson_str(Rson *rson, char **str, bool ordered);    // TODO


/* getting from Rson */

Rson *rson_get(Rson *rson, char *path, RsonList *state);
RsonValueList rson_get_id(Rson *rson, RsonList *state); // TODO
size_t rson_get_arr_n(Rson *rson, char *path);    // TODO
char *rson_get_statemsg(RsonList *state);   // TODO


/* setting in Rson */

RsonList rson_set_null(Rson *rson, char *path);   // TODO
RsonList rson_set_obj(Rson *rson, char *path);    // TODO
RsonList rson_set_arr(Rson *rson, char *path, size_t n);  // TODO
// RsonList rson_set_val(Rson *rson, char *path, char *val);    // TODO
RsonList rson_set_str(Rson *rson, char *path, const char *s); // TODO
RsonList rson_set_int(Rson *rson, char *path, uint64_t i);    // TODO
RsonList rson_set_dbl(Rson *rson, char *path, double d);  // TODO
RsonList rson_set_bool(Rson *rson, char *path, bool b);   // TODO


/* deleting in Rson */

RsonList rson_free(Rson *rson, char *path);
RsonList rson_del_empty(Rson *rson, char *path, bool recursive);  // TODO; automatically remove empty ones


/* various utility functions */

RsonList rson_foreach(Rson *rson, const char *path, RsonPairCallback *callback);

































#if USE_R2STR
bool rson_r2str(Rson *rson, bool ordered, R2str *r2str);
#endif




// total memory size?
// root / parent ? maybe ???


/* todo
-> format bei print/r2str/rstr/str
-> funktionen vvvvvvvvvvvv
bool rson_rstr(Rson *rson, bool ordered, Rstr *rstr);
size_t rson_str(Rson *rson, bool ordered, char **str);
rson_get(Rson *rson, Rson *val, char *path, ...);
rson_set(Rson *rson, Rson *val, char *path, ...);
rson_del(Rson *rson, char *path, ...);
rson_ARRAY???(Rson *rson, char *path, ...);
*/
#if 0
// bool rson_string(Rson *rson, char **str);

/*


bool rson_parse(RsonObject *rson, char *str);

bool rson_merge(RsonObject *a, RsonObject *b)
{
    return false;
}

*/
#endif

#define RSON_H
#endif