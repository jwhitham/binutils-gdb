
#include <stdio.h>

extern void asm_entry (void);

int main (void)
{
    asm_entry ();
    return 0;
}

