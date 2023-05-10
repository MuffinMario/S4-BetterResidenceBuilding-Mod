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
#include <S4ModApi.h>

struct S4Building
{
	// vtable included by offset
	OFFSETDATA(1, 0x8); // 0 -> 8h
	uint16_t buildingID; // 8h -> Ah
	// something about the rendering type?
	uint8_t unknownByte1; // Ah -> Bh
	uint8_t unknownByte2; // Bh -> Ch
	S4_BUILDING_ENUM buildingType; // Ch -> Eh
	OFFSETDATA(2, 6); // Eh -> 14h
	uint32_t guiFlags; // 14h
	OFFSETDATA(3, 0x8); // 18h
	uint8_t playerFlags; // 20h
	OFFSETDATA(4, 0xF);
	uint32_t logicQueueNumber; // 30h -> 34h
	OFFSETDATA(5, 0x10);
	uint32_t buildingPosXY;
};
