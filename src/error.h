
#ifndef ASMPP_ERROR_H
#define ASMPP_ERROR_H
#define ERROR_ACTION_QUIT

#include <setjmp.h>

typedef enum {
    ERROR_NONE,
    ERROR_ALLOC,
    ERROR_INVALID
} error_kind_t;



jmp_buf env;

void error(const char* message, error_kind_t err);
void error_and_quit(const char* message, error_kind_t err);
void error_and_jump(const char* message, error_kind_t err);
#endif
