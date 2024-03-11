//Fletcher Morton
//Assignment 2

//Include/Define
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "lexer.h"
#include "lexer_output.h"


int main(int argc, char **argv)
{
    //Assert that there were no errors when passing/receiving the filename
    assert(argc != 0 && argv != NULL);

    //Pass the filename to machine
    lexer_open("file.txt");
    lexer_output();

    return EXIT_SUCCESS; //Macro for exiting with 0 (success)
}
