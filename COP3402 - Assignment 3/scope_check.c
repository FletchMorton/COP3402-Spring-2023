//Fletcher Morton
//Assignment 3

/* Declaration Checker */
//Check for duplicate names and undeclared vars that get used

//Include/Define
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>


//Global Variables


//Prototypes
extern void scope_initialize();
extern void scope_check_program(AST *);


void scope_initialize()
{
    //Initialize static variables in symbol table module
}

void scope_check_program(AST *progast)
{
    //Build the symbol table and check the AST for errors
}

