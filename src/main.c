#include "cli.h"

int main(int argc, char** argv) {
    config_t* config = parse_args(argc, argv);
    if (config == NULL) {
        print_usage(argv[0]);
        return 1;
    }
    eval_args(config);
    return 0;
}

