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
#include <cstdint>
#include <S4ModApi.h>
#include "S4MemoryOffsets.h"
#include "winutil.h"
#include "S4Vars.h"

/* Utility functions for S4 to save myself time and space */
namespace S4Util
{
	/* YYYY.YYYY YYYY.YYYY XXXX.XXXX XXXX.XXXX*/
	uint32_t getX(uint32_t pos);
	/* YYYY.YYYY YYYY.YYYY XXXX.XXXX XXXX.XXXX*/
	uint32_t getY(uint32_t pos);

	uint32_t getSectorID(uint32_t x, uint32_t y);

	void showTextMessage(S4API pS4, STRING8 s);

	/* PPPP.???? */
	uint32_t getPartyID(uint8_t playerFlags);

	// todo maybe use original s4 function for this instead?
	race_t getRace(uint32_t partyID);

	enum class PARTY_BUILDING_TYPE
	{
		SMALL_RESIDENCE,
		MEDIUM_RESIDENCE,
		BIG_RESIDENCE
	};

	// todo get entire array instead of all single, maybe use original s4 function for this instead?
	uint32_t getBuildingCount(uint32_t partyID, PARTY_BUILDING_TYPE buildingType);

	enum class RACE_INFO
	{
		SMALL_RESIDENCE_INHABITANTS,
		MEDIUM_RESIDENCE_INHABITANTS,
		BIG_RESIDENCE_INHABITANTS
	};

	uint32_t getRaceInfo(race_t race, RACE_INFO raceInfo);

	uint32_t getInitialBedCount(uint32_t partyID);

	uint32_t getResidenceBedCount(uint32_t partyID);
	uint32_t getBedCount(uint32_t partyID);

	uint32_t countTotalSettlersParty(uint32_t partyID);
}
