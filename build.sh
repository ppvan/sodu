#/usr/bin/bash
set -xe
shopt -s extglob

mkdir -p ./build

BUILD_DIR="$PWD/build"
CFLAGS="-I../headers/ $(pkg-config --cflags sdl2) -Wall -Wextra -ggdb -O3"
LDFLAGS="-L. -lkissat -lm $(pkg-config --libs sdl2)"

SRC_FILES=../src/*.c

# Build sub-project
# cd "$PWD/kissat/"
make -C "$PWD/kissat/build"

cd $BUILD_DIR
cp --update -r ../kissat/build/libkissat.a $BUILD_DIR

# Compile
gcc -c $SRC_FILES $CFLAGS

OBJ_FILES=!(*_test|main).o

# Link main program
gcc -o main.out main.o $OBJ_FILES $LDFLAGS

# Link test
gcc -o core_test.out core_test.o $OBJ_FILES $LDFLAGS