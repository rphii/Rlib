#ifndef R2STR_H

#include "rstr.h"

#define R2STR_DEFAULT_BLOCKSIZE     ((size_t)0x100)
#define R2STR_DEFAULT_SUB_BLOCKSIZE ((size_t)0x1000)
#define R2STR_DEFAULT_THRESHOLD     ((size_t)0x100000)

/**
 * @brief R2str is handy for fast formattet text file output
 * 
 */
typedef struct R2str
{
    /* substrings */
    Rstr *rstr;
    /* how many substrings are used */
    size_t count; 
    /* how many substrings allocated so far */
    size_t allocd; 
    /* blocksize of 'allocd' */
    size_t blocksize; 
    /* sub blocksize */
    size_t sub_blocksize;
    /* minimumg length of one string to get new substring */
    size_t threshold;
    /* output filename */
    Rstr filename;
    /* how many bytes we've already written to the file */
    size_t written;
}
R2str;

void r2str_print(R2str *r2str);
void r2str_free(R2str *r2str);
void r2str_filename(R2str *r2str, char *filename);
void r2str_recycle(R2str *r2str, bool keep_written);
bool r2str_append(R2str *r2str, char *format, ...);
bool r2str_mv_rstr(R2str *r2str, Rstr *rstr);
bool r2str_cp_rstr(R2str *r2str, Rstr *rstr);
size_t r2str_file_write(R2str *r2str, bool keep_written);

#define R2STR_H
#endif