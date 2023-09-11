#/usr/bin/bash
set -xe
shopt -s extglob

mkdir -p ./build

BUILD_DIR="$PWD/build"
CFLAGS="-I../headers/ $(pkg-config --cflags sdl2 SDL2_image) -Wall -Wextra -ggdb"
LDFLAGS="-L. -lkissat -lm $(pkg-config --libs sdl2 SDL2_image)"

SRC_FILES=../src/*.c

# Build sub-project
cd ./kissat/
make -C ./build

cd $BUILD_DIR
cp --update -r ../kissat/build/libkissat.a $BUILD_DIR

# Compile
gcc -c $SRC_FILES $CFLAGS

OBJ_FILES=!(*_test|main).o

# Link main program
gcc -o main.out main.o $OBJ_FILES $LDFLAGS

# Link test
gcc -o core_test.out core_test.o $OBJ_FILES $LDFLAGS