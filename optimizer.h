#include "ir.h"

BF_PROG simplify_clear_commands(BF_PROG in);
BF_PROG copy_and_multiply_commands(BF_PROG in);
BF_PROG add_offsets(BF_PROG in);
BF_PROG remove_unneeded_jumps(BF_PROG in);