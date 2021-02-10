; Example handmade kernel
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024"
target triple = "spir-unknown-unknown"

define spir_kernel void @plus1(float addrspace(1)* %src, float addrspace(1)* %dst) #0 !kernel_arg_addr_space !5 !kernel_arg_access_qual !6 !kernel_arg_type !7 !kernel_arg_type_qual !8 !kernel_arg_base_type !7 !kernel_arg_name !9 {
entry:
    %idx = call spir_func i32 @_Z13get_global_idj(i32 0) #1
    %srcidx = getelementptr inbounds float, float addrspace(1)* %src, i32 %idx
    %arrayidx = getelementptr inbounds float, float addrspace(1)* %dst, i32 %idx
    %ld = load float, float addrspace(1)* %srcidx, align 4
    %add = fadd float %ld, 1.0
    store float %add, float addrspace(1)* %arrayidx, align 4
    ret void
}

declare spir_func i32 @_Z13get_global_idj(i32) #1

attributes #0 = { noinline nounwind }
attributes #1 = { nounwind readnone }

!spirv.Source = !{!0}
!opencl.spir.version = !{!1}
!opencl.ocl.version = !{!2}
!opencl.used.extensions = !{!3}
!opencl.used.optional.core.features = !{!3}
!spirv.Generator = !{!4}

!0 = !{i32 3, i32 100000}
!1 = !{i32 1, i32 2}
!2 = !{i32 1, i32 0}
!3 = !{}
!4 = !{i16 6, i16 14}
!5 = !{i32 1, i32 1}
!6 = !{!"none", !"none"}
!7 = !{!"float*", !"float*"}
!8 = !{!"", !""}
!9 = !{!"src", !"dst"}