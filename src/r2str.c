#include "r2str.h"

/**
 * @brief Print the whole of an R2str's string. Highly advised to not use on large strings (as it may take quite a while to print)
 * 
 * @param r2str 
 */
void r2str_print(R2str *r2str)
{
    for(size_t i = 0; i <= r2str->count; i++)
    {
        printf("%.*s", (int)r2str->rstr[i].len, r2str->rstr[i].s);
    }
}

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
    rstr_free(&r2str->filename);
    r2str->rstr = 0;
    r2str->allocd = 0;
    r2str->blocksize = 0;
    r2str->count = 0;
}

/**
 * @brief Set / overwrite the filename saved within an R2str
 * 
 * @param r2str 
 * @param filename 
 */
void r2str_filename(R2str *r2str, char *filename)
{
    if(!r2str || !filename);
    rstr_recycle(&r2str->filename);
    rstr_append(&r2str->filename, filename);
}

/**
 * @brief Recycle an R2str (keep memory, but reset string lengths)
 * 
 * @param r2str 
 * @param keep_written set this to true, if you whish to keep the 'written' amount
 */
void r2str_recycle(R2str *r2str, bool keep_written)
{
    if(!r2str) return;
    for(size_t i = 0; i <= r2str->count; i++)
    {
        rstr_recycle(&r2str->rstr[i]);
    }
    r2str->count = 0;
    if(!keep_written) r2str->written = 0;
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
bool r2str_append(R2str *r2str, size_t sub_blocksize, char *format, ...)
{
    if(!r2str || !format) return false;

    // calculate required memory
    size_t threshold = r2str->threshold ? r2str->threshold : R2STR_DEFAULT_THRESHOLD;
    size_t blocksize = r2str->blocksize ? r2str->blocksize : R2STR_DEFAULT_BLOCKSIZE;
    size_t required = blocksize * ((r2str->count + 1) / blocksize + 1);

    // make sure to have enought memory
    if(required > r2str->allocd)
    {
        if(r2str->allocd && r2str->allocd == blocksize)
        {
            // write to a file
            r2str_file_write(r2str, true);
            r2str_recycle(r2str, true);
        }
        else
        {
            void *temp = realloc(r2str->rstr, sizeof(*r2str->rstr) * required);
            if(!temp) return false;
            r2str->rstr = temp;
            // initialize the newly acquired memory
            for(size_t i = r2str->allocd; i < required; i++)
            {
                r2str->rstr[i].blocksize = sub_blocksize ? sub_blocksize : R2STR_DEFAULT_SUB_BLOCKSIZE;
                r2str->rstr[i].len = 0;
                r2str->rstr[i].s = 0;
                r2str->rstr[i].allocd = 0;
            }
            r2str->allocd = required;
        }
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
 * @brief Write an R2str's contents to a file. If 'written' is not zero, conents will be appended to the file.
 * 
 * @param r2str 
 * @param keep_written set this to true, if you wish to keep the 'written' variable
 * @return size_t length of total string
 */
size_t r2str_file_write(R2str *r2str, bool keep_written)
{
    if(!r2str) return 0;

    // open file
    FILE *file = 0;
    if(!r2str->written) file = fopen(r2str->filename.s, "wb");
    else file = fopen(r2str->filename.s, "ab");
    if(!file) return 0;

    // write file
    for(size_t i = 0; i <= r2str->count; i++)
    {
        r2str->written += fwrite(r2str->rstr[i].s, sizeof(char), r2str->rstr[i].len, file);
    }

    // close file
    fclose(file);
    size_t result = r2str->written;
    if(!keep_written) r2str->written = 0;
    return result;
}
