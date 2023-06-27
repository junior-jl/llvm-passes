#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"
using namespace llvm;

namespace {
  struct CreateIntrinsicCall : public FunctionPass {
    static char ID;
    CreateIntrinsicCall() : FunctionPass(ID) {}
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
                        FunctionType *fTy = FunctionType::get(or_inst->getType(), {operand1->getType(), operand2->getType()}, false);
                        auto orn_func = Intrinsic::getDeclaration(M, Intrinsic::arithmetic_fence, fTy);
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

char CreateIntrinsicCall::ID = 0;
static RegisterPass<CreateIntrinsicCall>
Y("create-intrinsic", "Anything");

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerCreateIntrinsicCall(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new CreateIntrinsicCall());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerCreateIntrinsicCall);
