#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/ADT/StringRef.h"
#include <unordered_map>
using namespace llvm;

namespace {
  struct PrintFrequency : public FunctionPass {
    static char ID;
    PrintFrequency() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      std::unordered_map<const char *, int> occurrences; 
      for (auto &B : F) 
      {
        for (auto &I : B) 
        {
          auto opcode = I.getOpcodeName();
          auto it = occurrences.find(opcode);
          if (it != occurrences.end()) 
          {
            it->second++;
          } else {
            occurrences.emplace(opcode, 1);
          }
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

char PrintFrequency::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void registerSkeletonPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new PrintFrequency());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerSkeletonPass);
