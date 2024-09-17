#ifndef ASMPP_UTIL_H
#define ASMPP_UTIL_H


typedef struct {
    char* data;
    int size;
    int capacity;
    int tab_size;
    int tab_count;
} string_buffer_t;

string_buffer_t* new_string_buffer();
void string_buffer_writeln(string_buffer_t* buffer, char* str);
void string_buffer_write(string_buffer_t* buffer, char* str);
void string_buffer_write_tabbed(string_buffer_t* buffer, char* str);
void string_buffer_writeln_tabbed(string_buffer_t* buffer, char* str);
void string_buffer_printf(string_buffer_t* buffer, const char* format, ...);
void string_buffer_printf_tabbed(string_buffer_t* buffer, const char* format, ...);
void string_buffer_add_tab(string_buffer_t* buffer, int n);
void string_buffer_remove_tab(string_buffer_t* buffer, int n);
void free_string_buffer(string_buffer_t* buffer);

typedef struct {
    int len;
    int capacity;
    char** strings;
} string_list_t;

string_list_t* new_string_list();
void append_string(string_list_t* list, char* string);
char* get_string(string_list_t* list, int index);
int find_string(string_list_t* list, char* string);
void free_string_list(string_list_t* list);

#endif //ASMPP_UTIL_H
