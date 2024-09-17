#ifndef ASMPP_CODEGEN_H
#define ASMPP_CODEGEN_H

#include "ast.h"
#include "asm.h"



typedef struct {
    char *name;
    label_t *label;
} label_hashtable_entry_t;

typedef struct {
    label_hashtable_entry_t *entries;
    int size;
    int capacity;
} label_hashtable_t;

label_hashtable_t *new_label_hashtable();
void label_hashtable_insert(label_hashtable_t *table, char *name, label_t *label);
label_t *label_hashtable_get(label_hashtable_t *table, char *name);
void free_label_hashtable(label_hashtable_t *table);

typedef enum {
    CODEGEN_TEXT,
    CODEGEN_LABEL
} codegen_entry_point_t;

typedef struct {
    stmt_list_t *stmts;
    label_hashtable_t *labels;
    asm_t *asm_;
    codegen_entry_point_t entry_point;
    asm_instruction_t *current_label;
    int should_add_label;
    int count;
} codegen_t;


codegen_t *new_codegen(stmt_list_t *stmts);
void codegen_insert_instruction(codegen_t *codegen, asm_instruction_t *instruction);
void codegen(codegen_t *codegen);
void codegen_instr(codegen_t *codegen, instr_t *instr);
void codegen_label(codegen_t *codegen, label_t *label);
void codegen_extern(codegen_t *codegen, extern_t *extern_);
void codegen_data(codegen_t *codegen, data_t *data);

call_abi_t *get_c_call_abi();
void free_codegen(codegen_t *codegen);

#endif //ASMPP_CODEGEN_H
