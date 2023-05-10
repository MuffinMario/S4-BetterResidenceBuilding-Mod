// /*
// * S4-BetterResidenceBuilding-Mod
// Version 0.2, May 2023
// Copyright (C) 2023 MuffinMario https://github.com/MuffinMario https://muffinmar.io/
// This software and associated documentation files (the "Software") are licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License ("CC BY-NC-SA License").
// You are free to:
// - Share: Copy and redistribute the Software in any medium or format.
// - Adapt: Remix, transform, and build upon the Software.
//  
// Under the following terms:
// 1. Attribution: You must give appropriate credit, provide a link to the license, and indicate if changes were made. 
// You must give credit to the original authors of the Software and all contributors. 
// You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
// 2. NonCommercial: You may not use the Software for commercial purposes without obtaining explicit permission from the licensor.
// 3. ShareAlike: If you remix, transform, or build upon the Software, you must distribute your contributions under the same license as the original.
// The above licensing terms apply to all parts of the Software and any modifications made to it.
// This license does not grant you any patent rights, trademark rights, or any other intellectual property rights. The Software is provided "as is," without any warranty or guarantee, expressed or implied.
// For more details about the CC BY-NC-SA License, please visit: https://creativecommons.org/licenses/by-nc-sa/4.0/
// */

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
