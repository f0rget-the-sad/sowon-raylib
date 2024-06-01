#!/bin/bash

set -ex

SRC=main.c

mkdir -p build

# native build
RL=./raylib/raylib-5.0_linux_amd64
clang -I$RL/include -o build/sowon main.c -L$RL/lib/ -lraylib -lm

# web build
RL=./raylib/raylib-5.0_webassembly
RLLIB=$RL/lib/libraylib.a
emcc -o index.html $SRC -Os -Wall $RLLIB -I. -I$RL/include \
-L. $RLLIB -s USE_GLFW=3 --shell-file ./minshell.html \
--preload-file ./resources \
-DPLATFORM_WEB
