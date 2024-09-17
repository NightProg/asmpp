cmake_minimum_required(VERSION 3.28)
project(asmpp C)

set(CMAKE_C_STANDARD 17)
add_executable(asmpp src/main.c
        src/ast.h
        src/ast.c
        src/error.c
        src/error.h
        src/lexer.c
        src/lexer.h
        src/parser.c
        src/parser.h
        src/codegen.c
        src/codegen.h
        src/asm.c
        src/asm.h
        src/util.c
        src/util.h
        src/log.c
        src/log.h
        src/cli.c
        src/cli.h
        src/fs.c
        src/fs.h)
