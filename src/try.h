#ifndef TRY_H

#include <stdio.h>
#include <setjmp.h>
#include "rgbf.h"

extern _Thread_local jmp_buf try_env;
extern _Thread_local int try_val;

#define ERROR(msg, ...) fprintf(stderr, F("[ ERROR ] ", BOLD F_RB)F("%s:%s:%d ", F_WB)F(msg, F_KB)"\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)
#define WARN(msg, ...)  fprintf(stderr, F("[ WARN ] ", BOLD F_YB)F("%s:%s:%d ", F_WB)F(msg, F_KB)"\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#define TRY             try_val = setjmp(try_env); if(!try_val)
#define THROW(fmt, ...) do { ERROR(fmt, ##__VA_ARGS__); longjmp(try_env, 1); } while(0)
#define CATCH           if(try_val)


#define TRY_H
#endif