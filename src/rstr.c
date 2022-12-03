#include <string.h>
#include <ctype.h>
#include "rstr.h"

/////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES //
/////////////////////////////////

static void static_rstr_zero(Rstr *rstr);

//////////////////////////////////////
// PRIVATE FUNCTION IMPLEMENTATIONS //
//////////////////////////////////////

/**
 * @brief set everything witin Rstr to zero
 * 
 * @param rstr 
 */
static void static_rstr_zero(Rstr *rstr)
{
    if(!rstr) return;
    rstr->allocd = 0;
    rstr->blocksize = 0;
    rstr->len = 0;
    rstr->s = 0;
}

/////////////////////////////////////
// PUBLIC FUNCTION IMPLEMENTATIONS //
/////////////////////////////////////


/**
 * @brief free an Rstr's used memory
 * 
 * @param rstr 
 */
void rstr_free(Rstr *rstr)
{
    free(rstr->s);
    static_rstr_zero(rstr);
}

/**
 * @brief Recycle an Rstr, meaning, we keep the assigned memory, but we set the length to 0.
 *        This way, we can append faster next time, since we don't need to bother about
 *        using up memory.
 * 
 * @param rstr 
 */
void rstr_recycle(Rstr *rstr)
{
    rstr->len = 0;
}

/**
 * @brief Append a formatted string onto an Rstr.
 * 
 * @param rstr 
 * @param format 
 * @param ... 
 * @return true success
 * @return false failure
 */
bool rstr_append(Rstr *rstr, char *format, ...)
{
    if(!rstr || !format) return false;

    // calculate length of append string
    va_list argp;
    va_start(argp, format);
    int len_app = vsnprintf(0, 0, format, argp);
    va_end(argp);
    
    // calculate required memory
    size_t len_new = rstr->len + len_app;
    size_t required = rstr->allocd ? rstr->allocd : RSTR_DEFAULT_BLOCKSIZE;
    while(required < len_new) required = required << 1;

    // make sure to have enough memory
    if(required > rstr->allocd)
    {
        char *temp = realloc(rstr->s, required);
        // safety check
        // apply address and set new allocd
        if(!temp) return false;
        rstr->s = temp;
        rstr->allocd = required;
    }

    // actual append
    va_start(argp, format);
    int len_chng = vsnprintf(&(rstr->s)[rstr->len], len_app + 1, format, argp);
    va_end(argp);
    // check for success
    if(len_chng >= 0 && len_chng <= len_app) rstr->len += len_chng; // successful, change length
    else return false;  // encoding error or string hasn't been fully written
    return true;
}

/**
 * @brief Append a formatted string onto an Rstr.
 *        !!! caller has to va_start and va_end before / after calling this function !!!
 * 
 * @param rstr 
 * @param format 
 * @param argp 
 * @return true success
 * @return false failure
 */
bool rstr_append_va(Rstr *rstr, char *format, va_list argp)
{
    if(!rstr) return false;

    va_list argl;
    va_copy(argl, argp); /* TODO check for error */
    int len_app = vsnprintf(0, 0, format, argl);
    va_end(argl);
    
    // calculate required memory
    size_t len_new = rstr->len + len_app;
    size_t required = rstr->allocd ? rstr->allocd : RSTR_DEFAULT_BLOCKSIZE;
    while(required < len_new) required = required << 1;

    // make sure to have enough memory
    if(required > rstr->allocd)
    {
        char *temp = realloc(rstr->s, required);
        // safety check
        if(!temp) return false;
        // apply address and set new allocd
        rstr->s = temp;
        rstr->allocd = required;
    }
    
    // actual append
    int len_chng = vsnprintf(&(rstr->s)[rstr->len], len_app + 1, format, argp);

    // check for success
    if(len_chng >= 0 && len_chng <= len_app) rstr->len += len_chng; // successful, change length
    else return false;  // encoding error or string hasn't been fully written
    return true;
}

/**
 * @brief hashing function 
 * 
 * @param rstr 
 * @return size_t 
 */
size_t rstr_djb2(Rstr *rstr)
{
    size_t hash = 5381;
    unsigned char c;
    size_t i = 0;
    while(i < rstr->len)
    {
        c = rstr->s[i++];
        // printf("%c", c);
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

/**
 * @brief compare two rstr's for equality.
 * 
 * @param a rstr A
 * @param b rstr B
 * @return same as memcmp
 */
int rstr_cmp(Rstr *a, Rstr *b)
{
    if(!a || !b) return -1;
    if(a->len != b->len) return -1;
    return memcmp(a->s, b->s, b->len);
}

/**
 * @brief copy the content from one rstr to another rstr
 * 
 * @param a destination
 * @param b source
 * @return true success
 * @return false an error occured
 */
bool rstr_cpy(Rstr *a, Rstr *b)
{
    if(!a || !b) return false;
    // printf("rstr_cpy:b->s:%s / a->len = %zu\n", b->s, a->len);
    rstr_recycle(a);
    a->blocksize = b->blocksize;
    bool result = rstr_append(a, "%.*s", (int)b->len, b->s);
    // printf("rstr_cpy:a->s:%s / a->len = %zu\n", a->s, a->len);
    return result;
}

bool rstr_ncpy(Rstr *a, Rstr *b, size_t n)
{
    if(!a || !b) return false;
    // printf("rstr_cpy:b->s:%s / a->len = %zu\n", b->s, a->len);
    rstr_recycle(a);
    a->blocksize = b->blocksize;
    size_t m = n < b->len ? n : b->len;
    bool result = rstr_append(a, "%.*s", m, b->s);
    // printf("rstr_cpy:a->s:%s / a->len = %zu\n", a->s, a->len);
    return result;
}

ssize_t rstr_find(const Rstr *str, const Rstr *sub)
{
    if(!str || !sub) return -1;
    if(sub->len > str->len) return -1;
    char *found = strstr(str->s, sub->s);
    if(!found) return -1;
    return found - str->s;
}

void rstr_upper(const Rstr *str)
{
    if(!str) return;
    char *c = str->s;
    for(size_t i = 0; i < str->len; i++)
    {
        *c = toupper(*c);
        c++;
    }
}

void rstr_lower(const Rstr *str)
{
    if(!str) return;
    char *c = str->s;
    for(size_t i = 0; i < str->len; i++)
    {
        *c = tolower(*c);
        c++;
    }
}
