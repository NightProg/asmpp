//
// Created by antoine barbier on 12/09/2024.
//

#include "codegen.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "string.h"

call_abi_t *get_c_call_abi() {
    argument_list_t *args = new_argument_list();
    append_argument(args, new_argument_register(RAX));
    append_argument(args, new_argument_register(RDI));
    append_argument(args, new_argument_register(RSI));
    append_argument(args, new_argument_register(RDX));
    append_argument(args, new_argument_register(RCX));
    append_argument(args, new_argument_register(R8));
    append_argument(args, new_argument_register(R9));
    append_argument(args, new_argument_stack());
    return new_call_abi("C", args);
}

char* cmp_kind_to_jmp(cmp_kind_t kind) {
    char* jmp[] = {
            "je",
            "jne",
            "jl",
            "jle",
            "jg",
            "jge",
    };

    return jmp[kind];
}


label_hashtable_t *new_label_hashtable() {
    label_hashtable_t *table = malloc(sizeof(label_hashtable_t));
    if (table == NULL) {
        return NULL;
    }
    table->entries = malloc(sizeof(label_hashtable_entry_t) * 10);
    if (table->entries == NULL) {
        free(table);
        return NULL;
    }
    table->size = 0;
    table->capacity = 10;
    return table;
}

void label_hashtable_insert(label_hashtable_t *table, char *name, label_t *label) {
    if (table->size >= table->capacity) {
        label_hashtable_entry_t *new_entries = realloc(table->entries, sizeof(label_hashtable_entry_t) * table->capacity * 2);
        if (new_entries == NULL) {
            return;
        }
        table->entries = new_entries;
        table->capacity *= 2;
    }
    table->entries[table->size].name = name;
    table->entries[table->size].label = label;
    table->size++;
}

label_t *label_hashtable_get(label_hashtable_t *table, char *name) {
    for (int i = 0; i < table->size; i++) {
        if (strcmp(table->entries[i].name, name) == 0) {
            return table->entries[i].label;
        }
    }
    return NULL;
}

void free_label_hashtable(label_hashtable_t *table) {
    free(table->entries);
    free(table);
}


codegen_t *new_codegen(stmt_list_t *stmts) {
    codegen_t *codegen = malloc(sizeof(codegen_t));
    if (codegen == NULL) {
        return NULL;
    }
    codegen->stmts = stmts;
    codegen->labels = new_label_hashtable();
    codegen->asm_ = asm_new();
    return codegen;
}

void codegen_insert_instruction(codegen_t *codegen, asm_instruction_t *instruction) {
    if (codegen->entry_point == CODEGEN_TEXT) {
        section_text_add_instruction(codegen->asm_->text, instruction);
    } else {
        instruction_add_instr(codegen->current_label, instruction);
    }
}

char* codegen_expr(codegen_t *codegen, expr_t *expr) {
    switch (expr->kind) {
        case IMMEDIATE: {
            char *imm = malloc(100);
            sprintf(imm, "%lld", expr->immediate);
            return imm;
        }
        case REGISTER:
            return register_kind_to_string(expr->register_);
        case MEMORY:
            assert(0);
        case LABEL:
            return expr->label;
        case STRING: {
            string_buffer_t *buffer = new_string_buffer();
            string_buffer_printf(buffer, "\"%s\"", expr->string);
            return buffer->data;
        }
    }
}

char** codegen_list_expr(codegen_t *codegen, expr_list_t *list) {
    char **args = malloc(sizeof(char *) * list->len);
    if (args == NULL) {
        return NULL;
    }
    for (int i = 0; i < list->len; i++) {
        args[i] = codegen_expr(codegen, get_expr(list, i));
    }
    return args;
}

void codegen_instr(codegen_t *codegen, instr_t *instr) {
    switch (instr->kind) {
        case INSTR_IF: {
            char *arg0 = codegen_expr(codegen, get_expr(instr->instr_if->condition, 0));
            char *arg1 = codegen_expr(codegen, get_expr(instr->instr_if->condition, 1));

            char *op = cmp_kind_to_jmp(instr->instr_if->cmp);
            asm_instruction_t *cmp = instruction_new(ASM_INSTR, "cmp");
            instruction_add_arg(cmp, arg0);
            instruction_add_arg(cmp, arg1);
            codegen_insert_instruction(codegen, cmp);

            asm_instruction_t *jmp = instruction_new(ASM_INSTR, op);
            char* label_then_name = malloc(100);
            snprintf(label_then_name, 100, ".L%d", codegen->count++);
            instruction_add_arg(jmp, label_then_name);
            codegen_insert_instruction(codegen, jmp);

            asm_instruction_t *jmp_else = instruction_new(ASM_INSTR, "jmp");
            char* label_else_name = malloc(100);
            snprintf(label_else_name, 100, ".L%d", codegen->count++);
            instruction_add_arg(jmp_else, label_else_name);
            codegen_insert_instruction(codegen, jmp_else);
            instr_list_t* then_instr = instr->instr_if->then_instrs;
            instr_list_t* else_instr = instr->instr_if->else_instrs;
            expr_list_t *args = new_expr_list();
            char* label_after_name = malloc(100);
            snprintf(label_after_name, 100, ".L%d", codegen->count++);
            append_expr(args, new_expr_label(label_after_name));
            instr_t* jmp_instr = new_asm_instr(
                    new_instr_asm("jmp", args)
                    );
            append_instr(then_instr, jmp_instr);
            append_instr(else_instr, jmp_instr);
            label_t *label_then = new_label(label_then_name, new_call_abi("default", new_argument_list()), then_instr, new_attribute_list());
            label_t *label_else = new_label(label_else_name, new_call_abi("default", new_argument_list()), else_instr, new_attribute_list());
            codegen_label(codegen, label_then);
            codegen_label(codegen, label_else);
            asm_instruction_t *lab_after = instruction_new(ASM_LABEL, label_after_name);
            codegen->current_label = lab_after;
            codegen->entry_point = CODEGEN_LABEL;
            codegen->should_add_label = 1;
            break;
        }
        case INSTR_ASM: {
            asm_instruction_t *asm_instr = instruction_new(ASM_INSTR, instr->instr_asm->name);
            char **args = codegen_list_expr(codegen, instr->instr_asm->args);
            for (int i = 0; i < instr->instr_asm->args->len; i++) {
                instruction_add_arg(asm_instr, args[i]);
            }

            codegen_insert_instruction(codegen, asm_instr);
            break;
        }
        case INSTR_CALL: {
            label_t *callee = label_hashtable_get(codegen->labels, instr->instr_call->callee);
            if (callee == NULL) {
                assert(0 && "Implement error message");
            }
            argument_list_t *args = callee->abi->args;
            int arg_count = 0;
            for (int i = 0; i < instr->instr_call->args->len; i++) {
                argument_t* arg = get_argument(args, i);
                if (arg->kind == ARGUMENT_REGISTER) {
                    if (arg_count >= callee->abi->args->len) {
                        assert(0 && "Implement error message");
                    }
                    char* expr = codegen_expr(codegen, get_expr(instr->instr_call->args, i));
                    arg_count++;
                    asm_instruction_t *mov = instruction_new(ASM_INSTR, "mov");
                    instruction_add_arg(mov, register_kind_to_string(arg->reg));
                    instruction_add_arg(mov, expr);
                    codegen_insert_instruction(codegen, mov);
                } else if (arg->kind == ARGUMENT_STACK) {
                    for (int j = arg_count; j < instr->instr_call->args->len; j++) {
                        char* expr = codegen_expr(codegen, get_expr(instr->instr_call->args, j));
                        asm_instruction_t *push = instruction_new(ASM_INSTR, "push");
                        instruction_add_arg(push, expr);
                        codegen_insert_instruction(codegen, push);
                    }
                    break;
                }
            }
            asm_instruction_t *call = instruction_new(ASM_INSTR, "call");
            instruction_add_arg(call, callee->name);
            codegen_insert_instruction(codegen, call);
            break;
        }
    }
}

void codegen_label(codegen_t *codegen, label_t *label) {
    asm_instruction_t *l = instruction_new(ASM_LABEL, label->name);
    call_abi_t *abi = label->abi;
    if (has_attribute(label->attributes, "abi")) {
        int index = find_attribute(label->attributes, "abi");
        attribute_t *abi_attr = get_attribute(label->attributes, index);
        if (has_argument(abi_attr, "C") > 0) {
            abi = get_c_call_abi();
        } else if (has_argument(abi_attr, "stack") > 0) {
            abi = new_call_abi("stack", abi->args);
            append_argument(abi->args, new_argument_stack());
        }
    }
    label->abi = abi;
    label_hashtable_insert(codegen->labels, label->name, label);
    codegen_entry_point_t entry_point = codegen->entry_point;
    asm_instruction_t *saved_label = NULL;
    if (codegen->entry_point == CODEGEN_LABEL) {
        saved_label = codegen->current_label;
    }
    if (codegen->should_add_label) {
        section_text_add_instruction(codegen->asm_->text, codegen->current_label);
        codegen->should_add_label = 0;
    }
    codegen->entry_point = CODEGEN_LABEL;
    codegen->current_label = l;
    for (int i = 0; i < label->instrs->len; i++) {
        instr_t *instr = get_instr(label->instrs, i);
        codegen_instr(codegen, instr);
    }


    section_text_add_instruction(codegen->asm_->text, l);
    codegen->entry_point = entry_point;
    if (codegen->entry_point == CODEGEN_LABEL) {
        codegen->current_label = saved_label;
    }

    if (codegen->should_add_label) {
        section_text_add_instruction(codegen->asm_->text, codegen->current_label);
        codegen->should_add_label = 0;
    }
}

void codegen_extern(codegen_t *codegen, extern_t *extern_) {
    asm_instruction_t *extern_instr = instruction_new(ASM_INSTR, "extern");
    instruction_add_arg(extern_instr, extern_->name);
    call_abi_t *abi = extern_->abi;
    if (has_attribute(extern_->attributes, "abi")) {
        int index = find_attribute(extern_->attributes, "abi");
        attribute_t *abi_attr = get_attribute(extern_->attributes, index);
        if (has_argument(abi_attr, "C") > 0) {
            abi = get_c_call_abi();
        } else if (has_argument(abi_attr, "stack") > 0) {
            abi = new_call_abi("stack", abi->args);
            append_argument(abi->args, new_argument_stack());
        }
    }
    label_t* l = new_label(extern_->name, abi, new_instr_list(), extern_->attributes);
    label_hashtable_insert(codegen->labels, extern_->name, l);
    codegen_insert_instruction(codegen, extern_instr);
}

void codegen_data(codegen_t *codegen, data_t *data) {
    asm_size_t size;
    int array_size = 1;

    switch (data->type->kind) {
        case TYPE_BYTE:
            size = BYTE;
            break;
        case TYPE_WORD:
            size = WORD;
            break;
        case TYPE_DWORD:
            size = DWORD;
            break;
        case TYPE_QWORD:
            size = QWORD;
            break;
        case TYPE_ARRAY: {
            size = data->type->base->kind == TYPE_BYTE ? BYTE : (data->type->base->kind == TYPE_WORD ? WORD : (data->type->base->kind == TYPE_DWORD ? DWORD : QWORD));
            array_size = data->type->array_size;
            break;
        }
        default:
            assert(0);
    }

    if (data->values) {
        string_list_t *value_list = new_string_list();
        for (int i = 0; i < data->values->len; i++) {
            append_string(value_list, codegen_expr(codegen, get_expr(data->values, i)));
        }
        asm_data_t *asm_data = data_new(data->name, size, value_list);
        section_data_add_data(codegen->asm_->data, asm_data);
    } else {
        asm_bss_t *asm_bss = bss_new(data->name, *bss_size_new(array_size, size));
        section_bss_add_bss(codegen->asm_->bss, asm_bss);
    }
}

void codegen(codegen_t *codegen) {

    for (int i = 0; i < codegen->stmts->len; i++) {
        stmt_t *stmt = get_stmt(codegen->stmts, i);
        switch (stmt->kind) {
            case STMT_INSTR:
                codegen_instr(codegen, stmt->instr);
                break;
            case STMT_LABEL:
                codegen_label(codegen, stmt->label);
                break;
            case STMT_EXTERN:
                codegen_extern(codegen, stmt->extern_);
                break;
            case STMT_DATA:
                codegen_data(codegen, stmt->data);
                break;
        }
    }

    if (codegen->should_add_label) {
        section_text_add_instruction(codegen->asm_->text, codegen->current_label);
        codegen->should_add_label = 0;
    }
}

void free_codegen(codegen_t *codegen) {
    free_stmt_list(codegen->stmts);
    asm_free(codegen->asm_);
    free(codegen);
}
