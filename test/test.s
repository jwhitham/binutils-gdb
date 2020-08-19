
.global asm_loop
.global _asm_loop

asm_loop:
_asm_loop:

    mov $1000, %ecx
    inc %eax
    inc %eax
0:
    dec %eax
    dec %eax
breakpoint:
    add $0x90909090, %eax

    dec %eax
    dec %eax
    loop 0b
    inc %eax
    inc %eax

    ret

