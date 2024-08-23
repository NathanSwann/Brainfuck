/*
Brainfuck
*/
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "a.h"
#include "utility.h"
#include "ir.h"
#include "arguments.h"
#include "optimizer.h"
#include "asm_data.h"

BF_PROG bf_lex(STR prog)
{ // Turn string input of a bf programme into IR TODO: add optimization steps here
    BF_OP *is = (BF_OP *)malloc(strlen(prog) * SIZE(BF_OP));
    It ip = 0; // Allocate the instruction list and pointer
    MEMCL(is, strlen(prog) * SIZE(BF_OP));
    BF_FIXED_STACK js = {0};
    bf_fixed_stack_init(&js, 500, SIZE(It)); // Instantiate a stack for the locations of unclosed [
    BF_OP_KIND lastk = NOP;
    ITR(pi, strlen(prog))
    {
        switch (prog[pi])
        {                                                 // For every char in input:
        case '[':                                         //  [: jump forward ins
            It *j = (It *)bf_fixed_stack_alloc(&js);      //   Attempt to allocate a new indx for this jump
            ASSERTFALSE(j == NULL, "SO IN JUMP STACK\n"); //   If it failed then just crash
            *j = ip;
            is[ip].k = JMP;
            is[ip].op = ((U64t)ip) << 32;
            ip++;
            break;                                                //   jump without a jmp location and add the ip to the stack
        case ']':                                                 //  ]: jump backward ins
            j = (It *)bf_fixed_stack_pop(&js);                    //   get the most recent unclosed [
            ASSERTFALSE(j == NULL, "CLOSING NON OPEN BRACKET\n"); //   if there isnt one then the input is malformed
            // printf("%ld %ld\n",is[*j].op,ip);
            is[*j].op |= ip;
            is[ip].k = RET;
            is[ip].op = (((U64t)ip) << 32) | *j;
            ip++;
            break; //   update the unclosed [ with ip and ] with [ location
        case '+':
            if (INC == lastk)
                ip = (ip == 0) ? 0 : ip - 1;
            is[ip].k = INC;
            is[ip].op += 1;
            ip++;
            break; //  +: increment cell
        case '-':
            if (DEC == lastk)
                ip = (ip == 0) ? 0 : ip - 1;
            is[ip].k = DEC;
            is[ip].op += 1;
            ip++;
            break; //  -: decrement cell
        case '>':
            if (RHT == lastk)
                ip = (ip == 0) ? 0 : ip - 1;
            is[ip].k = RHT;
            is[ip].op += 1;
            ip++;
            break; //  >: shift to left cell
        case '<':
            if (LFT == lastk)
                ip = (ip == 0) ? 0 : ip - 1;
            is[ip].k = LFT;
            is[ip].op += 1;
            ip++;
            break; //  <: shift to right cell
        case '.':
            is[ip].k = OUT;
            is[ip].op += 1;
            ip++;
            break; //  .: output cell to stdout
        case ',':
            is[ip].k = INP;
            is[ip].op += 1;
            ip++;
            break; //  ,: input byte to cell
        default:
            break;
        }
        lastk = is[ip - 1].k;
    } //  otherwise must be a comment
    It x = js.curr_item;
    bf_fixed_stack_destroy(&js);               // free the jump stack
    ASSERTFALSE(x != 0, "UNCLOSED BRACKET\n"); // verify all [ jumps have be closed
    BF_PROG p = {is, ip};
    R(p);
}

void bf_to_asm(FILE *fd, BF_PROG prog)
{
    fprintf(fd, "global _start\nsection .text\n");
    fprintf(fd, "%.*s\n", asm_buffered_out_asm_len, asm_buffered_out_asm);
    fprintf(fd, "%.*s\n", asm_buffered_input_asm_len, asm_buffered_input_asm);
    fprintf(fd, "_start:\n");
    ITR(i, prog.l)
    {
        switch (prog.is[i].k)
        {
        case LFT:
            fprintf(fd, "sub r9, %ld\n", prog.is[i].op);
            break;
        case RHT:
            fprintf(fd, "add r9, %ld\n", prog.is[i].op);
            break;
        case INC:
            U32t amount = (U32t)prog.is[i].op;
            I32t offset = (I32t)(prog.is[i].op>>32);
            fprintf(fd, "add byte [tape+r9*1 + %d], %d\n", offset, amount);
            break;
        case DEC:
            amount = (U32t)prog.is[i].op;
            offset = (I32t)(prog.is[i].op>>32);
            fprintf(fd, "sub byte [tape+r9*1 + %d], %d\n", offset, amount);
            break;
        case OUT:
            fprintf(fd, "call output\n");
            // fprintf(fd,"mov rsi, tape\n");
            // fprintf(fd,"add rsi, r9\n");
            // fprintf(fd,"mov rdx, 1\n");
            // fprintf(fd,"mov rax, 1\n");
            // fprintf(fd,"mov rdi, 1\n");
            // fprintf(fd,"syscall\n");
            break;
        case INP:
            fprintf(fd, "call input\n");
            // fprintf(fd,"mov rsi, tape\n");
            // fprintf(fd,"add rsi, r9\n");
            // fprintf(fd,"mov rdx, 1\n");
            // fprintf(fd,"mov rax, 0\n");
            // fprintf(fd,"mov rdi, 0\n");
            // fprintf(fd,"syscall\n");
            break;
        case JMP:
            fprintf(fd, "mov r8b, byte [tape+r9*1]\n");
            fprintf(fd, "test r8b,r8b\n");
            fprintf(fd, "jz .ret_%ld\n", (prog.is[i].op << 32) >> 32);
        case EMPTY_JMP:
            fprintf(fd, ".jmp_%ld:\n", prog.is[i].op >> 32);
            break;
        case RET:
            fprintf(fd, "mov r8b,byte [tape+r9*1]\n");
            fprintf(fd, "test r8b,r8b\n");
            fprintf(fd, "jnz .jmp_%ld\n", (prog.is[i].op << 32) >> 32);
        case EMPTY_RET:
            fprintf(fd, ".ret_%ld:\n", prog.is[i].op >> 32);
            break;
        case SET_VALUE:
            amount = (U32t)prog.is[i].op;
            offset = (I32t)(prog.is[i].op>>32);
            fprintf(fd, "mov byte [tape+r9*1+%d], %d\n", offset,amount);
            break;
        case MUL_ADD_COPY:
            fprintf(fd, ";;MUL\n");
            MUL_OFFSET_DATA *mul_ins = (MUL_OFFSET_DATA *)prog.is[i].op; // yes i know
            int a = 0;
            while (mul_ins[a].offset != 0)
            {
                fprintf(fd, "mov al, byte [tape+r9*1]\n");
                fprintf(fd, "mov rdx, %ld\n", mul_ins[a].factor);
                fprintf(fd, "mul rdx\n");
                fprintf(fd, "add byte[tape + r9*1 + %ld], al\n", mul_ins[a].offset);
                a++;
            }
            fprintf(fd, "mov byte[tape + r9*1],0\n");
            fprintf(fd, ";;endMUL\n");
            break;
        default:
            printf("UNHANDLED OPERATOR!\n");
            printf("%d\n", prog.is[i].k == INP);
            fflush(stdout);
            exit(2);
            break;
        }
    }
    fprintf(fd, "%.*s\n", asm_cleanup_asm_len, asm_cleanup_asm);
    fprintf(fd, "%.*s\n", asm_data_segment_asm_len, asm_data_segment_asm);
}

int main(int argc, STR *argv)
{
    BF_ARGS args = parse_arguments(argc, argv);
    if (args.in_file_path == NULL)
    {
        printf("No Input File Provided see --help\n");
        exit(0);
    }
    STR data = load_file(args.in_file_path);
    BF_PROG a = bf_lex(data);
    // TODO: CLEAN THIS UP
    BF_PROG opt = simplify_clear_commands(a);
    BF_PROG opt2 = copy_and_multiply_commands(opt);
    BF_PROG opt3 = add_offsets(opt2);
    BF_PROG opt4 = remove_unneeded_jumps(opt3);
    FILE *fd = fopen("a.asm", "w");
    if (args.mode == COMPILE_MODE)
    {
        bf_to_asm(fd, opt4);
        fclose(fd);
        if (args.assemble)
        {
            system("nasm -felf64 a.asm && ld a.o");
        }
    }
    free(opt.is);
    return 2;
}