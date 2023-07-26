#include "common/l0helpers.h"
#include <limits>

int main(int argc, char *argv[])
{
  std::string spvFilename = "test.spv";
  if (argc > 1)
  {
    spvFilename = std::string(argv[1]);
  }

  ze_device_handle_t hDevice = nullptr;
  ze_module_handle_t hModule = nullptr;
  ze_driver_handle_t hDriver = nullptr;
  ze_command_queue_handle_t hCommandQueue = nullptr;
  ze_context_handle_t hContext = nullptr;
  L0InitContext(hDriver, hDevice, hModule, hCommandQueue, hContext);
  L0BuildModule(hDevice, hContext, hModule, spvFilename, "");

  ze_command_list_handle_t hCommandList;
  ze_kernel_handle_t hKernel;

  ze_command_list_desc_t commandListDesc;
  commandListDesc.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC;
  commandListDesc.pNext = nullptr;
  commandListDesc.flags = 0;
  commandListDesc.commandQueueGroupOrdinal = 0;

  L0_SAFE_CALL(zeCommandListCreate(hContext, hDevice, &commandListDesc, &hCommandList));

  ze_kernel_desc_t kernelDesc;
  kernelDesc.stype = ZE_STRUCTURE_TYPE_KERNEL_DESC;
  kernelDesc.pNext = nullptr;
  kernelDesc.flags = 0;
  kernelDesc.pKernelName = "plus1";

  L0_SAFE_CALL(zeKernelCreate(hModule, &kernelDesc, &hKernel));

  constexpr int a_size = 32;
  AlignedArray<float, a_size> a, b;
  for (auto i = 0; i < a_size; ++i)
  {
    a.data[i] = a_size - i;
    b.data[i] = i;
  }

  ze_device_mem_alloc_desc_t alloc_desc;
  alloc_desc.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC;
  alloc_desc.pNext = nullptr;
  alloc_desc.flags = 0;
  alloc_desc.ordinal = 0;

  const float copy_size = a_size * sizeof(float);
  void *dA = nullptr;
  L0_SAFE_CALL(zeMemAllocDevice(hContext, &alloc_desc, copy_size, 0 /*align*/, hDevice, &dA));
  void *dB = nullptr;
  L0_SAFE_CALL(zeMemAllocDevice(hContext, &alloc_desc, copy_size, 0 /*align*/, hDevice, &dB));

  int sz = 32;
  L0_SAFE_CALL(zeKernelSetArgumentValue(hKernel, 0, sizeof(void *), &dA));
  L0_SAFE_CALL(zeKernelSetArgumentValue(hKernel, 1, sizeof(void *), &dB));

  void *a_void = a.data;
  L0_SAFE_CALL(zeCommandListAppendMemoryCopy(hCommandList, dA, a_void, copy_size, nullptr, 0, nullptr));
  void *b_void = b.data;
  L0_SAFE_CALL(zeCommandListAppendMemoryCopy(hCommandList, dB, b_void, copy_size, nullptr, 0, nullptr));

  L0_SAFE_CALL(zeCommandListAppendBarrier(hCommandList, nullptr, 0, nullptr));
  L0_SAFE_CALL(zeKernelSetGroupSize(hKernel, 1, 1, 1));
  ze_group_count_t dispatchTraits = {1, 1, 1};

  L0_SAFE_CALL(zeCommandListAppendLaunchKernel(hCommandList, hKernel,
                                               &dispatchTraits, nullptr, 0, nullptr));

  L0_SAFE_CALL(zeCommandListAppendBarrier(hCommandList, nullptr, 0, nullptr));
  L0_SAFE_CALL(zeCommandListAppendMemoryCopy(hCommandList, b_void, dB,
                                             copy_size, nullptr, 0, nullptr));

  L0_SAFE_CALL(zeCommandListClose(hCommandList));
  L0_SAFE_CALL(zeCommandQueueExecuteCommandLists(hCommandQueue, 1, &hCommandList,
                                                 nullptr));
  L0_SAFE_CALL(zeCommandQueueSynchronize(hCommandQueue,
                                         std::numeric_limits<uint32_t>::max()));

  for (int i = 0; i < a_size; ++i)
  {
    std::cout << b.data[i] << " ";
  }
  std::cout << std::endl;

  L0_SAFE_CALL(zeMemFree(hContext, dA));
  L0_SAFE_CALL(zeMemFree(hContext, dB));
}
