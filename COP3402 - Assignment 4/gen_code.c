//Fletcher Morton
//Assignment 3

/* code_gen.c */
//Generate a code sequence for the program based on the passed AST


//------------------Include/Define------------------
#include "utilities.h"
#include "gen_code.h"

#define PROC_MAX    50


//----------------------Structs---------------------
typedef struct {
    code_seq bodyc;
    address addr;
} procStruct; //Holds the code_seq for a procedure as well as its starting address


//------------------Global Variables----------------
procStruct procStack[PROC_MAX]; //Array of procStructs
int stackP; //Pointer to current location in procStack
bool nestedError = false; //If there is a nested loop, end the program


//----------------------Functions-------------------

//Initialize the code generator
void gen_code_initialize()
{
    //Initialize the procStruct array global variable
    for(int i = 0; i < PROC_MAX; i++) {
        procStack[i].bodyc = code_seq_empty();
        procStack[i].addr = 0;
    }

    //Initialize the stack pointer and set the first procedure's address to 1
    procStack[0].addr = 1;
    stackP = 0;
}

//Generate code for the given AST
code_seq gen_code_program(AST *prog)
{
    //Evaluate code for the program's procedures
    gen_code_procDecls(prog->data.program.pds);

    //Report error if the program found a nested procedure
    if(nestedError == true) {
        fprintf(stderr, "Code for nested procedures not implemented yet!\n");
        return code_seq_singleton(code_hlt());
    }

    //Create a new empty code sequence
    code_seq result = code_seq_empty();

    //If there were procedures found in gen_code_procDecls...
    if(!code_seq_is_empty(procStack[0].bodyc)) {
        //Calculate the size of all the procedure code
        int procSize = 1;
        for(int i = 0; i <= stackP; i++) {
            if(procStack[i].bodyc != code_seq_empty()){
                procSize += code_seq_size(procStack[i].bodyc);
                code_seq_fix_labels(procStack[i].bodyc); //Fix labels
            }
        }

        //Jump past all the procedure code
        result = code_seq_concat(result, code_seq_singleton(code_jmp(procSize)));

        //Concatenate the procedure code into the main code sequence
        for(int i = 0; i <= stackP; i++)
            result = code_seq_concat(result, procStack[i].bodyc);
    }

    //Generate the code to make space for the static link, dynamic link, and return address, and add it to a new sequence
    result = code_seq_add_to_end(result, code_inc(LINKS_SIZE));

    //Add the code for the program block
    result = code_seq_concat(result, gen_code_block(prog));

    //Add the HLT code
    result = code_seq_add_to_end(result, code_hlt());

    //Return the code sequence for the program
    return result;
}

//Generate code for the block
code_seq gen_code_block(AST *blk)
{
    //Add the code to allocate space for all the constants declared
    code_seq result = code_seq_singleton(gen_code_constDecls(blk->data.program.cds));

    //Add the code to allocate space for all variables declared
    result = code_seq_concat(result, gen_code_varDecls(blk->data.program.vds));

    //Add the code for the statement
    result = code_seq_concat(result, gen_code_stmt(blk->data.program.stmt));

    //Return the code sequence for the block
    return result;
}

//Generate code for the declarations in cds
code_seq gen_code_constDecls(AST_list cds)
{
    //Create an empty code sequence
    code_seq result = code_seq_empty();

    //While there are constant declarations, add the code for them into the sequence
    while(!ast_list_is_empty(cds)) {
        //Take a cd from cds and add the code for it to the sequence
        result = code_seq_concat(result, gen_code_constDecl(ast_list_first(cds)));

        //Update cds AST_list
        cds = ast_list_rest(cds);
    }

    //Return the code sequence for the const decls
    return result;
}

//Generate code for the const declaration cd
code_seq gen_code_constDecl(AST *cd)
{
    //Return the LIT code
    return code_seq_singleton(code_lit(cd->data.const_decl.num_val));
}

//Generate code for the declarations in vds
code_seq gen_code_varDecls(AST_list vds)
{
    //Create an empty code sequence
    code_seq result = code_seq_empty();

    //While there are variable declarations, add the code for them into the sequence
    while(!ast_list_is_empty(vds)) {
        //Take a vd from vds and add the code for it to the sequence
        result = code_seq_concat(result, gen_code_varDecl(ast_list_first(vds)));

        //Update vds AST_list
        vds = ast_list_rest(vds);
    }

    //Return the code sequence for the var decls
    return result;
}

//Generate code for the var declaration vd
code_seq gen_code_varDecl(AST *vd)
{
    //Return the INC code
    return code_seq_singleton(code_inc(1));
}

//Generate code for the declarations in pds
void gen_code_procDecls(AST_list pds)
{
    //While there are procedures declarations, add the code for them into the sequence
    while(!ast_list_is_empty(pds)) {
        //Take a pd from pds and add the code for it to the sequence
        gen_code_procDecl(ast_list_first(pds));

        //Update pds AST_list
        pds = ast_list_rest(pds);
    }
}

//Generate code for the procedure declaration pd
void gen_code_procDecl(AST *pd)
{
    //Bail if the code contains a nested procedure...
    if(!ast_list_is_empty(pd->data.proc_decl.block->data.program.pds)) {
        nestedError = true;
        return;
    }

    //Bail if the maximum number of procedures is about to be breached
    if(stackP == PROC_MAX - 1) bail_with_error("No space to allocate procedure!");

    //Add the code to allocate space for the procedure block
    code_seq proc = code_seq_singleton(gen_code_block(pd->data.proc_decl.block));

    //Count the variables declared within the procedure
    int procVds = 0;
    while(!ast_list_is_empty(pd->data.proc_decl.block->data.program.vds)) {
        procVds++;

        pd->data.proc_decl.block->data.program.vds = ast_list_rest(pd->data.proc_decl.block->data.program.vds);
    }

    //Count the constants declared within the procedure
    int procCds = 0;
    while(!ast_list_is_empty(pd->data.proc_decl.block->data.program.cds)) {
        procCds++;

        pd->data.proc_decl.block->data.program.cds = ast_list_rest(pd->data.proc_decl.block->data.program.cds);
    }

    //Take any constants or variables declared within the procedure off of the stack
    if(procVds + procCds > 0) proc = code_seq_add_to_end(proc, code_inc(-(procVds+procCds)));

    //Add the RTN code
    proc = code_seq_add_to_end(proc, code_rtn());

    //Add the procedure's code sequence to the procStack for access later
    procStack[stackP].bodyc = proc;

    //Calculate the starting address for the procedure
    if(stackP > 0)
        procStack[stackP].addr = procStack[stackP-1].addr + code_seq_size(procStack[stackP-1].bodyc);

    //Set the procedure's label
    label_set(pd->data.proc_decl.lab, procStack[stackP].addr);

    //Increment the array pointer
    stackP++;
}

//Generate code for the statement
code_seq gen_code_stmt(AST *stmt)
{
    //Determine which type of stmt was encountered
    switch(stmt->type_tag) {
        case assign_ast:
            return gen_code_assignStmt(stmt);

        case call_ast:
            return gen_code_callStmt(stmt);

        case begin_ast:
            return gen_code_beginStmt(stmt);

        case if_ast:
            return gen_code_ifStmt(stmt);

        case while_ast:
            return gen_code_whileStmt(stmt);

        case read_ast:
            return gen_code_readStmt(stmt);

        case write_ast:
            return gen_code_writeStmt(stmt);

        case skip_ast:
            return gen_code_skipStmt(stmt);

        default:
            bail_with_error("Bad AST passed to code_gen_stmt!");
            break;
    }

    //End of function return
    return code_seq_empty();
}

//Generate code for the assign statement
code_seq gen_code_assignStmt(AST *stmt)
{
    //Get the fp for the variable on top of the stack
    unsigned int outLevels = stmt->data.assign_stmt.ident->data.ident.idu->levelsOutward;
    code_seq result = code_compute_fp(outLevels);

    //Get value of the expression on top of the stack
    result = code_seq_concat(result, gen_code_expr(stmt->data.assign_stmt.exp));

    //Add the STO code with the variable offset
    unsigned int ofst = stmt->data.assign_stmt.ident->data.ident.idu->attrs->loc_offset;
    result = code_seq_add_to_end(result, code_sto(ofst));

    //Return the assign statement code sequence
    return result;
}

//Generate code for the call statement
code_seq gen_code_callStmt(AST *stmt)
{
    //Return the CAL code
    return code_seq_singleton(code_cal(stmt->data.call_stmt.ident->data.ident.idu->attrs->lab));
}

//Generate code for the begin statement
code_seq gen_code_beginStmt(AST *stmt)
{
    //Create a new code sequence
    code_seq result = code_seq_empty();

    //While there are statements contained, add their code to the sequence
    AST_list stmts = stmt->data.begin_stmt.stmts;
    while(!ast_list_is_empty(stmts)) {
        //Add the code
        result = code_seq_concat(result, gen_code_stmt(ast_list_first(stmts)));

        //Update the list
        stmts = ast_list_rest(stmts);
    }

    //Return the begin statement code sequence
    return result;
}

//Generate code for the if statement
code_seq gen_code_ifStmt(AST *stmt)
{
    //Code for pushing the condition on top of the stack
    code_seq condc = gen_code_cond(stmt->data.if_stmt.cond);
    code_seq thenc = gen_code_stmt(stmt->data.if_stmt.thenstmt);
    code_seq elsec = gen_code_stmt(stmt->data.if_stmt.elsestmt);

    //JPC 2
    code_seq result = code_seq_add_to_end(condc, code_jpc(2));

    //JMP[around then]
    result = code_seq_add_to_end(result, code_jmp(code_seq_size(thenc)+2));

    //Code for then statement
    result = code_seq_concat(result, thenc);

    //JMP[around else]
    result = code_seq_add_to_end(result, code_jmp(code_seq_size(elsec)+1));

    //Code for else statement
    result = code_seq_concat(result, elsec);

    //Return the if statement code sequence
    return result;
}

//Generate code for the while statement
code_seq gen_code_whileStmt(AST *stmt)
{
    //Code for pushing the condition on top of the stack
    code_seq condc = gen_code_cond(stmt->data.while_stmt.cond);
    code_seq bodyc = gen_code_stmt(stmt->data.while_stmt.stmt);

    //JPC 2
    code_seq result = code_seq_add_to_end(condc, code_jpc(2));

    //JMP[around body]
    result = code_seq_add_to_end(result, code_jmp(code_seq_size(bodyc)+2));

    //Code for body
    result = code_seq_concat(result, bodyc);

    //JMP[back to cond]
    result = code_seq_add_to_end(result, code_jmp(-(code_seq_size(condc))));

    //Return the while statement code sequence
    return result;
}

//Generate code for the read statement
code_seq gen_code_readStmt(AST *stmt)
{
    //Code to put the fp for the variable on top of the stack
    id_use *idu = stmt->data.read_stmt.ident->data.ident.idu;
    code_seq result = code_compute_fp(idu->levelsOutward);

    //Add the CHI code
    result = code_seq_add_to_end(result, code_chi());

    //Add the STO code with the variable offset
    result = code_seq_add_to_end(result, code_sto(idu->attrs->loc_offset));

    //Return the read statement code
    return result;
}

//Generate code for the write statement
code_seq gen_code_writeStmt(AST *stmt)
{
    //Code to put the expression's value on top of the stack
    code_seq result = gen_code_expr(stmt->data.write_stmt.exp);

    //Add the CHO code
    return code_seq_add_to_end(result, code_cho());
}

//Generate code for the skip statement
code_seq gen_code_skipStmt(AST *stmt)
{
    //Return the NOP code
    return code_seq_singleton(code_nop());
}

//Generate code for the condition
code_seq gen_code_cond(AST *cond)
{
    //See which type of condition was encountered
    switch(cond->type_tag) {
        case bin_cond_ast:
            return gen_code_bin_cond(cond);

        case odd_cond_ast:
            return gen_code_odd_cond(cond);

        default:
            bail_with_error("gen_code_cond passed bad AST!");
            break;
    }

    //End of function return
    return code_seq_empty();
}

//Generate code for the odd condition
code_seq gen_code_odd_cond(AST *cond)
{
    //Add the code for the odd condition's expression
    code_seq result = code_seq_singleton(gen_code_expr(cond->data.odd_cond.exp));

    //Add the code to push the 2 literal on the stack
    result = code_seq_add_to_end(result, code_lit(2));

    //Add the mod code and return the code sequence
    return code_seq_add_to_end(result, code_mod());
}

//Generate code for the binary condition
code_seq gen_code_bin_cond(AST *cond)
{
    //Code to push left exp's value on top of the stack
    code_seq result = gen_code_expr(cond->data.bin_cond.leftexp);

    //Code to push right exp's value on top of the stack
    result = code_seq_concat(result, gen_code_expr(cond->data.bin_cond.rightexp));

    //Instruction that implements the relational op
    switch(cond->data.bin_cond.relop) {
        case eqop:
            return code_seq_add_to_end(result, code_eql());

        case gtop:
            return code_seq_add_to_end(result, code_gtr());

        case geqop:
            return code_seq_add_to_end(result, code_geq());

        case ltop:
            return code_seq_add_to_end(result, code_lss());

        case leqop:
            return code_seq_add_to_end(result, code_leq());

        case neqop:
            return code_seq_add_to_end(result, code_neq());

        default:
            bail_with_error("gen_code_bin_cond passed AST with bad op!");
            break;
    }

    //End of function return
    return code_seq_empty();
}

//Generate code for the expression
code_seq gen_code_expr(AST *exp)
{
    //See which type of expression was encountered
    switch(exp->type_tag) {
        case number_ast:
            return gen_code_number_expr(exp);

        case ident_ast:
            return gen_code_ident_expr(exp);

        case bin_expr_ast:
            return gen_code_bin_expr(exp);

        case op_expr_ast:
            return gen_code_bin_cond(exp);

        default:
            bail_with_error("gen_code_expr passed bad AST!");
            break;
    }

    //End of function return
    return code_seq_empty();
}

//Generate code for the binary expression
code_seq gen_code_bin_expr(AST *exp)
{
    //Code to push left exp's value on top of the stack
    code_seq result = gen_code_expr(exp->data.bin_expr.leftexp);

    //Code to push right exp's value on top of the stack
    result = code_seq_concat(result, gen_code_expr(exp->data.bin_expr.rightexp));

    //Instruction that implements the arithmetic op
    switch(exp->data.bin_expr.arith_op) {
        case addop:
            return code_seq_add_to_end(result, code_add());

        case subop:
            return code_seq_add_to_end(result, code_sub());

        case multop:
            return code_seq_add_to_end(result, code_mul());

        case divop:
            return code_seq_add_to_end(result, code_div());

        default:
            bail_with_error("gen_code_bin_expr passed AST with bad op!");
            break;
    }

    //End of function return
    return code_seq_empty();
}

//Generate code for the ident expression
code_seq gen_code_ident_expr(AST *ident)
{
    //Code to load fp for the variable
    id_use *idu = ident->data.ident.idu;

    //Add the STO code with the variable offset
    lexical_address *la = lexical_address_create(idu->levelsOutward, idu->attrs->loc_offset);

    //Return the code for the indent expr
    return code_load_from_lexical_address(la);
}

//Generate code for the number expression
code_seq gen_code_number_expr(AST *num)
{
    //Return the LIT code
    return code_seq_singleton(code_lit(num->data.number.value));
}
