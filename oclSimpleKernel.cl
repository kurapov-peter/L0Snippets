__kernel void plus1(__global float* src, __global float* dst) {
    int idx = get_global_id(0);
    dst[idx] = src[idx] + 1;
}