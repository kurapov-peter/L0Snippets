#include "common/helpers.h"
#include "common/l0helpers.h"

#include <level_zero/ze_api.h>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <iterator>
#include <cassert>

#include <memory>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

void L0BuildModuleInMemory(ze_device_handle_t &hDevice,
                           ze_context_handle_t &hContext,
                           ze_module_handle_t &hModule,
                           const std::string &spv)
{
  size_t codeSize = spv.size();
  assert(codeSize != 0 && "Code size is 0.");
  unsigned char *codeBin = new unsigned char[codeSize];
  std::copy(spv.data(), spv.data() + codeSize, codeBin);

  std::ofstream out("complete.spv", std::ios::binary);
  out.write((char *)codeBin, codeSize);

  assert(codeSize && "CodeBin is empty");

  ze_module_desc_t moduleDesc;
  moduleDesc.stype = ZE_STRUCTURE_TYPE_MODULE_DESC;
  moduleDesc.pNext = nullptr;
  moduleDesc.pBuildFlags = "";
  moduleDesc.format = ZE_MODULE_FORMAT_IL_SPIRV;
  moduleDesc.inputSize = codeSize;
  moduleDesc.pConstants = nullptr;
  moduleDesc.pInputModule = (uint8_t *)codeBin;

  ze_module_build_log_handle_t buildlog = nullptr;
  L0_SAFE_CALL(
      zeModuleCreate(hContext, hDevice, &moduleDesc, &hModule, &buildlog));
  size_t szLog = 0;
  L0_SAFE_CALL(zeModuleBuildLogGetString(buildlog, &szLog, nullptr));
  std::cout << "Got build log size " << szLog << std::endl;
  char *strLog = (char *)malloc(szLog);
  L0_SAFE_CALL(zeModuleBuildLogGetString(buildlog, &szLog, strLog));
  std::fstream log;
  log.open("log.txt", std::ios::app);
  if (!log.good())
  {
    std::cerr << "Unable to open log file" << std::endl;
    exit(1);
  }
  log << strLog;
  log.close();
}

int main(int argc, char *argv[])
{
  auto spv = generatePlusOneSPV();

  ze_device_handle_t hDevice = nullptr;
  ze_module_handle_t hModule = nullptr;
  ze_driver_handle_t hDriver = nullptr;
  ze_command_queue_handle_t hCommandQueue = nullptr;
  ze_context_handle_t hContext = nullptr;
  L0InitContext(hDriver, hDevice, hModule, hCommandQueue, hContext);
  L0BuildModuleInMemory(hDevice, hContext, hModule, spv);

  ze_command_list_handle_t hCommandList;
  ze_kernel_handle_t hKernel;

  L0CreateCommandList(hCommandList, hContext, hDevice, hCommandQueue);
  L0CreateKernel(hKernel, hModule, "plus1");

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
  L0_SAFE_CALL(zeMemAllocDevice(hContext, &alloc_desc, copy_size, 0 /*align*/,
                                hDevice, &dA));
  void *dB = nullptr;
  L0_SAFE_CALL(zeMemAllocDevice(hContext, &alloc_desc, copy_size, 0 /*align*/,
                                hDevice, &dB));

  int sz = 32;
  L0_SAFE_CALL(zeKernelSetArgumentValue(hKernel, 0, sizeof(void *), &dA));
  L0_SAFE_CALL(zeKernelSetArgumentValue(hKernel, 1, sizeof(void *), &dB));

  void *a_void = a.data;
  L0_SAFE_CALL(zeCommandListAppendMemoryCopy(hCommandList, dA, a_void,
                                             copy_size, nullptr, 0, nullptr));
  void *b_void = b.data;
  L0_SAFE_CALL(zeCommandListAppendMemoryCopy(hCommandList, dB, b_void,
                                             copy_size, nullptr, 0, nullptr));

  L0_SAFE_CALL(zeCommandListAppendBarrier(hCommandList, nullptr, 0, nullptr));
  L0_SAFE_CALL(zeKernelSetGroupSize(hKernel, 1, 1, 1));
  ze_group_count_t dispatchTraits = {1, 1, 1};

  L0_SAFE_CALL(zeCommandListAppendLaunchKernel(
      hCommandList, hKernel, &dispatchTraits, nullptr, 0, nullptr));

  L0_SAFE_CALL(zeCommandListAppendBarrier(hCommandList, nullptr, 0, nullptr));
  L0_SAFE_CALL(zeCommandListAppendMemoryCopy(hCommandList, b_void, dB,
                                             copy_size, nullptr, 0, nullptr));

  L0_SAFE_CALL(zeCommandListClose(hCommandList));
  L0_SAFE_CALL(zeCommandQueueExecuteCommandLists(hCommandQueue, 1,
                                                 &hCommandList, nullptr));
  L0_SAFE_CALL(zeCommandQueueSynchronize(hCommandQueue,
                                         std::numeric_limits<uint32_t>::max()));

  for (int i = 0; i < a_size; ++i)
  {
    std::cout << b.data[i] << " ";
  }
  std::cout << std::endl;

  L0_SAFE_CALL(zeMemFree(hContext, dA));
  L0_SAFE_CALL(zeMemFree(hContext, dB));

  return 0;
}
