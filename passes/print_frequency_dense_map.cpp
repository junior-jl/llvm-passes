//===- PrintFrequencyDenseMap - Print Instruction Frequency in Function pass --------===//
//
// This pass prints the frequency of each instruction in a function.
//
// Author: José Lira Junior
//
//===-----------------------------------------------------------------------===//

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/DenseMap.h"
using namespace llvm;

namespace {
  struct PrintFrequencyDenseMap : public FunctionPass {
    static char ID;
    PrintFrequencyDenseMap() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      DenseMap<const char *, int> occurrences; 
      for (auto &B : F) 
      {
        for (auto &I : B) 
        {
          auto opcode = I.getOpcodeName();
          occurrences[opcode]++;
        }
      }
      outs() << "Instructions by frequency in Function " << F.getName() << "\n";
      for (const auto &Pair : occurrences) 
      {
        outs() << Pair.first << ": " << Pair.second << "\n";
      }
      return false;
    }
  };
}

char PrintFrequencyDenseMap::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new PrintFrequencyDenseMap());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerSkeletonPass);
