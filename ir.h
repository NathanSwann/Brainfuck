#ifndef BF_IR
#define BF_IR
#include <stddef.h>
#include <stdint.h>

typedef enum BF_OP_KIND
{
    NOP,
    LFT,
    RHT,
    INC,
    DEC,
    INP,
    OUT,
    JMP,
    RET,
    SET_VALUE,
    MUL_ADD_COPY,
    EMPTY_RET,
    EMPTY_JMP
} BF_OP_KIND;

typedef struct
{
    BF_OP_KIND k;
    __uint64_t op;
} BF_OP;

typedef struct
{
    BF_OP *is;
    __uint64_t l;
} BF_PROG;

typedef struct
{
    __int64_t offset;
    __int64_t factor;
} MUL_OFFSET_DATA;


#endif