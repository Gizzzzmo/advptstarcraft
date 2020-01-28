#!/bin/sh

directory="$(dirname $0)"

build_type="Release"

if ! [ -z "$1" ]; then
    build_type="$1"
fi

cmake -B "$directory"/build -DCMAKE_BUILD_TYPE="$build_type" "$directory"
cmake --build "$directory"/build -j 4
