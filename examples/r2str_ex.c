
#include <time.h>
#include "../src/r2str.h"

int main(void)
{
    R2str r2str = {0};
    clock_t t_0 = clock();
    r2str_filename(&r2str, "NumbersAreCool.txt");
    for(size_t i = 0; i < 1000000000; i++)
    {
        r2str_append(&r2str, 0x100, "The number %llu rocks!\n", i);
    }
    clock_t t_M = clock();
    size_t length = r2str_file_write(&r2str, false);
    clock_t t_E = clock();
    printf("Final string length: %llu\n", length);
    printf("Allocd: %llu\n", r2str.allocd);
    printf("Blocksize: %llu\n", r2str.blocksize);
    printf("Count: %llu\n", r2str.count);
    printf("Threshold: %llu\n", r2str.threshold);
    // r2str_print(&r2str);
    r2str_free(&r2str);
    clock_t t_F = clock();
    printf("%.3lfs appending time\n", (double)(t_M - t_0) / (double)CLOCKS_PER_SEC);
    printf("%.3lfs writing time\n", (double)(t_E - t_M) / (double)CLOCKS_PER_SEC);
    printf("%.3lfs freeing time\n", (double)(t_F - t_E) / (double)CLOCKS_PER_SEC);
    printf("%.3lfs total time\n", (double)(t_F - t_0) / (double)CLOCKS_PER_SEC);

    return 0;
}