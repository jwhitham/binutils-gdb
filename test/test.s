
.global asm_loop
.global _asm_loop
.global breakpoint
.global _breakpoint
.global avoid_danger
.global _avoid_danger

asm_loop:
_asm_loop:

    mov $1000, %ecx
    inc %eax
    inc %eax
0:
    dec %eax
    dec %eax

    /* must be 5 byte instruction: */
breakpoint:
_breakpoint:
    add $0x90909090, %eax

    dec %eax
    dec %eax
    loop 0b
    inc %eax
    inc %eax

    ret


avoid_danger:
_avoid_danger:
    in $0xcc, %eax
    ret
