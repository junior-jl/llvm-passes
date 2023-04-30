#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Usage: ./run_pass.sh <pass_name_with_initials_capitalized> <file.c>"
    exit 1
fi

make -C build
clang -flegacy-pass-manager -Xclang -load -Xclang build/passes/lib"$1".so "$2"

