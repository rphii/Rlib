
#include "rstr.h"

static void static_rstr_zero(Rstr *str)
{
    if(!str) return;
    str->allocd = 0;
    str->blocksize = 0;
    str->len = 0;
    str->s = 0;
}

void rstr_free(Rstr *rstr)
{
    free(rstr->s);
    static_rstr_zero(rstr);
}

void rstr_recycle(Rstr *rstr)
{
    rstr->len = 0;
}

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

bool rstr_append_va(Rstr *rstr, char *format, va_list argp)
{
    if(!rstr) return false;

    /* caller has to va_start and va_end before / after calling this function! */
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
