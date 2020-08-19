
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define AREA_SIZE 20

volatile BOOL run = TRUE;
HANDLE thread1_handle;
HANDLE thread2_handle;
DWORD start;
DWORD end;

unsigned counter[AREA_SIZE];

extern void asm_loop (void);

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
    memset (&context, 0, sizeof (CONTEXT));

    while (run) {
        if (SuspendThread (thread1_handle) == (DWORD) -1) {
            error ("SuspendThread");
        }
        context.ContextFlags =
                CONTEXT_CONTROL | CONTEXT_INTEGER | CONTEXT_i386;

        if (!GetThreadContext (thread1_handle, &context)) {
            error ("GetThreadContext");
        }

        if ((context.Eip >= start) && (context.Eip < end)) {
            counter[context.Eip - start] ++;
        }

        if (ResumeThread (thread1_handle) == (DWORD) -1) {
            error ("ResumeThread");
        }
    }
    return 0;
}

int main (void)
{
    DWORD i;
    start = (intptr_t) ((void *) asm_loop);
    end = start + AREA_SIZE;
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
    for (i = 0; i < AREA_SIZE; i ++) {
        printf ("counter[0x%x] = %d\n", (unsigned) (i + start),
                    (unsigned) counter[i]);
    }
    return 0;
}

