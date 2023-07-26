target triple = "spir64-unknown-unknown"
target datalayout = "e-p:64:64-i64:64-v16:16-v24:32-v32:32-v48:64-v96:128-v192:256-v256:256-v512:512-v1024:1024-n8:16:32:64"


define spir_kernel void @vadd(i32 addrspace(1)* %a, i32 addrspace(1)* %b, i32 addrspace(1)* %c, i32 %n) {
.entry:
    ; %thread_x = call i16 @llvm.genx.thread.x()
    ; %out = bitcast i32 addrspace(1)* %c to <8 x i32> addrspace(1)*
    ; call void @llvm.genx.vstore.v8i32.p1v8i32(<8 x i32> <i32 1, i32 2, i32 3, i32 4, i32 5, i32 6, i32 7, i32 8>, <8 x i32> addrspace(1)* %out)
    ; store i32 13, i32 addrspace(1)* %c, align 4
    ret void
}
