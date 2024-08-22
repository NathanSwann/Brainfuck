#!/bin/bash

generate_asm() {
    echo -e "#ifndef BF_ASM_DATA\n#define BF_ASM_DATA"
    files=$(find asm/ -name '*.asm')
    for asm in $files; do
        xxd -i $asm
    done
    echo "#endif"
}

generate_asm > asm_data.h
cc m.c arguments.c utility.c optimizer.c -o build/bf
