#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

/*
Algorithm to transform multiplication into left shifts and adds
a * b
int result = 0
while (b > 0)
{
  if (b & 1) // lsb is 1
  {
    result += a // result = result + a
  }
  a <<= 1; // a = a << 1
  b >>= 1; // b = b >> 1
}
  return result

*/

// CreateAdd(Value * LHS, Value * RHS, cont Twine & Name = "", bool HasNUW=false, bool HasNSW=false)
// CreateShl(Value * LHS, [const APInt/uint64_t/Value*] & RHS, cont Twine & Name = "", bool HasNUW=false, bool HasNSW=false)
// CreateLShr(Value * LHS, [const APInt/uint64_t/Value*] & RHS, cont Twine & Name = "", bool HasNUW=false, bool HasNSW=false)

namespace {
  struct MultiplicationShiftsPass : public FunctionPass {
    static char ID;
    MultiplicationShiftsPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      errs() << "I saw a function called " << F.getName() << "!\n";
      return false;
    }
  };
}

char MultiplicationShiftsPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerMultiplicationShiftsPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new MultiplicationShiftsPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerMultiplicationShiftsPass);
