
#include <stdio.h>

extern void asm_entry (void);

int test_variable = 1;

int main (void)
{
    asm_entry ();
    return 0;
}

