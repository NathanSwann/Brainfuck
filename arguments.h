#include <stdbool.h>

typedef struct 
{
    enum
    {
        EMULATE_MODE,
        COMPILE_MODE
    } mode;
    char* in_file_path;
    bool assemble;
} BF_ARGS;

BF_ARGS parse_arguments(int argc, char *argv[]);