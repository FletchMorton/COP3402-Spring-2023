//Fletcher Morton
//Assignment 3

/* Parser */
//Produce ASTs by analyzing a given input file

//Include/Define
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "parserInternal.h"
#include "lexer.h"
#include "utilities.h"


//Global Variables
static token tok;


//Initialize the parser to work on the given input file
void parser_open(const char *fname)
{
    //Pass filename to lexer
    lexer_open(fname);

    //Initialize parser global variables
    tok = lexer_next();
}


//Close the input file
void parser_close()
 {
    lexer_close();
 }


//Concatenate two AST Lists
static void add_AST_to_end(AST_list *head, AST_list *tail, AST_list catList)
{
    //If the tracked list is empty, make the passed list the tracked list
    if(ast_list_is_empty(*head)) {
        *head = catList;
        *tail = ast_list_last_elem(catList);
    } else {
        assert(*tail != NULL);
        ast_list_splice(*tail, catList); //Concatenate the catList to the end of the tracked list
        *tail = ast_list_last_elem(catList); //Update the tail pointer
    }
}



//If the lexer has not finished, advance the input by grabbing the next token
static void advance()
{
    if(!lexer_done()) tok = lexer_next();
}


//If the encountered token was the expected token, advance the input
static void eat(token_type ttyp)
{
    if(tok.typ == ttyp) {
        advance();
    } else {
        token_type expected[1] = {ttyp};
        parse_error_unexpected(expected, 1, tok);
    }
}

//Parse the program using tokens from lexer_next() and generate an AST for it, checking for parse errors
//<program> ::= <const-decls> <var-decls> <stmt> .
AST *parseProgram()
{
    //Expected grammar
    AST_list cds = parseConstDecls();
    AST_list vds = parseVarDecls();
    //AST *stmt = parseStmt();
    AST *stmt = NULL;
    eat(periodsym);

    file_location floc;

    //If there were constant declarations
    if(!ast_list_is_empty(cds)) {
        //Start file there
        if(ast_list_first(cds)->type_tag == const_decl_ast) {
            floc = ast_list_first(cds)->file_loc;
        } else {
            bail_with_error("Bad AST for const declarations\n");
        }

    //If there were no constant declarations, but were variable declarations
    } else if(!ast_list_is_empty(vds)) {
        //Start file at variable declarations
        if(ast_list_first(vds)->type_tag == var_decl_ast) {
            floc = ast_list_first(vds)->file_loc;
        } else {
            bail_with_error("Bad AST for var declarations\n");
        }

    //Otherwise start file at the statement
    } else {
        floc = stmt->file_loc;
    }

    return ast_program(floc.filename, floc.line, floc.column, cds, vds, stmt);
}


//<const-decls> ::= {<const-decl>}
//      & <const-decl> ::= const <const-defs> ;
static AST_list parseConstDecls()
{
    AST_list result = ast_list_empty_list();
    AST_list tail = ast_list_empty_list();

    //Repeatedly get a <const-decl> and add it to an AST list to return
    while(tok.typ == constsym) {
        AST_list cdASTs; //List of all <const-decl> ASTs present in <const-decls>

        //<const-decl>
        eat(constsym); //const
        cdASTs = parseConstDefs(); //<const-defs>
        eat(semisym); // ;

        //Update <const-decl> AST list
        add_AST_to_end(&result, &tail, cdASTs);
    }

    return result;
}


//<const-defs> ::= <const-def> {<comma-const-def>}
static AST_list parseConstDefs()
{
    //<const-def>
    AST *cd = parseConstDef();

    //Add that <const-def> to a new list
    AST_list result = ast_list_singleton(cd);
    AST_list tail = result;

    printf("Saw a %s being set to %d\n", cd->data.const_decl.name, cd->data.const_decl.num_val);

    //{<comma-const-def>} ::= {, <const-def>}
    //Repeatedly get an <const-def> and add it to an AST list to return
    while(tok.typ == commasym) {
        eat(commasym); // ,

        //Add <const-def> to the list
        AST *cd = parseConstDef();
        add_AST_to_end(&result, &tail, ast_list_singleton(cd));
        printf("Saw a %s being set to %d\n", cd->data.const_decl.name, cd->data.const_decl.num_val);
    }

    return result;
}


//<const-def> ::= <ident> = <number>
static AST_list parseConstDef()
{
    //<ident>
    token idTok = tok;
    eat(identsym);

    // =
    eat(eqsym);

    //<number>
    short int num = tok.value;
    eat(numbersym);

    //Return the <const-def> AST
    return ast_const_def(idTok, idTok.text, num);
}


//<var-decls> ::= {<var-decl>}
//      & <var-decl> ::= var <idents> ;
static AST_list parseVarDecls()
{
    AST_list result = ast_list_empty_list();
    AST_list tail = ast_list_empty_list();

    //Repeatedly get a <var-decl> and add it to an AST list to return
    while(tok.typ == varsym) {
        AST_list vdASTs; //List of all <var-decl> ASTs present in <var-decls>

        //<var-decl>
        eat(varsym); //var
        vdASTs = parseIdents(); //<idents>
        eat(semisym); // ;

        //Update <var-decl> AST list
        add_AST_to_end(&result, &tail, vdASTs);
    }

    return result;
}


//<idents> ::= <ident> {<comma-ident>}
static AST_list parseIdents()
{
    //<ident>
    token idTok = tok;
    eat(identsym);

    //Add that <ident> to a new list
    AST_list result = ast_list_singleton(ast_var_decl(idTok, idTok.text));
    AST_list tail = result;

    printf("Saw a declaration of %s\n", result->data.var_decl.name);

    //{<comma-ident>} ::= {, <ident>}
    //Repeatedly get an <ident> and add it to an AST list to return
    while(tok.typ == commasym) {
        eat(commasym); // ,

        //<ident>
        token idTok = tok;
        eat(identsym);

        //Add <ident> to the list
        AST *vd = ast_var_decl(idTok, idTok.text);
        add_AST_to_end(&result, &tail, ast_list_singleton(vd));
        printf("Saw a declaration of %s\n", vd->data.var_decl.name);
    }

    return result;
}







