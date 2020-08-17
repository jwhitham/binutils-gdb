#!/bin/bash -xe

cd /j/gdbproject/binutils-gdb/test

export PATH=/j/gdbproject/mingw32/bin:$PATH

gcc -o x.exe -g main.c test.s

/j/gdbproject/build/gdb/gdb ./x.exe --batch-silent -x commands.txt


