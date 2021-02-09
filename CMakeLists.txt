cmake_minimum_required(VERSION 3.0)
project(L0Snippets)

add_executable(gpu_avail_check gpu_discover.cpp)
target_link_libraries(gpu_avail_check PRIVATE ze_loader)

add_executable(simple_kernel_run host.cpp)
target_link_libraries(simple_kernel_run PRIVATE ze_loader)

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