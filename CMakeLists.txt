cmake_minimum_required(VERSION 3.5)
project(L0Snippets)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(gpu_avail_check gpu_discover.cpp)
target_link_libraries(gpu_avail_check PRIVATE ze_loader)


find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
llvm_map_components_to_libnames(llvm_libs support core irreader bitwriter)
message(STATUS "llvm libs: ${llvm_libs}")

find_package(PkgConfig REQUIRED)
pkg_check_modules(LLVMSPIRVLib REQUIRED IMPORTED_TARGET LLVMSPIRVLib)

add_subdirectory(common)

add_executable(simple_kernel_run host.cpp)
target_link_libraries(simple_kernel_run PRIVATE l0common)

add_executable(smem_test smem_host.cpp)
target_link_libraries(smem_test PRIVATE l0common)

add_executable(vc_test host_vc.cpp)
target_link_libraries(vc_test PRIVATE l0common)

add_executable(ll_gen ll_gen.cpp)
target_include_directories(ll_gen PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(ll_gen PRIVATE ${LLVM_DEFINITIONS})
target_link_libraries(ll_gen PRIVATE ${llvm_libs})


add_executable(spv_gen spv_gen.cpp)
target_link_libraries(spv_gen PRIVATE common)

add_executable(complete complete_flow.cpp)
target_link_libraries(complete PRIVATE common l0common)


set(OCLKernelBC ${PROJECT_BINARY_DIR}/test.bc)
set(OCLKernelSRC ${PROJECT_SOURCE_DIR}/kernels/oclSimpleKernel.cl)
set(OCLKernelSPV ${PROJECT_BINARY_DIR}/test.spv)
set(HandWrittenLL ${PROJECT_SOURCE_DIR}/kernels/kernel_sample.ll)
set(HandWrittenBC ${PROJECT_BINARY_DIR}/hand.bc)
set(HandWrittenSPV ${PROJECT_BINARY_DIR}/hand.spv)
set(SharedMemLL ${PROJECT_SOURCE_DIR}/kernels/smem.ll)
set(SharedMemBC ${PROJECT_BINARY_DIR}/smem.bc)
set(SharedMemSPV ${PROJECT_BINARY_DIR}/smem.spv)
set(vcLL ${PROJECT_SOURCE_DIR}/kernels/vc.ll)
set(vcBC ${PROJECT_BINARY_DIR}/vc.bc)
set(vcSPV ${PROJECT_BINARY_DIR}/vc.spv)

add_custom_target(TestBC ALL DEPENDS ${OCLKernelBC})
add_custom_target(TestSPV ALL DEPENDS ${OCLKernelSPV})
add_custom_target(HandBC ALL DEPENDS ${HandWrittenBC})
add_custom_target(HandSPV ALL DEPENDS ${HandWrittenSPV})
add_custom_target(SmemBC ALL DEPENDS ${SharedMemBC})
add_custom_target(SmemSPV ALL DEPENDS ${SharedMemSPV})
add_custom_target(VCBC ALL DEPENDS ${vcBC})
add_custom_target(VCSPV ALL DEPENDS ${vcSPV})

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

add_custom_command(
    OUTPUT ${SharedMemBC}
    COMMAND llvm-as ${SharedMemLL} -o ${SharedMemBC}
    DEPENDS ${SharedMemLL}
)

add_custom_command(
    OUTPUT ${vcBC}
    COMMAND llvm-as ${vcLL} -o ${vcBC}
    DEPENDS ${vcLL}
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
add_compile_spirv_cmd(${SharedMemBC} ${SharedMemSPV})
add_compile_spirv_cmd(${vcBC} ${vcSPV})
