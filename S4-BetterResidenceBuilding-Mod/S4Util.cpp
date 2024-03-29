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
#include "S4Util.h"


/* YYYY.YYYY YYYY.YYYY XXXX.XXXX XXXX.XXXX*/
uint32_t S4Util::getX(uint32_t pos)
{
	return pos & 0xFFFF;
}

/* YYYY.YYYY YYYY.YYYY XXXX.XXXX XXXX.XXXX*/
uint32_t S4Util::getY(uint32_t pos)
{
	return pos >> 0x10;
}

uint32_t S4Util::getSectorID(uint32_t x, uint32_t y)
{
	uint32_t mapsize = *pgMapsize;
	uint16_t* aFieldTypes = *pgpaMapFieldTypes;
	uint16_t fieldTypeAtPos = aFieldTypes[mapsize * y + x];
	return *(gaFieldIDType + pgaFieldIDTypeStructSize * fieldTypeAtPos);
}

void S4Util::showTextMessage(S4API pS4, STRING8 s)
{
	pS4->ShowTextMessage(s.c_str(), 9, 0);
}


/* PPPP.RRRR */
uint32_t S4Util::getPartyID(uint8_t playerFlags)
{
	return playerFlags >> 4;
}

race_t S4Util::getRace(uint32_t partyID)
{
	return *(gaPlayerRace + Offsets::Sizes::APLAYERRACETBL_SIZE * partyID);
}

uint32_t S4Util::getBuildingCount(uint32_t partyID, PARTY_BUILDING_TYPE buildingType)
{
	// enum class can only pass these three, no ambiguity?
	switch (buildingType)
	{
	case PARTY_BUILDING_TYPE::SMALL_RESIDENCE:
		return *(gaPartyBuildingStatsOffsetSmallResidenceCount + Offsets::Sizes::APARTYBUILDINGSTATS_SIZE * partyID);
	case PARTY_BUILDING_TYPE::MEDIUM_RESIDENCE:
		return *(gaPartyBuildingStatsOffsetMediumResidenceCount + Offsets::Sizes::APARTYBUILDINGSTATS_SIZE * partyID);
	case PARTY_BUILDING_TYPE::BIG_RESIDENCE:
		return *(gaPartyBuildingStatsOffsetBigResidenceCount + Offsets::Sizes::APARTYBUILDINGSTATS_SIZE * partyID);
	}
}

uint32_t S4Util::getRaceInfo(race_t race, RACE_INFO raceInfo)
{
	switch (raceInfo)
	{
	case RACE_INFO::SMALL_RESIDENCE_INHABITANTS:
		return *(gaRaceInfoOffsetSmallResidenceSettlerCount + Offsets::Sizes::ARACEINFO_SIZE * race);
	case RACE_INFO::MEDIUM_RESIDENCE_INHABITANTS:
		return *(gaRaceInfoOffsetMediumResidenceSettlerCount + Offsets::Sizes::ARACEINFO_SIZE * race);
	case RACE_INFO::BIG_RESIDENCE_INHABITANTS:
		return *(gaRaceInfoOffsetBigResidenceSettlerCount + Offsets::Sizes::ARACEINFO_SIZE * race);
	}
}

uint32_t S4Util::getInitialBedCount(uint32_t partyID)
{
	return gaInitialBeds[partyID]; // yes, 0 is empty
}

uint32_t S4Util::getResidenceBedCount(uint32_t partyID)
{
	uint32_t smallCount = 0, smallInh = 0;
	uint32_t mediumCount = 0, mediumInh = 0;
	uint32_t bigCount = 0, bigInh = 0;
	smallCount = getBuildingCount(partyID, PARTY_BUILDING_TYPE::SMALL_RESIDENCE);
	mediumCount = getBuildingCount(partyID, PARTY_BUILDING_TYPE::MEDIUM_RESIDENCE);
	bigCount = getBuildingCount(partyID, PARTY_BUILDING_TYPE::BIG_RESIDENCE);

	race_t race = getRace(partyID);
	smallInh = getRaceInfo(race, RACE_INFO::SMALL_RESIDENCE_INHABITANTS);
	mediumInh = getRaceInfo(race, RACE_INFO::MEDIUM_RESIDENCE_INHABITANTS);
	bigInh = getRaceInfo(race, RACE_INFO::BIG_RESIDENCE_INHABITANTS);

	return smallCount * smallInh + mediumCount * mediumInh + bigCount * bigInh;
}

uint32_t S4Util::getBedCount(uint32_t partyID)
{
	return getResidenceBedCount(partyID) + getInitialBedCount(partyID);
}

uint32_t S4Util::countTotalSettlersParty(uint32_t partyID)
{
	uint32_t settlerCountLeft = 0, settlerCountMiddle = 0, settlerCountRight = 0; // probably just compiler optimisation
	constexpr size_t sizeOfPlayerStatElement = 0x10c;
	uint32_t* paSettlerCountsPartyArray = gaPlayerStats + (partyID * sizeOfPlayerStatElement) / sizeof(uint32_t);
	// 0x1545660 table addr, 0x10c sizeof
	for (int i = 16; i > 0; i--)
	{
		settlerCountLeft += paSettlerCountsPartyArray[-1];
		settlerCountMiddle += paSettlerCountsPartyArray[0];
		settlerCountRight += paSettlerCountsPartyArray[1];
		paSettlerCountsPartyArray = paSettlerCountsPartyArray + 3; // should be + 0xC
	}
	return settlerCountLeft + settlerCountMiddle + settlerCountRight;
}
