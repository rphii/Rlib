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
    size_t len_app = vsnprintf(0, 0, format, argp);
    va_end(argp);

    // calculate required memory
    size_t blocksize = rstr->blocksize ? rstr->blocksize : RSTR_DEFAULT_BLOCKSIZE;
    size_t required = blocksize * ((len_app + rstr->len + 1) / blocksize + 1);

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
    va_start(argp, format);
    size_t len_chng = vsnprintf(&(rstr->s)[rstr->len], len_app + 1, format, argp);
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

    va_list argl = 0;
    va_copy(argl, argp); /* TODO check for error */
    size_t len_app = vsnprintf(0, 0, format, argl);
    va_end(argl);
    
    // calculate required memory
    size_t blocksize = rstr->blocksize ? rstr->blocksize : RSTR_DEFAULT_BLOCKSIZE;
    size_t required = blocksize * ((len_app + rstr->len + 1) / blocksize + 1);

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
    size_t len_chng = vsnprintf(&(rstr->s)[rstr->len], len_app + 1, format, argp);

    // check for success
    if(len_chng >= 0 && len_chng <= len_app) rstr->len += len_chng; // successful, change length
    else return false;  // encoding error or string hasn't been fully written
    return true;
}
