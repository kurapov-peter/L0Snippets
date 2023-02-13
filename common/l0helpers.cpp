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
                   ze_context_handle_t &hContext,
                   std::string &spvFilename)
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

    // todo: separate out the module creation
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