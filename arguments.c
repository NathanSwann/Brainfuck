#include <argp.h>
#include <stdbool.h>
#include "arguments.h"

const char *argp_program_version = "1.0.0";
const char *argp_program_bug_address = "please@dont";
static char doc[] = "Simple optimizing Brain Fuck Compiler";

static char args_doc[] = "FILENAME";

static struct argp_option options[] = {
    {"noassemble", 1, 0, 0, "Do not run the assembler and instead stop after assembly generation."},
    {0}};



static error_t parse_options(int key, char *arg, struct argp_state *state)
{
    BF_ARGS *arguments = state->input;
    switch (key)
    {
    case 1:
        arguments->assemble= false;
        break;
    case ARGP_KEY_ARG:
        arguments->in_file_path=arg;
        return 0;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_options, args_doc, doc, 0, 0, 0};

BF_ARGS parse_arguments(int argc, char *argv[])
{
    BF_ARGS arguments;
    arguments.mode = COMPILE_MODE;
    arguments.assemble = true;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    return arguments;
}