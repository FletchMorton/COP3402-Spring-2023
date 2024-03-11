//Fletcher Morton
//Assignment 3

/* Internal Header File for the parser.c file */

#ifndef _PARSERINTERNAL_H_
#define _PARSERINTERNAL_H_

#include "parser.h"

//Concatenate two AST Lists
static void add_AST_to_end(AST_list *, AST_list *, AST_list);

//If the lexer has not finished, advance the input by grabbing the next token
static void advance();

//If the encountered token was the expected token, advance the input
static void eat(token_type);

//Tests to see if passed token a relational operator
static bool is_relational_op(token_type);

//Return the corresponding operator enum for the given token type
//static oper tt2oper(token_type);

//Tests to see if the current token is a begin token
static bool is_stmt_beginning_token(token);

//<const-decls> ::= {<const-decl>}
//      & <const-decl> ::= const <const-defs> ;
static AST_list parseConstDecls();

//<const-defs> ::= <const-def> {<comma-const-def>}
static AST_list parseConstDefs();

//<const-def> ::= <ident> = <number>
static AST *parseConstDef();

//<var-decls> ::= {<var-decl>}
//      & <var-decl> ::= var <idents> ;
static AST_list parseVarDecls();

//<idents> ::= <ident> {<comma-ident>}
static AST_list parseIdents();

//begin <stmt> {<semi-stmt>} end
static AST_list parseBeginStmt();

//if <condition> then <stmt> else <stmt>
static AST *parseIfStmt();

//while <condition> do <stmt>
static AST *parseWhileStmt();

//read <ident>
static AST *parseReadStmt();

//write <expr>
static AST *parseWrite();

//<semi-stmt> ::= ; <stmt>
static AST *parseSemiStmt();

//<condition> ::= odd <expr>
//          | <expr> <rel-op> <expr>
static AST *parseCondition();

//<rel-op> ::=  =|<>|<|<=|>|>=
static AST *parseRelOp();

//<add-sub-term> ::= <add-sub> <term>
static AST *parseAddSubTerm();

//<add-sub> ::= <plus> | <minus>
static AST *parseAddSub();

//<term> ::= <factor> {<mult-div-factor>}
static AST *parseTerm();

//<mult-div-factor> ::= <mult-div> <factor>
static AST *parseMultDivFactor();

//<mult-div> ::= <mult> | <div>
static AST *parseMultDiv();

//<factor> ::= <ident> | <sign> <number> | ( <expr> )
static AST *parseFactor();

//<sign> ::= <plus> | <minus> | <empty>
static AST *parseSign();


#endif
