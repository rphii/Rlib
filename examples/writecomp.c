
#include <time.h>
#include <ctype.h>
#include "../src/rfile.h"
#include "../src/rbar.h"

#define BYTES   250000000
#define FILENAME "Writecomp"
#define WRITETIMES  10000
#define CHUNK_SIZE  0x100000

int main(void)
{
    char *str = malloc(BYTES);
    if(!str) return 0;

    // create the strings
    printf("Creating the string....\n");
    Rbar bar = RBAR(0);
    rbar_init(&bar, 0, BYTES);
    for(size_t i = 0; i < BYTES; i++)
    {
        // get a random character
        char c = 0;
        while(!isgraph(c)) c = rand() % 256;
        str[i] = c;
        rbar_show(&bar, i, BYTES);
    }
    printf("\n");
    printf("Created the string, we will now write to the files...\n");
    // write in chunk mode
    printf("Writing to %s...\n", FILENAME"_chunk.txt");
    clock_t t_0_chunk = clock();
    rbar_init(&bar, 0, WRITETIMES);
    for(size_t i = 0; i < WRITETIMES; i++)
    {
        rfile_write(FILENAME"_chunk.txt", str, BYTES, CHUNK_SIZE);
        rbar_show(&bar, i, WRITETIMES);
    }
    printf("\n");
    clock_t t_E_chunk = clock();
    // write in bulk mode
    printf("Writing to %s...\n", FILENAME"_bulk.txt");
    clock_t t_0_bulk = clock();
    rbar_init(&bar, 0, WRITETIMES);
    for(size_t i = 0; i < WRITETIMES; i++)
    {
        rfile_write(FILENAME"_bulk.txt", str, BYTES, 0);
        rbar_show(&bar, i, WRITETIMES);
    }
    printf("\n");
    clock_t t_E_bulk = clock();
    free(str);

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
