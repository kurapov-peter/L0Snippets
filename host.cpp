#include <level_zero/ze_api.h>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <cassert>
#include <fstream>
#include <string>

#define L0_SAFE_CALL(call)                                                     \
  {                                                                            \
    auto status = (call);                                                      \
    if (status) {                                                              \
      std::cerr << "L0 error: " << std::hex << (int)status << " ";             \
      std::cerr << std::dec << __FILE__ << ":" << __LINE__ << std::endl;       \
      exit(status);                                                            \
    }                                                                          \
  }

template<typename T, size_t N>
struct alignas(4096) AlignedArray {
    T data[N];
};

static void L0InitContext(ze_driver_handle_t &hDriver,
                          ze_device_handle_t &hDevice,
                          ze_module_handle_t &hModule,
                          ze_command_queue_handle_t &hCommandQueue,
                          ze_context_handle_t &hContext,
                          std::string &spvFilename) {
  ze_init_flag_t init_flag;
  L0_SAFE_CALL(zeInit(0));

  // Discover all the driver instances
  uint32_t driverCount = 0;
  L0_SAFE_CALL(zeDriverGet(&driverCount, nullptr));

  ze_driver_handle_t *allDrivers =
      (ze_driver_handle_t *)malloc(driverCount * sizeof(ze_driver_handle_t));
  L0_SAFE_CALL(zeDriverGet(&driverCount, allDrivers));

  // Find a driver instance with a GPU device
  for (uint32_t i = 0; i < driverCount; ++i) {
    uint32_t deviceCount = 0;
    hDriver = allDrivers[i];
    L0_SAFE_CALL(zeDeviceGet(hDriver, &deviceCount, nullptr));
    ze_device_handle_t *allDevices =
        (ze_device_handle_t *)malloc(deviceCount * sizeof(ze_device_handle_t));
    L0_SAFE_CALL(zeDeviceGet(hDriver, &deviceCount, allDevices));
    for (uint32_t d = 0; d < deviceCount; ++d) {
      ze_device_properties_t device_properties;
      L0_SAFE_CALL(zeDeviceGetProperties(allDevices[d], &device_properties));
      if (ZE_DEVICE_TYPE_GPU == device_properties.type) {
        hDevice = allDevices[d];
        break;
      }
    }
    free(allDevices);
    if (nullptr != hDevice) {
      break;
    }
  }
  free(allDrivers);
  assert(hDriver);
  assert(hDevice);

  // Create context
  ze_context_desc_t ctxtDesc = {ZE_STRUCTURE_TYPE_CONTEXT_DESC, nullptr, 0};
  zeContextCreate(hDriver, &ctxtDesc, &hContext);
  // Create a command queue
  ze_command_queue_desc_t commandQueueDesc = {
      ZE_STRUCTURE_TYPE_COMMAND_QUEUE_DESC,
      nullptr,
      0, // computeQueueGroupOrdinal
      0, // index
      0, // flags
      ZE_COMMAND_QUEUE_MODE_DEFAULT,
      ZE_COMMAND_QUEUE_PRIORITY_NORMAL};
  L0_SAFE_CALL(zeCommandQueueCreate(hContext, hDevice, &commandQueueDesc,
                                    &hCommandQueue));

  std::ifstream is;
  std::string fn = spvFilename;

  is.open(fn, std::ios::binary);
  if (!is.good()) {
    fprintf(stderr, "Open %s failed\n", fn.c_str());
    exit(1);
  }

  is.seekg(0, std::ios::end);
  size_t codeSize = is.tellg();
  is.seekg(0, std::ios::beg);

  if (codeSize == 0) {
    return;
  }
  unsigned char *codeBin = new unsigned char[codeSize];
  if (!codeBin) {
    return;
  }

  is.read((char *)codeBin, codeSize);
  is.close();

  ze_module_desc_t moduleDesc;
  moduleDesc.stype = ZE_STRUCTURE_TYPE_MODULE_DESC;
  moduleDesc.pNext = nullptr;
  moduleDesc.pBuildFlags = "";
  moduleDesc.format = ZE_MODULE_FORMAT_IL_SPIRV;
  moduleDesc.inputSize = codeSize;
  moduleDesc.pConstants = nullptr;
  moduleDesc.pInputModule = codeBin;

  ze_module_build_log_handle_t buildlog = nullptr;
  L0_SAFE_CALL(
      zeModuleCreate(hContext, hDevice, &moduleDesc, &hModule, &buildlog));
  size_t szLog = 0;
  L0_SAFE_CALL(zeModuleBuildLogGetString(buildlog, &szLog, nullptr));
  std::cout << "Got build log size " << szLog << std::endl;
  char* strLog = (char*)malloc(szLog);
  L0_SAFE_CALL(zeModuleBuildLogGetString(buildlog, &szLog, strLog));
  std::fstream log;
  log.open("log.txt", std::ios::app);
  if (!log.good()) {
    std::cerr << "Unable to open log file" << std::endl;
    exit(1);
  }
  log << strLog;
  log.close();
}

int main(int argc, char* argv[]) {
    std::string spvFilename = "test.spv";
    if (argc > 1) {
        spvFilename = std::string(argv[1]);
    }

    ze_device_handle_t hDevice = nullptr;
    ze_module_handle_t hModule = nullptr;
    ze_driver_handle_t hDriver = nullptr;
    ze_command_queue_handle_t hCommandQueue = nullptr;
    ze_context_handle_t hContext = nullptr;
    L0InitContext(hDriver, hDevice, hModule, hCommandQueue, hContext, spvFilename);

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
    for (auto i = 0; i < a_size; ++i) {
        a.data[i] = a_size - i;
        b.data[i] = i;
    }
    
    ze_device_mem_alloc_desc_t alloc_desc;
    alloc_desc.stype = ZE_STRUCTURE_TYPE_DEVICE_MEM_ALLOC_DESC;
    alloc_desc.pNext = nullptr;
    alloc_desc.flags = 0;
    alloc_desc.ordinal = 0;

    const float copy_size = a_size*sizeof(float);
    void *dA = nullptr;
    L0_SAFE_CALL(zeMemAllocDevice(hContext, &alloc_desc, copy_size, 0/*align*/, hDevice, &dA));
    void *dB = nullptr;
    L0_SAFE_CALL(zeMemAllocDevice(hContext, &alloc_desc, copy_size, 0/*align*/, hDevice, &dB));


    int sz = 32;
    L0_SAFE_CALL(zeKernelSetArgumentValue(hKernel, 0, sizeof(void*), &dA));
    L0_SAFE_CALL(zeKernelSetArgumentValue(hKernel, 1, sizeof(void*), &dB));

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

    for (int i = 0; i < a_size; ++i) {
        std::cout << b.data[i] << " ";
    }
    std::cout << std::endl;

    L0_SAFE_CALL(zeMemFree(hContext, dA));
    L0_SAFE_CALL(zeMemFree(hContext, dB));
}

