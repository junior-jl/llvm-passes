add_library(PrintFrequency MODULE
    print_frequency.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(PrintFrequency PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(PrintFrequency PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(PrintFrequency PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)

add_library(PrintAllFuncPass MODULE
    print_all_func_pass.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(PrintAllFuncPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(PrintAllFuncPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(PrintAllFuncPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)



# Add CreateVoidCall pass
add_library(CreateVoidCall MODULE
    create_void_call.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(CreateVoidCall PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(CreateVoidCall PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(CreateVoidCall PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)


# Add MultiplicationShiftsPass pass
add_library(MultiplicationShiftsPass MODULE
    multiplication_shifts_pass.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(MultiplicationShiftsPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(MultiplicationShiftsPass PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(MultiplicationShiftsPass PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)


# Add PrintFrequencyDenseMap pass
add_library(PrintFrequencyDenseMap MODULE
    print_frequency_dense_map.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(PrintFrequencyDenseMap PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(PrintFrequencyDenseMap PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(PrintFrequencyDenseMap PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)


# Add CreatePrintfCall pass
add_library(CreatePrintfCall MODULE
    create_printf_call.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(CreatePrintfCall PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(CreatePrintfCall PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(CreatePrintfCall PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)


# Add CreateIntrinsicCall pass
add_library(CreateIntrinsicCall MODULE
    create_intrinsic_call.cpp
)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(CreateIntrinsicCall PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(CreateIntrinsicCall PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
)

# Get proper shared-library behavior (where symbols are not necessarily
# resolved when the shared library is linked) on OS X.
if(APPLE)
    set_target_properties(CreateIntrinsicCall PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
    )
endif(APPLE)

