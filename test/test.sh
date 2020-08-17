#!/bin/bash -xe

cd /j/gdbproject/binutils-gdb/test

export PATH=/j/gdbproject/mingw32/bin:$PATH

gcc -o x.exe -g main.c test.s

/j/gdbproject/build/gdb/gdb.exe ./x.exe --batch-silent -x commands.txt

egrep '^(This version|Reached test|Print registers|warning: .etThreadContext called)' < log.txt | \
        sed -e 's/ *(tid=.*$//' > result.txt
diff result.txt expect.txt

echo "OK!"

