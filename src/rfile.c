#include "rfile.h"

/////////////////////////////////////
// PUBLIC FUNCTION IMPLEMENTATIONS //
/////////////////////////////////////

/**
 * @brief Read from a file into an Rstr
 * 
 * @param filename 
 * @param dump 
 * @return size_t count of bytes read
 */
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

/**
 * @brief Get the size of a file (I'm sure there is a better way to do it~)
 * 
 * @param filename 
 * @return size_t count of bytes of the file
 */
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

/**
 * @brief Write a string to a file
 * 
 * @param filename 
 * @param dump the string to be written
 * @param len the length of the string to be written
 * @return size_t count of bytes read
 */
size_t rfile_write(const char *filename, const char *dump, size_t len)
{
    // open file
    FILE *file = fopen(filename, "wb");
    if(!file) return 0;

    // write file
    size_t result = fwrite(dump, sizeof(char), len, file);
    fflush(file);

    // close file
    fclose(file);

    return result;
}

