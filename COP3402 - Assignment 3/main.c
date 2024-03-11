//Fletcher Morton
//Assignment 3

/* Compiler */

//Include/Define
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "parser.c"
#include "parserInternal.h"
#include "ast.h"
#include "unparser.h"


//Driver function
int main(int argc, char **argv)
{

    //Assert that there were no errors when receiving the file
    assert(argc != 0 && argv != NULL);


    /* call a function in the parser to open the file provided on the command line
       we suggest naming it parser_open and putting it in a file named parser.c
       The function should call your lexer_open function */

    //Run a fn to initialize the parser to work on the given input file
    //This eventually involves calling lexer_open on the input file name that is passed to your prg


    //Pass filename to parser
    parser_open("file.txt");


    /* call a function to parse the program and return its AST.
       we suggest naming the function that parses a program something like
       parseProgram. During parsing, your patser should report any syntax errors using the
       function parse_error_unexpected */

    //parse the program using tokens obtained by calling lexer_next and generate an AST for it, checking for parse errors
    // Parser will return a pointer to an AST; in what follows, suppose we ccall this "progast"

    AST * progast = parseProgram();


    /* close the input file (this should involve calling lexer_close) */

    parser_close();


    /*  call unparseProgram and pass it the FILE* stdout and the AST returned by the parser
        for the program*/

    //Test the AST by running it through unparse
    //unparseProgram(stdout, progast);

    /* Preform any required initializations on your compuler's symbol table (we suggest calling a function
        to initialize it, which might involve initializing any static vartiables in your syumvbol table module) */

    //scope_initialize();

    /* UIsing the AST prograst, build the symbol table and check the AST for identifiers that are
        declared more than once or uses of identifiers for which there is no corresponding declaration, issuing
        an errror message for sucgh problems */

    //scope_check_program(progast);


    return EXIT_SUCCESS;
}
