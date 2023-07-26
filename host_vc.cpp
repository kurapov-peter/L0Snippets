#include "common/l0helpers.h"

#include <vector>

int main(int argc, char *argv[])
{
    std::string spvFilename = "invalid";
    if (argc > 1)
    {
        spvFilename = std::string(argv[1]);
    }

    ze_driver_handle_t hDriver;
    ze_device_handle_t hDevice;
    ze_module_handle_t hModule;
    ze_command_queue_handle_t hCommandQueue;
    ze_context_handle_t hContext;

    L0InitContext(hDriver, hDevice, hModule, hCommandQueue, hContext);
    L0BuildModule(hDevice, hContext, hModule, spvFilename, "-vc-codegen -no-optimize -Xfinalizer '-presched'");

    constexpr size_t buffer_size = 8;

    std::vector<int> a(buffer_size), b(buffer_size), c(buffer_size);
    for (int i = 0; i < buffer_size; ++i)
    {
        a[i] = i;
        b[i] = i;
        c[i] = -1;
    }

    ze_command_list_handle_t hCommandList;
    ze_kernel_handle_t hKernel;

    L0CreateCommandList(hCommandList, hContext, hDevice, hCommandQueue);
    L0CreateKernel(hKernel, hModule, "vadd");

    ze_device_mem_alloc_desc_t alloc_desc;
    alloc_desc.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC;
    alloc_desc.pNext = nullptr;
    alloc_desc.flags = 0;
    alloc_desc.ordinal = 0;

    constexpr size_t copy_size = buffer_size * sizeof(int);

    void *dA = nullptr;
    void *dB = nullptr;
    void *dC = nullptr;

    L0_SAFE_CALL(zeMemAllocDevice(hContext, &alloc_desc, copy_size, 0 /*align*/, hDevice, &dA));
    L0_SAFE_CALL(zeMemAllocDevice(hContext, &alloc_desc, copy_size, 0 /*align*/, hDevice, &dB));
    L0_SAFE_CALL(zeMemAllocDevice(hContext, &alloc_desc, copy_size, 0 /*align*/, hDevice, &dC));

    L0_SAFE_CALL(zeCommandListAppendMemoryCopy(hCommandList, dA, a.data(), copy_size, nullptr, 0, nullptr));
    L0_SAFE_CALL(zeCommandListAppendMemoryCopy(hCommandList, dB, b.data(), copy_size, nullptr, 0, nullptr));
    L0_SAFE_CALL(zeCommandListAppendMemoryCopy(hCommandList, dC, c.data(), copy_size, nullptr, 0, nullptr));

    L0_SAFE_CALL(zeCommandListAppendBarrier(hCommandList, nullptr, 0, nullptr));

    L0_SAFE_CALL(zeKernelSetArgumentValue(hKernel, 0, sizeof(dA), &dA));
    L0_SAFE_CALL(zeKernelSetArgumentValue(hKernel, 1, sizeof(dB), &dB));
    L0_SAFE_CALL(zeKernelSetArgumentValue(hKernel, 2, sizeof(dC), &dC));
    L0_SAFE_CALL(zeKernelSetArgumentValue(hKernel, 3, sizeof(int), &buffer_size));

    ze_group_count_t dispatchTraits;

    dispatchTraits.groupCountX = buffer_size;
    dispatchTraits.groupCountY = 1;
    dispatchTraits.groupCountZ = 1;

    L0_SAFE_CALL(zeCommandListAppendLaunchKernel(hCommandList, hKernel, &dispatchTraits, nullptr, 0, nullptr));

    L0_SAFE_CALL(zeCommandListAppendBarrier(hCommandList, nullptr, 0, nullptr));

    L0_SAFE_CALL(zeCommandListAppendMemoryCopy(hCommandList, c.data(), dC, copy_size, nullptr, 0, nullptr));

    L0_SAFE_CALL(zeCommandListClose(hCommandList));
    L0_SAFE_CALL(zeCommandQueueExecuteCommandLists(hCommandQueue, 1, &hCommandList,
                                                   nullptr));
    L0_SAFE_CALL(zeCommandQueueSynchronize(hCommandQueue,
                                           std::numeric_limits<uint32_t>::max()));

    for (auto x : c)
    {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    L0_SAFE_CALL(zeMemFree(hContext, dA));
    L0_SAFE_CALL(zeMemFree(hContext, dB));
    L0_SAFE_CALL(zeMemFree(hContext, dC));
}