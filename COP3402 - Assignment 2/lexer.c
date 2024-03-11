//Fletcher Morton
//Assignment 2

//Include/Define
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "lexer.h"
#include "utilities.h"



//Global Variables
char fileName[FILENAME_MAX];        //Contains the name of the provided file
token backlog[MAX_IDENT_LENGTH];    //Array of tokens to temporarily hold onto excess tokens

FILE *inputFile;                    //Holds the FILE variable for the open file
int line = 1;                       //Current Line
int col = 0;                        //Current Column
int backup = 0;                     //Tracks how many tokens are in the backlog
int done = true;                    //Keeps track of the lexer's operation status



//Prototypes
token createToken(token_type, char *, short int);   //Creates and initializes a token
void addBacklog(token);                             //Add a token to the token array
token getBacklog();                                 //Grab a token from the token array



//Initialize the lexer and start it reading from the given file name
void lexer_open(const char *fname)
{
    //Check that the filename was passed properly
    if(fname == NULL) {
        bail_with_error("An unreadable file name was passed to the program.\n");
    }

    //Open the file for reading
    inputFile = fopen(fname, "r");
    if(inputFile == NULL) {
        bail_with_error(fname);
    }

    //Initialize the lexer
    strcpy(fileName, fname); //Set the file name
    done = false; //Set the done status

}



//Close the file the lexer is working on and mark the lexer as done
void lexer_close()
{
    fclose(inputFile);
    done = true;
}



//Return the done status of the lexer
bool lexer_done() {
    return done;
}



//Return the next token in the input file, advancing in the input
token lexer_next()
{
    //Input strings that signify a reserved word
    const char *reserved[15] =
        {"const", "var", "procedure", "call", "begin", "end", "if", "then", "else",
         "while", "do", "read", "write", "skip", "odd"};

    //Token types indexed to mirror their respective reserved word strings
    const token_type resSym[15] =
        {constsym, varsym, procsym, callsym, beginsym, endsym, ifsym, thensym, elsesym,
         whilesym, dosym, readsym, writesym, skipsym, oddsym};

    //If there are tokens in the backlog, return those tokens instead of grabbing the next char
    if(backup > 0) {
        token t = getBacklog();
        return t;
    }

    //Variables
    token t; //Token to return
    int colP = 0; //Column pointer for keeping place when parsing an identifier
    char string[MAX_IDENT_LENGTH + 1]; //Array to hold a copy of the identifier

    //Get the next character in the input
    char c = getc(inputFile);
    col++;

    //Identify the input's type
    switch(c) {
        //periodsym
        case '.': t = createToken(periodsym, ".", 0);
            break;

        //semisym
        case ';': t = createToken(semisym, ";", 0);
            break;

        //commasym
        case ',': t = createToken(commasym, ",", 0);
            break;

        //lparensym
        case '(': t = createToken(lparensym, "(", 0);
            break;

        //rparensym
        case ')': t = createToken(rparensym, ")", 0);
            break;

        //eqsym
        case '=': t = createToken(eqsym, "=", 0);
            break;

        //plussym
        case '+': t = createToken(plussym, "+", 0);
            break;

        //minussym
        case '-': t = createToken(minussym, "-", 0);
            break;

        //multsym
        case '*': t = createToken(multsym, "*", 0);
            break;

        //divsym
        case '/': t = createToken(divsym, "/", 0);
            break;

        //becomessym
        case ':':
            //Check the immediate next character
            c = getc(inputFile);

            //Becomessym
            if(c == '=') {
                t = createToken(becomessym, ":=", 0);
                col++;

            //Some error must have occurred
            } else {
                col++;
                lexical_error(lexer_filename(), lexer_line(), lexer_column(), "Expecting '=' after a colon, not '%c'\n", c);
            }

            break;

        //gtrsym, geqsym
        case '>':
            //Check the immediate next input
            c = getc(inputFile);

            //geqsym
            if(c == '=') {
                t = createToken(geqsym, ">=", 0);
                col++;

            //gtrsym
            } else {
                ungetc(c, inputFile);

                t = createToken(gtrsym, ">", 0);
            }

            break;

        //lessym, neqsym, leqsym
        case '<':
            //Check the immediate next input
            c = getc(inputFile);

            //neqsym
            if(c == '>') {
                t = createToken(neqsym, "<>", 0);
                col++;

            //leqsym
            } else if(c == '=') {
                t = createToken(leqsym, "<=", 0);
                col++;

            //lessym
            } else {
                ungetc(c, inputFile);

                t = createToken(lessym, "<", 0);
            }

            break;

        //0-9 ASCII (numbersym)
        case 48 ... 57:
            //Set column pointer to keep track of the start of the number
            colP = 0;

            //Check the next character until the end of the number is identified or maximum number of digits is exceeded
            for(int i = 0; i <= 6; i++) {

                //Put the current character into the string
                string[i] = c;

                //Grab the next character
                c = getc(inputFile);
                colP++;

                //If it's not another number, the number is complete. Return the picked char and create token.
                if(c < 48 || c > 57) {
                    ungetc(c, inputFile);
                    colP--;
                    string[i+1] = '\0';

                    //If the maximum or minimum allowed short length is exceeded, report error
                    if(atoi(string) > SHRT_MAX || atoi(string) < SHRT_MIN) {
                        lexical_error(lexer_filename(), lexer_line(), lexer_column(), "The value of %d is too large for a short!\n", atoi(string));
                    }

                    t = createToken(numbersym, NULL, atoi(string));
                    col += colP;
                    break;

                //If the number accumulates 5 digits and the immediate input is another digit, error
                } else if(i == 6) {
                    lexical_error(lexer_filename(), lexer_line(), lexer_column(), "The value of %d is too large for a short!\n", atoi(string));
                }
            }

            break;

        //a-z && A-Z ASCII (identsym)
        //(varsym, procsym, constsym, callsym, beginsym, endsym, ifsym)
        //(thensym, elsesym, whilesym, dosym, readsym, writesym, skipsym, oddsym)
        case 65 ... 90:
        case 97 ... 122:
            //Set column pointer to keep track of the start of the identifier
            colP = 0;

            //For each character until a non-alphanumeric char is detected (respecting the max identifier length)...
            for(int i = 0; i <= MAX_IDENT_LENGTH - 1; i++) {

                //Put the current character into the string
                string[i] = c;

                //Grab the next character
                c = getc(inputFile);
                colP++;

                //If it's a not another letter or number, the identifier is complete. Return the picked char and create token.
                if(c < 48 || (c > 57 && c < 65) || (c > 90 && c < 97) || c > 122) {
                    ungetc(c, inputFile);
                    colP--;
                    string[i+1] = '\0';

                    /* Check for reserved words */
                    for(int j = 0; j < 15; j++) { //for each of the reserved words
                        for(int k = 0; k <= strlen(string); k++) { //for each character in String

                            //if the current character of the String matches the first character of the reserved word, test for match
                            if(string[k] == reserved[j][0]) {
                                char identL[k + 1]; //String representing identifier to the left of the reserved word
                                char key[strlen(reserved[j]) + 1]; //String potentially representing reserved word found
                                char identR[strlen(string) - (k + strlen(reserved[j]))]; //String representing identifier to the right of the reserved word

                                //from the first matching letter (at k) copy over number of chars equal to number of char in the reserved word
                                for(int m = 0; m <= strlen(reserved[j]); m++) {
                                    key[m] = string[k+m];
                                    if(m == strlen(reserved[j])) key[m] = '\0';
                                }

                                //If the two strings match...
                                if(strcmp(key, reserved[j]) == 0) {
                                    token_type ttyp = resSym[j]; //Get the reserved word's corresponding type

                                    //If there was an identifier to the left, copy it into a string
                                    if(k > 0) {
                                        for(int m = 0; m < k; m++) {
                                            identL[m] = string[m];
                                            if(m == k-1) identL[m+1] = '\0';
                                        }
                                    }

                                    //If there was an identifier to the right, copy it into a string
                                    if(strlen(string) > strlen(identL) + strlen(reserved[j])) {
                                        for(int m = 0; m <= strlen(string) - (strlen(identL) + strlen(reserved[j])); m++) {
                                            identR[m] = string[m + strlen(identL) + strlen(reserved[j])];
                                        }
                                    }

                                    //Throw the token for the reserved word found into the backlog
                                    addBacklog(createToken(ttyp, key, 0));
                                    col += strlen(reserved[j])-1;

                                    //If it exists, throw the token for the left identifier into the backlog
                                    if(k > 0) {
                                        addBacklog(createToken(identsym, identL, 0));
                                        col += strlen(identL);
                                    }

                                    //If it exists, put the right identifier back onto the input to parse further
                                    if(strlen(string) > strlen(identL) + strlen(reserved[j])) {
                                        for(int m = strlen(identR) - 1; m >= 0; m--) {
                                            ungetc(identR[m], inputFile);
                                        }
                                    }

                                    //Recursively grab a token to return from this call with
                                    return lexer_next();

                                }
                            }
                        }
                    }



                    /* Nothing was similar to a reserved word */

                    //Return identifier
                    t = createToken(identsym, string, 0);
                    col += colP;
                    break;

                //If the maximum allowed identifier length is exceeded, report error
                } else if(i == MAX_IDENT_LENGTH - 1) {
                    lexical_error(lexer_filename(), lexer_line(), lexer_column(), "Identifier starting \"%s\" is too long!\n", string);
                }
            }

            break;

        //Comment
        case '#':
            //Eat input until newline
            while(c != 10) {
                c = getc(inputFile);
                col++;

                //If the input is terminated during the comment, send an error message
                if(c == -1) {
                    lexical_error(lexer_filename(), lexer_line(), lexer_column(), "File ended while reading comment!\n");
                }
            }

            /* Fall through */

        //Newline
        case 10:
            col = 0;
            line++;
            t = lexer_next(); //Recursively grab a token to return from this call with
            break;

        //Ignored items (whitespace, horizontal tab, vertical tab, formfeed, carriage return)
        case 9:
        case 11:
        case 12:
        case 13:
        case 32:
            t = lexer_next(); //Recursively grab a token to return from this call with
            break;

        //eofsym
        case -1:
            t = createToken(eofsym, NULL, 0);
            lexer_close();
            break;

        //Unrecognized character
        default:
            lexical_error(lexer_filename(), lexer_line(), lexer_column(), "Illegal character '%c' (%03o)\n", c, c);
            break;
    }



    return t;
}



// Return the name of the current file
const char *lexer_filename()
{
    if(lexer_done() || fileName[0] == '\0') bail_with_error("There is no file currently open.\n");

    return fileName;

}



// Return the line number of the next token
unsigned int lexer_line()
{
    return line;
}



// Return the column number of the next token
unsigned int lexer_column()
{
    return col;
}



//Create and initialize a token
token createToken(token_type typ, char *text, short int value)
{
    token result;

    result.filename = lexer_filename();
    result.column = lexer_column();
    result.line = lexer_line();
    result.typ = typ;
    result.value = value;

    if(text != NULL) {
        result.text = (char *) calloc((strlen(text) + 1), sizeof(char));
        strcpy(result.text, text);

    } else {
        result.text = NULL;

    }

    return result;
}



//Add a token into the backlog
void addBacklog(token t)
{
    backlog[backup] = t;
    backup++;
}



//Delete and return a token from the backlog
token getBacklog()
{
    backup--;
    return backlog[backup];
}
