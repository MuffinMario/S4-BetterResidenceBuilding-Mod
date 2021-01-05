#pragma once
#include <cstdint>
#include <sstream>
#include <iostream>

// theres probably a prettier way to do this and a more compiler friendly way
#define OFFSETDATA(t,bytes) uint8_t unknown##t[bytes]

#define DBGPRINTLINE(var) { SSTREAM ss; ss << #var << " - " << std::hex <<  (uint32_t)var; S4Util::showTextMessage(pS4Api, ss.str()); ss.str("");ss.clear(); } //clear buf + error flags

// messagebox before overwriting vtable override (YES/NO window)
#define ASKBEFOREOVERWRITE 0