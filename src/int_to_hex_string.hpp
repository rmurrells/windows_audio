#pragma once
#include <string>
#include <sstream>

template <class Int>
std::string iths(Int i)
{
    std::stringstream sstr;
    sstr << std::hex << i;
    return "0x" + sstr.str();
}