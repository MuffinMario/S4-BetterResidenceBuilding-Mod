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

#pragma once
#include "macros.h"
#include <cstdint>

// this is probably a generalized struct with information about every building (?) hence the 1 billion 0x0 fields and the sometimes important data
struct S4UnknownInformationStructResidenceBuilding
{
	OFFSETDATA(1, 0x1de);
	/* Settler Type that spawns from building */
	uint8_t settlerTypeSpawn; // 1de
	OFFSETDATA(2, 0x1);
	uint8_t runsTillReleaseSettler; // 1e0
	OFFSETDATA(3, 0xF);
	uint32_t maxInhabitantsCounts; // 1f0
};
