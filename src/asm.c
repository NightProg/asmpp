#include "asm.h"
#include "util.h"
#include <stdlib.h>
#include "log.h"
#include "fs.h"
#include <string.h>
#include <errno.h>

asm_section_bss_t *section_bss_new() {
    asm_section_bss_t *section = malloc(sizeof(asm_section_bss_t));
    if (section == NULL) {
        return NULL;
    }
    section->bss = malloc(sizeof(asm_data_t) * 10);
    if (section->bss == NULL) {
        free(section);
        return NULL;
    }
    section->size = 0;
    section->capacity = 10;
    return section;
}

int section_bss_add_bss(asm_section_bss_t *section, asm_bss_t *bss) {
    if (section->size >= section->capacity) {
        asm_bss_t *new_bss = realloc(section->bss, sizeof(asm_bss_t) * section->capacity * 2);
        if (new_bss == NULL) {
            return -1;
        }
        section->bss = new_bss;
        section->capacity *= 2;
    }
    section->bss[section->size++] = *bss;
    return 0;
}

void section_bss_free(asm_section_bss_t *section) {
    free(section->bss);
    free(section);
}

asm_data_t *data_new(char *name, asm_size_t size, string_list_t *values) {
    asm_data_t *data = malloc(sizeof(asm_data_t));
    asm_size_t *size_ = malloc(sizeof(asm_size_t));
    if (data == NULL || size_ == NULL) {
        return NULL;
    }
    *size_ = size;
    data->name = name;
    data->size = size_;
    data->values = values;
    return data;
}

void data_free(asm_data_t *data) {
    free(data->name);
    free(data->size);
    free_string_list(data->values);
    free(data);
}

asm_bss_t *bss_new(char *name, asm_bss_size_t size) {
    asm_bss_t *bss = malloc(sizeof(asm_bss_t));
    if (bss == NULL) {
        return NULL;
    }
    bss->name = name;
    bss->size = size;
    return bss;
}

void bss_free(asm_bss_t *bss) {
    free(bss->name);
    free(bss);
}

char *size_to_string(asm_size_t size) {
    switch (size) {
        case BYTE:
            return "db";
        case WORD:
            return "dw";
        case DWORD:
            return "dd";
        case QWORD:
            return "dq";
    }
}

char *bss_size_to_string(asm_bss_size_t size) {
    string_buffer_t *buffer = new_string_buffer();
    if (buffer == NULL) {
        return NULL;
    }
    switch (size.size) {
        case BYTE:
            string_buffer_printf(buffer, "resb %d", size.n);
            break;
        case WORD:
            string_buffer_printf(buffer, "resw %d", size.n);
            break;
        case DWORD:
            string_buffer_printf(buffer, "resd %d", size.n);
            break;
        case QWORD:
            string_buffer_printf(buffer, "resq %d", size.n);
            break;
    }
    return buffer->data;
}


asm_bss_size_t *bss_size_new(int n, asm_size_t size) {
    asm_bss_size_t *bss_size = malloc(sizeof(asm_bss_size_t));
    if (bss_size == NULL) {
        return NULL;
    }
    bss_size->n = n;
    bss_size->size = size;
    return bss_size;
}

asm_section_data_t *section_data_new() {
    asm_section_data_t *section = malloc(sizeof(asm_section_data_t));
    if (section == NULL) {
        return NULL;
    }
    section->data = malloc(sizeof(asm_data_t) * 10);
    if (section->data == NULL) {
        free(section);
        return NULL;
    }
    section->size = 0;
    section->capacity = 10;
    return section;
}

int section_data_add_data(asm_section_data_t *asm_section_data, asm_data_t *data) {
    if (asm_section_data->size >= asm_section_data->capacity) {
        asm_data_t *new_data = realloc(asm_section_data->data, sizeof(asm_data_t) * asm_section_data->capacity * 2);
        if (new_data == NULL) {
            return -1;
        }
        asm_section_data->data = new_data;
        asm_section_data->capacity *= 2;
    }
    asm_section_data->data[asm_section_data->size++] = *data;
    return 0;
}

void section_data_free(asm_section_data_t *section) {
    free(section->data);
    free(section);
}

asm_instruction_t *instruction_new(asm_instruction_type_t type, char* name) {
    asm_instruction_t *instruction = malloc(sizeof(asm_instruction_t));
    if (instruction == NULL) {
        return NULL;
    }
    instruction->type = type;
    instruction->args = malloc(sizeof(char *) * 10);
    if (instruction->args == NULL) {
        free(instruction);
        return NULL;
    }
    instruction->args[0] = name;
    instruction->arg_size = 1;
    instruction->arg_capacity = 10;
    if (type == ASM_LABEL) {
        instruction->list = malloc(sizeof(asm_instruction_t) * 10);
        if (instruction->list == NULL) {
            free(instruction->args);
            free(instruction);
            return NULL;
        }
        instruction->instr_size = 0;
        instruction->instr_capacity = 10;
    }
    return instruction;
}


int instruction_add_arg(asm_instruction_t *instruction, char *arg) {
    if (instruction->arg_size >= instruction->arg_capacity) {
        char **new_args = realloc(instruction->args, sizeof(char *) * instruction->arg_capacity * 2);
        if (new_args == NULL) {
            return -1;
        }
        instruction->args = new_args;
        instruction->arg_capacity *= 2;
    }
    instruction->args[instruction->arg_size++] = arg;
    return 0;
}

int instruction_add_instr(asm_instruction_t *instruction, asm_instruction_t *instr) {
    if (instruction->type != ASM_LABEL) {
        return -1;
    }
    if (instruction->instr_size >= instruction->instr_capacity) {
        asm_instruction_t *new_instrs = realloc(instruction->list, sizeof(asm_instruction_t) * instruction->instr_capacity * 2);
        if (new_instrs == NULL) {
            return -1;
        }
        instruction->list = new_instrs;
        instruction->instr_capacity *= 2;
    }
    instruction->list[instruction->instr_size++] = *instr;
    return 0;
}

asm_instruction_t *copy_instruction(asm_instruction_t *instruction) {
    asm_instruction_t *new_instruction = malloc(sizeof(asm_instruction_t));
    if (new_instruction == NULL) {
        return NULL;
    }
    new_instruction->type = instruction->type;
    new_instruction->args = malloc(sizeof(char *) * instruction->arg_capacity);
    if (new_instruction->args == NULL) {
        free(new_instruction);
        return NULL;
    }
    for (int i = 0; i < instruction->arg_size; i++) {
        new_instruction->args[i] = instruction->args[i];
    }
    new_instruction->arg_size = instruction->arg_size;
    new_instruction->arg_capacity = instruction->arg_capacity;
    if (instruction->type == ASM_LABEL) {
        new_instruction->list = malloc(sizeof(asm_instruction_t) * instruction->instr_capacity);
        if (new_instruction->list == NULL) {
            free(new_instruction->args);
            free(new_instruction);
            return NULL;
        }
        for (int i = 0; i < instruction->instr_size; i++) {
            new_instruction->list[i] = *instruction->list;
        }
        new_instruction->instr_size = instruction->instr_size;
        new_instruction->instr_capacity = instruction->instr_capacity;
    }
    return new_instruction;
}

void instruction_free(asm_instruction_t *instruction) {
    free(instruction->args);
    if (instruction->type == ASM_LABEL) {
        for (int i = 0; i < instruction->instr_size; i++) {
            instruction_free(&instruction->list[i]);
        }
        free(instruction->list);
    }
    free(instruction);
}

asm_section_text_t *section_text_new() {
    asm_section_text_t *section = malloc(sizeof(asm_section_text_t));
    if (section == NULL) {
        return NULL;
    }
    section->instructions = malloc(sizeof(asm_instruction_t) * 10);
    if (section->instructions == NULL) {
        free(section);
        return NULL;
    }
    section->size = 0;
    section->capacity = 10;
    return section;
}

int section_text_add_instruction(asm_section_text_t *section, asm_instruction_t *instruction) {
    if (section->size >= section->capacity) {
        asm_instruction_t *new_instructions = realloc(section->instructions, sizeof(asm_instruction_t) * section->capacity * 2);
        if (new_instructions == NULL) {
            return -1;
        }
        section->instructions = new_instructions;
        section->capacity *= 2;
    }
    section->instructions[section->size++] = *instruction;
    return 0;
}

void section_text_free(asm_section_text_t *section) {
    free(section->instructions);
    free(section);
}

asm_t *asm_new() {
    asm_t *asm_ = malloc(sizeof(asm_t));
    asm_->text = section_text_new();
    asm_->data = section_data_new();
    asm_->bss = section_bss_new();

    if (asm_->text == NULL || asm_->data == NULL || asm_->bss == NULL) {
        free(asm_);
        return NULL;
    }

    return asm_;
}

int asm_set_text_section(asm_t* asm_, asm_section_text_t* section) {
    asm_->text = section;
    return 0;
}

int asm_set_data_section(asm_t* asm_, asm_section_data_t* section) {
    asm_->data = section;
    return 0;
}

int asm_set_bss_section(asm_t* asm_, asm_section_bss_t* section) {
    asm_->bss = section;
    return 0;
}

int asm_compile(asm_t* asm_, config_t* config, char* output_name) {
    if (fs_mkdir(config->output_dir) != 0) {
        if (errno != EEXIST) {
            log_(LOG_ERROR, "Could not create output directory: %s", config->output_dir);
            return -1;
        }

    }

    char *file = malloc(strlen(config->output_dir) + strlen(output_name) + 2);
    if (file == NULL) {
        return -1;
    }
    snprintf(file, strlen(config->output_dir) + strlen(output_name) + 2, "%s/%s", config->output_dir, output_name);
    FILE *fp = fopen(file, "w");
    if (fp == NULL) {
        free(file);
        return -1;
    }

    char *asm_str = asm_emit(asm_);
    if (asm_str == NULL) {
        free(file);
        fclose(fp);
        return -1;
    }

    fprintf(fp, "%s", asm_str);
    free(asm_str);
    fclose(fp);

    if (config->verbose) {
        printf("Generated file: %s\n", file);
    }
//    char* nasm = malloc(strlen(config->as) + strlen(file) + 2);
//    if (nasm == NULL) {
//        free(dir);
//        free(file);
//        return -1;
//    }
//
    return 0;
}

char* asm_emit(asm_t* asm_) {
    asm_section_data_t *data = asm_->data;
    asm_section_text_t *text = asm_->text;
    asm_section_bss_t *bss = asm_->bss;
    string_buffer_t *buffer = new_string_buffer();
    if (buffer == NULL) {
        return NULL;
    }
    string_buffer_writeln(buffer, "section .data");
    string_buffer_add_tab(buffer, 1);

    for (int i = 0; i < data->size; i++) {
        asm_data_t *data_ = &data->data[i];
        string_buffer_printf_tabbed(buffer, "%s %s", data_->name, size_to_string(*data_->size));
        for (int j = 0; j < data_->values->len; j++) {
            string_buffer_printf(buffer, " %s", get_string(data_->values, j));
            if (j < data_->values->len - 1) {
                string_buffer_write(buffer, ",");
            }
        }

        string_buffer_write(buffer, "\n");

    }

    string_buffer_remove_tab(buffer, 1);
    string_buffer_writeln(buffer, "section .bss");
    string_buffer_add_tab(buffer, 1);

    for (int i = 0; i < bss->size; i++) {
        asm_bss_t *bss_ = &bss->bss[i];
        string_buffer_printf_tabbed(buffer, "%s %s\n", bss_->name, bss_size_to_string(bss_->size));
    }

    string_buffer_remove_tab(buffer, 1);
    string_buffer_writeln(buffer, "section .text");

    for (int i = 0; i < text->size; i++) {
        asm_instruction_t *instruction = &text->instructions[i];
        switch (instruction->type) {
            case ASM_LABEL:
                string_buffer_printf(buffer, "%s:\n", instruction->args[0]);
                string_buffer_add_tab(buffer, 1);
                for (int j = 0; j < instruction->instr_size; j++) {
                    asm_instruction_t *instr = &instruction->list[j];
                    string_buffer_printf_tabbed(buffer, "%s", instr->args[0]);
                    for (int k = 1; k < instr->arg_size; k++) {
                        if (k == 1) {
                            string_buffer_printf(buffer, " %s", instr->args[k]);
                        } else {
                            string_buffer_printf(buffer, ", %s", instr->args[k]);
                        }
                    }
                    string_buffer_write(buffer, "\n");
                }
                string_buffer_remove_tab(buffer, 1);
                break;
            case ASM_INSTR:
                string_buffer_printf(buffer, "%s", instruction->args[0]);
                for (int j = 1; j < instruction->arg_size; j++) {
                    if (j == 1) {
                        string_buffer_printf(buffer, " %s", instruction->args[j]);
                    } else {
                        string_buffer_printf(buffer, ", %s", instruction->args[j]);
                    }
                }
                string_buffer_printf(buffer, "\n");
                break;
        }
    }

    return buffer->data;
}

void asm_free(asm_t *asm_) {
    free(asm_);
}