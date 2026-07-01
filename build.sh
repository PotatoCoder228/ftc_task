#!/bin/bash

set -x

CMAKE_BUILD_TYPE=Release
ENABLE_SANITIZERS=OFF
ENABLE_LTO=ON

while getopts "b:s:s:" opt; do
    case "$opt" in
        b) CMAKE_BUILD_TYPE="$OPTARG" ;;
        s) ENABLE_SANITIZERS="$OPTARG" ;;
        l) ENABLE_LTO="$OPTARG" ;;
        *) echo "Usage: $0 [-b build_type] [-s sanitizers]" >&2; exit 1 ;;
    esac
done

cmake -B build -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DENABLE_SANITIZERS=$ENABLE_SANITIZERS -DENABLE_LTO=$ENABLE_LTO
cmake --build build

ln -sf build/compile_commands.json .