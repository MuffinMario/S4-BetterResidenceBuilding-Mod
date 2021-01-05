#pragma once
#include "macros.h"
#include <cstdint>
struct S4Building {
    OFFSETDATA(1,0x8); // 0 -> 8h
    uint16_t buildingID; // 8h -> Ah
    OFFSETDATA(2, 10);// Ah -> 0x14
    uint32_t guiFlags; // 14h
    OFFSETDATA(3, 0x8); // 18h
    uint8_t playerFlags; // 20h
    OFFSETDATA(4, 0xF);
    uint32_t logicQueueNumber; // 30h -> 34h
    OFFSETDATA(5, 0x10);
    uint32_t buildingPosXY;
};