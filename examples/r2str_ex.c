
#include <time.h>
#include "../src/r2str.h"
#include "../src/rfile.h"

#define COUNT 1000000
#define FILENAME "NumbersAreCool"

int main(void)
{
    // we will compare R2str against Rstr (R2str will be incrementally faster than Rstr as the file sizes reach the megabytes+)
    // first R2str
    R2str r2str = {0};
    r2str.blocksize = R2STR_DEFAULT_BLOCKSIZE;
    r2str.threshold = R2STR_DEFAULT_THRESHOLD;
    r2str.sub_blocksize = R2STR_DEFAULT_SUB_BLOCKSIZE;
    clock_t t_0_r2str = clock();
    for(size_t i = 0; i < COUNT; i++)
    {
        r2str_append(&r2str, "The number %llu rocks!\n", i);
    }
    clock_t t_M_r2str = clock();
    size_t length = r2str_file_write(&r2str, FILENAME"_r2str.txt");
    clock_t t_E_r2str = clock();
    printf("===== R2str string stats =====\n");
    printf("Final string length: %zu\n", length);
    printf("Allocd: %zu\n", r2str.allocd);
    printf("Blocksize: %zu\n", r2str.blocksize);
    printf("Count: %zu\n", r2str.count);
    printf("Threshold: %zu\n", r2str.threshold);
    r2str_free(&r2str);
    clock_t t_F_r2str = clock();
    printf("===== R2str time stats =====\n");
    printf("%.3lfs appending time\n", (double)(t_M_r2str - t_0_r2str) / (double)CLOCKS_PER_SEC);
    printf("%.3lfs writing time\n", (double)(t_E_r2str - t_M_r2str) / (double)CLOCKS_PER_SEC);
    printf("%.3lfs freeing time\n", (double)(t_F_r2str - t_E_r2str) / (double)CLOCKS_PER_SEC);
    printf("%.3lfs total time\n", (double)(t_F_r2str - t_0_r2str) / (double)CLOCKS_PER_SEC);

    // then Rstr
    Rstr rstr = {0};
    rstr.blocksize = R2STR_DEFAULT_SUB_BLOCKSIZE;
    size_t t_0_rstr = clock();
    for(size_t i = 0; i < COUNT; i++)
    {
        rstr_append(&rstr, "The number %zu rocks!\n", i);
    }
    size_t t_M_rstr = clock();
    length = rfile_write(FILENAME"_rstr.txt", rstr.s, rstr.len, 0);
    size_t t_E_rstr = clock();
    printf("===== Rstr string stats =====\n");
    printf("Final string length: %zu\n", length);
    printf("Allocd: %zu\n", rstr.allocd);
    printf("Blocksize: %zu\n", rstr.blocksize);
    rstr_free(&rstr);
    size_t t_F_rstr = clock();
    printf("===== Rstr time stats =====\n");
    printf("%.3lfs appending time\n", (double)(t_M_rstr - t_0_rstr) / (double)CLOCKS_PER_SEC);
    printf("%.3lfs writing time\n", (double)(t_E_rstr - t_M_rstr) / (double)CLOCKS_PER_SEC);
    printf("%.3lfs freeing time\n", (double)(t_F_rstr - t_E_rstr) / (double)CLOCKS_PER_SEC);
    printf("%.3lfs total time\n", (double)(t_F_rstr - t_0_rstr) / (double)CLOCKS_PER_SEC);
    
    // finally, compare
    printf("===== Compared time stats =====\n");
    printf("R2str appending was %.3lf times faster than Rstr\n", (double)(t_M_rstr - t_0_rstr) / (double)(t_M_r2str - t_0_r2str));
    printf("R2str writing was %.3lf times faster than Rstr\n", (double)(t_E_rstr - t_M_rstr) / (double)(t_E_r2str - t_M_r2str));
    printf("R2str freeing was %.3lf times faster than Rstr\n", (double)(t_F_rstr - t_E_rstr) / (double)(t_F_r2str - t_E_r2str));
    printf("R2str total was %.3lf times faster than Rstr\n", (double)(t_F_rstr - t_0_rstr) / (double)(t_F_r2str - t_0_r2str));
    
    return 0;
}