; Example handmade kernel
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v32:32:32-v64:64:64-v128:128:128-n16:32:64"
target triple = "spir64-unknown-unknown"

@slm.buf = internal local_unnamed_addr addrspace(3) global [1024 x i32] zeroinitializer, align 4
@slm.buf.i64 = internal local_unnamed_addr addrspace(3) global [1024 x i64] zeroinitializer, align 8

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

define spir_func i32 @atomic_add(i32 addrspace(4)* %ptr, i32 %val) {
entry:
    %old = atomicrmw add i32 addrspace(4)* %ptr, i32 %val monotonic
    ret i32 %old
}

; simulate an external function call with generic address spaces
define spir_kernel void @sum_smem_with_cast(i32 addrspace(1)* %src, i32 addrspace(1)* %dst) {
entry:
    %idx.i64 = call spir_func i64 @_Z32__spirv_BuiltInLocalInvocationIdi(i32 0)
    %idx = trunc i64 %idx.i64 to i32
    %slm.idx = getelementptr inbounds [1024 x i32], [1024 x i32] addrspace(3)* @slm.buf, i32 0, i32 0
    %srcidx = getelementptr inbounds i32, i32 addrspace(1)* %src, i32 %idx
    %ld = load i32, i32 addrspace(1)* %srcidx, align 4
    %cast = addrspacecast i32 addrspace(3)* %slm.idx to i32 addrspace(4)*
    %old = call spir_func i32 @atomic_add(i32 addrspace(4)* %cast, i32 %ld)
    call spir_func void @_Z22__spirv_ControlBarrieri(i32 2, i32 2, i32 256)
    %res = load i32, i32 addrspace(3)* %slm.idx, align 4
    %arrayidx = getelementptr inbounds i32, i32 addrspace(1)* %dst, i32 0
    store i32 %res, i32 addrspace(1)* %arrayidx, align 4
    ret void
}

define spir_func i32 @get_idx() {
entry:
    %idx.i64 = call spir_func i64 @_Z32__spirv_BuiltInLocalInvocationIdi(i32 0)
    %idx = trunc i64 %idx.i64 to i32
    ret i32 %idx
}

define spir_func i32 addrspace(3)* @init_smem(i32 addrspace(4)* %ptr, i32 %val) {
entry:
    %idx = call spir_func i32 @get_idx()
    %ld = load i32, i32 addrspace(4)* %ptr, align 4
    %slm.idx = getelementptr inbounds [1024 x i32], [1024 x i32] addrspace(3)* @slm.buf, i32 0, i32 %idx
    store i32 %val, i32 addrspace(3)* %slm.idx, align 4
    %res = bitcast [1024 x i32] addrspace(3)* @slm.buf to i32 addrspace(3)*
    ret i32 addrspace(3)* %res
}

define spir_kernel void @sum_smem_with_init(i32 addrspace(1)* %src, i32 addrspace(1)* %dst) {
entry:
    %idx = call spir_func i32 @get_idx()
    %dst.casted = addrspacecast i32 addrspace(1)* %dst to i32 addrspace(4)*
    %slm = call spir_func i32 addrspace(3)* @init_smem(i32 addrspace(4)* %dst.casted, i32 0)
    %srcidx = getelementptr inbounds i32, i32 addrspace(1)* %src, i32 %idx
    %ld = load i32, i32 addrspace(1)* %srcidx, align 4
    %slm.idx = getelementptr inbounds i32, i32 addrspace(3)* %slm, i32 0
    %old = atomicrmw add i32 addrspace(3)* %slm.idx, i32 %ld monotonic
    call spir_func void @_Z22__spirv_ControlBarrieri(i32 2, i32 2, i32 256)
    %res = load i32, i32 addrspace(3)* %slm.idx, align 4
    %arrayidx = getelementptr inbounds i32, i32 addrspace(1)* %dst, i32 0
    store i32 %res, i32 addrspace(1)* %arrayidx, align 4
    ret void
}

define spir_kernel void @sum_smem_i64(i64 addrspace(1)* %src, i64 addrspace(1)* %dst) {
entry:
    %idx = call spir_func i64 @_Z32__spirv_BuiltInLocalInvocationIdi(i32 0)
    %slm.idx = getelementptr inbounds [1024 x i64], [1024 x i64] addrspace(3)* @slm.buf.i64, i64 0, i64 0    
    %srcidx = getelementptr inbounds i64, i64 addrspace(1)* %src, i64 %idx
    %ld = load i64, i64 addrspace(1)* %srcidx, align 8
    %cast = addrspacecast i64 addrspace(3)* %slm.idx to i64 addrspace(4)*
    %old = atomicrmw add i64 addrspace(3)* %slm.idx, i64 %ld monotonic
    call spir_func void @_Z22__spirv_ControlBarrieri(i32 2, i32 2, i32 256)
    %res = load i64, i64 addrspace(3)* %slm.idx, align 8
    %arrayidx = getelementptr inbounds i64, i64 addrspace(1)* %dst, i64 0
    store i64 %res, i64 addrspace(1)* %arrayidx, align 8
    ret void
}
