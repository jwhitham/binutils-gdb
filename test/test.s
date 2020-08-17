
.global asm_entry
.global _asm_entry

asm_entry:
_asm_entry:
/* check which instruction is used as the breakpoint:
 * should be HLT (0xf4) */
    mov test1, %al
test1:
    cmp $0xf4, %al
    jnz explode

/* pushf after a breakpoint should not have the trap flag set */
test2:
    pushf
    pop %eax

    pushf
    pop %ecx

    cmp %eax, %ecx
    jnz explode

/* test software single stepping with a branch */
    mov $4, %ecx
1:
    nop
0:
    dec %ecx
    cmp $2, %ecx
test3:
    jz 1b
    cmp $0, %ecx
    jnz 0b

/* call */
    mov $4, %ecx
0:
    dec %ecx
test4:
    call sample_proc
    cmp $0, %ecx
    jnz 0b

    mov $4, %eax
/* expect 4 visits to test5 - without single step, the BP triggers only once */
0:
    dec %eax
test5:
    jnz 0b

    mov $4, %ecx
/* same basic idea but use loop instruction */
0:
    nop
test6:
    loop 0b

/* Now we will begin to see some warnings, because hardware single step will be forced. */
/* Firstly, by the use of ret */
    call test7
/* repz ret */
    call test8

/* in fact, any use of rep will trigger hardware single step */
    mov $scratch, %edi
    mov $4, %ecx
    mov $0x88, %al
    cld
test9:
    rep stosb

    cmpl $0x88888888, scratch
    jnz explode

/* as will an indirect branch */
    mov $sample_proc, %eax
test10:
    call *%eax

/* In order to pass test11, we must change the register values, so that EAX = ECX */
    mov $explode, %eax
    mov $0f, %ecx
test11:
    nop
    jmp *%eax

0:
    nop

    mov $4, %ecx
/* very tight loop - must use single step */
test12:
    loop test12

/* add more tests here */
    nop

end_of_test:
    nop
    ret





sample_proc:
    ret

test7:
    ret

test8:
    repz ret

explode:
    ud2

.data
scratch:
    .long 0x12345678

