#include "parser.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

parser_t *new_parser(token_list_t *tokens) {
    parser_t *parser = malloc(sizeof(parser_t));
    if (parser == NULL) {
        error("Failed to allocate memory for parser", ERROR_ALLOC);
    }
    parser->tokens = tokens;
    parser->stmts = new_stmt_list();
    parser->index = 0;
    return parser;
}

bool eof(parser_t *parser) {
    return parser->index >= parser->tokens->len;
}

token_t *peek(parser_t *parser) {
    if (parser->index >= parser->tokens->len) {
        error("Index out of bounds", ERROR_INVALID);
    }
    return &parser->tokens->tokens[parser->index];
}

token_t *advance(parser_t *parser, int n) {
    parser->index += n;
    if (eof(parser)) {
        return NULL;
    }
    return peek(parser);
}

token_t *expect(parser_t *parser, token_kind_t kind) {
    token_t *token = peek(parser);
    if (token->kind != kind) {
        char* error_message = malloc(100);
        sprintf(error_message, "Unexpected token: %s", token->lexeme);
        error(error_message, ERROR_INVALID);
    }
    advance(parser, 1);
    return token;
}

token_t *expect_ident(parser_t *parser, const char *ident) {
    token_t *token = peek(parser);
    if (token->kind != TOKEN_IDENT || strcmp(token->lexeme, ident) != 0) {
        error("Unexpected token", ERROR_INVALID);
    }
    advance(parser, 1);
    return token;
}

bool check(parser_t *parser, token_kind_t kind) {
    if (eof(parser)) {
        return false;
    }
    token_t *token = peek(parser);
    return token->kind == kind;
}

bool match(parser_t *parser, token_kind_t kind) {
    if (eof(parser)) {
        return false;
    }


    token_t *token = peek(parser);
    if (token->kind == kind) {
        advance(parser, 1);
        return true;
    }
    return false;
}

bool match_ident(parser_t *parser, const char *ident) {
    token_t *token = peek(parser);
    if (token->kind == TOKEN_IDENT && strcmp(token->lexeme, ident) == 0) {
        advance(parser, 1);
        return true;
    }
    return false;
}

void parse(parser_t *parser) {
    while (parser->index < parser->tokens->len) {
        if (match(parser, TOKEN_NEWLINE)) {
            continue;
        }
        if (match_ident(parser, "label")) {
            token_t *ident = expect(parser, TOKEN_IDENT);
            label_t *label = new_label(ident->lexeme, new_call_abi("default", new_argument_list()), new_instr_list(), new_attribute_list());
            if (match(parser, TOKEN_LPAREN)) {
                while (!match(parser, TOKEN_RPAREN) && !eof(parser)) {
                    if (eof(parser)) {
                        error("Unexpected end of file, expected ')' ", ERROR_INVALID);
                    }
                    if (label->abi->args->len > 0) {
                        expect(parser, TOKEN_COMMA);
                    }
                    token_t *reg = expect(parser, TOKEN_IDENT);
                    register_kind_t kind = get_register_kind_by_name(reg->lexeme);
                    append_argument(label->abi->args, new_argument_register(kind));
                }
            }

            if (check(parser, TOKEN_LBRACKET)) {
                attribute_list_t *attributes = parse_attribute_list(parser);
                label->attributes = attributes;
            }

            expect(parser, TOKEN_LBRACE);

            while (!check(parser, TOKEN_RBRACE)) {
                if (eof(parser)) {
                    error("Unexpected end of file, expected '}' ", ERROR_INVALID);
                }

                if (match(parser, TOKEN_NEWLINE)) {
                    continue;
                }

                instr_t* instr = parse_instr(parser);
                append_instr(label->instrs, instr);
            }
            expect(parser, TOKEN_RBRACE);
            append_stmt(parser->stmts, new_label_stmt(label));
        } else if (match_ident(parser, "extern")) {
            token_t *ident = expect(parser, TOKEN_IDENT);
            call_abi_t *abi = new_call_abi("default", new_argument_list());
            if (match(parser, TOKEN_LPAREN)) {
                while (!match(parser, TOKEN_RPAREN) && !eof(parser)) {
                    if (eof(parser)) {
                        error("Unexpected end of file, expected ')' ", ERROR_INVALID);
                    }
                    if (abi->args->len > 0) {
                        expect(parser, TOKEN_COMMA);
                    }
                    token_t *reg = expect(parser, TOKEN_IDENT);
                    register_kind_t kind = get_register_kind_by_name(reg->lexeme);
                    append_argument(abi->args, new_argument_register(kind));
                }
            }

            attribute_list_t *attributes = new_attribute_list();
            if (check(parser, TOKEN_LBRACKET)) {
                attributes = parse_attribute_list(parser);
            }

            append_stmt(parser->stmts, new_extern_stmt(new_extern(abi, ident->lexeme, attributes)));

        } else if (match_ident(parser, "data")) {
            token_t *ident = expect(parser, TOKEN_IDENT);
            expect(parser, TOKEN_COLON);
            type_t *type = parse_type(parser);
            if (match(parser, TOKEN_ASSIGN)) {
                expr_list_t *values = new_expr_list();
                while (!check(parser, TOKEN_NEWLINE) && !eof(parser)) {
                    if (eof(parser)) {
                        error("Unexpected end of file, expected newline", ERROR_INVALID);
                    }
                    if (values->len > 0) {
                        expect(parser, TOKEN_COMMA);
                    }
                    expr_t *expr = parse_expr(parser);
                    append_expr(values, expr);
                }
                append_stmt(parser->stmts, new_data_stmt(new_data(ident->lexeme, type, values)));
            } else {
                append_stmt(parser->stmts, new_data_stmt(new_data_uninitialized(ident->lexeme, type)));
            }
        } else {
            instr_t *instr = parse_instr(parser);
            append_stmt(parser->stmts, new_instr_stmt(instr));
        }
    }
}

attribute_list_t *parse_attribute_list(parser_t *parser) {
    attribute_list_t *attributes = new_attribute_list();
    expect(parser, TOKEN_LBRACKET);
    while (!match(parser, TOKEN_RBRACKET) && !eof(parser)) {
        if (eof(parser)) {
            error("Unexpected end of file, expected ']' ", ERROR_INVALID);
        }
        if (attributes->len > 0) {
            expect(parser, TOKEN_COMMA);
        }
        token_t *ident = expect(parser, TOKEN_IDENT);
        if (match(parser, TOKEN_LPAREN)) {
            string_list_t *args = new_string_list();
            while (!match(parser, TOKEN_RPAREN) && !eof(parser)) {
                if (eof(parser)) {
                    error("Unexpected end of file, expected ')' ", ERROR_INVALID);
                }
                if (args->len > 0) {
                    expect(parser, TOKEN_COMMA);
                }
                token_t *arg = expect(parser, TOKEN_STRING);
                append_string(args, arg->lexeme);
            }
            append_attribute(attributes, new_attribute(ATTR_DIRECTIVE, ident->lexeme, args));
        } else {
            append_attribute(attributes, new_attribute(ATTR_FLAG, ident->lexeme, NULL));
        }
    }
    return attributes;
}

type_t* parse_type(parser_t *parser) {
    token_t *token = expect(parser, TOKEN_IDENT);
    type_kind_t kind;
    if (strcmp(token->lexeme, "byte") == 0) {
        kind = TYPE_BYTE;
    } else if (strcmp(token->lexeme, "word") == 0) {
        kind = TYPE_WORD;
    } else if (strcmp(token->lexeme, "dword") == 0) {
        kind = TYPE_DWORD;
    } else if (strcmp(token->lexeme, "qword") == 0) {
        kind = TYPE_QWORD;
    } else {
        error("Invalid type", ERROR_INVALID);
    }
    if (match(parser, TOKEN_LBRACKET)) {
        token_t* number = expect(parser, TOKEN_NUMBER);
        int length = atoi(number->lexeme);
        expect(parser, TOKEN_RBRACKET);
        return new_array_type(
                new_simple_type(kind),
                length
                );
    } else {
        return new_simple_type(kind);
    }

}

instr_t* parse_instr(parser_t *parser) {
    token_t *token = peek(parser);

    if (match_ident(parser, "if")) {
        token_t* cmp_op = expect(parser, TOKEN_IDENT);
        expect(parser, TOKEN_LPAREN);
        expr_list_t *cond = new_expr_list();
        expr_t* arg0 = parse_expr(parser);
        append_expr(cond, arg0);
        expect(parser, TOKEN_COMMA);
        expr_t* arg1 = parse_expr(parser);
        append_expr(cond, arg1);
        expect(parser, TOKEN_RPAREN);
        expect(parser, TOKEN_LBRACE);

        instr_if_t* instr_if = new_instr_if(cond, get_cmp_kind_by_name(cmp_op->lexeme), new_instr_list(), new_instr_list());

        while (!check(parser, TOKEN_RBRACE) && !eof(parser)) {
            if (eof(parser)) {
                error("Unexpected end of file, expected '}' ", ERROR_INVALID);
            }

            if (match(parser, TOKEN_NEWLINE)) {
                continue;
            }

            instr_t* instr = parse_instr(parser);
            append_instr(instr_if->then_instrs, instr);
        }
        match(parser, TOKEN_RBRACE);

        if (match_ident(parser, "else")) {
            expect(parser, TOKEN_LBRACE);
            while (!match(parser, TOKEN_RBRACE) && !eof(parser)) {
                if (eof(parser)) {
                    error("Unexpected end of file, expected '}' ", ERROR_INVALID);
                }

                if (match(parser, TOKEN_NEWLINE)) {
                    continue;
                }

                instr_t* instr = parse_instr(parser);
                append_instr(instr_if->else_instrs, instr);
            }
        }

        return new_if_instr(instr_if);
    } else if (match(parser, TOKEN_IDENT)){
        token_t* opcode = token;
        if (match(parser, TOKEN_LPAREN)) {
            expr_list_t *args = new_expr_list();
            while (!check(parser, TOKEN_RPAREN) && !eof(parser)) {
                if (eof(parser)) {
                    error("Unexpected end of file, expected ')' ", ERROR_INVALID);
                }
                if (args->len > 0) {
                    expect(parser, TOKEN_COMMA);
                }
                expr_t* arg = parse_expr(parser);
                append_expr(args, arg);
            }
            expect(parser, TOKEN_RPAREN);
            return new_call_instr(new_instr_call(opcode->lexeme, args));
        }
        instr_asm_t* instr_asm = new_instr_asm(opcode->lexeme, new_expr_list());
        if (!match(parser, TOKEN_NEWLINE)) {
            expr_t* arg0 = parse_expr(parser);
            append_expr(instr_asm->args, arg0);
            while (check(parser, TOKEN_COMMA) && !eof(parser)) {
                advance(parser, 1);
                if (eof(parser)) {
                    error("Unexpected end of file, expected newline", ERROR_INVALID);
                }
                expr_t* expr = parse_expr(parser);
                append_expr(instr_asm->args, expr);
            }

            if (!eof(parser)) {
                advance(parser, 1);
            }
        }


        return new_asm_instr(instr_asm);
    } else {
        error("Unexpected token", ERROR_INVALID);
    }
}

expr_t* parse_expr(parser_t *parser) {
    token_t* token = peek(parser);
    if (match(parser, TOKEN_NUMBER)) {
        int value = atoi(token->lexeme);
        return new_expr_immediate(value);
    }
    if (match(parser, TOKEN_IDENT)) {
        return new_expr_register(get_register_kind_by_name(token->lexeme));
    }
    if (match(parser, TOKEN_STRING)) {
        return new_expr_string(token->lexeme);
    }
    error("Unexpected token", ERROR_INVALID);
}