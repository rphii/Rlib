
#include <time.h>
#include <ctype.h>
#include "../src/r2str.h"
#include "../src/rfile.h"

#define BYTES   250000000
#define FILENAME "NumbersAreCool"
#define WRITETIMES  10000

int main(void)
{
    // we will compare R2str against Rstr (R2str will be incrementally faster than Rstr as the file sizes reach the megabytes )
    // first R2str
    char *bulk = malloc(BYTES);
    if(!bulk) return 0;
    R2str r2str = {0};
    r2str.blocksize = R2STR_DEFAULT_BLOCKSIZE;
    r2str.threshold = R2STR_DEFAULT_THRESHOLD;
    r2str.sub_blocksize = R2STR_DEFAULT_SUB_BLOCKSIZE;
    r2str_filename(&r2str, FILENAME"_chunk.txt");
    // create the two strings
    for(size_t i = 0; i < BYTES; i++)
    {
        // get a random character
        char c = 0;
        while(!isgraph(c)) c = rand() % 256;
        // append to the strings
        r2str_append(&r2str, "%c", c);
        bulk[i] = c;
    }
    printf("Created the strings, we will now write to the files\n");
    // write in chunk mode
    clock_t t_0_chunk = clock();
    for(size_t i = 0; i < WRITETIMES; i++)
    {
        r2str_file_write(&r2str, false);
    }
    clock_t t_E_chunk = clock();
    r2str_free(&r2str);
    // write in bulk mode
    clock_t t_0_bulk = clock();
    for(size_t i = 0; i < WRITETIMES; i++)
    {
        rfile_write(FILENAME"_bulk.txt", bulk, BYTES);
    }
    clock_t t_E_bulk = clock();
    free(bulk);

    // more thorough writing time stats
    printf("===== Writing time stats =====\n");
    printf("Written %u bytes %u times to the destinated output files\n", BYTES, WRITETIMES);
    double t_W_chunk = (double)(t_E_chunk - t_0_chunk) / (double)(CLOCKS_PER_SEC * WRITETIMES);
    double t_W_bulk = (double)(t_E_bulk - t_0_bulk) / (double)(CLOCKS_PER_SEC * WRITETIMES);
    printf("On average, chunk writing took %.4lfs/file (%.0lfB/s)\n", t_W_chunk, (double)BYTES / t_W_chunk);
    printf("On average, bulk writing took %.4lfs/file (%.0lfB/s)\n", t_W_bulk, (double)BYTES / t_W_bulk);
    printf("This makes chunk writing %.4lfs/file %s than bulk writing\n", t_W_bulk - t_W_chunk >= 0 ? t_W_bulk - t_W_chunk : t_W_chunk - t_W_bulk, t_W_bulk - t_W_chunk >= 0 ? "faster" : "slower");
    printf("This makes chunk writing %.4lf times faster than bulk writing\n", t_W_bulk / t_W_chunk);

    return 0;
}