#!/bin/bash

source ../emsdk/emsdk_env.sh --build=Release

debug=0
emflags="-O3 -g0 --closure 1"
[ $debug == 1 ] && emflags="-O2 -g1 -s ASSERTIONS=1"

emcc -DWASM main.c -o gravityblur.html --shell-file shell.html \
	$emflags -s EXPORTED_FUNCTIONS="['_malloc', '_free']" -s ALLOW_MEMORY_GROWTH=1
mv gravityblur.html index.html

echo "press enter..."; read dummy < /dev/tty

