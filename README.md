# llvm-passes

This repository contains a collection of LLVM passes implemented using C++. 

## Usage

`./create-pass.sh NewPass` adds `NewPass` to the CMake file and creates the file `passes/new_pass.cpp` with a pass template provided in https://github.com/sampsyo/llvm-pass-skeleton.

`./rename-pass.sh OldPass NewPass` renames the `OldPass` in CMake file to `NewPass` and the `passes/old_pass.cpp` to `passes/new_pass.cpp`.

`./run-pass.sh PassName cfile.c [-clang flags]` runs the pass `PassName` in the file `cfile.c` in the following way:

```bash
make -C build
clang -flegacy-pass-manager -Xclang -load -Xclang build/passes/libPassName.so cfile.c [-clang flags]
```