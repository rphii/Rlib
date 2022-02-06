#include "rfile.h"

size_t rfile_read(const char *filename, Rstr *dump)
{
    if(!filename || !dump) return 0;
    FILE *file = fopen(filename, "rb");
    if(!file) return 0;

    // get file length 
    fseek(file, 0, SEEK_END);
    size_t bytes_file = ftell(file);
    fseek(file, 0, SEEK_SET);

    // allocate memory
    if(bytes_file + 1 > dump->allocd)
    {
        char *temp = realloc(dump->s, bytes_file + 1);
        if(!temp) return 0;
        dump->s = temp;
        dump->allocd = bytes_file + 1;
    }
    
    // read file
    size_t bytes_read = fread(dump->s, 1, bytes_file, file);
    if(bytes_file != bytes_read) return 0;
    dump->s[bytes_read] = 0;
    
    // close file
    fclose(file);
    return bytes_read;
}


size_t rfile_size(const char *filename)
{
    if(!filename) return 0;
    FILE *file = fopen(filename, "rb");
    if(!file) return 0;

    // get file length 
    fseek(file, 0, SEEK_END);
    size_t bytes_file = ftell(file);
    fseek(file, 0, SEEK_SET);

    // close file
    fclose(file);
    return bytes_file;
}

size_t rfile_write(const char *filename, const char *dump, size_t len)
{
    // open file
    FILE *file = fopen(filename, "wb");
    if(!file) return 0;

    // write file
    size_t result = fwrite(dump, sizeof(char), len, file);

    // close file
    fclose(file);

    return result;
}

