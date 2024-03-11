//Fletcher Morton
//Assignment 3

/* External Header File for the parser.c file */

#ifndef _PARSER_H_
#define _PARSER_H_

#include "ast.h"

//Initialize the parser to work on the given input file
extern void parser_open(const char *);

//Close the input file
extern void parser_close();

//Parse the program using tokens from lexer_next() and generate an AST for it, checking for parse errors
//<program> ::= <const-decls> <var-decls> <stmt> .
extern AST *parseProgram();


/*
    <stmt> ::= <ident> := <expr>
        | begin <stmt> {<semi-stmt>} end
        | if <condition> then <stmt> else <stmt>
        | while <condition> do <stmt>
        | read <ident>
        | write <expr>
        | skip
*/
extern AST *parseStmt();


//<expr> ::= <term> {<add-sub-term>}
extern AST *parseExpr();

#endif
