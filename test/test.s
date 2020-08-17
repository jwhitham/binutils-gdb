
.global asm_entry
.global _asm_entry

asm_entry:
_asm_entry:
    nop

test1:
    pushf
    pop %eax

    pushf
    pop %ecx

test2:
    nop
    nop
    ret

