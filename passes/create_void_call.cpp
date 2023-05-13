#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"

// TODO: Enable the function call to be placed anywhere in the basic block
// TODO: Enable the function call to be place in a chosen function (by the user)

using namespace llvm;

namespace {
  struct CreateVoidCall : public FunctionPass {
    static char ID;
    CreateVoidCall() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      if (F.getName() == "main")
      {
        // step tells the number of instruction that the new call will assume (starting by zero)
        int step = 0;
        Module *M = F.getParent();
        auto myfunc = M->getOrInsertFunction("my_func", Type::getVoidTy(F.getContext()));
        for (auto &B : F)
        {
          for (auto I = B.begin(); I != B.end(); ++I, --step)
          {
            if (step == 0)
            {
              IRBuilder<> builder(&B, I);
              builder.CreateCall(myfunc);
            }
          }
        }
      }
      
      
      return true;
    }
  };
}

char CreateVoidCall::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerCreateVoidCall(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new CreateVoidCall());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerCreateVoidCall);
