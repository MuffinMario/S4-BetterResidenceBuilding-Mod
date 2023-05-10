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
// MOVE TO TYPES, shoulda done that earlier tbh
using partybuildingstats_residencecount_t = uint32_t;
using raceinfo_residenceinhabitantcount_t = uint32_t;
using race_t = uint32_t;
// (yet uncertain, fluctuates between uint8 & uint32) typedef uint32_t party_t;

namespace Offsets
{
	namespace
	{
		//ghidra
		constexpr uint32_t BASEOFFSET = 0x400000;
	}

	namespace Functions
	{
		constexpr uint32_t CRESIDENTIALBUILDING_VTABLE = 0x1059010 - BASEOFFSET;
		constexpr uint32_t CRESIDENTIALBUILDING_VTABLE_UPDATELOGIC = CRESIDENTIALBUILDING_VTABLE + 0xC;

		constexpr uint32_t REGISTERFORLOGICUPDATE = 0x56D5D0 - BASEOFFSET;
		constexpr uint32_t ADDSETTLER = 0x582600 - BASEOFFSET;
		constexpr uint32_t CANSPAWNSETTLER = 0x5857c0 - BASEOFFSET;

		constexpr uint32_t CRESIDENTIALBUILDING_UPDATELOGIC = 0x54DCA0 - BASEOFFSET;
	}

	namespace Sizes
	{
		constexpr uint32_t ARACEINFO_SIZE = 0x10744 / sizeof(raceinfo_residenceinhabitantcount_t);
		constexpr uint32_t APARTYBUILDINGSTATS_SIZE = 0x298 / sizeof(partybuildingstats_residencecount_t);
		constexpr uint32_t APLAYERRACETBL_SIZE = 0xF * 0x4 / sizeof(race_t);
	}

	namespace Values
	{
		constexpr uint32_t MAPSIZE = 0x1054138;
		constexpr uint32_t PARTIESINMAP = 0x1294828 - BASEOFFSET;
		constexpr uint32_t APLAYERSTATS = 0x1545660 - BASEOFFSET;
		constexpr uint32_t CURRENTPARTYID = 0x0129482c - BASEOFFSET;

		// pointers actually
		constexpr uint32_t PAIMAIN = 0x106B148; // ptr to struct S4AIMain
		// what exactly these two represent, i dont know. and I dont NEED to know. (i need to know)
		constexpr uint32_t PAFIELDIDTYPE = 0x11630C4; // ptr to array uint16_t*

		constexpr uint32_t APLAYERRACETBL = 0x109B5D4;

		constexpr uint32_t AINITIALBEDS = 0x107C8A8;

		constexpr uint32_t ARACEINFOOFFSETSMALLRESIDENCESETTLERCOUNT = 0x10A46D0;
		constexpr uint32_t ARACEINFOOFFSETMEDIUMRESIDENCESETTLERCOUNT = 0x10A49FC;
		constexpr uint32_t ARACEINFOOFFSETBIGRESIDENCESETTLERCOUNT = 0x10A4D28;

		constexpr uint32_t APARTYBUILDINGSTATSOFFSETSMALLRESIDENCECOUNT = 0x10EF764;
		constexpr uint32_t APARTYBUILDINGSTATSOFFSETMEDIUMRESIDENCECOUNT = 0x10EF76C;
		constexpr uint32_t APARTYBUILDINGSTATSOFFSETBIGRESIDENCECOUNT = 0x10EF774;

		constexpr uint32_t AMAPFIELDIDTYPE_IGUESS = 0xEF1A26; // sizeof per field = 0x14 = 20

		constexpr uint32_t APARTYINFOOFFSETFREESETTLERS = 0x11abf48 - BASEOFFSET;
		constexpr uint32_t APARTYINFOOFFSETSETTLERSADDEDTOTAL = 0x11ac050 - BASEOFFSET;
	}
}
