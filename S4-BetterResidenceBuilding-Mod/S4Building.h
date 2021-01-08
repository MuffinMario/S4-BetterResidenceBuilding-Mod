#pragma once
#include "macros.h"
#include <cstdint>
struct S4Building {
    // vtable included by offset
    OFFSETDATA(1,0x8); // 0 -> 8h
    uint16_t buildingID; // 8h -> Ah
    // something about the rendering type?
    uint8_t unknownByte1; // Ah -> Bh
    uint8_t unknownByte2; // Bh -> Ch
    uint16_t buildingType; // Ch -> Eh
    OFFSETDATA(2, 6);// Eh -> 14h
    uint32_t guiFlags; // 14h
    OFFSETDATA(3, 0x8); // 18h
    uint8_t playerFlags; // 20h
    OFFSETDATA(4, 0xF);
    uint32_t logicQueueNumber; // 30h -> 34h
    OFFSETDATA(5, 0x10);
    uint32_t buildingPosXY;
};