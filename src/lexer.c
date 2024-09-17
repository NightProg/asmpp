#include "lexer.h"
#include "error.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

token_t *new_token(token_kind_t kind, char *lexeme, size_t len, size_t line, size_t col) {
    token_t *token = malloc(sizeof(token_t));
    if (token == NULL) {
        error("Failed to allocate memory for token", ERROR_ALLOC);
    }
    token->kind = kind;
    token->lexeme = lexeme;
    token->len = len;
    token->line = line;
    token->col = col;
    return token;
}


void free_token(token_t *token) {
    free(token->lexeme);
    free(token);
}

token_list_t *new_token_list() {
    token_list_t *list = malloc(sizeof(token_list_t));
    if (list == NULL) {
        error("Failed to allocate memory for token list", ERROR_ALLOC);
    }
    list->len = 0;
    list->capacity = 8;
    list->tokens = malloc(sizeof(token_t) * list->capacity);
    if (list->tokens == NULL) {
        error("Failed to allocate memory for token list", ERROR_ALLOC);
    }
    return list;
}

void append_token(token_list_t *list, token_t *token) {
    if (list->len == list->capacity) {
        list->capacity *= 2;
        list->tokens = realloc(list->tokens, sizeof(token_t) * list->capacity);
        if (list->tokens == NULL) {
            error("Failed to reallocate memory for token list", ERROR_ALLOC);
        }
    }
    list->tokens[list->len++] = *token;
}

token_t *get_token(token_list_t *list, int index) {
    if (index < 0 || index >= list->len) {
        error("Index out of bounds", ERROR_INVALID);
    }
    return &list->tokens[index];
}

void free_token_list(token_list_t *list) {
    for (int i = 0; i < list->len; i++) {
        free_token(&list->tokens[i]);
    }
    free(list->tokens);
    free(list);
}

lexer_t *new_lexer(char *source) {
    lexer_t *lexer = malloc(sizeof(lexer_t));
    if (lexer == NULL) {
        error("Failed to allocate memory for lexer", ERROR_ALLOC);
    }
    lexer->source = source;
    lexer->len = strlen(source);
    lexer->pos = 0;
    lexer->line = 1;
    lexer->col = 1;
    lexer->tokens = new_token_list();
    return lexer;
}

char lexer_peek(lexer_t *lexer) {
    if (lexer->pos >= lexer->len) {
        return '\0';
    }
    return lexer->source[lexer->pos];
}

char lexer_advance(lexer_t *lexer, int n) {
    lexer->pos += n;
    if (lexer->pos >= lexer->len) {
        return '\0';
    }
    lexer->col += n;
    return lexer->source[lexer->pos];
}


void lexer_skip_comment(lexer_t *lexer) {
    if (lexer_peek(lexer) == ';') {
        while (lexer_peek(lexer) != '\n' && lexer_peek(lexer) != '\0') {
            lexer_advance(lexer, 1);
        }

    }
}

void lexer_append_token(lexer_t *lexer, token_kind_t kind, size_t len) {
    char *lexeme = malloc(len + 1);
    if (lexeme == NULL) {
        error("Failed to allocate memory for lexeme", ERROR_ALLOC);
    }
    strncpy(lexeme, &lexer->source[lexer->pos - len], len);
    lexeme[len] = '\0';
    token_t *token = new_token(kind, lexeme, len, lexer->line, lexer->col);
    append_token(lexer->tokens, token);
}

void lexer_lex(lexer_t* lexer) {
    char c;
    while((c=lexer_peek(lexer)) != '\0') {
        int pos = lexer->pos;
        switch (c) {
            case '\"':
                lexer_advance(lexer, 1);
                while (lexer_peek(lexer) != '\"' && lexer_peek(lexer) != '\0') {
                    lexer_advance(lexer, 1);
                }
                if (lexer_peek(lexer) == '\0') {
                    error("Unterminated string", ERROR_INVALID);
                }
                lexer_append_token(lexer, TOKEN_STRING, lexer->pos - pos - 1);
                lexer_advance(lexer, 1);
                break;
            case ';':
                lexer_skip_comment(lexer);
                break;
            case ',':
                lexer_advance(lexer, 1);
                lexer_append_token(lexer, TOKEN_COMMA, 1);
                break;
            case ':':
                lexer_advance(lexer, 1);
                lexer_append_token(lexer, TOKEN_COLON, 1);
                break;
            case '=':
                lexer_advance(lexer, 1);
                lexer_append_token(lexer, TOKEN_ASSIGN, 1);
                break;
            case '(':
                lexer_advance(lexer, 1);
                lexer_append_token(lexer, TOKEN_LPAREN, 1);
                break;
            case ')':
                lexer_advance(lexer, 1);
                lexer_append_token(lexer, TOKEN_RPAREN, 1);
                break;
            case '{':
                lexer_advance(lexer, 1);
                lexer_append_token(lexer, TOKEN_LBRACE, 1);
                break;
            case '}':
                lexer_advance(lexer, 1);
                lexer_append_token(lexer, TOKEN_RBRACE, 1);
                break;
            case '[':
                lexer_advance(lexer, 1);
                lexer_append_token(lexer, TOKEN_LBRACKET, 1);
                break;
            case ']':
                lexer_advance(lexer, 1);
                lexer_append_token(lexer, TOKEN_RBRACKET, 1);
                break;
            case ' ':
            case '\t':
                lexer_advance(lexer, 1);
                break;
            case '\n':
                lexer_advance(lexer, 1);
                lexer_append_token(lexer, TOKEN_NEWLINE, 1);
                lexer->line++;
                lexer->col = 1;
                break;
            default:
                if (isalpha(c)) {
                    int n = 0;
                    while (isalnum(lexer_peek(lexer)) || lexer_peek(lexer) == '_') {
                        lexer_advance(lexer, 1);
                        n++;
                    }
                    lexer_append_token(lexer, TOKEN_IDENT, n);
                } else if (isdigit(c)) {
                    while (isdigit(lexer_peek(lexer))) {
                        lexer_advance(lexer, 1);
                    }
                    lexer_append_token(lexer, TOKEN_NUMBER, lexer->pos - pos);
                } else if (c == '"') {
                    lexer_advance(lexer, 1);
                    while (lexer_peek(lexer) != '"' && lexer_peek(lexer) != '\0') {
                        lexer_advance(lexer, 1);
                    }

                    if (lexer_peek(lexer) == '\0') {
                        error("Unterminated string", ERROR_INVALID);
                    }
                    lexer_append_token(lexer, TOKEN_STRING, lexer->pos - pos - 1);
                    lexer_advance(lexer, 1);
                } else {
                    error("Unexpected character", ERROR_INVALID);
                }

        }
    }
}

void free_lexer(lexer_t *lexer) {
    free(lexer->source);
    free_token_list(lexer->tokens);
    free(lexer);
}

int count_string(char *haystack, char *needle){
    int count = 0;
    const char *tmp = haystack;
    while((tmp = strstr(tmp, needle)))
    {
        count++;
        tmp++;
    }
    return count;
}

char* display_token(token_t* token) {
    char* kind;
    switch (token->kind) {
        case TOKEN_COMMA:
            kind = "COMMA";
            break;
        case TOKEN_COLON:
            kind = "COLON";
            break;
        case TOKEN_LPAREN:
            kind = "LPAREN";
            break;
        case TOKEN_RPAREN:
            kind = "RPAREN";
            break;
        case TOKEN_LBRACE:
            kind = "LBRACE";
            break;
        case TOKEN_RBRACE:
            kind = "RBRACE";
            break;
        case TOKEN_LBRACKET:
            kind = "LBRACKET";
            break;
        case TOKEN_RBRACKET:
            kind = "RBRACKET";
            break;
        case TOKEN_IDENT:
            kind = "IDENT";
            break;
        case TOKEN_NUMBER:
            kind = "NUMBER";
            break;
        case TOKEN_STRING:
            kind = "STRING";
            break;
        case TOKEN_NEWLINE:
            kind = "NEWLINE";
            break;
        default:
            kind = "UNKNOWN";
    }
    char* lexeme = malloc(token->len + 1);
    strncpy(lexeme, token->lexeme, token->len);
    lexeme[token->len] = '\0';
    char* display = malloc(100);
    sprintf(display, "Token: %s, Kind: %s, Line: %zu, Col: %zu", lexeme, kind, token->line, token->col);
    free(lexeme);
    return display;
}