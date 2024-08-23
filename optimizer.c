#include "ir.h"
#include <stdlib.h>
#include "a.h"
#include "utility.h"
#include <stdio.h>
#include <stdbool.h>

BF_PROG simplify_clear_commands(BF_PROG in)
{
    BF_OP *is = calloc(in.l, SIZE(BF_OP));
    is[0] = in.is[0]; // cheap hack so we can start from the rollowing window
    U64t ip = 0;
    ITRFROM(i, 1, in.l)
    {
        if (i < in.l - 1 && is[ip].k == JMP && (in.is[i].k == INC || in.is[i].k == DEC) && in.is[i + 1].k == RET)
        {
            is[ip].k = SET_VALUE;
            is[ip].op = 0;
            i++; // skip the next instruction since it will be a return to a jump we deleted.
        }
        else if (is[ip].k == SET_VALUE && in.is[i].k == INC)
        {
            is[ip].op = in.is[i].op;
        }
        else if (is[ip].k == SET_VALUE && in.is[i].k == DEC)
        {
            is[ip].op = 255 - in.is[i].op;
        }
        else
        {
            ip++;
            is[ip] = in.is[i];
        }
    }
    BF_PROG p = {is, ip + 1};
    return p;
}

BF_OP optimize_mul_and_copy(BF_OP *segment, U64t length)
{
    U64t mem_size = 20;
    MUL_OFFSET_DATA *mul_ins = calloc(mem_size + 1, SIZE(MUL_OFFSET_DATA));
    MEMCL(mul_ins, (1 + mem_size) * SIZE(MUL_OFFSET_DATA));
    U64t mul_p = 0;
    I64t offset = 0;
    ITR(i, length)
    {
        if (segment[i].k == LFT)
        {
            offset -= segment[i].op;
        }
        if (segment[i].k == RHT)
        {
            offset += segment[i].op;
        }
        if (segment[i].k == INC && offset != 0)
        {
            if (mul_p < mem_size)
            {
                MUL_OFFSET_DATA ins = {offset, segment[i].op};
                mul_ins[mul_p] = ins;
            }
            else
            {
                BF_OP x = {0, (U64t)mul_ins};
                return x;
            }
            mul_p++;
        }
        if (segment[i].k == DEC && offset != 0)
        {
            if (mul_p < mem_size)
            {
                MUL_OFFSET_DATA ins = {offset, -1 * segment[i].op};
                mul_ins[mul_p] = ins;
            }
            else
            {
                BF_OP x = {0, (U64t)mul_ins};
                return x;
            }
            mul_p++;
        }
    }
    BF_OP x = {MUL_ADD_COPY, (U64t)mul_ins};
    return x;
}

/*
Conditions:
- Inside a loop with no other loops
- the loop starts and stops on the same cell
- that cell is decremented by only 1
e.g.:
[->+>+<<]
or
[>+>++<<-]
*/
BF_PROG copy_and_multiply_commands(BF_PROG in)
{
    U64t loop_start = 0;
    I64t shifts = 0;
    bool in_valid_loop = false;
    I64t main_change = 0;
    U64t ip = 0;
    BF_OP *is = calloc(in.l, SIZE(BF_OP));
    ITR(i, in.l)
    {
        if (in.is[i].k == RET)
        {
            if (in_valid_loop && shifts == 0 && main_change == -1)
            {
                BF_OP ins = optimize_mul_and_copy(is + loop_start + 1, ip - loop_start - 1);
                if (ins.k == MUL_ADD_COPY)
                {
                    ip = loop_start;
                    is[ip] = ins;
                    in_valid_loop = false;
                    ip++;
                    continue;
                }
                free((void *)ins.op);
            }
            in_valid_loop = false;
        }
        if (!(in.is[i].k == INC || in.is[i].k == DEC || in.is[i].k == LFT || in.is[i].k == RHT))
        {
            in_valid_loop = false;
        }
        if (in.is[i].k == LFT)
        {
            shifts -= in.is[i].op;
        }
        if (in.is[i].k == RHT)
        {
            shifts += in.is[i].op;
        }
        if (in.is[i].k == DEC && shifts == 0)
        {
            main_change -= in.is[i].op;
        }
        if (in.is[i].k == INC && shifts == 0)
        {
            main_change -= in.is[i].op;
        }
        if (in.is[i].k == JMP)
        {
            in_valid_loop = true;
            loop_start = ip;
            shifts = 0;
            main_change = 0;
        }
        is[ip] = in.is[i];
        ip++;
    }
    BF_PROG p = {is, ip};
    return p;
}

BF_PROG add_offsets(BF_PROG in)
{
    U64t ip = 0;
    BF_OP *is = calloc(in.l, SIZE(BF_OP));
    I32t current_shift_offset = 0;
    ITR(i, in.l)
    {
        if (in.is[i].k == LFT)
        {
            current_shift_offset -= (I32t)in.is[i].op;
        }
        else if (in.is[i].k == RHT)
        {
            current_shift_offset += (I32t)in.is[i].op;
        }
        else if (in.is[i].k == INC || in.is[i].k == DEC || in.is[i].k == SET_VALUE)
        {
            is[ip] = in.is[i];
            is[ip].op = (((U64t)current_shift_offset) << 32) | is[ip].op;
            ip ++;
        } else {
            if (current_shift_offset < 0) {
                BF_OP op = {LFT,(U64t)abs(current_shift_offset)};
                is[ip] = op;
                ip++;
            }
            if (current_shift_offset > 0) {
                BF_OP op = {RHT,(U64t)current_shift_offset};
                is[ip] = op;
                ip++;
            }
            is[ip] = in.is[i];
            ip++;
            current_shift_offset = 0;
            
        }
    }
    BF_PROG p = {is, ip};
    return p;
}

BF_PROG remove_unneeded_jumps(BF_PROG in)
{
    U64t ip = 0;
    BF_OP *is = calloc(in.l, SIZE(BF_OP));
    ITR(i, in.l)
    {
        if (i>0 && in.is[i].k == JMP && (in.is[i-1].k == JMP || in.is[i-1].k == EMPTY_JMP))
        {
            is[ip] = in.is[i];
            is[ip].k = EMPTY_JMP;
            ip++;
        }
        else if (i>0 && in.is[i].k == RET && (in.is[i-1].k == RET || in.is[i-1].k == EMPTY_RET))
        {
            is[ip] = in.is[i];
            is[ip].k = EMPTY_RET;
            ip++;
        }
        else {
            is[ip] = in.is[i];
            ip++;            
        }
    }
    BF_PROG p = {is, ip};
    return p;
}