//Include/Define
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define MAX_STACK_HEIGHT    2048
#define MAX_CODE_LENGTH     512

//Structs
typedef struct {
    int op; //The operation code
    int m;  //Either a number, or an address, depending on the operator
} Instruction;

//Prototypes
extern void machine(const char *fileName);
extern void usage();

//argc is number of arguments passed in cmd. Name of program itself is an arg, so should be 1 by default
//argv is the argument in String form
int main(int argc, char *argv[1])
{
    const char *prgName = argv[0];

    --argc;

    printf("\n%s\n", argv[1]);

    //print message to stderr if something went wrong
    if(argc == 1 && argv[0][1] != '-') printf("\nWonr\n");

    machine(argv[1]); //filename. I think machine might be the VM

    return EXIT_SUCCESS; //Macro for exiting with 0 (success)
}

//________________________________________________________________

void machine(const char* fileName)
{
    int BP = 0; //Bottom of the current activation record
    int SP = 0; //The next location in the stack to allocate
    int PC = 0; //List of instructions

    //Make stack
    int stack[MAX_STACK_HEIGHT-1];
    for(int i = 0; i < MAX_STACK_HEIGHT; i++) {
        stack[i] = 0;
    }

    //Make the code "stack"
    Instruction code[MAX_CODE_LENGTH-1];

    //Read in file contents
    FILE *inputFile = fopen(fileName, "r");
    int opCode;
    int mField;
    int i = 0;

    while(!feof(inputFile)) {
        fscanf(inputFile, "%d ", &opCode);
        fscanf(inputFile, "%d", &mField);

        //Use line's values to initialize an instruction struct, and put it in the code array
        Instruction inst;
        inst.op = opCode;
        inst.m = mField;
        code[i] = inst;

        printf("I read in:\t%d\t%d\n", code[i].op, code[i].m);
        i++;
    }

    //Reading is finished.
    fclose(inputFile);


    //Execute the code
    for(int i = 0; i < MAX_CODE_LENGHT; i++) {

        //Determine which instruction should be executed based off of the opcode of the current instruction
        switch(code[PC].op) {
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
            //Literal Push
            case 1:
                break;
        }
    }




}

void readFile(int* stack, Instruction* code, char* filename, )

void usage()
{
    fprintf(stderr, "\n\n-----Something happened-----\n\n");
    exit(0);
}
