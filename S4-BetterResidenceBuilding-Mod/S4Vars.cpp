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
