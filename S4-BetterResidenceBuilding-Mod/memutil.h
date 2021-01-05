#pragma once
#include <cstdint>
#include <Windows.h>

template<typename T>
void assignExternal(T& var, HANDLE baseHandle, uint32_t offset) {
    // TODO?: s4 api memget_s instead?
    var = reinterpret_cast<T>((uint32_t)baseHandle + offset);
}