cmake_minimum_required(VERSION 3.0)
project(L0Snippets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(gpu_avail_check gpu_discover.cpp)
target_link_libraries(gpu_avail_check PRIVATE ze_loader)

add_executable(simple_kernel_run host.cpp)
target_link_libraries(simple_kernel_run PRIVATE ze_loader)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


add_executable(ll_gen ll_gen.cpp)
target_include_directories(ll_gen PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(ll_gen PRIVATE ${LLVM_DEFINITIONS})
llvm_map_components_to_libnames(llvm_libs support core irreader bitwriter)
target_link_libraries(ll_gen PRIVATE ${llvm_libs})

find_package(PkgConfig REQUIRED)
pkg_check_modules(LLVMSPIRVLib REQUIRED IMPORTED_TARGET LLVMSPIRVLib)

add_executable(spv_gen spv_gen.cpp)
target_include_directories(spv_gen PUBLIC ${LLVM_INCLUDE_DIRS})
target_compile_definitions(spv_gen PUBLIC ${LLVM_DEFINITIONS})
message(STATUS "llvm libs: ${llvm_libs}")
target_link_libraries(spv_gen PRIVATE PkgConfig::LLVMSPIRVLib LLVM)

add_executable(complete complete_flow.cpp)
target_include_directories(complete PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(complete PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(complete PRIVATE PkgConfig::LLVMSPIRVLib LLVM ze_loader)


set(OCLKernelBC ${PROJECT_BINARY_DIR}/test.bc)
set(OCLKernelSRC ${PROJECT_SOURCE_DIR}/oclSimpleKernel.cl)
set(OCLKernelSPV ${PROJECT_BINARY_DIR}/test.spv)
set(HandWrittenLL ${PROJECT_SOURCE_DIR}/kernel_sample.ll)
set(HandWrittenBC ${PROJECT_BINARY_DIR}/hand.bc)
set(HandWrittenSPV ${PROJECT_BINARY_DIR}/hand.spv)

add_custom_target(TestBC ALL DEPENDS ${OCLKernelBC})
add_custom_target(TestSPV ALL DEPENDS ${OCLKernelSPV})
add_custom_target(HandBC ALL DEPENDS ${HandWrittenBC})
add_custom_target(HandSPV ALL DEPENDS ${HandWrittenSPV})

add_custom_command(
    OUTPUT test.bc
    COMMAND clang -cc1 -triple spir -finclude-default-header ${OCLKernelSRC} -O0 -emit-llvm-bc -o ${OCLKernelBC}
    DEPENDS ${OCLKernelSRC}
)

add_custom_command(
    OUTPUT ${HandWrittenBC}
    COMMAND llvm-as ${HandWrittenLL} -o ${HandWrittenBC}
    DEPENDS ${HandWrittenLL}
)

function(add_compile_spirv_cmd IN OUT)
    add_custom_command(
        OUTPUT ${OUT}
        COMMAND llvm-spirv ${IN} -o ${OUT}
        DEPENDS ${IN}
    )
endfunction()

add_compile_spirv_cmd(${OCLKernelBC} ${OCLKernelSPV})
add_compile_spirv_cmd(${HandWrittenBC} ${HandWrittenSPV})