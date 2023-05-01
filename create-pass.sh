#!/bin/bash

if [ $# -eq 0 ]; then
  echo "Usage: $0 <file>"
  echo "Example: $0 mypass"
  exit 1
fi

#!/bin/bash

# Get the pass name from command-line argument
pass=$1

# Convert the pass name to have only the first letter of each word capitalized
pass_name=$(echo "$pass" | sed 's/_\([a-z]\)/\U\1/g;s/^\([a-z]\)/\U\1/g')

underscored_pass_name=$(echo "$pass_name" | sed 's/\([A-Z]\)/_\L\1/g' | sed 's/^_//')


# Append the CMake code to passes/CMakeLists.txt
cat <<EOF >> passes/CMakeLists.txt

# Add $pass_name pass
add_library(${pass_name} MODULE
    ${underscored_pass_name}.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(${pass_name} PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(${pass_name} PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(${pass_name} PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)

EOF

# Append the template pass to passes folder

cat <<EOF > passes/${underscored_pass_name}.cpp
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
using namespace llvm;

namespace {
  struct ${pass_name} : public FunctionPass {
    static char ID;
    ${pass_name}() : FunctionPass(ID) {}

    virtual bool runOnFunction(Function &F) {
      errs() << "I saw a function called " << F.getName() << "!\n";
      return false;
    }
  };
}

char ${pass_name}::ID = 0;

// Automatically enable the pass.
// http://adriansampson.net/blog/clangpass.html
static void register${pass_name}(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new ${pass_name}());
}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 register${pass_name});
EOF

echo "Appended CMakeLists.txt for ${pass_name}"
echo "Created file ${underscored_pass_name}.cpp"