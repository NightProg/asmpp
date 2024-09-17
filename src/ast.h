
#ifndef ASMPP_AST_H
#define ASMPP_AST_H

#include <sys/types.h>
#include <stdbool.h>
#include "util.h"

typedef struct expr_t expr_t;
typedef struct expr_list_t expr_list_t;
typedef struct instr_t instr_t;
typedef struct instr_if_t instr_if_t;
typedef struct instr_call_t instr_call_t;
typedef struct instr_asm_t instr_asm_t;
typedef struct instr_list_t instr_list_t;
typedef struct label_t label_t;
typedef struct stmt_t stmt_t;
typedef struct extern_t extern_t;
typedef struct register_kind_list_t register_kind_list_t;
typedef enum register_kind_t register_kind_t;
typedef struct stmt_list_t stmt_list_t;
typedef struct attribute_t attribute_t;
typedef struct attribute_list_t attribute_list_t;
typedef struct data_t data_t;
typedef struct type_t type_t;
typedef enum type_kind_t type_kind_t;

enum register_kind_t {
    RAX,
    RBX,
    RCX,
    RDX,
    RSI,
    RDI,
    RBP,
    RSP,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    RIP,
    EAX,
    EBX,
    ECX,
    EDX,
    ESI,
    EDI,
    EBP,
    ESP,
    R8D,
    R9D,
    R10D,
    R11D,
    R12D,
    R13D,
    R14D,
    R15D,
    EIP,
    AX,
    BX,
    CX,
    DX,
    SP,
    BP,
    DI,
    SI,
    R8W,
    R9W,
    R10W,
    R11W,
    R12W,
    R13W,
    R14W,
    R15W,
    IP,
    AH,
    AL,
    BH,
    BL,
    CH,
    CL,
    SPL,
    BPL,
    DIL,
    SIL,
    DH,
    DL,
    R8B,
    R9B,
    R10B,
    R11B,
    R12B,
    R13B,
    R14B,
    R15B,
};

typedef enum {
    ARGUMENT_REGISTER,
    ARGUMENT_STACK
} argument_kind_t;

typedef struct {
    argument_kind_t kind;
    register_kind_t reg;
} argument_t;

argument_t *new_argument_register(register_kind_t reg);
argument_t *new_argument_stack();
void free_argument(argument_t *arg);

typedef struct {
    int len;
    int capacity;
    argument_t *args;
} argument_list_t;

argument_list_t *new_argument_list();
void append_argument(argument_list_t *list, argument_t *arg);
argument_t *get_argument(argument_list_t *list, int index);
void free_argument_list(argument_list_t *list);

typedef struct {
    char *name;
    argument_list_t *args;
} call_abi_t;

call_abi_t *new_call_abi(char *name, argument_list_t *args);
void free_call_abi(call_abi_t *abi);

typedef enum {
    ATTR_FLAG,
    ATTR_DIRECTIVE,
} attribute_kind_t;

struct attribute_t {
    attribute_kind_t kind;
    char* name;
    string_list_t* value;
};

attribute_t *new_attribute(attribute_kind_t kind, char *name, string_list_t* value);
int has_argument(attribute_t *attribute, char *arg);
void free_attribute(attribute_t *attribute);

struct attribute_list_t {
    int len;
    int capacity;
    attribute_t* attributes;
};

attribute_list_t* new_attribute_list();
void append_attribute(attribute_list_t* list, attribute_t* attribute);
attribute_t* get_attribute(attribute_list_t* list, int index);
int has_attribute(attribute_list_t* list, const char* name);
int find_attribute(attribute_list_t* list, const char* name);
void free_attribute_list(attribute_list_t* list);


struct stmt_list_t {
    int len;
    int capacity;
    stmt_t* stmts;
};

stmt_list_t* new_stmt_list();
void append_stmt(stmt_list_t* list, stmt_t* stmt);
stmt_t* get_stmt(stmt_list_t* list, int index);
void free_stmt_list(stmt_list_t* list);

typedef enum {
    STMT_LABEL,
    STMT_EXTERN,
    STMT_INSTR,
    STMT_DATA
} stmt_kind_t;

struct stmt_t {
    stmt_kind_t kind;
    union {
        label_t* label;
        extern_t* extern_;
        instr_t* instr;
        data_t* data;
    };
};

stmt_t* new_label_stmt(label_t* label);
stmt_t* new_extern_stmt(extern_t* extern_);
stmt_t* new_instr_stmt(instr_t* instr);
stmt_t* new_data_stmt(data_t* data);
void free_stmt(stmt_t* stmt);

enum type_kind_t {
    TYPE_BYTE,
    TYPE_WORD,
    TYPE_DWORD,
    TYPE_QWORD,
    TYPE_ARRAY,
};

struct type_t {
    type_kind_t kind;
    type_t *base;
    int array_size;
};

type_t* new_simple_type(type_kind_t kind);
type_t* new_array_type(type_t* base, int array_size);
void free_type(type_t* type);

struct data_t {
    char* name;
    type_t* type;
    expr_list_t* values;
};

data_t* new_data(char* name, type_t* type, expr_list_t* values);
data_t* new_data_uninitialized(char* name, type_t* type);

struct extern_t {
    call_abi_t *abi;
    attribute_list_t *attributes;
    char* name;
};

extern_t* new_extern(call_abi_t* abi, char* name, attribute_list_t *attributes);
void free_extern(extern_t* extern_);

struct label_t {
    char* name;
    call_abi_t *abi;
    instr_list_t *instrs;
    attribute_list_t *attributes;
};

label_t* new_label(char* name, call_abi_t* abi, instr_list_t* instrs, attribute_list_t *attributes);
void free_label(label_t* label);

struct instr_list_t {
    int len;
    int capacity;
    instr_t* instrs;
};

instr_list_t* new_instr_list();
void append_instr(instr_list_t* list, instr_t* instr);
instr_t* get_instr(instr_list_t* list, int index);
instr_list_t** split_instr_list(instr_list_t* list, int index);
void free_instr_list(instr_list_t* list);

typedef enum {
    INSTR_IF,
    INSTR_ASM,
    INSTR_CALL
} instr_kind_t;

struct instr_t {
    instr_kind_t kind;
    union {
        instr_if_t* instr_if;
        instr_asm_t* instr_asm;
        instr_call_t* instr_call;
    };
};

instr_t* new_if_instr(instr_if_t* instr_if);
instr_t* new_asm_instr(instr_asm_t* asm_instr);
instr_t* new_call_instr(instr_call_t* call_instr);
void free_instr(instr_t* instr);

typedef enum {
    EQ,
    NE,
    LT,
    LE,
    GT,
    GE
} cmp_kind_t;

cmp_kind_t get_cmp_kind_by_name(const char* name);

struct instr_if_t {
    expr_list_t* condition;
    cmp_kind_t cmp;
    instr_list_t* then_instrs;
    instr_list_t* else_instrs;
};

instr_if_t* new_instr_if(expr_list_t* condition, cmp_kind_t cmp, instr_list_t* then_instrs, instr_list_t* else_instrs);
void free_instr_if(instr_if_t* instr_if);

struct instr_call_t {
    char* callee;
    expr_list_t* args;
};

instr_call_t* new_instr_call(char* callee, expr_list_t* args);
void free_instr_call(instr_call_t* instr_call);

struct instr_asm_t {
    char* name;
    expr_list_t* args;
};

instr_asm_t* new_instr_asm(char* name, expr_list_t* args);
void free_instr_asm(instr_asm_t* instr);

struct register_kind_list_t {
    int len;
    int capacity;
    register_kind_t* registers;
};

register_kind_list_t* new_register_kind_list();
void append_register_kind(register_kind_list_t* list, register_kind_t register_);
register_kind_t get_register_kind(register_kind_list_t* list, int index);
void free_register_kind_list(register_kind_list_t* list);



register_kind_t get_register_kind_by_name(char* name);
char* register_kind_to_string(register_kind_t register_);
bool is_64_bit(register_kind_t register_);
bool is_32_bit(register_kind_t register_);
bool is_16_bit(register_kind_t register_);
bool is_8_bit(register_kind_t register_);

// ASM x86 expr kind
typedef enum {
    IMMEDIATE,
    REGISTER,
    MEMORY,
    LABEL,
    STRING
} expr_kind_t;

struct expr_list_t {
    int len;
    int capacity;
    expr_t* exprs;
};

expr_list_t* new_expr_list();
void append_expr(expr_list_t* list, expr_t* expr);
expr_t* get_expr(expr_list_t* list, int index);
void free_expr_list(expr_list_t* list);



struct expr_t {
    expr_kind_t kind;
    expr_list_t* args;
    union {
        int64_t immediate;
        register_kind_t register_;
        struct {
            register_kind_t base;
            register_kind_t index;
            int64_t scale;
            int64_t displacement;
        } memory;
        char* label;
        char* string;
    };
};

expr_t* new_expr_immediate(int64_t immediate);
expr_t* new_expr_register(register_kind_t register_);
expr_t* new_expr_memory(register_kind_t base, register_kind_t index, int64_t scale, int64_t displacement);
expr_t* new_expr_string(char* string);
expr_t* new_expr_label(char* label);

int64_t get_immediate(expr_t* expr);
register_kind_t get_register(expr_t* expr);
register_kind_t get_base(expr_t* expr);
register_kind_t get_index(expr_t* expr);
int64_t get_scale(expr_t* expr);
int64_t get_displacement(expr_t* expr);
char* get_label(expr_t* expr);

void free_expr(expr_t* expr);



#endif //ASMPP_AST_H
