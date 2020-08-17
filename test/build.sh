#!/bin/bash -xe

cd /j/gdbproject
rm -rf build
mkdir build
cd build
# export PATH=/r/F/GNAT/2017/bin:$PATH
export PATH=/j/gdbproject/mingw32/bin:$PATH
../binutils-gdb/configure \
    --enable-targets="i686-w64-mingw32" \
    --target="i686-w64-mingw32" \
    --build="i686-w64-mingw32" \
    --host="i686-w64-mingw32" \
    --enable-64-bit-bfd \
    --disable-werror \
    --disable-win32-registry \
    --disable-rpath \
    --with-zlib \
    --disable-tui \
    --enable-source-highlight=no \
    --prefix=/c/tools/gdb
make

# --with-system-readline \
# --with-curses \
# --with-expat \
# --with-lzma \

# --with-system-gdbinit=${MINGW_PREFIX}/etc/gdbinit \
# --with-python=${MINGW_PREFIX}/bin/python \
# --with-libiconv-prefix=${MINGW_PREFIX} \
#

