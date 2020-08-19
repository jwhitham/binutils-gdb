
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

volatile BOOL run = TRUE;
HANDLE thread1_handle;
HANDLE thread2_handle;

unsigned danger_unknown = 0;
unsigned safe_int_pending = 0;
unsigned safe_int_triggered = 0;
unsigned danger_int_removed = 0;
unsigned danger_step_pending = 0;
unsigned danger_step_finished = 0;
unsigned danger_int_unrestored = 0;
unsigned safe_int_removed = 0;
unsigned safe_step_pending = 0;
unsigned safe_step_finished = 0;
unsigned safe_int_unrestored = 0;
unsigned safe_after = 0;
unsigned safe_elsewhere = 0;

extern void asm_loop (void);
extern void breakpoint (void);
extern int avoid_danger (void);

void error (const char * text)
{
    printf ("%s -> %d\n", text, (int) GetLastError());
    exit (1);
}


DWORD WINAPI thread1 (LPVOID arg)
{
    while (run) {
        asm_loop ();
    }
    return 0;
}

DWORD WINAPI thread2 (LPVOID arg)
{
    CONTEXT context;
    BOOL bp_present;
    BOOL trap_flag;
    DWORD bp_loc = (DWORD) ((intptr_t) ((void *) breakpoint));

    memset (&context, 0, sizeof (CONTEXT));

    while (run) {
        /* thread2: State T1 */
        if (SuspendThread (thread1_handle) == (DWORD) -1) {
            error ("SuspendThread");
        }
        context.ContextFlags =
                CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_i386;

        /* thread2: State T2 */
        if (!GetThreadContext (thread1_handle, &context)) {
            error ("GetThreadContext");
        }

        /* thread2: State T3 */
        bp_present = ((((volatile uint8_t *) breakpoint)[0]) == 0xcc);
        trap_flag = !!(context.EFlags & 0x100);

        if (context.Eip == bp_loc) {
            if (trap_flag) {
                if (bp_present) {
                    /* Step pending on a breakpoint? Should not happen. */
                    danger_unknown++;
                } else {
                    /* Danger, TF is set, we're about to step */
                    /* thread1: State M4 */
                    if (avoid_danger()) {
                        safe_step_pending++;
                    } else {
                        danger_step_pending++;
                    }
                }
            } else {
                if (bp_present) {
                    /* Safe: About to run the interrupt, but it's known */
                    /* thread1: State M1 */
                    safe_int_pending++;
                } else {
                    /* Danger: interrupt removed, not in single step. */
                    danger_unknown++;
                }
            }
        } else if (context.Eip == 1 + bp_loc) {
            if (trap_flag) {
                /* Should not see this due to simultaneous change to
                 * EIP and EFLAGS */
                danger_unknown++;
            } else {
                if (bp_present) {
                    /* Safe: Just ran the interrupt, but it's known */
                    /* thread1: State M2 */
                    safe_int_triggered++;
                } else {
                    /* Danger: interrupt removed, not in single step. */
                    /* thread1: State M3 */
                    if (avoid_danger()) {
                        safe_int_removed++;
                    } else {
                        danger_int_removed++;
                    }
                }
            }
        } else if (context.Eip == 5 + bp_loc) {
            if (trap_flag) {
                /* Danger, TF is still set */
                /* thread1: State M5 */
                if (avoid_danger()) {
                    safe_step_finished++;
                } else {
                    danger_step_finished++;
                }
            } else {
                if (bp_present) {
                    /* Safe: we're clear of the breakpoint */
                    safe_after++;
                } else {
                    /* Danger: BP not restored */
                    /* thread1: State M6 */
                    if (avoid_danger()) {
                        safe_int_unrestored++;
                    } else {
                        danger_int_unrestored++;
                    }
                }
            }
        } else if ((context.Eip > (5 + bp_loc)) || (context.Eip < bp_loc)) {
            if (trap_flag) {
                /* Should not see this: TF not used elsewhere */
                danger_unknown++;
            } else {
                if (bp_present) {
                    /* Safe: we're clear of the breakpoint */
                    safe_elsewhere++;
                } else {
                    /* Danger: BP not restored */
                    danger_unknown++;
                }
            }
        } else {
            /* Danger: we're in the middle of an instruction */
            danger_unknown++;
        }

        if (ResumeThread (thread1_handle) == (DWORD) -1) {
            error ("ResumeThread");
        }
    }
    return 0;
}

int main (void)
{
    run = TRUE;

    thread1_handle = CreateThread (NULL, 0, thread1, NULL, 0, NULL);
    if (!thread1_handle) {
        error ("CreateThread 1");
    }
    thread2_handle = CreateThread (NULL, 0, thread2, NULL, 0, NULL);
    if (!thread2_handle) {
        error ("CreateThread 2");
    }
    Sleep (10000);
    run = FALSE;
    WaitForSingleObject (thread1_handle, INFINITE);
    WaitForSingleObject (thread2_handle, INFINITE);
    printf ("M1 safe_int_pending = %u\n", safe_int_pending);
    printf ("M2 safe_int_triggered = %u\n", safe_int_triggered);
    printf ("M3 safe_int_removed = %u\n", safe_int_removed);
    printf ("M4 safe_step_pending = %u\n", safe_step_pending);
    printf ("M5 safe_step_finished = %u\n", safe_step_finished);
    printf ("M6 safe_int_unrestored = %u\n", safe_int_unrestored);
    printf ("M3 danger_int_removed = %u\n", danger_int_removed);
    printf ("M4 danger_step_pending = %u\n", danger_step_pending);
    printf ("M5 danger_step_finished = %u\n", danger_step_finished);
    printf ("M6 danger_int_unrestored = %u\n", danger_int_unrestored);
    printf ("   safe_after = %u\n", safe_after);
    printf ("   safe_elsewhere = %u\n", safe_elsewhere);
    printf ("?? danger_unknown = %u\n", danger_unknown);
    return 0;
}

