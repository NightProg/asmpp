#include "error.h"
#include <stdio.h>
#include <stdlib.h>

void error_and_quit(const char* message, error_kind_t err) {
    fprintf(stderr, "Error: %s\n", message);
    exit(err);
}

void error_and_jump(const char* message, error_kind_t err) {
    fprintf(stderr, "Error: %s\n", message);
    longjmp(env, err);
}

void error(const char* message, error_kind_t err) {
#ifdef ERROR_ACTION_QUIT
        error_and_quit(message, err);
#else
        error_and_jump(message, err);
#endif
}