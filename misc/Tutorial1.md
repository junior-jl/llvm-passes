# Tutorial - Replacing OR and NOT instructions in LLVM IR with orn instrinsic function and mapping it to RISC-V ORN

## Step 0 - Requirements

Before starting with the tutorial, ensure that you have the following:

- A basic understanding of LLVM infrastructure and its Intermediate Representation (IR).
- A C++ development environment set up to build LLVM.
- Familiarity with the RISC-V instruction set architecture (ISA) and its assembly language.

You can use this guide to build LLVM+Clang for RISC-V target: https://github.com/sifive/riscv-llvm.

## Step 1 - add the intrinsic definition

To achieve our goal, first we'll write a LLVM pass that replaces OR and NOT instructions by a call to the intrinsic function ORN that we'll define.

1. Open `llvm/include/llvm/IR/Intrinsics.td`.
2. Add the following code:
```
def int_orn : Intrinsic<[llvm_any_ty], [llvm_any_ty, llvm_any_ty], [IntrNoMem, IntrWillReturn]>;
```

This declares the function `int_orn` that takes two parameters of any LLVM type (`llvm_any_ty`) and returns a value of any type as well. The `[IntrNoMem, IntrWillReturn]` attributes specify that this intrinsic does not have any memory side effects (`IntrNoMem`) and will always return a value (`IntrWillReturn`).

## Step 2 - write the LLVM pass

Now we need to write a LLVM pass that replaces a combination of NOT and OR instructions by a call to our newly defined intrinsic. **Note:** The NOT instruction is implemented in LLVM IR as a XOR with -1 in the two's complement representation.

For simplicity, you can use the Hello Pass Example (`llvm/lib/Transforms/Hello/Hello.cpp`) as a template and modify it.

1. Add the necessary headers;

```
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Intrinsics.h"
```
2. Declare the variable `modified` and assign it to false. (If the pass does not find the combination of instructions to replace it will not modify the function).

```
bool modified = false;
```
3. Declare a `std::vector<Instruction*>` (or any container you'd prefer) to store the instructions to be deleted after the process (This is to avoid segmentation fault errors trying to delete instructions while iterating over them).

```
std::vector<Instruction*> to_delete;
```
4. (Optional) If you want to run the pass in a specific function, add a condition to check if the function being processed is the one you want. Here, we added a condition to check if we are processing the `main` function, i.e., the pass will only work on the `main` function.

```
if (F.getName() == "main")
{
    // The replacement steps
}
```

5. (Optional) Print (using `outs()`) the function before and after the execution of the pass to check if everything is working without having to look at the output file.

```
outs() << F << "\n";
...

outs() << "Modified function:\n";
outs() << F << "\n"; 
```

6. Declare a pointer to the module that contains the current function. This is necessary for some functions we'll use.

```
Module *M = F.getParent();
```

7. Iterate over each basic block in the function and each instruction in the basic blocks.

```
for (auto &B : F)
{
  for (auto &I : B)
  {
  ...
  }
}
```

8. Check if the instruction being processed is a NOT instruction (XOR with -1).
    - Use the `getOpcode()` function and check if it is `Instruction::Xor`;
    - Try to cast the second operand of `I` to a `ConstantInt` object using `dyn_cast`. If the cast fails, i.e., the second operand is not a constant integer, it returns a null pointer (false);
    - Use the `getSExtValue()` function to extract the value of the second operand as a signed extended value and check if it is equal to -1.
    If found a NOT instruction, assign it to a pointer to `Instruction`.

```
if (I.getOpcode() == Instruction::Xor)
{
  if (auto *c = dyn_cast<ConstantInt>(I.getOperand(1)))
  {
    if (c->getSExtValue() == -1)
    {
    // If this point is reached, I is a NOT instruction
    Instruction *not_inst = &I;
    ...
    }
  ...
  }
  ...
}
```

9. Now iterate over the users of the NOT instruction to check if any of the users is a OR instruction, i.e., check if it is not an isolated NOT instruction.
    - Use the `users()` function to iterate over the users.
    - Check if the user is a instruction with `dyn_cast` and assign it to a variable (`or_inst`).
    - Check if the opcode of the instruction is `Instruction::Or` and if its first operand is the NOT instruction.

```
for (User *U : not_inst->users())
{
    if (Instruction *or_inst = dyn_cast<Instruction>(U))
    {
      if (or_inst->getOpcode() == Instruction::Or && or_inst->getOperand(0) == not_inst)
      {
```

If all these checks pass, we have the address of the NOT and OR instructions to replace.

10. Create an `IRBuilder` object (it is an interface to create new instructions in LLVM IR). 
    - The `IRBuilder` constructor needs an iterator (the insertion point for the new instructions).
    - Pass a pointer of the instruction next to the OR (you can choose other point).
    
```
auto it = or_inst->getIterator();
it = std::next(it);
auto *next_inst = &(*it);
IRBuilder<> Builder(next_inst);
```

11. Extract the arguments for the intrinsic function (the first operand of the NOT instruction and the second operand of the OR instruction).

```
Value *operand1 = not_inst->getOperand(0);
Value *operand2 = or_inst->getOperand(1);
```

12. Retrieve the declaration of the ORN intrinsic function with `Intrinsic::getDeclaration()` (this uses the module, the intrinsic ID and an array of pointer to types) and store the arguments in an array.

```
auto orn_func = Intrinsic::getDeclaration(M, Intrinsic::orn, {or_inst->getType(), operand1->getType(), operand2->getType()});
Value *args[] = {operand1, operand2};
```

13. Use the builder to create a call to the ORN intrinsic.

```
auto *orn_result = Builder.CreateCall(orn_func, args);
```

14. Replace all uses of the OR instruction (it was the one with the result) with the newly created call and indicate that changes were made in the function.

```
or_inst->replaceAllUsesWith(orn_result);
modified = true;
```

15. Finally, delete the obsolete instructions.

```
// Right after modified = true, inside all loops and conditions
to_delete.push_back(not_inst);
to_delete.push_back(or_inst);
// Outside all loops and conditions (still on runOnFunction)
while(!to_delete.empty())
  {
    Instruction* inst = to_delete.back();
    to_delete.pop_back();
    inst->eraseFromParent();
  }
```

The whole pass should look something like this:

```
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Intrinsics.h"

using namespace llvm;

#define DEBUG_TYPE "hello"

namespace {
  struct Hello : public FunctionPass {
    static char ID;
    Hello() : FunctionPass(ID) {}
    virtual bool runOnFunction(Function &F) override
    {
      bool modified = false;
      std::vector<Instruction*> to_delete;
      if (F.getName() == "main")
      {
        outs() << F << "\n";
        Module *M = F.getParent();
        for (auto &B : F)
        {
          for (auto &I : B)
          {
            // NOT is implemented as XOR with -1
            if (I.getOpcode() == Instruction::Xor)
            {
              if (auto *c = dyn_cast<ConstantInt>(I.getOperand(1)))
              {
                if (c->getSExtValue() == -1)
                {
                  // If found a NOT instruction
                  Instruction *not_inst = &I;
                  // Find if one of the users is a OR instruction
                  for (User *U : not_inst->users())
                  {
                    if (Instruction *or_inst = dyn_cast<Instruction>(U))
                    {
                      if (or_inst->getOpcode() == Instruction::Or && or_inst->getOperand(0) == not_inst)
                      {
                        auto it = or_inst->getIterator();
                        it = std::next(it);
                        auto *next_inst = &(*it);
                        IRBuilder<> Builder(next_inst);
                        Value *operand1 = not_inst->getOperand(0);
                        Value *operand2 = or_inst->getOperand(1); 
                        auto orn_func = Intrinsic::getDeclaration(M, Intrinsic::orn, {or_inst->getType(), operand1->getType(), operand2->getType()});
                        Value *args[] = {operand1, operand2};
                        auto *orn_result = Builder.CreateCall(orn_func, args);
                        or_inst->replaceAllUsesWith(orn_result);
                        modified = true;
                        to_delete.push_back(not_inst);
                        to_delete.push_back(or_inst);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
      while(!to_delete.empty())
      {
        Instruction* inst = to_delete.back();
        to_delete.pop_back();
        inst->eraseFromParent();
      }
      outs() << "Modified function:\n";
      outs() << F << "\n";
      return modified;
    }
  };
}
char Hello::ID = 0;
static RegisterPass<Hello> X("hello", "Hello World Pass");
```

## Step 3 - Add Target Opcode

1. In `llvm/include/llvm/Support/TargetOpcodes.def`, add the following line:

```
HANDLE_TARGET_OPCODE(G_ORN)
```

You can use any name, `G_ORN` was used to match the pattern of the file.

2. Now, open `llvm/lib/Target/RISCV/RISCVAsmPrinter.cpp` and locate the function `RISCVAsmPrinter::lowerToMCInst`. In the `switch` statement, include the following case:

```
case TargetOpcode::G_ORN:
{
  Register DestReg = MI->getOperand(0).getReg();
  Register SourceReg1 = MI->getOperand(1).getReg();
  Register SourceReg2 = MI->getOperand(2).getReg();
  OutMI = MCInst();
  OutMI.setOpcode(RISCV::ORN);
  OutMI.addOperand(MCOperand::createReg(DestReg));
  OutMI.addOperand(MCOperand::createReg(SourceReg1));
  OutMI.addOperand(MCOperand::createReg(SourceReg2));
  break;
}
```

This creates a new machine instruction with `RISCV::ORN` opcode and find registers for the two operands (sources) and for the result of the operation.

## Step 4 - Modify SelectionDAGBuilder

The SelectionDAGBuilder file is responsible for translating LLVM IR to SelectionDAG IR (a lower graph-like representation).

1. In the file `llvm/lib/CodeGen/SelectionDAG/SelectionDAGBuilder.cpp`, find the function `SelectionDAGBuilder::visitIntrinsicCall` and a new case for the `orn` intrinsic.

```
case Intrinsic::orn:
{
    //
}
```

2. Save the root value and the operands values in variables of type `SDValue`.

```
SDValue Ops[2];
SDValue Root = getRoot();
Ops[0] = getValue(I.getArgOperand(0));
Ops[1] = getValue(I.getArgOperand(1));
```

3. Declare a new `MachineSDNode` (Machine Selection DAG Node) and use it to construct a `SDValue`.

```
MachineSDNode* Op = DAG.getMachineNode(TargetOpcode::G_ORN, sdl, Ops[0].getValueType(), Ops[0], Ops[1]);
SDValue Op_new1 = SDValue(Op, 0);
```

4. Set the value of the instruction to the new `SDValue` and set the DAG root to the original root.

```
setValue(&I, Op_new1);
DAG.setRoot(Root);
```

Hence, the new case should look something like this:

```
case Intrinsic::orn:
{
    SDValue Ops[2];
    SDValue Root = getRoot();
    Ops[0] = getValue(I.getArgOperand(0));
    Ops[1] = getValue(I.getArgOperand(1));
    MachineSDNode* Op = DAG.getMachineNode(TargetOpcode::G_ORN, sdl, Ops[0].getValueType(), Ops[0], Ops[1]);
    SDValue Op_new1 = SDValue(Op, 0);
    setValue(&I, Op_new1);
    DAG.setRoot(Root);
    return; 
}
```

## Step 5 - rebuild LLVM

Inside your build folder (if you built following https://github.com/sifive/riscv-llvm, it should be `path/to/repo/riscv/riscv-llvm/_build`), perform the command `cmake --build . --target install`.

## Test Example

1. Let's create a file that contains operations which can be transformed into an ORN operation. For example, `testorn.c`.

```
int main() {
    long int operand1 = 5;
    long int operand2 = 10;
    long int result;
    result = ~operand1 | operand2;
    return 0;
}
```

2. Transform the C source code into LLVM IR with `clang testorn.c -S -emit-llvm -o testorn.ll`. The new file should look something similar to this:

```
; ModuleID = 'testorn.c'
source_filename = "testorn.c"
target datalayout = "e-m:e-p:64:64-i64:64-i128:128-n32:64-S128"
target triple = "riscv64-unknown-unknown-elf"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local signext i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %operand1 = alloca i64, align 8
  %operand2 = alloca i64, align 8
  %result = alloca i64, align 8
  store i32 0, ptr %retval, align 4
  store i64 5, ptr %operand1, align 8
  store i64 10, ptr %operand2, align 8
  %0 = load i64, ptr %operand1, align 8
  %not = xor i64 %0, -1
  %1 = load i64, ptr %operand2, align 8
  %or = or i64 %not, %1
  store i64 %or, ptr %result, align 8
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic-rv64" "target-features"="+64bit,+a,+c,+m,+relax,-d,-e,-experimental-smaia,-experimental-ssaia,-experimental-zca,-experimental-zcb,-experimental-zcd,-experimental-zcf,-experimental-zcmp,-experimental-zcmt,-experimental-zfa,-experimental-zfbfmin,-experimental-zicond,-experimental-zihintntl,-experimental-ztso,-experimental-zvbb,-experimental-zvbc,-experimental-zvfbfmin,-experimental-zvfbfwma,-experimental-zvfh,-experimental-zvkg,-experimental-zvkn,-experimental-zvknc,-experimental-zvkned,-experimental-zvkng,-experimental-zvknha,-experimental-zvknhb,-experimental-zvks,-experimental-zvksc,-experimental-zvksed,-experimental-zvksg,-experimental-zvksh,-experimental-zvkt,-f,-h,-save-restore,-svinval,-svnapot,-svpbmt,-v,-xsfvcp,-xtheadba,-xtheadbb,-xtheadbs,-xtheadcmo,-xtheadcondmov,-xtheadfmemidx,-xtheadmac,-xtheadmemidx,-xtheadmempair,-xtheadsync,-xtheadvdot,-xventanacondops,-zawrs,-zba,-zbb,-zbc,-zbkb,-zbkc,-zbkx,-zbs,-zdinx,-zfh,-zfhmin,-zfinx,-zhinx,-zhinxmin,-zicbom,-zicbop,-zicboz,-zicntr,-zicsr,-zifencei,-zihintpause,-zihpm,-zk,-zkn,-zknd,-zkne,-zknh,-zkr,-zks,-zksed,-zksh,-zkt,-zmmul,-zve32f,-zve32x,-zve64d,-zve64f,-zve64x,-zvl1024b,-zvl128b,-zvl16384b,-zvl2048b,-zvl256b,-zvl32768b,-zvl32b,-zvl4096b,-zvl512b,-zvl64b,-zvl65536b,-zvl8192b" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"target-abi", !"lp64"}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 7, !"frame-pointer", i32 2}
!4 = !{i32 8, !"SmallDataLimit", i32 8}
!5 = !{!"clang version 17.0.0 (https://github.com/llvm/llvm-project.git fbdeb8cbc147f8f49fbd4bf23fae01bd142f0f5d)"}
```

Notice that we have a XOR operation of `%0` with `-1` assigned to `%not` and OR of `%not` and `%1`. These two instructions will be replaced by the call to the `orn` intrinsic.

```
%0 = load i64, ptr %operand1, align 8
%not = xor i64 %0, -1
%1 = load i64, ptr %operand2, align 8
%or = or i64 %not, %1
```

3. Run the pass in the LLVM IR file.

```
path/to/repo/riscv/_install/bin/opt -enable-new-pm=0 -load path/to/repo/riscv/riscv-llvm/_build/lib/LLVMHello.so -hello -S testorn.ll -o testornoutput.ll
```

This command prints the function before and after the pass and generates a new file with the following content:

```
; ModuleID = 'testorn.ll'
source_filename = "testorn.c"
target datalayout = "e-m:e-p:64:64-i64:64-i128:128-n32:64-S128"
target triple = "riscv64-unknown-unknown-elf"

; Function Attrs: noinline nounwind optnone uwtable
define dso_local signext i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %operand1 = alloca i64, align 8
  %operand2 = alloca i64, align 8
  %result = alloca i64, align 8
  store i32 0, ptr %retval, align 4
  store i64 5, ptr %operand1, align 8
  store i64 10, ptr %operand2, align 8
  %0 = load i64, ptr %operand1, align 8
  %1 = load i64, ptr %operand2, align 8
  %2 = call i64 @llvm.orn.i64.i64.i64(i64 %0, i64 %1)
  store i64 %2, ptr %result, align 8
  ret i32 0
}

; Function Attrs: nounwind willreturn memory(none)
declare i64 @llvm.orn.i64.i64.i64(i64, i64) #1

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic-rv64" "target-features"="+64bit,+a,+c,+m,+relax,-d,-e,-experimental-smaia,-experimental-ssaia,-experimental-zca,-experimental-zcb,-experimental-zcd,-experimental-zcf,-experimental-zcmp,-experimental-zcmt,-experimental-zfa,-experimental-zfbfmin,-experimental-zicond,-experimental-zihintntl,-experimental-ztso,-experimental-zvbb,-experimental-zvbc,-experimental-zvfbfmin,-experimental-zvfbfwma,-experimental-zvfh,-experimental-zvkg,-experimental-zvkn,-experimental-zvknc,-experimental-zvkned,-experimental-zvkng,-experimental-zvknha,-experimental-zvknhb,-experimental-zvks,-experimental-zvksc,-experimental-zvksed,-experimental-zvksg,-experimental-zvksh,-experimental-zvkt,-f,-h,-save-restore,-svinval,-svnapot,-svpbmt,-v,-xsfvcp,-xtheadba,-xtheadbb,-xtheadbs,-xtheadcmo,-xtheadcondmov,-xtheadfmemidx,-xtheadmac,-xtheadmemidx,-xtheadmempair,-xtheadsync,-xtheadvdot,-xventanacondops,-zawrs,-zba,-zbb,-zbc,-zbkb,-zbkc,-zbkx,-zbs,-zdinx,-zfh,-zfhmin,-zfinx,-zhinx,-zhinxmin,-zicbom,-zicbop,-zicboz,-zicntr,-zicsr,-zifencei,-zihintpause,-zihpm,-zk,-zkn,-zknd,-zkne,-zknh,-zkr,-zks,-zksed,-zksh,-zkt,-zmmul,-zve32f,-zve32x,-zve64d,-zve64f,-zve64x,-zvl1024b,-zvl128b,-zvl16384b,-zvl2048b,-zvl256b,-zvl32768b,-zvl32b,-zvl4096b,-zvl512b,-zvl64b,-zvl65536b,-zvl8192b" }
attributes #1 = { nounwind willreturn memory(none) }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"target-abi", !"lp64"}
!2 = !{i32 7, !"uwtable", i32 2}
!3 = !{i32 7, !"frame-pointer", i32 2}
!4 = !{i32 8, !"SmallDataLimit", i32 8}
!5 = !{!"clang version 17.0.0 (https://github.com/llvm/llvm-project.git fbdeb8cbc147f8f49fbd4bf23fae01bd142f0f5d)"}
```
`
We can see that the replacement was successful in the following snippet. 

```
%0 = load i64, ptr %operand1, align 8
%1 = load i64, ptr %operand2, align 8
%2 = call i64 @llvm.orn.i64.i64.i64(i64 %0, i64 %1)
```

Now, let's check if this intrinsic function is correctly mapped to the RISCV ORN instruction.

4. Assemble the LLVM IR file into RISC-V Assembly.

Execute the command `llc testornoutput.ll`. It generates a `testornoutput.s` if you do not pass the output name. The file contains the following:

```
	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p1"
	.file	"testorn.c"
	.globl	main                            # -- Begin function main
	.p2align	1
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	addi	sp, sp, -48
	.cfi_def_cfa_offset 48
	sd	ra, 40(sp)                      # 8-byte Folded Spill
	sd	s0, 32(sp)                      # 8-byte Folded Spill
	.cfi_offset ra, -8
	.cfi_offset s0, -16
	addi	s0, sp, 48
	.cfi_def_cfa s0, 0
	sw	zero, -20(s0)
	li	a0, 5
	sd	a0, -32(s0)
	li	a0, 10
	sd	a0, -40(s0)
	ld	a0, -32(s0)
	ld	a1, -40(s0)
	orn	a0, a0, a1
	sd	a0, -48(s0)
	li	a0, 0
	ld	ra, 40(sp)                      # 8-byte Folded Reload
	ld	s0, 32(sp)                      # 8-byte Folded Reload
	addi	sp, sp, 48
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.ident	"clang version 17.0.0 (https://github.com/llvm/llvm-project.git fbdeb8cbc147f8f49fbd4bf23fae01bd142f0f5d)"
	.section	".note.GNU-stack","",@progbits
```

Instead of a call to a function, we have the `orn` instruction. Hence, everything worked correctly.

## Enhancement

What would happen if we use `int` (generally 32 bit) instead of `long int` (generally 64 bit) in our C source code? This would be our LLVM IR file after the pass:

```
; Function Attrs: noinline nounwind optnone uwtable
define dso_local signext i32 @main() #0 {
entry:
  %retval = alloca i32, align 4
  %operand1 = alloca i32, align 4
  %operand2 = alloca i32, align 4
  %result = alloca i32, align 4
  store i32 0, ptr %retval, align 4
  store i32 5, ptr %operand1, align 4
  store i32 10, ptr %operand2, align 4
  %0 = load i32, ptr %operand1, align 4
  %1 = load i32, ptr %operand2, align 4
  %2 = call i32 @llvm.orn.i32.i32.i32(i32 %0, i32 %1)
  store i32 %2, ptr %result, align 4
  ret i32 0
}
```

If we try to use `llc` with the default target (64-bit RISC-V) to compile this file to RISC-V Assembly, we would get a segmentation fault and this message `Don't know how to custom type legalize this operation!` related to the `G_ORN` operation. Hence, we can use the function `getSExtOrTrunc` defined in the file `llvm/lib/CodeGen/SelectionDAG/SelectionDAG.cpp` to sign extend the operands of our SDNode.

To do this, in the code provided in Step 4, add the following lines before creating the node:

```
Ops[0] = DAG.SelectionDAG::getSExtOrTrunc(Ops[0], sdl, MVT::i64);
Ops[1] = DAG.SelectionDAG::getSExtOrTrunc(Ops[1], sdl, MVT::i64);
```

Now, try to use `int` instead of `long int` in your C source code and the process should work correctly as well!


