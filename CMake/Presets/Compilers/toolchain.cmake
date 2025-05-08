# Specify the target operating system
set(CMAKE_SYSTEM_NAME Windows)

# Specify the compilers
set(CMAKE_C_COMPILER clang-cl)
set(CMAKE_CXX_COMPILER clang-cl)

# Specify the linker
set(CMAKE_LINKER lld-link)

# Specify the paths to the LLVM tools
set(LLVM_ROOT "C:/LLVM16")
set(CMAKE_C_COMPILER "${LLVM_ROOT}/bin/clang-cl.exe")
set(CMAKE_CXX_COMPILER "${LLVM_ROOT}/bin/clang-cl.exe")
set(CMAKE_LINKER "${LLVM_ROOT}/bin/lld-link.exe")

