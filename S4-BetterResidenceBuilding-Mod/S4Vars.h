#include "pch.h"
#include <cstdint>
#include "S4DataTypes.h"
// Global vars in .EXE
// convention as defined with regex (pointer)?(global)?$(vartype)$(Name)
// e.g. pgMapsize := pointer to global value Mapsize
//      pgpaMapFieldTypes := pointer to a global pointer that points to an array MapFieldTypes
// global in this context means static adress, does not really have to be a "global" variable, u cant find that out by assembler code
//ptr to vars
extern uint16_t* pgMapsize;
extern uint32_t* pgPartiesInMap;
extern uint32_t* pgCurrentPartyID;
extern S4AIMain* pgAIMain;
extern uint16_t** pgpaMapFieldTypes;

//ptrs (/arrays)
extern uint32_t* gaPlayerStats;

extern uint16_t* gaFieldIDType;
extern size_t pgaFieldIDTypeStructSize;

extern int32_t* gaPartyInfoOffsetFreeSettlerBuilderDigger;
extern int32_t* gaPartyInfoOffsetSettlersAddedTotal;

extern uint32_t* gaPlayerRace;

extern uint32_t* gaInitialBeds;

extern uint32_t* gaRaceInfoOffsetSmallResidenceSettlerCount;
extern uint32_t* gaRaceInfoOffsetMediumResidenceSettlerCount;
extern uint32_t* gaRaceInfoOffsetBigResidenceSettlerCount;

extern uint32_t* gaPartyBuildingStatsOffsetSmallResidenceCount;
extern uint32_t* gaPartyBuildingStatsOffsetMediumResidenceCount;
extern uint32_t* gaPartyBuildingStatsOffsetBigResidenceCount;