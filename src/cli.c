#include "cli.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "log.h"
#include <stdlib.h>
#include <string.h>

void print_help(char *program_name) {
    printf("Usage: %s [options] file\n", program_name);
    printf("Options:\n");
    printf("  -o <output>  Output file\n");
    printf("  -O <dir>     Output directory\n");
    printf("  -v           Verbose\n");
    printf("  -d           Debug\n");
    printf("  -a <a>       Assembler\n");
    printf("  -l           Link with libc\n");
    printf("  -h           Print this help\n");
}

void print_usage(char *program_name) {
    printf("Usage: %s [options] file\n", program_name);
    printf("Try '%s -h' for more information.\n", program_name);
}

config_t* parse_args(int argc, char** argv) {
    char* program_name = argv[0];
    config_t* config = malloc(sizeof(config_t));
    config->input = new_string_list();
    config->output_dir = "build";
    config->output_name = NULL;
    config->verbose = 0;
    config->debug = 0;
    config->as = "nasm";
    config->link_libc = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == '\0') {
                fprintf(stderr, "Invalid option: %s\n", argv[i]);
                print_usage(program_name);
                exit(1);
            }
            switch (argv[i][1]) {
                case 'o':
                    if (i + 1 >= argc) {
                        fprintf(stderr, "Missing argument for option -o\n");
                        print_usage(program_name);
                        exit(1);
                    }
                    config->output_name = argv[++i];
                    break;
                case 'O':
                    if (i + 1 >= argc) {
                        fprintf(stderr, "Missing argument for option -O\n");
                        print_usage(program_name);
                        exit(1);
                    }
                    config->output_dir = argv[++i];
                    break;
                case 'v':
                    config->verbose = 1;
                    break;
                case 'd':
                    config->debug = 1;
                    break;
                case 'a':
                    if (i + 1 >= argc) {
                        fprintf(stderr, "Missing argument for option -A\n");
                        print_usage(program_name);
                        exit(1);
                    }
                    config->as = argv[++i];
                    break;
                case 'l':
                    config->link_libc = 1;
                    break;
                case 'h':
                    print_help(program_name);
                    exit(0);
                default:
                    fprintf(stderr, "Unknown option: %s\n", argv[i]);
                    print_usage(program_name);
                    exit(1);
            }
        } else {
            append_string(config->input, argv[i]);
        }
    }

    if (config->input == NULL) {
        fprintf(stderr, "No input file\n");
        print_usage(program_name);
        exit(1);
    }

    return config;
}

void eval_args(config_t* config) {
    error_kind_t err = setjmp(env);
    if (err != ERROR_NONE) {
        fprintf(stderr, "An error occurred\n");
        exit(1);
    }
    if (config->verbose) {
        printf("Verbose mode enabled\n");
        if (config->as != NULL) {
            printf("Assembler: %s\n", config->as);
        }
        if (config->link_libc) {
            printf("Linking with libc\n");
        }
        if (config->output_name != NULL) {
            printf("Output file: %s\n", config->output_name);
        }
        if (config->output_dir != NULL) {
            printf("Output directory: %s\n", config->output_dir);
        }
    }
    if (config->debug) {
        printf("Debug mode enabled\n");
    }
    if (config->input->len > 1 && config->output_name != NULL) {
        log_(LOG_WARN, "Multiple input files detected, only the first output name will be processed the other will be created like <file>.asm");
    }
    for (int i = 0; i < config->input->len; i++) {
        char* file_path = get_string(config->input, i);
        char* output_name;
        if (config->output_name == NULL) {
            output_name = malloc(strlen(file_path) + 5);
            if (output_name == NULL) {
                error("Failed to allocate memory for output name", ERROR_ALLOC);
            }
            sprintf(output_name, "%s.asm", file_path);
            config->output_name = output_name;
        } else {
            output_name = config->output_name;
            config->output_name = NULL;
        }
        if (config->verbose) {
            printf("Processing file: %s\n", file_path);
        }
        FILE* file = fopen(file_path, "r");
        if (file == NULL) {
            error("Failed to open file", ERROR_INVALID);
        }
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);
        char* buffer = malloc(size + 1);
        if (buffer == NULL) {
            error("Failed to allocate memory for buffer", ERROR_ALLOC);
        }
        fread(buffer, 1, size, file);
        buffer[size] = '\0';
        if (config->verbose) {
            printf("Lexing...\n");
        }
        lexer_t *lexer = new_lexer(buffer);
        lexer_lex(lexer);
        if (config->verbose) {
            printf("Parsing...\n");
        }
        parser_t *parser = new_parser(lexer->tokens);
        parse(parser);
        stmt_list_t *stmts = parser->stmts;
        if (config->verbose) {
            printf("Codegen...\n");
        }
        codegen_t *code = new_codegen(stmts);
        codegen(code);
        if (config->verbose) {
            printf("Emitting assembly...\n");
        }
        asm_compile(code->asm_, config, output_name);

    }

}