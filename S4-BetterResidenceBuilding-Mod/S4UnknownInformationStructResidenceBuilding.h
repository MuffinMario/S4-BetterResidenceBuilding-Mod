#pragma once
#include "macros.h"
#include <cstdint>
// this is probably a generalized struct with information about every building (?) hence the 1 billion 0x0 fields and the sometimes important data
struct S4UnknownInformationStructResidenceBuilding {
    OFFSETDATA(1, 0x1de);
    /* Settler Type that spawns from building */
    uint8_t settlerTypeSpawn; // 1de
    OFFSETDATA(2, 0x1);
    uint8_t runsTillReleaseSettler; // 1e0
    OFFSETDATA(3, 0xF);
    uint32_t maxInhabitantsCounts; // 1f0
};