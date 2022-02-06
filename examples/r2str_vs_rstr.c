
#include <time.h>
#include "../src/r2str.h"
#include "../src/rfile.h"

#define COUNT 10000000
#define FILENAME "NumbersAreCool"
#define WRITETIMES  10000

int main(void)
{
    // we will compare R2str against Rstr (R2str will be incrementally faster than Rstr as the file sizes reach the megabytes )
    // first R2str
    R2str r2str = {0};
    r2str.blocksize = R2STR_DEFAULT_BLOCKSIZE;
    r2str.threshold = R2STR_DEFAULT_THRESHOLD;
    r2str.sub_blocksize = R2STR_DEFAULT_SUB_BLOCKSIZE;
    clock_t t_0_r2str = clock();
    r2str_filename(&r2str, FILENAME"_r2str.txt");
    for(size_t i = 0; i < COUNT; i++)
    {
        r2str_append(&r2str, "The number %llu rocks!\n", i);
    }
    clock_t t_M_r2str = clock();
    size_t size_r2str = 0;
    for(size_t i = 0; i < WRITETIMES; i++)
    {
        size_r2str = r2str_file_write(&r2str, false);
    }
    clock_t t_E_r2str = clock();
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
        rstr_append(&rstr, "The number %llu rocks!\n", i);
    }
    size_t t_M_rstr = clock();
    size_t size_rstr = 0;
    for(size_t i = 0; i < WRITETIMES; i++)
    {
        size_rstr = rfile_write(FILENAME"_rstr.txt", rstr.s, rstr.len);
    }
    size_t t_E_rstr = clock();
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
    
    // more thorough writing time stats
    printf("===== Writing time stats =====\n");
    printf("Written %llu bytes %u times to the destinated output files\n", size_r2str == size_rstr ? size_rstr : 0, WRITETIMES);
    double t_W_R2str = (double)(t_E_r2str - t_M_r2str) / (double)(CLOCKS_PER_SEC * WRITETIMES);
    double t_W_Rstr = (double)(t_E_rstr - t_M_rstr) / (double)(CLOCKS_PER_SEC * WRITETIMES);
    printf("On average, R2str took %.4lfs/file (%.0lfB/s) (chunk write)\n", t_W_R2str, (double)size_r2str / t_W_R2str);
    printf("On average, Rstr took %.4lfs/file (%.0lfB/s) (bulk write)\n", t_W_Rstr, (double)size_rstr / t_W_Rstr);
    printf("This makes chunk writing %.4lfs/file %s than bulk writing\n", t_W_Rstr - t_W_R2str >= 0 ? t_W_Rstr - t_W_R2str : t_W_R2str - t_W_Rstr, t_W_Rstr - t_W_R2str >= 0 ? "faster" : "slower");
    printf("This makes chunk writing %.4lf times faster than bulk writing\n", t_W_Rstr / t_W_R2str);

    return 0;
}