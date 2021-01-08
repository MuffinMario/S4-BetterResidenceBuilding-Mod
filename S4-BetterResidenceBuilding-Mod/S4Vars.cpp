#include "pch.h"
#include <cstdint>
#include "S4DataTypes.h"
uint16_t* pgMapsize;
uint32_t* pgPartiesInMap;
uint32_t* pgCurrentPartyID;
S4AIMain* pgAIMain = nullptr;
uint16_t** pgpaMapFieldTypes;

uint32_t* gaPlayerStats;

uint16_t* gaFieldIDType;
size_t pgaFieldIDTypeStructSize = 0x14 / sizeof(uint16_t);

int32_t* gaPartyInfoOffsetFreeSettlerBuilderDigger;
int32_t* gaPartyInfoOffsetSettlersAddedTotal;

uint32_t* gaPlayerRace;

uint32_t* gaInitialBeds;

uint32_t* gaRaceInfoOffsetSmallResidenceSettlerCount;
uint32_t* gaRaceInfoOffsetMediumResidenceSettlerCount;
uint32_t* gaRaceInfoOffsetBigResidenceSettlerCount;

uint32_t* gaPartyBuildingStatsOffsetSmallResidenceCount;
uint32_t* gaPartyBuildingStatsOffsetMediumResidenceCount;
uint32_t* gaPartyBuildingStatsOffsetBigResidenceCount;