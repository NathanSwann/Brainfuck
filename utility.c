#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "utility.h"
#include "a.h"

Vt bf_fixed_stack_init(BF_FIXED_STACK *stack, SZt stack_size, SZt element_size)
{
    stack->curr_item = 0;
    stack->stack_size = stack_size;
    stack->element_size = element_size;
    stack->data = malloc(stack_size * element_size);
    MEMCL(stack->data, stack_size * element_size); // enusre the data is actually clear
}

Vt bf_fixed_stack_destroy(BF_FIXED_STACK *st)
{
    free(st->data);
    st->stack_size = 0; // setting to zero st when attempting to pop will return null
}

Vt *bf_fixed_stack_alloc(BF_FIXED_STACK *st)
{
    if (st->stack_size <= st->curr_item)
        R(NULL);
    Vt *p = st->data + st->curr_item * st->element_size;
    st->curr_item++;
    R(p);
}

Vt *bf_fixed_stack_pop(BF_FIXED_STACK *st)
{
    if (st->curr_item == 0)
        R(NULL);
    st->curr_item--;
    R(st->data + st->curr_item * st->element_size);
}

STR load_file(STR x)
{
    FILE *fd = fopen(x, "r");
    if (fd == NULL)
    {
        printf("%s: %s\n", x, strerror(errno));
        exit(1);
    }
    fseek(fd, 0, SEEK_END);
    It fl = ftell(fd);
    rewind(fd);
    It p = 0;
    STR d = calloc(fl, 1);
    DO(p < fl)
    {
        fgets(d + p, fl + 1 - p, fd);
        p = ftell(fd);
    }
    fclose(fd);
    R(d);
}
