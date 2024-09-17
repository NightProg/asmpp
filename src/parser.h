#ifndef ASMPP_PARSER_H
#define ASMPP_PARSER_H
#include "ast.h"
#include "lexer.h"

typedef struct parser_t parser_t;

struct parser_t {
    token_list_t* tokens;
    stmt_list_t* stmts;
    int index;
};

parser_t* new_parser(token_list_t* tokens);
bool eof(parser_t* parser);
token_t* advance(parser_t* parser, int n);
token_t* peek(parser_t* parser);
token_t* expect(parser_t* parser, token_kind_t kind);
token_t* expect_ident(parser_t* parser, const char* ident);
bool check(parser_t* parser, token_kind_t kind);
bool match(parser_t* parser, token_kind_t kind);
bool match_ident(parser_t* parser, const char* ident);
void parse(parser_t* parser);
type_t* parse_type(parser_t* parser);
instr_t* parse_instr(parser_t *parser);
expr_t* parse_expr(parser_t *parser);
attribute_list_t *parse_attribute_list(parser_t *parser);
void free_parser(parser_t* parser);

#endif //ASMPP_PARSER_H
