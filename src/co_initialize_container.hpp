
#pragma once

#include <stdexcept>

#include <objbase.h>

#include "int_to_hex_string.hpp"

#pragma comment(lib, "ole32.lib")

class CoInitializeContainer
{
public:
    inline CoInitializeContainer();
    inline ~CoInitializeContainer();
};

CoInitializeContainer::CoInitializeContainer()
{
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        throw std::runtime_error("CoInitialize failed with " + iths(hr));
    }
}

CoInitializeContainer::~CoInitializeContainer()
{
    CoUninitialize();
}