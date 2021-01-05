#pragma once
static_assert(sizeof(uint8_t) == 1, "uint8_t is not 1 byte size");
static_assert(sizeof(uint8_t*) == 4, "pointer size does not equal 4 bytes, build this project with x86 settings");
