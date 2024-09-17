#ifndef ASMPP_LEXER_H
#define ASMPP_LEXER_H

#include <stddef.h>
#include <stdlib.h>

typedef enum token_kind_t token_kind_t;
typedef struct token_t token_t;
typedef struct token_list_t token_list_t;
typedef struct lexer_t lexer_t;


enum token_kind_t {
    TOKEN_EOF,
    TOKEN_NEWLINE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,
    TOKEN_ASSIGN,
    TOKEN_IDENT,
    TOKEN_NUMBER,
    TOKEN_STRING,

};

struct token_t {
    token_kind_t kind;
    char* lexeme;
    size_t len;
    size_t line;
    size_t col;
};


struct token_list_t {
    int len;
    int capacity;
    token_t* tokens;
};

token_list_t* new_token_list();
void append_token(token_list_t* list, token_t* token);
token_t *get_token(token_list_t* list, int index);
char* display_token(token_t* token);
void free_token_list(token_list_t* list);





token_t* new_token(token_kind_t kind, char* lexeme, size_t len, size_t line, size_t col);
void free_token(token_t* token);

struct lexer_t {
    char* source;
    size_t len;
    size_t pos;
    size_t line;
    size_t col;
    token_list_t* tokens;
};

lexer_t* new_lexer(char* source);
char lexer_peek(lexer_t* lexer);
char lexer_advance(lexer_t* lexer, int n);
void lexer_skip_whitespace(lexer_t* lexer);
void lexer_skip_comment(lexer_t* lexer);
void lexer_append_token(lexer_t* lexer, token_kind_t kind, size_t len);
void lexer_lex(lexer_t* lexer);
void free_lexer(lexer_t* lexer);

int count_string(char* source, char* elt);


#endif //ASMPP_LEXER_H
