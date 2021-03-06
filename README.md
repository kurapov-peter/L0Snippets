# Level zero & gfx snippets

## Build
Build dependencies:
* llvm (targets llvm10)
* SPIRV translator
* level zero headers

Runtime dependencies:
* level zero (NEO runtime driver)
* IGC

All dependencies can be installed with https://github.com/kurapov-peter/gfx_deps

## Check gpu availability via L0

Compile manually:  
`g++ gpu_discover.cpp -L/path/to/libze_loader/dir -Wl,-rpath=/path/to/libze_loader/dir -lze_loader`

Cmake:  
mkdir build && cd build  
cmake ..  
PATH=/path/to/llvm-spirv:$PATH make -j`nproc`

## Code generated spirv execution
```
./ll_gen  // would generate gen.bc  
llvm-spirv gen.bc -o gen.spv  
./simple_kernel_run gen.spv
```

## Generated spirv execution
```
./spv_gen  // would generate spv_gen.spv  
./simple_kernel_run spv_gen.spv
```

## Complete flow
```
./complete  // generates spirv in memory and runs it with similar to host.cpp code
```
