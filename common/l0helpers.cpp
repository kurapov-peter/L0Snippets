#include "l0helpers.h"

#include <level_zero/ze_api.h>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <string>
#include <fstream>

void L0InitContext(ze_driver_handle_t &hDriver,
                   ze_device_handle_t &hDevice,
                   ze_module_handle_t &hModule,
                   ze_command_queue_handle_t &hCommandQueue,
                   ze_context_handle_t &hContext)
{
    ze_init_flag_t init_flag;
    L0_SAFE_CALL(zeInit(0));

    // Discover all the driver instances
    uint32_t driverCount = 0;
    L0_SAFE_CALL(zeDriverGet(&driverCount, nullptr));

    ze_driver_handle_t *allDrivers =
        (ze_driver_handle_t *)malloc(driverCount * sizeof(ze_driver_handle_t));
    L0_SAFE_CALL(zeDriverGet(&driverCount, allDrivers));

    // Find a driver instance with a GPU device
    for (uint32_t i = 0; i < driverCount; ++i)
    {
        uint32_t deviceCount = 0;
        hDriver = allDrivers[i];
        L0_SAFE_CALL(zeDeviceGet(hDriver, &deviceCount, nullptr));
        ze_device_handle_t *allDevices =
            (ze_device_handle_t *)malloc(deviceCount * sizeof(ze_device_handle_t));
        L0_SAFE_CALL(zeDeviceGet(hDriver, &deviceCount, allDevices));
        for (uint32_t d = 0; d < deviceCount; ++d)
        {
            ze_device_properties_t device_properties;
            L0_SAFE_CALL(zeDeviceGetProperties(allDevices[d], &device_properties));
            if (ZE_DEVICE_TYPE_GPU == device_properties.type)
            {
                hDevice = allDevices[d];
                break;
            }
        }
        free(allDevices);
        if (nullptr != hDevice)
        {
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
}

void L0BuildModule(ze_device_handle_t &hDevice,
                   ze_context_handle_t &hContext,
                   ze_module_handle_t &hModule,
                   const std::string &spvFilename,
                   const std::string &buildParams)
{
    std::ifstream is;
    std::string fn = spvFilename;

    is.open(fn, std::ios::binary);
    if (!is.good())
    {
        fprintf(stderr, "Open %s failed\n", fn.c_str());
        exit(1);
    }

    is.seekg(0, std::ios::end);
    size_t codeSize = is.tellg();
    is.seekg(0, std::ios::beg);

    if (codeSize == 0)
    {
        return;
    }
    unsigned char *codeBin = new unsigned char[codeSize];
    if (!codeBin)
    {
        return;
    }

    is.read((char *)codeBin, codeSize);
    is.close();

    ze_module_desc_t moduleDesc;
    moduleDesc.stype = ZE_STRUCTURE_TYPE_MODULE_DESC;
    moduleDesc.pNext = nullptr;
    moduleDesc.pBuildFlags = buildParams.c_str();
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

void L0CreateCommandList(ze_command_list_handle_t &hCommandList,
                         ze_context_handle_t &hContext,
                         ze_device_handle_t &hDevice,
                         ze_command_queue_handle_t &hCommandQueue)
{
    ze_command_list_desc_t commandListDesc;
    commandListDesc.stype = ZE_STRUCTURE_TYPE_COMMAND_LIST_DESC;
    commandListDesc.pNext = nullptr;
    commandListDesc.flags = 0;
    commandListDesc.commandQueueGroupOrdinal = 0;

    L0_SAFE_CALL(zeCommandListCreate(hContext, hDevice, &commandListDesc, &hCommandList));
}

void L0CreateKernel(ze_kernel_handle_t &hKernel,
                    ze_module_handle_t &hModule,
                    const std::string &kernelName)
{
    ze_kernel_desc_t kernelDesc;
    kernelDesc.stype = ZE_STRUCTURE_TYPE_KERNEL_DESC;
    kernelDesc.pNext = nullptr;
    kernelDesc.flags = 0;
    kernelDesc.pKernelName = kernelName.c_str();

    L0_SAFE_CALL(zeKernelCreate(hModule, &kernelDesc, &hKernel));
}

std::string L0ErrorToString(ze_result_t status)
{
    switch (status)
    {
    case ZE_RESULT_NOT_READY:
        return "L0 error: synchronization primitive not signaled";
    case ZE_RESULT_ERROR_DEVICE_LOST:
        return "L0 error: device hung, reset, was removed, or driver update occurred";
    case ZE_RESULT_ERROR_OUT_OF_HOST_MEMORY:
        return "L0 error: insufficient host memory to satisfy call";
    case ZE_RESULT_ERROR_OUT_OF_DEVICE_MEMORY:
        return "L0 error: insufficient device memory to satisfy call";
    case ZE_RESULT_ERROR_MODULE_BUILD_FAILURE:
        return "L0 error: error occurred when building module, see build log for details";
    case ZE_RESULT_ERROR_MODULE_LINK_FAILURE:
        return "L0 error: error occurred when linking modules, see build log for details";
    case ZE_RESULT_ERROR_INSUFFICIENT_PERMISSIONS:
        return "L0 error: access denied due to permission level";
    case ZE_RESULT_ERROR_NOT_AVAILABLE:
        return "L0 error: resource already in use and simultaneous access not allowed or "
               "resource was removed";
    case ZE_RESULT_ERROR_DEPENDENCY_UNAVAILABLE:
        return "L0 error: external required dependency is unavailable or missing";
    case ZE_RESULT_ERROR_UNINITIALIZED:
        return "L0 error: driver is not initialized";
    case ZE_RESULT_ERROR_UNSUPPORTED_VERSION:
        return "L0 error: generic error code for unsupported versions";
    case ZE_RESULT_ERROR_UNSUPPORTED_FEATURE:
        return "L0 error: generic error code for unsupported features";
    case ZE_RESULT_ERROR_INVALID_ARGUMENT:
        return "L0 error: generic error code for invalid arguments";
    case ZE_RESULT_ERROR_INVALID_NULL_HANDLE:
        return "L0 error: handle argument is not valid";
    case ZE_RESULT_ERROR_HANDLE_OBJECT_IN_USE:
        return "L0 error: object pointed to by handle still in-use by device";
    case ZE_RESULT_ERROR_INVALID_NULL_POINTER:
        return "L0 error: pointer argument may not be nullptr";
    case ZE_RESULT_ERROR_INVALID_SIZE:
        return "L0 error: size argument is invalid (e.g., must not be zero)";
    case ZE_RESULT_ERROR_UNSUPPORTED_SIZE:
        return "L0 error: size argument is not supported by the device (e.g., too large)";
    case ZE_RESULT_ERROR_UNSUPPORTED_ALIGNMENT:
        return "L0 error: alignment argument is not supported by the device";
    case ZE_RESULT_ERROR_INVALID_SYNCHRONIZATION_OBJECT:
        return "L0 error: synchronization object in invalid state";
    case ZE_RESULT_ERROR_INVALID_ENUMERATION:
        return "L0 error: enumerator argument is not valid";
    case ZE_RESULT_ERROR_UNSUPPORTED_ENUMERATION:
        return "L0 error: enumerator argument is not supported by the device";
    case ZE_RESULT_ERROR_UNSUPPORTED_IMAGE_FORMAT:
        return "L0 error: image format is not supported by the device";
    case ZE_RESULT_ERROR_INVALID_NATIVE_BINARY:
        return "L0 error: native binary is not supported by the device";
    case ZE_RESULT_ERROR_INVALID_GLOBAL_NAME:
        return "L0 error: global variable is not found in the module";
    case ZE_RESULT_ERROR_INVALID_KERNEL_NAME:
        return "L0 error: kernel name is not found in the module";
    case ZE_RESULT_ERROR_INVALID_FUNCTION_NAME:
        return "L0 error: function name is not found in the module";
    case ZE_RESULT_ERROR_INVALID_GROUP_SIZE_DIMENSION:
        return "L0 error: group size dimension is not valid for the kernel or device";
    case ZE_RESULT_ERROR_INVALID_GLOBAL_WIDTH_DIMENSION:
        return "L0 error: global width dimension is not valid for the kernel or device";
    case ZE_RESULT_ERROR_INVALID_KERNEL_ARGUMENT_INDEX:
        return "L0 error: kernel argument index is not valid for kernel";
    case ZE_RESULT_ERROR_INVALID_KERNEL_ARGUMENT_SIZE:
        return "L0 error: kernel argument size does not match kernel";
    case ZE_RESULT_ERROR_INVALID_KERNEL_ATTRIBUTE_VALUE:
        return "L0 error: value of kernel attribute is not valid for the kernel or device";
    case ZE_RESULT_ERROR_INVALID_MODULE_UNLINKED:
        return "L0 error: module with imports needs to be linked before kernels can be "
               "created from it";
    case ZE_RESULT_ERROR_INVALID_COMMAND_LIST_TYPE:
        return "L0 error: command list type does not match command queue type";
    case ZE_RESULT_ERROR_OVERLAPPING_REGIONS:
        return "L0 error: copy operations do not support overlapping regions of memory";
    case ZE_RESULT_ERROR_UNKNOWN:
        return "L0 error: unknown or internal error";
    default:
        return "L0 unexpected error code";
    }
}
