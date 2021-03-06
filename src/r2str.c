#include "r2str.h"

/////////////////////////////////
// PRIVATE FUNCTION PROTOTYPES //
/////////////////////////////////

static void static_r2str_zero(R2str *r2str);

//////////////////////////////////////
// PRIVATE FUNCTION IMPLEMENTATIONS //
//////////////////////////////////////

/**
 * @brief Set everything within an R2str to zero.
 * 
 * @param r2str 
 */
static void static_r2str_zero(R2str *r2str)
{
    if(!r2str) return;
    r2str->allocd = 0;
    r2str->blocksize = 0;
    r2str->count = 0;
    r2str->rstr = 0;
    r2str->sub_blocksize = 0;
    r2str->threshold = 0;
}

/////////////////////////////////////
// PUBLIC FUNCTION IMPLEMENTATIONS //
/////////////////////////////////////

/**
 * @brief Free everything in an R2str
 * 
 * @param r2str 
 */
void r2str_free(R2str *r2str)
{
    if(!r2str) return;
    for(size_t i = 0; i < r2str->allocd; i++)
    {
        rstr_free(&r2str->rstr[i]);
    }
    free(r2str->rstr);
    static_r2str_zero(r2str);
}

/**
 * @brief Recycle an R2str (keep memory, but reset string lengths)
 * 
 * @param r2str 
 * @param keep_written set this to true, if you whish to keep the 'written' amount
 */
void r2str_recycle(R2str *r2str)
{
    if(!r2str) return;
    if(!r2str->allocd) return;
    for(size_t i = 0; i <= r2str->count; i++)
    {
        rstr_recycle(&r2str->rstr[i]);
    }
    r2str->count = 0;
}

/**
 * @brief Append an ordinary string to an R2str
 * 
 * @param r2str 
 * @param sub_blocksize blocksize of the substrings
 * @param format formatted string
 * @param ... 
 * @return true success
 * @return false error
 */
bool r2str_append(R2str *r2str, char *format, ...)
{
    if(!r2str || !format) return false;

    // calculate required memory
    size_t threshold = r2str->threshold ? r2str->threshold : R2STR_DEFAULT_THRESHOLD;
    size_t blocksize = r2str->blocksize ? r2str->blocksize : R2STR_DEFAULT_BLOCKSIZE;
    size_t required = blocksize * ((r2str->count + 1) / blocksize + 1);

    // make sure to have enough memory
    if(required > r2str->allocd)
    {
        void *temp = realloc(r2str->rstr, sizeof(*r2str->rstr) * required);
        if(!temp) return false;
        r2str->rstr = temp;
        // initialize the newly acquired memory
        for(size_t i = r2str->allocd; i < required; i++)
        {
            r2str->rstr[i].blocksize = r2str->sub_blocksize ? r2str->sub_blocksize : R2STR_DEFAULT_SUB_BLOCKSIZE;
            r2str->rstr[i].len = 0;
            r2str->rstr[i].s = 0;
            r2str->rstr[i].allocd = 0;
        }
        r2str->allocd = required;
    }

    // actual append
    va_list argp;
    va_start(argp, format);
    bool result = rstr_append_va(&r2str->rstr[r2str->count], format, argp);
    va_end(argp);
    if(!result) return false;
    if(r2str->rstr[r2str->count].len >= threshold) r2str->count++;
    return true; 
}

/**
 * @brief Write an R2str's contents to a file. If 'written' is not zero, conents will be appended to the file.
 * 
 * @param r2str 
 * @param keep_written set this to true, if you wish to keep the 'written' variable
 * @return size_t length of total string
 */
size_t r2str_file_write(R2str *r2str, const char *filename)
{
    if(!r2str) return 0;
    if(!r2str->allocd) return 0;

    // open file
    FILE *file = fopen(filename, "wb");
    if(!file) return 0;

    // write file
    size_t written = 0;
    for(size_t i = 0; i <= r2str->count; i++)
    {
        written += fwrite(r2str->rstr[i].s, sizeof(char), r2str->rstr[i].len, file);
        fflush(file);
    }

    // close file
    fclose(file);
    return written;
}

/**
 * @brief Move the contents of an R2str to an ordinary Rstr
 * 
 * @param r2str 
 * @param rstr 
 * @return true success
 * @return false error
 */
bool r2str_mv_rstr(R2str *r2str, Rstr *rstr)
{
    if(!r2str || !rstr) return false;
    bool result = true;
    for(size_t i = 0; i <= r2str->count; i++)
    {
        if(!rstr_append(rstr, r2str->rstr[i].s)) break;
        rstr_free(&r2str->rstr[i]);
    }
    return result;
}

/**
 * @brief Copy the contents of an R2str to an ordinary Rstr
 * 
 * @param r2str 
 * @param rstr 
 * @return true 
 * @return false 
 */
bool r2str_cp_rstr(R2str *r2str, Rstr *rstr)
{
    if(!r2str || !rstr) return false;
    bool result = true;
    for(size_t i = 0; i <= r2str->count; i++)
    {
        result &= rstr_append(rstr, r2str->rstr[i].s);
    }
    return result;
}

/**
 * @brief Print the whole of an R2str's string. Highly advised to not use on large strings (as it may take quite a while to print)
 * 
 * @param r2str 
 */
void r2str_print(R2str *r2str)
{
    if(!r2str) return;
    if(!r2str->allocd) return;
    for(size_t i = 0; i <= r2str->count; i++)
    {
        printf("%.*s", (int)r2str->rstr[i].len, r2str->rstr[i].s);
    }
}
