module attributes {gpu.container_module, torch.debug_module_name = "VectorAdd"} {
  llvm.func @malloc(i64) -> !llvm.ptr
  llvm.mlir.global internal constant @VectorAdd_kernel_VectorAdd_kernel_kernel_name("VectorAdd_kernel\00") {addr_space = 0 : i32}
  llvm.mlir.global internal constant @VectorAdd_kernel_spirv_binary("\03\02#\07\00\00\01\00\16\00\00\00\1D\00\00\00\00\00\00\00\11\00\02\00\0B\00\00\00\11\00\02\00\06\00\00\00\11\00\02\00\04\00\00\00\0E\00\03\00\02\00\00\00\02\00\00\00\0F\00\09\00\06\00\00\00\0C\00\00\00VectorAdd_kernel\00\00\00\00\04\00\00\00\05\00\09\00\04\00\00\00__builtin__WorkgroupId__\00\00\00\00\05\00\07\00\0C\00\00\00VectorAdd_kernel\00\00\00\00G\00\04\00\04\00\00\00\0B\00\00\00\1A\00\00\00\15\00\04\00\03\00\00\00@\00\00\00\00\00\00\00\17\00\04\00\02\00\00\00\03\00\00\00\03\00\00\00 \00\04\00\01\00\00\00\01\00\00\00\02\00\00\00;\00\04\00\01\00\00\00\04\00\00\00\01\00\00\00\13\00\02\00\06\00\00\00\16\00\03\00\09\00\00\00 \00\00\00\15\00\04\00\0A\00\00\00 \00\00\00\00\00\00\00+\00\04\00\0A\00\00\00\0B\00\00\00\02\00\00\00\1C\00\04\00\08\00\00\00\09\00\00\00\0B\00\00\00 \00\04\00\07\00\00\00\05\00\00\00\08\00\00\00!\00\05\00\05\00\00\00\06\00\00\00\07\00\00\00\07\00\00\00+\00\05\00\03\00\00\00\12\00\00\00\00\00\00\00\00\00\00\00+\00\05\00\03\00\00\00\13\00\00\00\01\00\00\00\00\00\00\00 \00\04\00\16\00\00\00\05\00\00\00\09\00\00\006\00\05\00\06\00\00\00\0C\00\00\00\00\00\00\00\05\00\00\007\00\03\00\07\00\00\00\0D\00\00\007\00\03\00\07\00\00\00\0E\00\00\00\F8\00\02\00\0F\00\00\00=\00\04\00\02\00\00\00\10\00\00\00\04\00\00\00Q\00\05\00\03\00\00\00\11\00\00\00\10\00\00\00\00\00\00\00\84\00\05\00\03\00\00\00\14\00\00\00\13\00\00\00\11\00\00\00\80\00\05\00\03\00\00\00\15\00\00\00\12\00\00\00\14\00\00\00A\00\05\00\16\00\00\00\17\00\00\00\0D\00\00\00\15\00\00\00=\00\04\00\09\00\00\00\18\00\00\00\17\00\00\00\81\00\05\00\09\00\00\00\19\00\00\00\18\00\00\00\18\00\00\00\84\00\05\00\03\00\00\00\1A\00\00\00\13\00\00\00\11\00\00\00\80\00\05\00\03\00\00\00\1B\00\00\00\12\00\00\00\1A\00\00\00A\00\05\00\16\00\00\00\1C\00\00\00\0E\00\00\00\1B\00\00\00>\00\03\00\1C\00\00\00\19\00\00\00\FD\00\01\008\00\01\00") {addr_space = 0 : i32}
  llvm.func private @refbackend_consume_func_return_mrf32(%arg0: i64, %arg1: !llvm.ptr) attributes {llvm.emit_c_interface, sym_visibility = "private"} {
    %0 = llvm.mlir.constant(1 : index) : i64
    %1 = llvm.mlir.undef : !llvm.struct<(i64, ptr)>
    %2 = llvm.insertvalue %arg0, %1[0] : !llvm.struct<(i64, ptr)> 
    %3 = llvm.insertvalue %arg1, %2[1] : !llvm.struct<(i64, ptr)> 
    %4 = llvm.alloca %0 x !llvm.struct<(i64, ptr)> : (i64) -> !llvm.ptr
    llvm.store %3, %4 : !llvm.struct<(i64, ptr)>, !llvm.ptr
    llvm.call @_mlir_ciface_refbackend_consume_func_return_mrf32(%4) : (!llvm.ptr) -> ()
    llvm.return
  }
  llvm.func @_mlir_ciface_refbackend_consume_func_return_mrf32(!llvm.ptr) attributes {llvm.emit_c_interface, sym_visibility = "private"}
  llvm.mlir.global private @global_seed(0 : i64) {addr_space = 0 : i32} : i64
  llvm.func @VectorAdd(%arg0: i64, %arg1: !llvm.ptr) attributes {llvm.emit_c_interface} {
    %0 = llvm.mlir.constant(0 : i32) : i32
    %1 = llvm.mlir.constant(1 : i64) : i64
    %2 = llvm.mlir.constant(660 : i64) : i64
    %3 = llvm.mlir.constant(0 : i64) : i64
    %4 = llvm.mlir.constant(1 : i32) : i32
    %5 = llvm.mlir.constant(64 : i64) : i64
    %6 = llvm.mlir.constant(1 : index) : i64
    %7 = llvm.mlir.constant(2 : index) : i64
    %8 = llvm.mlir.undef : !llvm.struct<(i64, ptr)>
    %9 = llvm.insertvalue %arg0, %8[0] : !llvm.struct<(i64, ptr)> 
    %10 = llvm.insertvalue %arg1, %9[1] : !llvm.struct<(i64, ptr)> 
    %11 = llvm.load %arg1 : !llvm.ptr -> !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)>
    %12 = llvm.mlir.null : !llvm.ptr<i8>
    %13 = llvm.mlir.null : !llvm.ptr<i8>
    %14 = llvm.call @gpuCreateStream(%12, %13) : (!llvm.ptr<i8>, !llvm.ptr<i8>) -> !llvm.ptr<i8>
    %15 = llvm.mlir.null : !llvm.ptr
    %16 = llvm.getelementptr %15[2] : (!llvm.ptr) -> !llvm.ptr, f32
    %17 = llvm.ptrtoint %16 : !llvm.ptr to i64
    %18 = llvm.call @gpuMemAlloc(%14, %17, %5, %4) : (!llvm.ptr<i8>, i64, i64, i32) -> !llvm.ptr<i8>
    %19 = llvm.mlir.undef : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)>
    %20 = llvm.bitcast %18 : !llvm.ptr<i8> to !llvm.ptr
    %21 = llvm.insertvalue %20, %19[0] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %22 = llvm.bitcast %18 : !llvm.ptr<i8> to !llvm.ptr
    %23 = llvm.insertvalue %22, %21[1] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %24 = llvm.insertvalue %3, %23[2] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %25 = llvm.insertvalue %7, %24[3, 0] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %26 = llvm.insertvalue %6, %25[4, 0] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %27 = llvm.mlir.constant(1 : index) : i64
    %28 = llvm.extractvalue %11[3, 0] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %29 = llvm.mul %28, %27  : i64
    %30 = llvm.mlir.null : !llvm.ptr
    %31 = llvm.getelementptr %30[1] : (!llvm.ptr) -> !llvm.ptr, f32
    %32 = llvm.ptrtoint %31 : !llvm.ptr to i64
    %33 = llvm.mul %29, %32  : i64
    %34 = llvm.extractvalue %11[1] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %35 = llvm.extractvalue %11[2] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %36 = llvm.getelementptr %34[%35] : (!llvm.ptr, i64) -> !llvm.ptr, f32
    %37 = llvm.getelementptr %22[%3] : (!llvm.ptr, i64) -> !llvm.ptr, f32
    "llvm.intr.memcpy"(%37, %36, %33) <{isVolatile = false}> : (!llvm.ptr, !llvm.ptr, i64) -> ()
    %38 = llvm.mlir.null : !llvm.ptr
    %39 = llvm.getelementptr %38[2] : (!llvm.ptr) -> !llvm.ptr, f32
    %40 = llvm.ptrtoint %39 : !llvm.ptr to i64
    %41 = llvm.call @gpuMemAlloc(%14, %40, %5, %4) : (!llvm.ptr<i8>, i64, i64, i32) -> !llvm.ptr<i8>
    %42 = llvm.mlir.undef : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)>
    %43 = llvm.bitcast %41 : !llvm.ptr<i8> to !llvm.ptr
    %44 = llvm.insertvalue %43, %42[0] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %45 = llvm.bitcast %41 : !llvm.ptr<i8> to !llvm.ptr
    %46 = llvm.insertvalue %45, %44[1] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %47 = llvm.insertvalue %3, %46[2] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %48 = llvm.insertvalue %7, %47[3, 0] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %49 = llvm.insertvalue %6, %48[4, 0] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %50 = llvm.mlir.addressof @VectorAdd_kernel_spirv_binary : !llvm.ptr<array<660 x i8>>
    %51 = llvm.getelementptr %50[0, 0] : (!llvm.ptr<array<660 x i8>>) -> !llvm.ptr<i8>
    %52 = llvm.call @gpuModuleLoad(%14, %51, %2) : (!llvm.ptr<i8>, !llvm.ptr<i8>, i64) -> !llvm.ptr<i8>
    %53 = llvm.mlir.addressof @VectorAdd_kernel_VectorAdd_kernel_kernel_name : !llvm.ptr<array<17 x i8>>
    %54 = llvm.getelementptr %53[0, 0] : (!llvm.ptr<array<17 x i8>>) -> !llvm.ptr<i8>
    %55 = llvm.call @gpuKernelGet(%14, %52, %54) : (!llvm.ptr<i8>, !llvm.ptr<i8>, !llvm.ptr<i8>) -> !llvm.ptr<i8>
    %56 = llvm.alloca %1 x !llvm.ptr : (i64) -> !llvm.ptr<ptr>
    %57 = llvm.alloca %1 x !llvm.ptr : (i64) -> !llvm.ptr<ptr>
    %58 = llvm.alloca %1 x !llvm.array<3 x struct<(ptr<i8>, i64)>> : (i64) -> !llvm.ptr<array<3 x struct<(ptr<i8>, i64)>>>
    %59 = llvm.mlir.undef : !llvm.array<3 x struct<(ptr<i8>, i64)>>
    %60 = llvm.mlir.null : !llvm.ptr<ptr>
    %61 = llvm.getelementptr %60[1] : (!llvm.ptr<ptr>) -> !llvm.ptr<ptr>
    %62 = llvm.ptrtoint %61 : !llvm.ptr<ptr> to i64
    llvm.store %22, %56 : !llvm.ptr<ptr>
    %63 = llvm.bitcast %56 : !llvm.ptr<ptr> to !llvm.ptr<i8>
    %64 = llvm.mlir.undef : !llvm.struct<(ptr<i8>, i64)>
    %65 = llvm.insertvalue %63, %64[0] : !llvm.struct<(ptr<i8>, i64)> 
    %66 = llvm.insertvalue %62, %65[1] : !llvm.struct<(ptr<i8>, i64)> 
    %67 = llvm.insertvalue %66, %59[0] : !llvm.array<3 x struct<(ptr<i8>, i64)>> 
    %68 = llvm.mlir.null : !llvm.ptr<ptr>
    %69 = llvm.getelementptr %68[1] : (!llvm.ptr<ptr>) -> !llvm.ptr<ptr>
    %70 = llvm.ptrtoint %69 : !llvm.ptr<ptr> to i64
    llvm.store %45, %57 : !llvm.ptr<ptr>
    %71 = llvm.bitcast %57 : !llvm.ptr<ptr> to !llvm.ptr<i8>
    %72 = llvm.mlir.undef : !llvm.struct<(ptr<i8>, i64)>
    %73 = llvm.insertvalue %71, %72[0] : !llvm.struct<(ptr<i8>, i64)> 
    %74 = llvm.insertvalue %70, %73[1] : !llvm.struct<(ptr<i8>, i64)> 
    %75 = llvm.insertvalue %74, %67[1] : !llvm.array<3 x struct<(ptr<i8>, i64)>> 
    %76 = llvm.mlir.null : !llvm.ptr<i8>
    %77 = llvm.mlir.undef : !llvm.struct<(ptr<i8>, i64)>
    %78 = llvm.insertvalue %76, %77[0] : !llvm.struct<(ptr<i8>, i64)> 
    %79 = llvm.insertvalue %3, %78[1] : !llvm.struct<(ptr<i8>, i64)> 
    %80 = llvm.insertvalue %79, %75[2] : !llvm.array<3 x struct<(ptr<i8>, i64)>> 
    llvm.store %80, %58 : !llvm.ptr<array<3 x struct<(ptr<i8>, i64)>>>
    %81 = llvm.bitcast %58 : !llvm.ptr<array<3 x struct<(ptr<i8>, i64)>>> to !llvm.ptr<struct<(ptr<i8>, i64)>>
    llvm.call @gpuLaunchKernel(%14, %55, %7, %6, %6, %6, %6, %6, %0, %81) : (!llvm.ptr<i8>, !llvm.ptr<i8>, i64, i64, i64, i64, i64, i64, i32, !llvm.ptr<struct<(ptr<i8>, i64)>>) -> ()
    llvm.call @gpuWait(%14) : (!llvm.ptr<i8>) -> ()
    %82 = llvm.mlir.constant(2 : index) : i64
    %83 = llvm.mlir.constant(1 : index) : i64
    %84 = llvm.mlir.null : !llvm.ptr
    %85 = llvm.getelementptr %84[2] : (!llvm.ptr) -> !llvm.ptr, f32
    %86 = llvm.ptrtoint %85 : !llvm.ptr to i64
    %87 = llvm.call @malloc(%86) : (i64) -> !llvm.ptr
    %88 = llvm.mlir.undef : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)>
    %89 = llvm.insertvalue %87, %88[0] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %90 = llvm.insertvalue %87, %89[1] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %91 = llvm.mlir.constant(0 : index) : i64
    %92 = llvm.insertvalue %91, %90[2] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %93 = llvm.insertvalue %82, %92[3, 0] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %94 = llvm.insertvalue %83, %93[4, 0] : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> 
    %95 = llvm.mlir.constant(1 : index) : i64
    %96 = llvm.mul %7, %95  : i64
    %97 = llvm.mlir.null : !llvm.ptr
    %98 = llvm.getelementptr %97[1] : (!llvm.ptr) -> !llvm.ptr, f32
    %99 = llvm.ptrtoint %98 : !llvm.ptr to i64
    %100 = llvm.mul %96, %99  : i64
    %101 = llvm.getelementptr %45[%3] : (!llvm.ptr, i64) -> !llvm.ptr, f32
    %102 = llvm.getelementptr %87[%91] : (!llvm.ptr, i64) -> !llvm.ptr, f32
    "llvm.intr.memcpy"(%102, %101, %100) <{isVolatile = false}> : (!llvm.ptr, !llvm.ptr, i64) -> ()
    llvm.call @gpuMemFree(%14, %41) : (!llvm.ptr<i8>, !llvm.ptr<i8>) -> ()
    llvm.call @gpuMemFree(%14, %18) : (!llvm.ptr<i8>, !llvm.ptr<i8>) -> ()
    llvm.call @gpuStreamDestroy(%14) : (!llvm.ptr<i8>) -> ()
    %103 = llvm.mlir.constant(1 : index) : i64
    %104 = llvm.alloca %103 x !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)> : (i64) -> !llvm.ptr
    llvm.store %94, %104 : !llvm.struct<(ptr, ptr, i64, array<1 x i64>, array<1 x i64>)>, !llvm.ptr
    %105 = llvm.mlir.constant(1 : index) : i64
    %106 = llvm.mlir.undef : !llvm.struct<(i64, ptr)>
    %107 = llvm.insertvalue %105, %106[0] : !llvm.struct<(i64, ptr)> 
    %108 = llvm.insertvalue %104, %107[1] : !llvm.struct<(i64, ptr)> 
    %109 = llvm.extractvalue %108[0] : !llvm.struct<(i64, ptr)> 
    %110 = llvm.extractvalue %108[1] : !llvm.struct<(i64, ptr)> 
    llvm.call @refbackend_consume_func_return_mrf32(%109, %110) : (i64, !llvm.ptr) -> ()
    llvm.return
  }
  llvm.func @_mlir_ciface_VectorAdd(%arg0: !llvm.ptr) attributes {llvm.emit_c_interface} {
    %0 = llvm.load %arg0 : !llvm.ptr -> !llvm.struct<(i64, ptr)>
    %1 = llvm.extractvalue %0[0] : !llvm.struct<(i64, ptr)> 
    %2 = llvm.extractvalue %0[1] : !llvm.struct<(i64, ptr)> 
    llvm.call @VectorAdd(%1, %2) : (i64, !llvm.ptr) -> ()
    llvm.return
  }
  llvm.func @gpuCreateStream(!llvm.ptr<i8>, !llvm.ptr<i8>) -> !llvm.ptr<i8>
  llvm.func @gpuMemAlloc(!llvm.ptr<i8>, i64, i64, i32) -> !llvm.ptr<i8>
  llvm.func @gpuModuleLoad(!llvm.ptr<i8>, !llvm.ptr<i8>, i64) -> !llvm.ptr<i8>
  llvm.func @gpuKernelGet(!llvm.ptr<i8>, !llvm.ptr<i8>, !llvm.ptr<i8>) -> !llvm.ptr<i8>
  llvm.func @gpuLaunchKernel(!llvm.ptr<i8>, !llvm.ptr<i8>, i64, i64, i64, i64, i64, i64, i32, !llvm.ptr<struct<(ptr<i8>, i64)>>)
  llvm.func @gpuWait(!llvm.ptr<i8>)
  llvm.func @gpuMemFree(!llvm.ptr<i8>, !llvm.ptr<i8>)
  llvm.func @gpuStreamDestroy(!llvm.ptr<i8>)
}

