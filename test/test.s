
.global asm_entry
.global _asm_entry

asm_entry:
_asm_entry:
    xor %edx, %edx
    mov test1, %eax
    mov test2, %ecx

/* breaking on test1 causes the TRAP flag to be set */
test1:
    pushf
    pop %eax

    pushf
    pop %ecx

/* TRAP visible in EAX but not in ECX */
test2:
    nop
    nop

/* So what about this? */
    push %eax
    popf
test3:
    inc %edx
    dec %edx

    cmp $0, %edx
    jnz explode

/* or this? */
    push %eax
test4:
    popf
    inc %edx
    dec %edx

    cmp $0, %edx
    jnz explode

    mov $10, %eax
/* expect 10 visits to test5 - but actually the BP only triggers once,
   unless we are using software single step, because there is no other
   BP in the loop */
test5:
    dec %eax
    jnz test5

    nop
    nop
    nop
    ret


explode:
    hlt
    int3
    hlt
    int3
    hlt
    int3
