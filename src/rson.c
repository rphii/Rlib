#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>   // HUGE_VALL
#include <inttypes.h>
#include "rson.h"

#define SPACE_COUNT  4

typedef struct RsonParse
{
    char *json;
    size_t i;
    size_t len;
    size_t count;
    RsonList state;
    struct
    {
        RsonObject *obj;
        Rson *val;
        RsonValueList id;
    }
    stack[RSON_MAX_DEPTH], *top;
}
RsonParse;

typedef struct RsonHash
{
    size_t main;
    size_t sub;
}
RsonHash;

typedef struct RsonOut
{
    size_t spaces;
    RsonList state;
    struct
    {
        RsonObject *obj;
        Rson *val;
        RsonValueList id;
        size_t count;
        RsonHash hash;
    }
    stack[RSON_MAX_DEPTH], *top;
}
RsonOut;

/////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES //
/////////////////////////////////

// static int utf8_to_point(char *in, uint32_t *point);
static int point_to_utf8(uintptr_t point, char out[4]);
// static size_t static_rson_patch_str(char *str, size_t len);
static size_t static_rson_assign_str(char **dest, char *str, size_t len);
static size_t static_rson_hash_get(char *str, size_t len);
static RsonList static_rson_prepare_string(char *str, size_t len, size_t *i, size_t *end);
static bool static_rson_register_key(RsonParse *parser, char *s, size_t len, RsonHash *hash);
static bool static_rson_parse_object(RsonParse *parser);
static bool static_rson_parse_value(RsonParse *parser);
static bool static_rson_parse_array(RsonParse *parser);
static bool static_rson_parse_string(RsonParse *parser);
static bool static_rson_parse_number(RsonParse *parser);
static bool static_rson_parse_bool(RsonParse *parser);
static bool static_rson_parse_null(RsonParse *parser);

//////////////////////////////////////
// PRIVATE FUNCTION IMPLEMENTATIONS //
//////////////////////////////////////

// utf8 stuff
#if 0
static int utf8_to_point(char *in, uint32_t *point)
{
    if(!in || !point) return 0;
    uint32_t tinker = 0;
    int bytes = 0;
    // figure out how many bytes we need
    if((*in & 0x80) == 0) bytes = 1;
    else if((*in & 0xE0) == 0xC0) bytes = 2;
    else if((*in & 0xF0) == 0xE0) bytes = 3;
    else if((*in & 0xF8) == 0xF0) bytes = 4;
    // magical mask shifting
    int shift = (bytes - 1) * 6;
    unsigned char mask = 0x7F;
    if(bytes > 1) mask >>= bytes;
    // extract info from bytes
    for(int i = 0; i < bytes; i++)
    {
        // add number to point
        tinker |= ((in[i] & mask) << shift);
        if(mask == 0x3F)
        {
            if((unsigned char)(in[i] & ~mask) != 0x80)
            {
                bytes = 0;
                break;
            }
        }
        // adjust shift amount
        shift -= 6;
        // update mask
        mask = 0x3F;
    }
    // one final check, unicode doesn't go that far
    if(tinker > 0x10FFFF || !bytes)
    {
        *point = (unsigned char)*in;
        return 1;
    }
    *point = tinker;
    return bytes;
}
#endif
static int point_to_utf8(uintptr_t point, char out[4])
{
    if(!out) return false;
    int bytes = 0;
    int shift = 0;  // shift in bits
    // figure out how many bytes we need
    if(point < 0x0080) bytes = 1;
    else if(point < 0x0800) bytes = 2;
    else if(point < 0x10000) bytes = 3;
    else if(point < 0x110000) bytes = 4;
    // magical mask shifting
    shift = (bytes - 1) * 6;
    unsigned char mask = 0x7F;
    if(bytes > 1) mask >>= bytes;
    // create bytes
    for(int i = 0; i < bytes; i++)
    {
        // add actual character coding
        out[i] = (point >> shift) & mask;
        // add first byte code
        if(!i && bytes > 1) out[i] |= (~0 << (8 - bytes)) & 0xFF;
        // add any other code
        if(i) out[i] |= 0x80;
        // adjust shift and reset mask
        shift -= 6;
        mask = 0x3F;
    }
    return bytes;
}

static size_t static_rson_assign_str(char **dest, char *str, size_t len)
{
    if(!dest || !str) return 0;
    // now we can assign memory
    char *temp = realloc(*dest, sizeof(**dest) * (len + 1));
    if(!temp) return 0;    // TODO: return true/false bruh
    *dest = temp;
    // copy the string contents
    memcpy(*dest, str, len);
    (*dest)[len] = 0;
    return len;
}

static size_t static_rson_hash_get(char *str, size_t len)
{
    uint32_t hash = 5381;
    size_t i = 0;
    while (i < len)
    {
        hash = ((hash << 5) + hash) + str[i++]; /* hash * 33 + c */
    }
    return hash;
}

static RsonList static_rson_prepare_string(char *str, size_t len, size_t *i, size_t *end)
{
    if(!str || !len || !i || !end) return __LINE__;
    // we expect a '"'
    if(str[(*i)++] != '"') return __LINE__;
    // we check the whole string
    char escape = 0;
    bool quit = false;
    size_t j = *i;
    size_t zeroed = 0;
    while(!quit)
    {
        switch(escape)
        {
            case 0: {
                // did we reach the end of the string?
                if(str[j] == '"' || !str[j]) quit = true;
                // did we reach an escape sequence?
                else if(str[j] == '\\') escape = '?';
                // if none of the above is true, go to next character
                else j++;
            } break;
            case '?': {
                if(j + 1 > len) return __LINE__;
                // check for a matching escape sequence
                switch(str[j + 1])
                {
                    case '"':  { escape = '"';  } break;
                    case 'u':  { escape = 'u';  } break;
                    case '/':  { escape = '/';  } break;
                    case 'b':  { escape = '\b'; } break;
                    case 'f':  { escape = '\f'; } break;
                    case 'n':  { escape = '\n'; } break;
                    case 'r':  { escape = '\r'; } break;
                    case 't':  { escape = '\t'; } break;
                    case '\\': { escape = '\\'; } break;
                    default: quit = true;
                }
            } break;
            case '"':
            case '\\':
            case '/':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t': {
                if(j + 1 > len) quit = true;
                else
                {
                    // replace string with correct one and append 0, we'll fix it later
                    str[j++] = escape;
                    str[j++] = 0;
                    escape = 0;
                    zeroed++;
                }
            } break;
            case 'u': {
                if(j + 6 > len) quit = true;
                else
                {
                    char *endptr = 0;
                    char hex[5] = {0};
                    // copy four bytes into temporary array
                    for(int k = 0; k < 4; k++) hex[k] = str[j + 2 + k];
                    // convert the string digits to utf-8 point
                    uint32_t point = strtoul(hex, &endptr, 16);
                    if(*endptr) quit = true;
                    else
                    {
                        // get utf-8 bytes from that
                        char utf8[4] = {0};
                        int bytes = point_to_utf8(point, utf8);
                        // replace, starting from the backslash
                        for(int k = 0; k < bytes; k++)
                        {
                            str[k + j] = utf8[k];
                        }
                        // fill rest with zeros, we'll fix it later
                        for(int k = bytes; k < 6; k++)
                        {
                            str[k + j] = 0;
                            zeroed++;
                        }
                        j += 6;
                        escape = 0;
                    }
                }
            } break;
            default: break;
        }
    }
    // check if there was an escape error
    if(escape) return __LINE__;
    // check if we reached an unexpected zero
    if(!str[j]) return __LINE__;
    // fix string if needed
    if(zeroed)
    {
        size_t write = *i;
        // copy string that is intact (nonzero bytes)
        while(*i < j + 1)
        {
            if(str[*i]) str[write++] = str[*i];
            (*i)++;
        }
        // adjust parser stuff
        *end = write - 1;  // end of string
        // fill rest with spaces
        *i = write;
        while(*i < j + 1)
        {
            str[(*i)++] = ' ';
        }
    }
    else
    {
        *end = j;
        *i = j + 1;
    }
    return RSON_OK;
}

static bool static_rson_register_key(RsonParse *parser, char *s, size_t len, RsonHash *hash)
{
    if(!parser || !hash) return false;
    // generate a hash
    hash->main = static_rson_hash_get(s, len) % RSON_MAP_SIZE;
    // check if it exists
    RsonObject *rson = parser->top->obj;
    size_t used = rson->used[hash->main];
    hash->sub = used;
    bool exist = false;
    // check if it doesn't exist yet
    if(!exist)
    {
        // create new one
        void *temp = realloc(rson->map[hash->main], sizeof(*rson->map[hash->main]) * (used + 1));
        // safety check
        if(!temp) return false;
        rson->map[hash->main] = temp;
        // allocate memory for the string
        rson->map[hash->main][hash->sub].key = malloc(len + 1);
        // initialize memory
        memcpy(rson->map[hash->main][hash->sub].key, s, len);
        rson->map[hash->main][hash->sub].key[len] = 0;
        rson->map[hash->main][hash->sub].len = len;
        rson->map[hash->main][hash->sub].val.id = RSON_NONE;
        rson->map[hash->main][hash->sub].val.count = 0;
        rson->map[hash->main][hash->sub].val.content.i = 0;
        // increment usage
        rson->used[hash->main]++;
    }
    return true;
}

static bool static_rson_parse_object(RsonParse *parser)
{
    if(!parser) return false;
    // top of stack boundary check
    if(parser->top + 1 > &parser->stack[RSON_MAX_DEPTH - 1] && (parser->state = RSON_ERROR_MAX_DEPTH)) return false;
    bool parse_object = false;
    // have we not yet started to parse an object?
    if(!(parser->top->obj->count - parser->count))
    {
        // skip whitespaces
        while(isspace((int)parser->json[parser->i])) parser->i++;    // this isn't really in the spec but it's a QOL upgrade
        // expect curly bracket
        if(parser->json[parser->i] != '{' && (parser->state = __LINE__)) return false;
        parse_object = true;
    }
    else
    {
        // we expect either ',' (object) or '}' (end)
        if(parser->json[parser->i] == '}')
        {
            parser->top--;
            parser->i++;
            // skip whitespace
            while(isspace((int)parser->json[parser->i])) parser->i++;
        }
        else if(parser->json[parser->i] == ',') parse_object = true;
        // else if(parser->json[parser->i] == '{') parse_object = true;     // TODO: HELP ME AHHHHHHHHHHHHHHHHHHH
        else if((parser->state = __LINE__)) return false;
    }
    if(parse_object)
    {
        // skip whitespace
        parser->i++;
        while(isspace((int)parser->json[parser->i])) parser->i++;
        // either break or parse string
        if(parser->json[parser->i] == '}')
        {
            parser->top--;
            parser->i++;
            // skip whitespace
            while(isspace((int)parser->json[parser->i])) parser->i++;
        }
        else
        {
            // prepare the string
            size_t start = parser->i + 1;
            size_t end = 0;
            if(static_rson_prepare_string(parser->json, parser->len, &parser->i, &end)) return false;
            // register the key
            RsonHash hash = {0};
            if(!static_rson_register_key(parser, &parser->json[start], end - start, &hash) && (parser->state = __LINE__)) return false;
            // skip whitespace
            while(isspace((int)parser->json[parser->i])) parser->i++;
            // expecting colon
            if(parser->json[parser->i++] != ':' && (parser->state = RSON_ERROR_EXPECT_COLON)) return false;
            // parse value
            parser->top->val = &parser->top->obj->map[hash.main][hash.sub].val;
            parser->top->obj->map[hash.main][hash.sub].pos = parser->top->obj->count++;
            parser->top++;
            parser->top->obj = (parser->top - 1)->obj;
            parser->top->val = (parser->top - 1)->val;
            parser->top->id = RSON_VALUE;
        }
    }
    // done
    return true;
}

static bool static_rson_parse_value(RsonParse *parser)
{
    if(!parser) return false;
    // skip whitespace
    while(isspace((int)parser->json[parser->i])) parser->i++;
    // decide what to parse next
    switch(parser->json[parser->i])
    {
        case '"': { parser->top->id = RSON_STRING; } break;
        case '[': { parser->top->id = RSON_ARRAY;  } break;
        case '{': { parser->top->id = RSON_OBJECT; } break;
        case 't': { parser->top->id = RSON_BOOL;   } break;
        case 'f': { parser->top->id = RSON_BOOL;   } break;
        case 'n': { parser->top->id = RSON_NULL;   } break;
        default:  { parser->top->id = RSON_INT;    } break;
    }
    // check if we want to create a sub-object
    if(parser->top->id == RSON_OBJECT)
    {
        // allocate memory for a sub-object
        // if(count && (parser->state = __LINE__)) return false;
        void *temp = realloc(parser->top->val->content.o, sizeof(*parser->top->val->content.o));
        // allocation check
        if(!temp && (parser->state = __LINE__)) return false;
        parser->top->val->content.o = temp;
        parser->top->val->id = RSON_OBJECT;
        // shift object
        parser->top->obj = parser->top->val->content.o;
        // initialize memory
        for(size_t i = 0; i < RSON_MAP_SIZE; i++)
        {
            parser->top->obj->map[i] = 0;
            parser->top->obj->used[i] = 0;
        }
        parser->top->obj->count = 0;
    }
    if(parser->top->id != RSON_ARRAY && parser->top->id != RSON_OBJECT) parser->top->val->count = 1;

    return true;
}

static bool static_rson_parse_array(RsonParse *parser)
{
    if(!parser) return false;
    // top of stack boundary check
    if(parser->top + 1 > &parser->stack[RSON_MAX_DEPTH - 1] && (parser->state = RSON_ERROR_MAX_DEPTH)) return false; 
    bool parse_val = false;
    // did we not yet parse any value?
    if(!parser->top->val->count)  // if(!(parser->top + 1)->id)
    {
        // we expect a '['
        parser->top->val->id = RSON_ARRAY;
        if(parser->json[parser->i] != '[' && (parser->state = __LINE__)) return false;
        parser->i++;
        // skip whitespace
        while(isspace((int)parser->json[parser->i])) parser->i++;
        // we can get a ']' else we'll parse a value
        if(parser->json[parser->i] == ']')
        {
            parser->top--;
            parser->i++;
        }
        else parse_val = true;
    }
    else 
    {
        // we either expect a ',' (value) or a ']' (end)
        if(parser->json[parser->i] == ']') parser->top--;
        else if(parser->json[parser->i] == ',') parse_val = true;
        else if((parser->state = __LINE__)) return false;
        parser->i++;
    }
    if(parse_val)
    {
        // parse a value
        parser->top++;
        parser->top->val = (parser->top - 1)->val;
        parser->top->obj = (parser->top - 1)->obj;
        // allocate more memory
        size_t count = parser->top->val->count;
        void *temp = realloc(parser->top->val->content.a, sizeof(*parser->top->val->content.a) * (count + 1));
        // memory check
        if(!temp && (parser->state = RSON_ERROR_REALLOC)) return false;
        parser->top->val->count++;  // I don't like to increase the count before reallocing, that's why it's down here
        parser->top->val->content.a = temp;
        // initialze memory
        parser->top->val = &parser->top->val->content.a[count];
        parser->top->val->id = RSON_NONE;
        parser->top->val->count = 0;
        parser->top->val->content.i = 0;
        parser->top->id = RSON_VALUE;
    }
    // skip whitespace
    while(isspace((int)parser->json[parser->i])) parser->i++;
    // done is handled further above
    return true;
}

static bool static_rson_parse_string(RsonParse *parser)
{
    if(!parser) return false;
    // prepare string
    size_t start = parser->i + 1;
    size_t end = 0;
    if(static_rson_prepare_string(parser->json, parser->len, &parser->i, &end) && (parser->state = __LINE__)) return false;
    // assign string
    parser->top->val->count = static_rson_assign_str(&parser->top->val->content.s, &parser->json[start], end - start);
    if(parser->top->val->count != end - start && (parser->state = __LINE__)) return false;
    parser->top->val->id = RSON_STRING;
    // skip whitespace
    while(isspace((int)parser->json[parser->i])) parser->i++;
    // done
    parser->top--;
    return true;
}

static bool static_rson_parse_number(RsonParse *parser)
{
    if(!parser) return false;
    // try to parse an int
    char *endptr = 0;
    parser->top->val->content.i = strtoll(&parser->json[parser->i], &endptr, 10);
    // check for success
    if(isspace((int)*endptr) || *endptr == ',' || *endptr == '}' || *endptr == ']' || endptr == parser->json + parser->len)
    {
        if(parser->top->val->content.i == LLONG_MAX && (parser->state = __LINE__)) return false;
        if(parser->top->val->content.i == LLONG_MIN && (parser->state = __LINE__)) return false;
        parser->top->val->id = RSON_INT;
    }
    else
    {
        endptr = 0;
        parser->top->val->content.d = strtod(&parser->json[parser->i], &endptr);
        if(parser->top->val->content.d == +HUGE_VALL && (parser->state = __LINE__)) return false;
        if(parser->top->val->content.d == -HUGE_VALL && (parser->state = __LINE__)) return false;
        parser->top->val->id = RSON_DOUBLE;
    }
    // did we even parse a value?
    if(&parser->json[parser->i] == endptr && (parser->state = __LINE__)) return false;
    // done
    parser->i = endptr - parser->json;
    // skip whitespace
    while(isspace((int)parser->json[parser->i])) parser->i++;
    parser->top--;
    return true;
}

static bool static_rson_parse_bool(RsonParse *parser)
{
    if(!parser) return false;
    // either true or false
    bool type = (parser->json[parser->i] == 't');
    char *cmp = type ? "true" : "false";
    size_t len = type ? 4 : 5;
    // check for boundary
    // if(parser->i + len > parser->len && (parser->state = __LINE__)) return false;
    // check for string
    if(!memcmp(&parser->json[parser->i], cmp, len))
    {
        parser->top->val->id = RSON_BOOL;
        parser->top->val->content.b = type;
    }
    if(parser->top->val->id != RSON_BOOL && (parser->state = __LINE__)) return false;
    // done
    parser->i += len;
    // skip whitespace
    while(isspace((int)parser->json[parser->i])) parser->i++;
    parser->top--;
    return true;
}

static bool static_rson_parse_null(RsonParse *parser)
{
    if(!parser) return false;
    // check for boundary
    // if(parser->i + len > parser->len && (parser->state = __LINE__)) return false;
    // check for string
    if(!memcmp(&parser->json[parser->i], "null", 4))
    {
        parser->top->val->id = RSON_NULL;
        parser->top->val->content.i = 0;
    }
    else if((parser->state = __LINE__)) return false;
    // done
    parser->i += 4;
    // skip whitespace
    while(isspace((int)parser->json[parser->i])) parser->i++;
    parser->top--;
    return true;
}

/////////////////////////////////////
// PUBLIC FUNCTION IMPLEMENTATIONS //
/////////////////////////////////////

/**
 * @brief 
 * 
 * @param rson 
 * @param json any json string to
 * @param len maximal length of json string to be parsed
 * @param flexible 'true' means it will: 
 *                      A) create any missing path. TODO
 *                      B) free any memory on that path. TODO
 *                      C) insert any object (from 'json') to that path, even duplicate keys. TODO
 *                 'false' means it will only insert a unique key (TODO) at the specified path, where it isn't required to free any memory (if the type is unused & match) (TODO)
 * @return RsonList 
 */
RsonList rson_from_json_n(Rson *rson, char *path, char *json, size_t len, bool flexible)
{
    // TODO: handle json==0
    rson = rson_get(rson, path, 0);
    if(!rson) return __LINE__;

    RsonParse parser = {0};
    parser.json = json;
    parser.top = parser.stack;
    parser.state = RSON_OK;
    parser.len = len;
    parser.top->val = rson;
    if(rson->id == RSON_OBJECT) // TODO: isch das grusig oder nöd??????
    {
        parser.top->obj = rson->content.o;
        parser.count = flexible ? parser.top->obj->count : 0;
    }

    // find first character
    while(isspace((int)parser.json[parser.i])) parser.i++;
    // differentiate between array or object
    if(parser.json[parser.i] == '[' && rson->id != RSON_ARRAY)
    {
        // is it not an array?
        if(rson->id && rson->id != RSON_ARRAY) return __LINE__;
        rson->id = RSON_ARRAY;
    }
    else if(parser.json[parser.i] == '{' && rson->id != RSON_OBJECT)
    {
        // is it not an object?
        if(rson->id && rson->id != RSON_OBJECT) return __LINE__;
        rson->id = RSON_OBJECT;
        // get memory
        void *temp = realloc(rson->content.o, sizeof(*rson->content.o));
        // error check
        if(!temp) return __LINE__;
        rson->content.o = temp;
        // initialize values
        parser.top->obj = rson->content.o;
        parser.top->obj->count = 0;
        for(size_t i = 0; i < RSON_MAP_SIZE; i++)
        {
            parser.top->obj->map[i] = 0;
            parser.top->obj->used[i] = 0;
        }
    }
    else
    {
        if(!rson->id) rson->id = RSON_VALUE;
    }
    // configure parser top to be the id
    parser.top->id = rson->id;
    // set the parse_top id to start from object    
    while(parser.i < len && json[parser.i])
    {
        if(parser.top < parser.stack) break;    // finished
        if(parser.top >= &parser.stack[RSON_MAX_DEPTH - 1] && (parser.state = __LINE__)) break;
        if(parser.state != RSON_OK) break;
        switch(parser.top->id)
        {
            case  RSON_OBJECT: { static_rson_parse_object(&parser); } break;
            case  RSON_VALUE:  { static_rson_parse_value(&parser);  } break;
            case  RSON_ARRAY:  { static_rson_parse_array(&parser);  } break;
            case  RSON_STRING: { static_rson_parse_string(&parser); } break;
            case  RSON_INT:    { static_rson_parse_number(&parser); } break;
            case  RSON_DOUBLE: { static_rson_parse_number(&parser); } break;
            case  RSON_BOOL:   { static_rson_parse_bool(&parser);   } break;
            case  RSON_NULL:   { static_rson_parse_null(&parser);   } break;
            case  RSON_NONE:   { parser.state = __LINE__;         } break;
            default: break;
        }
    }
    // skip whitespace
    while(isspace((int)parser.json[parser.i])) parser.i++;
    if(parser.i < parser.len) parser.state = __LINE__;
    // is everything ok?
    if(parser.state)
    {
        printf("Parser state number: %u\n", parser.state);
        printf("Parser is at '%.20s'... (%zu)\n", &parser.json[parser.i], parser.i);
    }
    return parser.state;
}

RsonList rson_from_json(Rson *rson, char *path, char *json, bool insert)
{
    // TODO: handle json==0
    if(!rson || !json) return false;
    
    size_t json_len = strlen(json);

    return rson_from_json_n(rson, path, json, json_len, insert);
}

void rson_print(Rson *rson, bool ordered)
{
    if(!rson) return;
    RsonOut out = {0};
    out.top = out.stack;
    out.top->obj = rson->content.o;
    out.top->val = rson;
    out.top->id = rson->id;
    // iterate over rson
    while(!out.state)
    {
        if(out.top < out.stack) break;
        switch(out.top->id)
        {
            case RSON_OBJECT: {
                size_t *i = &out.top->hash.main;
                size_t *j = &out.top->hash.sub;
                // boundary check
                RsonObject *rson_p = out.top->obj;
                if(!rson_p->count)
                {
                    out.top--;
                    printf("{}");
                    continue;
                }
                if(*j >= rson_p->used[*i])
                {
                    *j = 0;
                    if(++(*i) >= RSON_MAP_SIZE)
                    {
                        *i = 0;
                        if(ordered && out.top->count != rson_p->count) continue;
                        out.spaces -= SPACE_COUNT;
                        out.top->count = 0;
                        out.top--;
                        // fill with spaces
                        printf("\n");
                        for(size_t k = 0; k < out.spaces; k++) printf(" ");
                        printf("}");
                    }
                    continue;
                }
                // get key
                char *key = rson_p->map[*i][*j].key;
                // get len & val
                size_t len = rson_p->map[*i][*j].len;
                Rson *val = &rson_p->map[*i][*j].val;
                // do the count match
                if(ordered && out.top->count != rson_p->map[*i][*j].pos)
                {
                    (*j)++;
                    continue;
                }
                // is it our first object?
                if(!out.top->count++)
                {
                    out.spaces += SPACE_COUNT;
                    printf("{\n");
                }
                else printf(",\n");
                // fill with spaces
                for(size_t k = 0; k < out.spaces; k++) printf(" ");
                // print key
                printf("\"%.*s\": ", (int)len, key);
                // next
                out.top++;
                out.top->val = val;
                out.top->id = val->id;
                if(val->id == RSON_OBJECT)
                {
                    out.top->obj = rson_p->map[*i][*j].val.content.o;
                    out.top->hash.main = 0;
                    out.top->hash.sub = 0;
                }
                else
                {
                    out.top->obj = rson_p;
                    out.top->hash.main = *i;
                    out.top->hash.sub = *j;
                }
                (*j)++;
            } break;
            case RSON_ARRAY: {
                // boundary check
                Rson *val = out.top->val;
                if(!val->count)
                {
                    printf("[]");
                    out.top--;
                    continue;
                }
                else if(out.top->count >= val->count)
                {
                    out.top->count = 0;
                    out.spaces -= SPACE_COUNT;
                    out.top--;
                    // fill with spaces
                    printf("\n");
                    for(size_t k = 0; k < out.spaces; k++) printf(" ");
                    printf("]");
                    continue;
                }
                // is it our first value?
                if(!out.top->count)
                {
                    out.spaces += SPACE_COUNT;
                    printf("[\n");
                }
                else printf(",\n");
                // fill with spaces
                for(size_t k = 0; k < out.spaces; k++) printf(" ");
                // next
                val = &val->content.a[out.top->count++];
                out.top++;
                out.top->obj = val->content.o;
                out.top->val = val;
                out.top->id = val->id;
                if(val->id == RSON_OBJECT)
                {
                    out.top->hash.main = 0;
                    out.top->hash.sub = 0;
                }
                else
                {
                    out.top->hash.main = (out.top - 1)->hash.main;
                    out.top->hash.sub = (out.top - 1)->hash.sub;
                }
            } break;
            case RSON_STRING: {
                printf("\"%.*s\"", (int)out.top->val->count, out.top->val->content.s);
                out.top--;
            } break;
            case RSON_INT: {
                printf("%"PRIi64, (int64_t)out.top->val->content.i);
                out.top--;
            } break;
            case RSON_DOUBLE: {
                printf("%lf", out.top->val->content.d);
                out.top--;
            } break;
            case RSON_BOOL: {
                printf("%s", out.top->val->content.b ? "true" : "false");
                out.top--;
            } break;
            case RSON_NULL: {
                printf("null");
                out.top--;
            } break;
            // the ones after should not really ever appear in a valid RsonObject
            case RSON_VALUE:
            case RSON_NONE: {
                out.state = __LINE__;
            } break;
            default: break;
        }
    }
    printf("\n");
}

/**
 * @brief Get a pointer to a value at a path within an Rson.
 * 
 * @param rson  
 * @param path 
 * @param val 
 * @param staticd 
 * @return RsonList 
 */
Rson *rson_get(Rson *rson, char *path, RsonList *state)
{
    //           key    key   arr  key
    // >>> get(self, "global"."users".[24]."roflkopter")
    // >>> get(self, ["global"]["users"][24]["roflkopter"])
    if(!path) return rson;
    if(!rson && state && (*state = __LINE__)) return 0;
    // get to the correct key down the path
    RsonHash hash = {0};
    RsonList state_internal = RSON_OK;
    Rson *result = rson;
    bool expect_dot = false;
    size_t i = 0;
    // skip whitespace
    while(isspace((int)path[i])) i++;
    bool expect_path = (path[i] != 0);
    if(expect_path)
    {
        size_t len = strlen(path);
        // follow path
        while(!state_internal && path[i] && i < len)
        {
            // skip whitespace
            while(isspace((int)path[i])) i++;
            // do we expect a dot?
            if(expect_dot && path[i] && path[i++] != '.' && (state_internal = __LINE__)) break;
            expect_dot = false;
            expect_path = true;
            // skip whitespace
            while(isspace((int)path[i])) i++;
            // check for array or key
            if(path[i] == '"')
            {
                // verify that the current id is an object
                if(result->id != RSON_OBJECT && (state_internal = __LINE__)) break;
                size_t start = i + 1;
                size_t end = 0;
                // prepare the string
                if(static_rson_prepare_string(path, len, &i, &end) && (state_internal = __LINE__)) break;
                // find the key in the object's map
                bool found = false;
                RsonObject *obj = result->content.o;
                for(hash.main = 0; hash.main < RSON_MAP_SIZE; hash.main++)
                {
                    for(hash.sub = 0; hash.sub < obj->used[hash.main]; hash.sub++)
                    {
                        // do the lengths match?
                        if(obj->map[hash.main][hash.sub].len != end - start) continue;
                        // do the contents match?
                        if(memcmp(obj->map[hash.main][hash.sub].key, &path[start], end - start)) continue;
                        found = true;
                        break;
                    }
                    if(found) break;
                }
                // did we find it?
                if(!found && (state_internal = __LINE__)) break;
                // shift result
                result = &obj->map[hash.main][hash.sub].val;
                expect_dot = true;
                expect_path = false;
            }
            else if(path[i] == '[')
            {
                // verify that the current id is an array
                if(result->id != RSON_ARRAY && (state_internal = __LINE__)) break;
                i++;
                // skip any spaces
                while(isspace((int)path[i])) i++;
                // convert unsigned integer to number
                char *endptr = 0;
                size_t index = strtoull(&path[i], &endptr, 10);
                // check for conversion error
                if(index == ULLONG_MAX && (state_internal = __LINE__)) break;
                i = endptr - path;
                // skip any spaces
                while(isspace((int)path[i])) i++;
                // expect a ']'
                if(path[i] != ']' && (state_internal = __LINE__)) break;
                // are there enough array elements?
                if(result->count <= index && (state_internal = __LINE__)) break;
                // shift result
                result = &result->content.a[index];
                i++;
                expect_dot = true;
                expect_path = false;
            }
            else state_internal = __LINE__;
        }
        // free if needed
        // if(dynamic) free(path);
    }
    // finalize
    if(state_internal == RSON_OK)
    {
        // was there an error in the path?
        if(expect_path) state_internal = __LINE__;
    }
    // modify states
    if(state_internal)
    {
        result = 0;
        if(state) *state = state_internal;
    }
    // free(path);
    return result;
}


// TODO: check out of bounds when doing parser->top++/--
// TODO: check for more out of bounds
// TODO: implement 'rson_free(...)'
// TODO: modification functions (get, set, add, del...?)

#if 0
RsonList rson_free(Rson *rson, char *path)
{
    rson = rson_get(rson, path, 0);
    if(!rson) return RSON_ERROR_ARGS;
    // free
    RsonOut out = {0};
    out.top->obj = rson->content.o;
    out.top->val = rson;
    out.top->id = rson->id;
    // iterate over rson
    while(!out.state)
    {
        if(out.top < out.stack) break;
        switch(out.top->id)
        {
            case RSON_OBJECT: {
                size_t *i = &out.top->hash.main;
                size_t *j = &out.top->hash.sub;
                // boundary check
                RsonObject *rson_p = out.top->obj;
                if(!rson_p->count)
                {
                    out.top--;
                    continue;
                }
                if(*j >= rson_p->used[*i])
                {
                    *j = 0;
                    if(++(*i) >= RSON_MAP_SIZE)
                    {
                        *i = 0;
                        // if(ordered && out.top->count != rson_p->count) continue;
                        out.top->count = 0;
                        out.top--;
                    }
                    continue;
                }
                // get key
                char *key = rson_p->map[*i][*j].key;
                // get len & val
                size_t len = rson_p->map[*i][*j].len;
                Rson *val = &rson_p->map[*i][*j].val;
                // next
                out.top++;
                out.top->val = val;
                out.top->id = val->id;
                if(val->id == RSON_OBJECT)
                {
                    out.top->obj = rson_p->map[*i][*j].val.content.o;
                    out.top->hash.main = 0;
                    out.top->hash.sub = 0;
                }
                else
                {
                    out.top->obj = rson_p;
                    out.top->hash.main = *i;
                    out.top->hash.sub = *j;
                }
                (*j)++;
            } break;
            case RSON_ARRAY: {
            } break;
            case RSON_STRING: {
            } break;
            case RSON_INT: {
            } break;
            case RSON_DOUBLE: {
            } break;
            case RSON_BOOL: {
            } break;
            case RSON_NULL: {
            } break;
            case RSON_VALUE:
            default: {
                out.state = __LINE__;
            } break;
        }
    }
}
#endif



#if USE_R2STR
bool rson_r2str(Rson *rson, bool ordered, R2str *r2str)
{
    if(!rson || !r2str) return false;
    RsonOut out = {0};
    out.top = out.stack;
    out.top->obj = rson->content.o;
    out.top->val = rson;
    out.top->id = rson->id;
    // iterate over rson
    while(!out.state)
    {
        if(out.top < out.stack) break;
        switch(out.top->id)
        {
            case RSON_OBJECT: {
                size_t *i = &out.top->hash.main;
                size_t *j = &out.top->hash.sub;
                // boundary check
                RsonObject *rson_p = out.top->obj;
                if(!rson_p->count)
                {
                    out.top--;
                    r2str_append(r2str, "{}");
                    continue;
                }
                if(*j >= rson_p->used[*i])
                {
                    *j = 0;
                    if(++(*i) >= RSON_MAP_SIZE)
                    {
                        *i = 0;
                        if(ordered && out.top->count != rson_p->count) continue;
                        out.spaces -= SPACE_COUNT;
                        out.top->count = 0;
                        out.top--;
                        // fill with spaces
                        r2str_append(r2str, "\n");
                        for(size_t k = 0; k < out.spaces; k++) r2str_append(r2str, " ");
                        r2str_append(r2str, "}");
                    }
                    continue;
                }
                // get key
                char *key = rson_p->map[*i][*j].key;
                // get len & val
                size_t len = rson_p->map[*i][*j].len;
                Rson *val = &rson_p->map[*i][*j].val;
                // do the count match
                if(ordered && out.top->count != rson_p->map[*i][*j].pos)
                {
                    (*j)++;
                    continue;
                }
                // is it our first object?
                if(!out.top->count++)
                {
                    out.spaces += SPACE_COUNT;
                    r2str_append(r2str, "{\n");
                }
                else r2str_append(r2str, ",\n");
                // fill with spaces
                for(size_t k = 0; k < out.spaces; k++) r2str_append(r2str, " ");
                // print key
                r2str_append(r2str, "\"%.*s\": ", (int)len, key);
                // next
                out.top++;
                out.top->val = val;
                out.top->id = val->id;
                if(val->id == RSON_OBJECT)
                {
                    out.top->obj = rson_p->map[*i][*j].val.content.o;
                    out.top->hash.main = 0;
                    out.top->hash.sub = 0;
                }
                else
                {
                    out.top->obj = rson_p;
                    out.top->hash.main = *i;
                    out.top->hash.sub = *j;
                }
                (*j)++;
            } break;
            case RSON_ARRAY: {
                // boundary check
                Rson *val = out.top->val;
                if(!val->count)
                {
                    r2str_append(r2str, "[]");
                    out.top--;
                    continue;
                }
                else if(out.top->count >= val->count)
                {
                    out.top->count = 0;
                    out.spaces -= SPACE_COUNT;
                    out.top--;
                    // fill with spaces
                    r2str_append(r2str, "\n");
                    for(size_t k = 0; k < out.spaces; k++) r2str_append(r2str, " ");
                    r2str_append(r2str, "]");
                    continue;
                }
                // is it our first value?
                if(!out.top->count)
                {
                    out.spaces += SPACE_COUNT;
                    r2str_append(r2str, "[\n");
                }
                else r2str_append(r2str, ",\n");
                // fill with spaces
                for(size_t k = 0; k < out.spaces; k++) r2str_append(r2str, " ");
                // next
                val = &val->content.a[out.top->count++];
                out.top++;
                out.top->obj = val->content.o;
                out.top->val = val;
                out.top->id = val->id;
                if(val->id == RSON_OBJECT)
                {
                    out.top->hash.main = 0;
                    out.top->hash.sub = 0;
                }
                else
                {
                    out.top->hash.main = (out.top - 1)->hash.main;
                    out.top->hash.sub = (out.top - 1)->hash.sub;
                }
            } break;
            case RSON_STRING: {
                r2str_append(r2str, "\"%.*s\"", (int)out.top->val->count, out.top->val->content.s);
                out.top--;
            } break;
            case RSON_INT: {
                r2str_append(r2str, "%lli", (int64_t)out.top->val->content.i);
                out.top--;
            } break;
            case RSON_DOUBLE: {
                r2str_append(r2str, "%lf", out.top->val->content.d);
                out.top--;
            } break;
            case RSON_BOOL: {
                r2str_append(r2str, "%s", out.top->val->content.b ? "true" : "false");
                out.top--;
            } break;
            case RSON_NULL: {
                r2str_append(r2str, "null");
                out.top--;
            } break;
            // the ones after should not really ever appear in a valid RsonObject
            case RSON_VALUE:
            case RSON_NONE: {
                out.state = __LINE__;
            } break;
            default: break;
        }
    }
    r2str_append(r2str, "\n");
    return true;
}
#endif