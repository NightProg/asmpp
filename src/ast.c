#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "error.h"
#define MATCH(x, r) if (strcmp(str, #x) == 0) return r;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NullDereference"

argument_t *new_argument_register(register_kind_t reg) {
    argument_t *arg = malloc(sizeof(argument_t));
    if (arg == NULL) {
        error("Failed to allocate memory for register argument", ERROR_ALLOC);
    }
    arg->kind = ARGUMENT_REGISTER;
    arg->reg = reg;
    return arg;
}

argument_t *new_argument_stack() {
    argument_t *arg = malloc(sizeof(argument_t));
    if (arg == NULL) {
        error("Failed to allocate memory for stack argument", ERROR_ALLOC);
    }
    arg->kind = ARGUMENT_STACK;
    return arg;
}


argument_list_t *new_argument_list() {
    argument_list_t *list = malloc(sizeof(argument_list_t));
    if (list == NULL) {
        error("Failed to allocate memory for argument list", ERROR_ALLOC);
    }
    list->len = 0;
    list->capacity = 4;
    list->args = malloc(sizeof(argument_t) * list->capacity);
    if (list->args == NULL) {
        error("Failed to allocate memory for argument list", ERROR_ALLOC);
    }
    return list;
}

void append_argument(argument_list_t *list, argument_t *arg) {
    if (list->len == list->capacity) {
        list->capacity *= 2;
        list->args = realloc(list->args, sizeof(argument_t) * list->capacity);
        if (list->args == NULL) {
            error("Failed to reallocate memory for argument list", ERROR_ALLOC);
        }
    }
    list->args[list->len++] = *arg;
}

argument_t *get_argument(argument_list_t *list, int index) {
    if (index < 0 || index >= list->len) {
        error("Index out of bounds", ERROR_INVALID);
    }
    return &list->args[index];
}

void free_argument_list(argument_list_t *list) {
    free(list->args);
    free(list);
}

call_abi_t *new_call_abi(char *name, argument_list_t *args) {
    call_abi_t *abi = malloc(sizeof(call_abi_t));
    if (abi == NULL) {
        error("Failed to allocate memory for call abi", ERROR_ALLOC);
    }
    abi->name = name;
    abi->args = args;
    return abi;
}

void free_call_abi(call_abi_t *abi) {
    free(abi->name);
    free_argument_list(abi->args);
    free(abi);
}

attribute_t *new_attribute(attribute_kind_t kind, char *name, string_list_t *value) {
    attribute_t *attribute = malloc(sizeof(attribute_t));
    if (attribute == NULL) {
        error("Failed to allocate memory for attribute", ERROR_ALLOC);
    }
    attribute->kind = kind;
    attribute->name = name;
    attribute->value = value;
    return attribute;
}

int has_argument(attribute_t *attribute, char *arg) {
    if (attribute->value == NULL) {
        return -1;
    }
    return find_string(attribute->value, arg) != -1;
}

void free_attribute(attribute_t *attribute) {
    free(attribute->name);
    free_string_list(attribute->value);
    free(attribute);
}

attribute_list_t *new_attribute_list() {
    attribute_list_t *list = malloc(sizeof(attribute_list_t));
    if (list == NULL) {
        error("Failed to allocate memory for attribute list", ERROR_ALLOC);
    }
    list->len = 0;
    list->capacity = 4;
    list->attributes = malloc(sizeof(attribute_t) * list->capacity);
    if (list->attributes == NULL) {
        error("Failed to allocate memory for attribute list", ERROR_ALLOC);
    }
    return list;
}

void append_attribute(attribute_list_t *list, attribute_t *attribute) {
    if (list->len == list->capacity) {
        list->capacity *= 2;
        list->attributes = realloc(list->attributes, sizeof(attribute_t) * list->capacity);
        if (list->attributes == NULL) {
            error("Failed to reallocate memory for attribute list", ERROR_ALLOC);
        }
    }
    list->attributes[list->len++] = *attribute;
}

attribute_t *get_attribute(attribute_list_t *list, int index) {
    if (index < 0 || index >= list->len) {
        error("Index out of bounds", ERROR_INVALID);
    }
    return &list->attributes[index];
}

int has_attribute(attribute_list_t *list, const char *name) {
    return find_attribute(list, name) != -1;
}

int find_attribute(attribute_list_t *list, const char *name) {
    for (int i = 0; i < list->len; i++) {
        if (strcmp(list->attributes[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void free_attribute_list(attribute_list_t *list) {
    for (int i = 0; i < list->len; i++) {
        free_attribute(&list->attributes[i]);
    }
    free(list->attributes);
    free(list);
}


stmt_list_t *new_stmt_list() {
    stmt_list_t *list = malloc(sizeof(stmt_list_t));
    if (list == NULL) {
        error("Failed to allocate memory for statement list", ERROR_ALLOC);
    }
    list->len = 0;
    list->capacity = 4;
    list->stmts = malloc(sizeof(stmt_t) * list->capacity);
    if (list->stmts == NULL) {
        error("Failed to allocate memory for statement list", ERROR_ALLOC);
    }
    return list;
}

void append_stmt(stmt_list_t *list, stmt_t *stmt) {
    if (list->len == list->capacity) {
        list->capacity *= 2;
        list->stmts = realloc(list->stmts, sizeof(stmt_t) * list->capacity);
        if (list->stmts == NULL) {
            error("Failed to reallocate memory for statement list", ERROR_ALLOC);
        }
    }
    list->stmts[list->len++] = *stmt;
}

stmt_t *get_stmt(stmt_list_t *list, int index) {
    if (index < 0 || index >= list->len) {
        error("Index out of bounds", ERROR_INVALID);
    }
    return &list->stmts[index];
}

void free_stmt_list(stmt_list_t *list) {
    for (int i = 0; i < list->len; i++) {
        free_stmt(&list->stmts[i]);
    }
    free(list->stmts);
    free(list);
}

stmt_t *new_label_stmt(label_t *label) {
    stmt_t *stmt = malloc(sizeof(stmt_t));
    if (stmt == NULL) {
        error("Failed to allocate memory for label statement", ERROR_ALLOC);
    }
    stmt->kind = STMT_LABEL;
    stmt->label = label;
    return stmt;
}

stmt_t *new_extern_stmt(extern_t *extern_) {
    stmt_t *stmt = malloc(sizeof(stmt_t));
    if (stmt == NULL) {
        error("Failed to allocate memory for extern statement", ERROR_ALLOC);
    }
    stmt->kind = STMT_EXTERN;
    stmt->extern_ = extern_;
    return stmt;
}

stmt_t *new_instr_stmt(instr_t *instr) {
    stmt_t *stmt = malloc(sizeof(stmt_t));
    if (stmt == NULL) {
        error("Failed to allocate memory for instruction statement", ERROR_ALLOC);
    }
    stmt->kind = STMT_INSTR;
    stmt->instr = instr;
    return stmt;
}

stmt_t *new_data_stmt(data_t *data) {
    stmt_t *stmt = malloc(sizeof(stmt_t));
    if (stmt == NULL) {
        error("Failed to allocate memory for data statement", ERROR_ALLOC);
    }
    stmt->kind = STMT_DATA;
    stmt->data = data;
    return stmt;
}

void free_stmt(stmt_t *stmt) {
    if (stmt->kind == STMT_LABEL) {
        free_label(stmt->label);
    } else if (stmt->kind == STMT_EXTERN) {
        free_extern(stmt->extern_);
    }
    free(stmt);
}

type_t* new_simple_type(type_kind_t kind) {
    type_t *type = malloc(sizeof(type_t));
    if (type == NULL) {
        error("Failed to allocate memory for simple type", ERROR_ALLOC);
    }
    type->kind = kind;
    return type;
}

type_t* new_array_type(type_t* base, int array_size) {
    type_t *type = malloc(sizeof(type_t));
    if (type == NULL) {
        error("Failed to allocate memory for array type", ERROR_ALLOC);
    }
    type->kind = TYPE_ARRAY;
    type->base = base;
    type->array_size = array_size;
    return type;
}

void free_type(type_t* type) {
    if (type->kind == TYPE_ARRAY) {
        free_type(type->base);
    }
    free(type);
}

data_t* new_data(char* name, type_t* type, expr_list_t* value) {
    data_t *data = malloc(sizeof(data_t));
    if (data == NULL) {
        error("Failed to allocate memory for data", ERROR_ALLOC);
    }
    data->name = name;
    data->type = type;
    data->values = value;
    return data;
}

data_t* new_data_uninitialized(char* name, type_t* type) {
    data_t *data = malloc(sizeof(data_t));
    if (data == NULL) {
        error("Failed to allocate memory for uninitialized data", ERROR_ALLOC);
    }
    data->name = name;
    data->type = type;
    data->values = NULL;
    return data;
}

void free_data(data_t* data) {
    free(data->name);
    free_type(data->type);
    if (data->values != NULL) {
        free_expr_list(data->values);
    }
    free(data);
}

extern_t* new_extern(call_abi_t* abi, char* name, attribute_list_t *attributes) {
    extern_t *extern_ = malloc(sizeof(extern_t));
    if (extern_ == NULL) {
        error("Failed to allocate memory for extern", ERROR_ALLOC);
    }
    extern_->abi = abi;
    extern_->name = name;
    extern_->attributes = attributes;
    return extern_;
}

void free_extern(extern_t *extern_) {
    free(extern_->name);
    free_call_abi(extern_->abi);
    free(extern_);
}

label_t *new_label(char *name, call_abi_t *abi, instr_list_t *instrs, attribute_list_t* attributes) {
    label_t *label = malloc(sizeof(label_t));
    if (label == NULL) {
        error("Failed to allocate memory for label", ERROR_ALLOC);
    }
    label->name = name;
    label->abi = abi;
    label->instrs = instrs;
    label->attributes = attributes;
    return label;
}

void free_label(label_t *label) {
    free(label->name);
    free_instr_list(label->instrs);
    free_call_abi(label->abi);
    free(label);
}

instr_list_t * new_instr_list() {
    instr_list_t *list = malloc(sizeof(instr_list_t));
    if (list == NULL) {
        error("Failed to allocate memory for instruction list", ERROR_ALLOC);
    }
    list->len = 0;
    list->capacity = 4;
    list->instrs = malloc(sizeof(instr_t) * list->capacity);
    if (list->instrs == NULL) {
        error("Failed to allocate memory for instruction list", ERROR_ALLOC);
    }
    return list;
}

void append_instr(instr_list_t *list, instr_t *instr) {
    if (list->len == list->capacity) {
        list->capacity *= 2;
        list->instrs = realloc(list->instrs, sizeof(instr_t) * list->capacity);
        if (list->instrs == NULL) {
            error("Failed to reallocate memory for instruction list", ERROR_ALLOC);
        }
    }
    list->instrs[list->len++] = *instr;
}

instr_list_t** split_instr_list(instr_list_t *list, int index) {
    instr_list_t **lists = malloc(sizeof(instr_list_t) * 2);
    if (lists == NULL) {
        error("Failed to allocate memory for instruction list", ERROR_ALLOC);
    }
    lists[0] = new_instr_list();
    lists[1] = new_instr_list();
    for (int i = 0; i < list->len; i++) {
        if (i < index) {
            append_instr(lists[0], &list->instrs[i]);
        } else {
            append_instr(lists[1], &list->instrs[i]);
        }
    }
    return lists;
}

instr_t *get_instr(instr_list_t *list, int index) {
    if (index < 0 || index >= list->len) {
        error("Index out of bounds", ERROR_INVALID);
    }
    return &list->instrs[index];
}

void free_instr_list(instr_list_t *list) {
    for (int i = 0; i < list->len; i++) {
        free_instr(&list->instrs[i]);
    }
    free(list->instrs);
    free(list);
}

instr_t *new_if_instr(instr_if_t *instr_if) {
    instr_t *instr = malloc(sizeof(instr_t));
    if (instr == NULL) {
        error("Failed to allocate memory for if instruction", ERROR_ALLOC);
    }
    instr->kind = INSTR_IF;
    instr->instr_if = instr_if;
    return instr;
}

instr_t *new_asm_instr(instr_asm_t *asm_instr) {
    instr_t *instr = malloc(sizeof(instr_t));
    if (instr == NULL) {
        error("Failed to allocate memory for asm instruction", ERROR_ALLOC);
    }
    instr->kind = INSTR_ASM;
    instr->instr_asm = asm_instr;
    return instr;
}

instr_t *new_call_instr(instr_call_t *instr_call) {
    instr_t *instr = malloc(sizeof(instr_t));
    if (instr == NULL) {
        error("Failed to allocate memory for call instruction", ERROR_ALLOC);
    }
    instr->kind = INSTR_CALL;
    instr->instr_call = instr_call;
    return instr;
}

void free_instr(instr_t *instr) {
    if (instr->kind == INSTR_IF) {
        free_instr_if(instr->instr_if);
    } else {
        free_instr_asm(instr->instr_asm);
    }
    free(instr);
}

instr_call_t *new_instr_call(char *callee, expr_list_t *args) {
    instr_call_t *instr = malloc(sizeof(instr_call_t));
    if (instr == NULL) {
        error("Failed to allocate memory for call instruction", ERROR_ALLOC);
    }
    instr->callee = callee;
    instr->args = args;
    return instr;
}

void free_instr_call(instr_call_t *instr_call) {
    free_expr_list(instr_call->args);
    free(instr_call);
}

instr_asm_t *new_instr_asm(char* name, expr_list_t *args) {
    instr_asm_t *instr = malloc(sizeof(instr_asm_t));
    if (instr == NULL) {
        error("Failed to allocate memory for instruction", ERROR_ALLOC);
    }
    instr->name = name;
    instr->args = args;
    return instr;
}

cmp_kind_t get_cmp_kind_by_name(const char *name) {
    if (strcmp(name, "eq") == 0) {
        return EQ;
    } else if (strcmp(name, "ne") == 0) {
        return NE;
    } else if (strcmp(name, "lt") == 0) {
        return LT;
    } else if (strcmp(name, "le") == 0) {
        return LE;
    } else if (strcmp(name, "gt") == 0) {
        return GT;
    } else if (strcmp(name, "ge") == 0) {
        return GE;
    } else {
        error("Invalid comparison kind", ERROR_INVALID);
    }
}

instr_if_t* new_instr_if(expr_list_t* condition, cmp_kind_t cmp, instr_list_t* then_instrs, instr_list_t* else_instrs) {
    instr_if_t *instr_if = malloc(sizeof(instr_if_t));
    if (instr_if == NULL) {
        error("Failed to allocate memory for if instruction", ERROR_ALLOC);
    }
    instr_if->condition = condition;
    instr_if->cmp = cmp;
    instr_if->then_instrs = then_instrs;
    instr_if->else_instrs = else_instrs;
    return instr_if;
}

void free_instr_if(instr_if_t *instr_if) {
    free_expr_list(instr_if->condition);
    free_instr_list(instr_if->then_instrs);
    free_instr_list(instr_if->else_instrs);
    free(instr_if);
}

void free_instr_asm(instr_asm_t *instr) {
    free_expr_list(instr->args);
    free(instr);
}

register_kind_list_t *new_register_kind_list() {
    register_kind_list_t *list = malloc(sizeof(register_kind_list_t));
    if (list == NULL) {
        error("Failed to allocate memory for register kind list", ERROR_ALLOC);
    }
    list->len = 0;
    list->capacity = 4;
    list->registers = malloc(sizeof(register_kind_t) * list->capacity);
    if (list->registers == NULL) {
        error("Failed to allocate memory for register kind list", ERROR_ALLOC);
    }
    return list;
}

void append_register_kind(register_kind_list_t *list, register_kind_t register_) {
    if (list->len == list->capacity) {
        list->capacity *= 2;
        list->registers = realloc(list->registers, sizeof(register_kind_t) * list->capacity);
        if (list->registers == NULL) {
            error("Failed to reallocate memory for register kind list", ERROR_ALLOC);
        }
    }
    list->registers[list->len++] = register_;
}

register_kind_t get_register_kind(register_kind_list_t *list, int index) {
    if (index < 0 || index >= list->len) {
        error("Index out of bounds", ERROR_INVALID);
    }
    return list->registers[index];
}

void free_register_kind_list(register_kind_list_t *list) {
    free(list->registers);
    free(list);
}

int find(char** element, int len, char *elem) {
    for (int i = 0; i < len; i++) {
        if (strcmp(element[i], elem) == 0) {
            return i;
        }
    }
    return -1;
}

char** registers = (char*[]) {
        "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "rbp", "rsp", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", "rip",
        "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d", "eip",
        "ax", "bx", "cx", "dx", "si", "di", "bp", "sp", "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w", "ip",
        "al", "bl", "cl", "dl", "sil", "dil", "bpl", "spl", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b"
};

register_kind_t get_register_kind_by_name(char *str) {
    int index = find(registers, 67, str);
    if (index == -1) {
        error("Invalid register", ERROR_INVALID);
    }
    return index;
}

char* register_kind_to_string(register_kind_t kind) {
    return registers[kind];
}

bool is_64_bit(register_kind_t register_) {
    return register_ < EAX;
}

bool is_32_bit(register_kind_t register_) {
    return register_ >= EAX && register_ < AX;
}

bool is_16_bit(register_kind_t register_) {
    return register_ >= AX && register_ < AL;
}

bool is_8_bit(register_kind_t register_) {
    return register_ >= AL;
}

expr_list_t *new_expr_list() {
    expr_list_t *list = malloc(sizeof(expr_list_t));
    if (list == NULL) {
        error("Failed to allocate memory for expression list", ERROR_ALLOC);
    }
    list->len = 0;
    list->capacity = 4;
    list->exprs = malloc(sizeof(expr_t) * list->capacity);
    if (list->exprs == NULL) {
        error("Failed to allocate memory for expression list", ERROR_ALLOC);
    }
    return list;
}

void append_expr(expr_list_t *list, expr_t *expr) {
    if (list->len == list->capacity) {
        list->capacity *= 2;
        list->exprs = realloc(list->exprs, sizeof(expr_t) * list->capacity);
        if (list->exprs == NULL) {
            error("Failed to reallocate memory for expression list", ERROR_ALLOC);
        }
    }
    list->exprs[list->len++] = *expr;
}

expr_t *get_expr(expr_list_t *list, int index) {
    if (index < 0 || index >= list->len) {
        error("Index out of bounds", ERROR_INVALID);
    }
    return &list->exprs[index];
}

void free_expr_list(expr_list_t *list) {
    for (int i = 0; i < list->len; i++) {
        free_expr(&list->exprs[i]);
    }
    free(list->exprs);
    free(list);
}

expr_t* new_expr_immediate(int64_t immediate) {
    expr_t *expr = (expr_t *)malloc(sizeof(expr_t));
    if (expr == NULL) {
        error("Failed to allocate memory for immediate expression", ERROR_ALLOC);
    }
    expr->kind = IMMEDIATE;
    expr->immediate = immediate;
    return expr;
}

expr_t* new_expr_register(register_kind_t register_) {
    expr_t *expr = malloc(sizeof(expr_t));
    if (expr == NULL) {
        error("Failed to allocate memory for register expression", ERROR_ALLOC);
    }
    expr->kind = REGISTER;
    expr->register_ = register_;
    return expr;
}

expr_t* new_expr_string(char* string) {
    expr_t *expr = malloc(sizeof(expr_t));
    if (expr == NULL) {
        error("Failed to allocate memory for string expression", ERROR_ALLOC);
    }
    expr->kind = STRING;
    expr->string = string;
    return expr;
}

expr_t* new_expr_memory(register_kind_t base, register_kind_t index, int64_t scale, int64_t displacement) {
    expr_t *expr = malloc(sizeof(expr_t));
    if (expr == NULL) {
        error("Failed to allocate memory for memory expression", ERROR_ALLOC);
    }
    expr->kind = MEMORY;
    expr->memory.base = base;
    expr->memory.index = index;
    expr->memory.scale = scale;
    expr->memory.displacement = displacement;
    return expr;
}

expr_t* new_expr_label(char* label) {
    expr_t* expr = malloc(sizeof(expr_t));
    if (expr == NULL) {
        error("Failed to allocate memory for label expression", ERROR_ALLOC);
    }
    expr->kind = LABEL;
    expr->label = label;
    return expr;
}

int64_t get_immediate(expr_t* expr) {
    if (expr->kind != IMMEDIATE) {
        error("Expression is not an immediate", ERROR_INVALID);
    }
    return expr->immediate;
}

register_kind_t get_register(expr_t* expr) {
    if (expr->kind != REGISTER) {
        error("Expression is not a register", ERROR_INVALID);
    }
    return expr->register_;
}

register_kind_t get_base(expr_t* expr) {
    if (expr->kind != MEMORY) {
        error("Expression is not a memory", ERROR_INVALID);
    }
    return expr->memory.base;
}

register_kind_t get_index(expr_t* expr) {
    if (expr->kind != MEMORY) {
        error("Expression is not a memory", ERROR_INVALID);
    }
    return expr->memory.index;
}

int64_t get_scale(expr_t* expr) {
    if (expr->kind != MEMORY) {
        error("Expression is not a memory", ERROR_INVALID);
    }
    return expr->memory.scale;
}

int64_t get_displacement(expr_t* expr) {
    if (expr->kind != MEMORY) {
        error("Expression is not a memory", ERROR_INVALID);
    }
    return expr->memory.displacement;
}

char* get_label(expr_t* expr) {
    if (expr->kind != LABEL) {
        error("Expression is not a label", ERROR_INVALID);
    }
    return expr->label;
}

void free_expr(expr_t* expr) {
    if (expr->kind == LABEL) {
        free(expr->label);
    }
    free(expr);
}

#pragma clang diagnostic pop