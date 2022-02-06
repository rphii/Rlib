#ifndef RFILE_H

#include <stdio.h>
#include <stdlib.h>
#include "rstr.h"

size_t rfile_read(const char *filename, Rstr *dump);
size_t rfile_size(const char *filename);
size_t rfile_write(const char *filename, char *dump, size_t len);


#define RFILE_H
#endif