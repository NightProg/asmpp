#ifndef ASMPP_ASM_H
#define ASMPP_ASM_H

#include "cli.h"

#define SIZE_DATA_TARGET 1
#define SIZE_BSS_TARGET 2

typedef struct asm_section_text_t asm_section_text_t;
typedef struct asm_section_data_t asm_section_data_t;
typedef struct asm_section_bss_t asm_section_bss_t;
typedef struct asm_instruction_t asm_instruction_t;
typedef struct asm_data_t asm_data_t;
typedef struct asm_bss_t asm_bss_t;
typedef enum asm_size_t asm_size_t;
typedef struct asm_bss_size_t asm_bss_size_t;



typedef enum {
    BSS,
    DATA,
    TEXT
} asm_section_type_t;

typedef struct {
    asm_section_type_t type;
    int size;
    int capacity;
    union {
        asm_section_text_t *text;
        asm_section_data_t *data;
        asm_section_bss_t *bss;
    };
} asm_section_t;

struct asm_section_text_t {
    asm_instruction_t* instructions;
    int size;
    int capacity;
};

typedef enum {
    ASM_INSTR,
    ASM_LABEL
} asm_instruction_type_t;

struct asm_instruction_t {
    asm_instruction_type_t type;
    asm_instruction_t* list;
    char** args;
    int arg_size;
    int arg_capacity;
    int instr_size;
    int instr_capacity;
};

struct asm_data_t {
    char* name;
    asm_size_t* size;
    string_list_t* values;
};

struct asm_section_data_t {
    asm_data_t* data;
    int size;
    int capacity;
};

enum asm_size_t {
    BYTE,
    WORD,
    DWORD,
    QWORD
};

struct asm_bss_size_t {
    int n;
    asm_size_t size;
};

struct asm_bss_t {
    char* name;
    asm_bss_size_t size;
};


struct asm_section_bss_t {
    asm_bss_t* bss;
    int size;
    int capacity;
};

typedef struct {
    asm_section_text_t* text;
    asm_section_data_t* data;
    asm_section_bss_t* bss;
} asm_t;

asm_bss_size_t* bss_size_new(int n, asm_size_t size);

asm_bss_t* bss_new(char* name, asm_bss_size_t size);
void bss_free(asm_bss_t* bss);

asm_section_bss_t *section_bss_new();
int section_bss_add_bss(asm_section_bss_t *section, asm_bss_t *bss);
void section_bss_free(asm_section_bss_t *section);

char* size_to_string(asm_size_t size);
char* bss_size_to_string(asm_bss_size_t size);
asm_data_t *data_new(char *name, asm_size_t size, string_list_t *value);
void data_free(asm_data_t* data);

asm_section_data_t* section_data_new();
int section_data_add_data(asm_section_data_t* asm_section_data, asm_data_t* data);
void section_data_free(asm_section_data_t* data);

asm_instruction_t* instruction_new(asm_instruction_type_t type, char* name);
int instruction_add_arg(asm_instruction_t* instruction, char* arg);
int instruction_add_instr(asm_instruction_t* instruction, asm_instruction_t* child);
asm_instruction_t *copy_instruction(asm_instruction_t *instruction);
void instruction_free(asm_instruction_t* instruction);

asm_section_text_t* section_text_new();
int section_text_add_instruction(asm_section_text_t* text, asm_instruction_t* instruction);
void section_text_free(asm_section_text_t* text);

asm_section_t* section_new(asm_section_type_t type);
int section_set_text(asm_section_t* section, asm_section_text_t* text);
int section_set_data(asm_section_t* section, asm_section_data_t* data);
int section_set_bss(asm_section_t* section, asm_section_bss_t* bss);
void section_free(asm_section_t* section);


asm_t* asm_new();
int asm_set_text_section(asm_t* asm_, asm_section_text_t* section);
int asm_set_data_section(asm_t* asm_, asm_section_data_t* section);
int asm_set_bss_section(asm_t* asm_, asm_section_bss_t* section);
int asm_compile(asm_t* asm_, config_t* config, char* output_name);
char* asm_emit(asm_t* asm_);
void asm_free(asm_t* asm_);

#endif //ASMPP_ASM_H
