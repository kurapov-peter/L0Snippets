add_library(common helpers.cpp)
target_include_directories(common PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(common PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(common PRIVATE PkgConfig::LLVMSPIRVLib LLVM)

add_library(l0common l0helpers.cpp)
target_link_libraries(l0common PUBLIC ze_loader)
