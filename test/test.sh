#!/bin/bash -xe

cd /j/gdbproject/binutils-gdb/test

export PATH=/j/gdbproject/mingw32/bin:$PATH

gcc -o x.exe -g main.c test.s

/j/gdbproject/build/gdb/gdb.exe --batch-silent -x c2.txt --args \
    /j/gdbproject/build/gdb/gdb.exe ./x.exe --batch-silent -x commands.txt


