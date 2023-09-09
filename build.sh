set -xe

mkdir -p ./build

BUILD_DIR="$PWD/build"

cd ./kissat/
make -C ./build

cd $BUILD_DIR
cp --update ../headers/kissat.h $BUILD_DIR
cp --update ../src/*.c $BUILD_DIR
cp --update -r ../kissat/build/libkissat.a $BUILD_DIR

CFLAGS="-I./headers $(pkg-config --cflags sdl2)"
LDFLAGS="$(pkg-config --libs sdl2)"

gcc -c ./*.c $CFLAGS

gcc -o main.out *.o -L. -lkissat $LDFLAGS