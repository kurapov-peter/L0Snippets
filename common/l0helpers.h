#pragma once

#include <level_zero/ze_api.h>
#include <iostream>
#include <cstdlib>
#include <string>

#define L0_SAFE_CALL(call)                                                     \
    {                                                                          \
        auto status = (call);                                                  \
        if (status)                                                            \
        {                                                                      \
            std::cerr << "L0 error: " << std::hex << (int)status << " ";       \
            std::cerr << L0ErrorToString(status) << " ";                       \
            std::cerr << std::dec << __FILE__ << ":" << __LINE__ << std::endl; \
            exit(status);                                                      \
        }                                                                      \
    }

template <typename T, size_t N>
struct alignas(4096) AlignedArray
{
    T data[N];
};

void L0InitContext(ze_driver_handle_t &hDriver,
                   ze_device_handle_t &hDevice,
                   ze_module_handle_t &hModule,
                   ze_command_queue_handle_t &hCommandQueue,
                   ze_context_handle_t &hContext);

void L0BuildModule(ze_device_handle_t &hDevice,
                   ze_context_handle_t &hContext,
                   ze_module_handle_t &hModule,
                   const std::string &spvFilename,
                   const std::string &buildParams);

void L0CreateCommandList(ze_command_list_handle_t &hCommandList,
                         ze_context_handle_t &hContext,
                         ze_device_handle_t &hDevice,
                         ze_command_queue_handle_t &hCommandQueue);

void L0CreateKernel(ze_kernel_handle_t &hKernel,
                    ze_module_handle_t &hModule,
                    const std::string &kernelName);

std::string L0ErrorToString(ze_result_t status);
