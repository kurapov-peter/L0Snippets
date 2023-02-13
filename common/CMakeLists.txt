add_library(common helpers.cpp)
target_include_directories(common PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(common PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(common PRIVATE PkgConfig::LLVMSPIRVLib LLVM)
