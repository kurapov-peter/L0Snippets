cmake_minimum_required(VERSION 3.0)
project(L0Snippets)

add_executable(gpu_avail_check gpu_discover.cpp)
target_link_libraries(gpu_avail_check PRIVATE ze_loader)

add_executable(simple_kernel_run host.cpp)
target_link_libraries(simple_kernel_run PRIVATE ze_loader)

set(OCLKernelBC ${PROJECT_BINARY_DIR}/test.bc)
set(OCLKernelSRC ${PROJECT_SOURCE_DIR}/oclSimpleKernel.cl)
set(OCLKernelSPV ${PROJECT_BINARY_DIR}/test.spv)

add_custom_target(TestBC ALL DEPENDS ${OCLKernelBC})
add_custom_target(TestSPV ALL DEPENDS ${OCLKernelSPV})

add_custom_command(
    OUTPUT test.bc
    COMMAND clang -cc1 -triple spir -finclude-default-header ${OCLKernelSRC} -O0 -emit-llvm-bc -o ${OCLKernelBC}
    DEPENDS ${OCLKernelSRC}
)

add_custom_command(
    OUTPUT test.spv
    COMMAND llvm-spirv ${OCLKernelBC} -o ${OCLKernelSPV}
    DEPENDS ${OCLKernelBC}
)
