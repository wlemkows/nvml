#!/bin/sh

JEMALLOC_GEN=./../windows/jemalloc_gen
AC_PATH=./../../jemalloc

autoconf
if [ $? -ne 0 ]; then
	echo "Error $? in $i"
	exit 1
fi

if [ ! -d "$JEMALLOC_GEN" ]; then
	echo Creating... $JEMALLOC_GEN
	mkdir "$JEMALLOC_GEN"
fi

cd $JEMALLOC_GEN

echo "Run configure..."
$AC_PATH/configure \
	--enable-autogen \
	CC=cl \
	--enable-lazy-lock=no \
	--without-export \
	--with-jemalloc-prefix=je_vmem_ \
	--with-private-namespace=je_vmem_ \
	--disable-xmalloc \
	--disable-munmap \
	EXTRA_CFLAGS="-DJEMALLOC_LIBVMEM"

if [ $? -ne 0 ]; then
    echo "Error $? in $AC_PATH/configure"
    exit 1
fi
