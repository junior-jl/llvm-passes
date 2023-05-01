#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"

using namespace llvm;

namespace {
  struct CreateVoidCall : public FunctionPass {
    static char ID;
    CreateVoidCall() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      FunctionType *FuncType = FunctionType::get(Type::getVoidTy(F.getContext()), false);
      Function *newFunc = Function::Create(FuncType, Function::ExternalLinkage, "my_func");
      for (auto &B : F)
      {
        IRBuilder<> builder(&B, B.end());
        builder.CreateCall(newFunc);
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
