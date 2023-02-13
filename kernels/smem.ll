; Example handmade kernel
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v32:32:32-v64:64:64-v128:128:128-n16:32:64"
target triple = "spir64-unknown-unknown"

@slm.buf = internal local_unnamed_addr addrspace(3) global [1024 x i32] zeroinitializer, align 4

declare spir_func i64 @_Z32__spirv_BuiltInLocalInvocationIdi(i32)
declare spir_func void @_Z22__spirv_ControlBarrieri(i32,i32,i32)

; single group
define spir_kernel void @sum_smem(i32 addrspace(1)* %src, i32 addrspace(1)* %dst) {
entry:
    %idx.i64 = call spir_func i64 @_Z32__spirv_BuiltInLocalInvocationIdi(i32 0)
    %idx = trunc i64 %idx.i64 to i32
    %slm.idx = getelementptr inbounds [1024 x i32], [1024 x i32] addrspace(3)* @slm.buf, i32 0, i32 0 
    %srcidx = getelementptr inbounds i32, i32 addrspace(1)* %src, i32 %idx
    %ld = load i32, i32 addrspace(1)* %srcidx, align 4
    %old = atomicrmw add i32 addrspace(3)* %slm.idx, i32 %ld monotonic
    call spir_func void @_Z22__spirv_ControlBarrieri(i32 2, i32 2, i32 256)
    %res = load i32, i32 addrspace(3)* %slm.idx, align 4
    %arrayidx = getelementptr inbounds i32, i32 addrspace(1)* %dst, i32 0 
    store i32 %res, i32 addrspace(1)* %arrayidx, align 4
    ret void
}

define spir_kernel void @sum_no_smem(i32 addrspace(1)* %src, i32 addrspace(1)* %dst) {
entry:
    %idx.i64 = call spir_func i64 @_Z32__spirv_BuiltInLocalInvocationIdi(i32 0)
    %idx = trunc i64 %idx.i64 to i32
    %srcidx = getelementptr inbounds i32, i32 addrspace(1)* %src, i32 %idx
    %ld = load i32, i32 addrspace(1)* %srcidx, align 4
    %arrayidx = getelementptr inbounds i32, i32 addrspace(1)* %dst, i32 0
    %old = atomicrmw add i32 addrspace(1)* %arrayidx, i32 %ld monotonic
    ; store i32 %ld, i32 addrspace(1)* %arrayidx, align 4
    ret void
}
