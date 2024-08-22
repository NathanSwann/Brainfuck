#include <stddef.h>

typedef struct
{
    size_t stack_size;  // size of the stack
    size_t curr_item;  // current top of the stack
    size_t element_size; // size of a single element
    void* data;   // actual data
} BF_FIXED_STACK;

void bf_fixed_stack_init(BF_FIXED_STACK* stack,size_t size,size_t element_size);
void bf_fixed_stack_destroy(BF_FIXED_STACK* stack);

void* bf_fixed_stack_alloc(BF_FIXED_STACK* stack);
void* bf_fixed_stack_pop(BF_FIXED_STACK* stack);

char* load_file(char* x);