#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/IRBuilder.h"


using namespace llvm;

// CreateAdd(Value * LHS, Value * RHS, cont Twine & Name = "", bool HasNUW=false, bool HasNSW=false)
// CreateShl(Value * LHS, [const APInt/uint64_t/Value*] & RHS, cont Twine & Name = "", bool HasNUW=false, bool HasNSW=false)
// CreateLShr(Value * LHS, [const APInt/uint64_t/Value*] & RHS, cont Twine & Name = "", bool HasNUW=false, bool HasNSW=false)

namespace {
  struct MultiplicationShiftsPass : public FunctionPass {
    static char ID;
    MultiplicationShiftsPass() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) override{
      bool modified = false;
      std::vector<Instruction*> to_delete;
      for (auto &B : F) 
      {
        for (auto &I : B) 
        {
          if (auto *mul = dyn_cast<BinaryOperator>(&I))
          {
            if (mul->getOpcode() == Instruction::Mul)
            {
              IRBuilder<> builder(mul);
              Value* lhs = mul->getOperand(0);            
              Value* rhs = mul->getOperand(1);
              
              ConstantInt *cst;
              if ((cst = cast<ConstantInt>(rhs)))
              {
                  if (cst->getValue().isPowerOf2())
                  {
                    unsigned int amount = cst->getValue().countTrailingZeros();           
                    Value* shift = builder.CreateShl(lhs, amount);
                    mul->replaceAllUsesWith(shift);
                    modified = true;
                    to_delete.push_back(mul);
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
      
      return modified;
    }
     StringRef getPassName() const override {
    return "multiplication-shifts-pass";
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


