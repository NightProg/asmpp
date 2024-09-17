#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

string_buffer_t *new_string_buffer() {
    string_buffer_t *buffer = malloc(sizeof(string_buffer_t));
    if (buffer == NULL) {
        return NULL;
    }
    buffer->data = malloc(sizeof(char) * 10);
    if (buffer->data == NULL) {
        free(buffer);
        return NULL;
    }
    buffer->size = 0;
    buffer->capacity = 10;
    buffer->tab_size = 4;
    buffer->tab_count = 0;
    return buffer;
}

void string_buffer_write(string_buffer_t *buffer, char *str) {
    int len = strlen(str);
    if (buffer->size + len >= buffer->capacity) {
        char *new_data = realloc(buffer->data, sizeof(char) * buffer->capacity * 2);
        if (new_data == NULL) {
            return;
        }
        buffer->data = new_data;
        buffer->capacity *= 2;
    }
    memcpy(buffer->data + buffer->size, str, len);
    buffer->size += len;
}

void string_buffer_writeln(string_buffer_t *buffer, char *str) {
    string_buffer_write(buffer, str);
    string_buffer_write(buffer, "\n");
}

void string_buffer_write_tabbed(string_buffer_t *buffer, char *str) {
    for (int i = 0; i < buffer->tab_count; i++) {
        char* tab = malloc(sizeof(char) * buffer->tab_size);
        for (int j = 0; j < buffer->tab_size; j++) {
            tab[j] = ' ';
        }
        string_buffer_write(buffer, tab);
    }
    string_buffer_write(buffer, str);
}

void string_buffer_writeln_tabbed(string_buffer_t *buffer, char *str) {
    string_buffer_write_tabbed(buffer, str);
    string_buffer_write(buffer, "\n");
}

void string_buffer_printf(string_buffer_t *buffer, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *str;
    vasprintf(&str, format, args);
    va_end(args);
    string_buffer_write(buffer, str);
    free(str);
}

void string_buffer_printf_tabbed(string_buffer_t *buffer, const char *format, ...) {
    va_list args;
    va_start(args, format);
    char *str;
    vasprintf(&str, format, args);
    va_end(args);
    string_buffer_write_tabbed(buffer, str);
    free(str);
}

void string_buffer_add_tab(string_buffer_t *buffer, int n) {
    buffer->tab_count += n;
}

void string_buffer_remove_tab(string_buffer_t *buffer, int n) {
    buffer->tab_count -= n;
}

void free_string_buffer(string_buffer_t *buffer) {
    free(buffer->data);
    free(buffer);
}

string_list_t *new_string_list() {
    string_list_t *list = malloc(sizeof(string_list_t));
    if (list == NULL) {
        return NULL;
    }
    list->len = 0;
    list->capacity = 10;
    list->strings = malloc(sizeof(char *) * list->capacity);
    if (list->strings == NULL) {
        free(list);
        return NULL;
    }
    return list;
}

void append_string(string_list_t *list, char *string) {
    if (list->len >= list->capacity) {
        char **new_strings = realloc(list->strings, sizeof(char *) * list->capacity * 2);
        if (new_strings == NULL) {
            return;
        }
        list->strings = new_strings;
        list->capacity *= 2;
    }
    list->strings[list->len] = string;
    list->len++;
}

char *get_string(string_list_t *list, int index) {
    if (index < 0 || index >= list->len) {
        return NULL;
    }
    return list->strings[index];
}

int find_string(string_list_t *list, char *string) {
    for (int i = 0; i < list->len; i++) {
        if (strcmp(list->strings[i], string) == 0) {
            return i;
        }
    }
    return -1;
}

void free_string_list(string_list_t *list) {
    free(list->strings);
    free(list);
}