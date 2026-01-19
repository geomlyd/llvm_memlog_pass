include_directories(
    ${LLVM_MAIN_SRC_DIR}/include
    ${LLVM_MAIN_SRC_DIR}/tools/clang/include
)

add_llvm_loadable_module( LLVMMemlog
  MemlogPass.cpp
  )
