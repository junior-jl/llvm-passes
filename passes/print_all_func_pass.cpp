//===- PrintAllFuncPass - Print all instructions in a Function pass -------===//
//
// This pass prints all basic blocks and instructions in a function.
//
// Author: José Lira Junior
//
//===-----------------------------------------------------------------------===//

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

namespace {
  struct PrintAllFuncPass : public FunctionPass {
    static char ID;
    PrintAllFuncPass() : FunctionPass(ID) {}
    virtual bool runOnFunction(Function &F) {
        outs() << "Function: " << F.getName() << "\n";
        outs() << "*** Basic Blocks of " << F.getName() << ": *** \n";
        int i = 0;
        for (auto &B : F)
        {
          outs() << "Basic Block: " << B << "\n";
          outs() << "*** Instructions of Basic Block " << ++i << " of " << F.getName() << ": *** \n";
          int j = 0;
          for (auto &I : B)
          {
            outs() << "Instruction " << ++j << ": " << I << "\n";
          }
        }
      return false;
    }
  };
}

char PrintAllFuncPass::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerPrintAllPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new PrintAllFuncPass());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerPrintAllPass);
