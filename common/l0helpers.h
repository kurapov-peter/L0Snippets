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
                   ze_context_handle_t &hContext,
                   std::string &spvFilename);
