#ifndef ASMPP_CLI_H
#define ASMPP_CLI_H

#include <stdio.h>
#include "util.h"

typedef struct {
    string_list_t* input;
    char* output_dir;
    char* output_name;
    int verbose;
    int debug;
    char* arch;
    char* as;
    int link_libc;
} config_t;

void print_help(char *program_name);
void print_usage(char *program_name);
config_t* parse_args(int argc, char** argv);
void eval_args(config_t* config);

#endif //ASMPP_CLI_H
