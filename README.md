# Level zero & gfx snippets

## Build
Build dependencies:
* llvm (tested with versions 10, 14, 16)
* SPIRV translator
* level zero headers

Runtime dependencies:
* level zero (NEO runtime driver)
* IGC

All dependencies can be installed with https://github.com/kurapov-peter/gfx_deps

### Docker build
```
docker build .
docker run --network host --privileged -it --name snippets snippets:latest bash
```

### Conda build
First time conda env creation:
```
conda env create -f scripts/snippets-conda-env.yml
conda activate snippets-dev
```

Update existing conda env with:
```
conda env update -f scripts/snippets-conda-env.yml
```

```
cmake -Bbuild -GNinja .
cmake --build build
```

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
