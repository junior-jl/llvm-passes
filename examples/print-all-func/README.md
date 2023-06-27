The command `./run-pass.sh PrintAllFuncPass examples/print-all-func/test.c -S -emit-llvm -o examples/print-all-func/test.ll` outputs the following and the `test.ll` file:

```
... make output ...

************ Running pass PrintAllFuncPass on file examples/print-all-func/test.c ************

Function: my_func
*** Basic Blocks of my_func: *** 
Basic Block: 
  %1 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([15 x i8], [15 x i8]* @.str, i64 0, i64 0))
  ret void

*** Instructions of Basic Block 1 of my_func: *** 
Instruction 1:   %1 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([15 x i8], [15 x i8]* @.str, i64 0, i64 0))
Instruction 2:   ret void
Function: my_module
*** Basic Blocks of my_module: *** 
Basic Block: 
  %1 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([20 x i8], [20 x i8]* @.str.1, i64 0, i64 0))
  ret void

*** Instructions of Basic Block 1 of my_module: *** 
Instruction 1:   %1 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([20 x i8], [20 x i8]* @.str.1, i64 0, i64 0))
Instruction 2:   ret void
Function: main
*** Basic Blocks of main: *** 
Basic Block: 
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  call void @my_module()
  ret i32 0

*** Instructions of Basic Block 1 of main: *** 
Instruction 1:   %1 = alloca i32, align 4
Instruction 2:   store i32 0, i32* %1, align 4
Instruction 3:   call void @my_module()
Instruction 4:   ret i32 0

```

**Note**: running `./run-pass.sh PrintAllFuncPass examples/print-all-func/test.ll` outputs the same.