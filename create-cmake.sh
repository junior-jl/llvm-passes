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

# Append the CMake code to passes/CMakeLists.txt
cat <<EOF >> passes/CMakeLists.txt

# Add $pass_name pass
add_library(${pass_name} MODULE
    ${pass}.cpp
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

echo "Appended CMakeLists.txt for ${pass_name}"
