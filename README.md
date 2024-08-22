# Super Simple Brainfuck Compiler

A simple compiler written in C that attempts to compile and optimize Brainfuck programs for elf64 executables for Linux.
Ultimately this is a learning project and is by no means the best one out there, but at the very least in number crunching based
benchmarks this compiler produces executables that run faster than naive python implementations.

**THIS WILL NOT WORK ON WINDOWS**

## Building / Usage

To build:

```
bash build.sh

./build/bf examples/collatz.bf
./a.out < examples/numbers
```

## Project Overview:

- `a.*` : basic macros used throughout
- `arguments.*` : command line args parsing
- `asm_data.*` : although this file does not exist on first clone it is the contents of `asm/` in header form
- `ir.*`: definition of the intermediary representation
- `m.c` : entrypoint as well as the main lexer and compiler
- `optimizer.*` : location of all optimization code
- `utility.*` : random bits to make the code actually work

