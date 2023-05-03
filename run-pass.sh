#!/bin/bash

if [ $# -lt 2 ]; then
    echo "Usage: ./run-pass.sh <pass_name_with_initials_capitalized> <file.c> [clang_flags]"
    exit 1
fi

PASS_NAME="$1"
INPUT_FILE="$2"
shift 2
CLANG_FLAGS="$@"

make -C build
echo "************ Running pass $PASS_NAME on file $INPUT_FILE ************"
clang -flegacy-pass-manager -Xclang -load -Xclang build/passes/lib"$PASS_NAME".so "$INPUT_FILE" $CLANG_FLAGS
