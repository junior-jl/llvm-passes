//===- CreatePrintfCall - Insert a printf() call in main() function  -===//
//
//    This pass inserts a call to the printf() function in the main() function of the input program.
//
// NOTE:
//    The location of the call can be changed by modifying the "step" variable in the runOnFunction() method.
//
// Author: José Lira Junior
//===----------------------------------------------------------------------===//

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
using namespace llvm;

namespace {
  struct CreatePrintfCall : public FunctionPass {
    static char ID;
    CreatePrintfCall() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      LLVMContext& ctx = F.getContext();
      // int printf(const char *, ...)
      if (F.getName() == "main")
      {
        // step tells the number of instruction that the new call will assume (starting by zero)
        int step = 0;
        Module *M = F.getParent();
        auto printf = M->getOrInsertFunction("printf", Type::getInt32Ty(ctx), Type::getInt8PtrTy(ctx));

        const char* str= "Hello, Junior\n";
        auto constStr = ConstantDataArray::getString(ctx, str);
        auto globalStr = M->getOrInsertGlobal("Str", constStr->getType());
        dyn_cast<GlobalVariable>(globalStr)->setInitializer(constStr);
        for (auto &B : F)
        {
          for (auto I = B.begin(); I != B.end(); ++I, --step)
          {
            if (step == 0)
            {
              IRBuilder<> builder(&B, I);
              auto inputStr = builder.CreatePointerCast(globalStr, Type::getInt8PtrTy(ctx), "inputStr");
              builder.CreateCall(printf, {inputStr});
            }
          }
        }
      }
      
      
      return true;
    }
  };
}

char CreatePrintfCall::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerCreatePrintfCall(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new CreatePrintfCall());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerCreatePrintfCall);
