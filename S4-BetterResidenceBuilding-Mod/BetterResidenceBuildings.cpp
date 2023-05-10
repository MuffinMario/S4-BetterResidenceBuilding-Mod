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
#include "BetterResidenceBuildings.h"

#include <processthreadsapi.h>

// assignexternal
#include "memutil.h"
#include "S4Vars.h"
#include "S4Functions.h"
#include "S4MemoryOffsets.h"
// functions replacing the original function on vtable
#include "S4Detours.h"
S4API S4BetterResidenceBuildings::m_pS4API = nullptr;
Winutil::CHandle S4BetterResidenceBuildings::m_hS4Base = nullptr;
std::string S4BetterResidenceBuildings::m_lastErrorStr;
DWORD S4BetterResidenceBuildings::m_oldResidentialBuildingUpdateLogic = 0;


// DEBUG
S4API pS4Api;

void S4BetterResidenceBuildings::setupAdresses(HANDLE hS4)
{
	// function addresses we need
	assignExternal(RegisterForLogicUpdate, hS4, Offsets::Functions::REGISTERFORLOGICUPDATE);
	assignExternal(AddSettler, hS4, Offsets::Functions::ADDSETTLER);
	assignExternal(CanSpawnSettlerAtInSector, hS4, Offsets::Functions::CANSPAWNSETTLER);

	// all of this for sector id :bruh:
	assignExternal(pgMapsize, hS4, Offsets::Values::MAPSIZE);
	assignExternal(gaFieldIDType, hS4, Offsets::Values::AMAPFIELDIDTYPE_IGUESS);
	assignExternal(pgpaMapFieldTypes, hS4, Offsets::Values::PAFIELDIDTYPE);

	assignExternal(pgPartiesInMap, hS4, Offsets::Values::PARTIESINMAP);
	assignExternal(pgCurrentPartyID, hS4, Offsets::Values::CURRENTPARTYID);

	assignExternal(pgAIMain, hS4, Offsets::Values::PAIMAIN);

	assignExternal(gaPlayerRace, hS4, Offsets::Values::APLAYERRACETBL);

	assignExternal(gaInitialBeds, hS4, Offsets::Values::AINITIALBEDS);

	assignExternal(gaRaceInfoOffsetSmallResidenceSettlerCount, hS4,
	               Offsets::Values::ARACEINFOOFFSETSMALLRESIDENCESETTLERCOUNT);
	assignExternal(gaRaceInfoOffsetMediumResidenceSettlerCount, hS4,
	               Offsets::Values::ARACEINFOOFFSETMEDIUMRESIDENCESETTLERCOUNT);
	assignExternal(gaRaceInfoOffsetBigResidenceSettlerCount, hS4,
	               Offsets::Values::ARACEINFOOFFSETBIGRESIDENCESETTLERCOUNT);

	assignExternal(gaPartyBuildingStatsOffsetSmallResidenceCount, hS4,
	               Offsets::Values::APARTYBUILDINGSTATSOFFSETSMALLRESIDENCECOUNT);
	assignExternal(gaPartyBuildingStatsOffsetMediumResidenceCount, hS4,
	               Offsets::Values::APARTYBUILDINGSTATSOFFSETMEDIUMRESIDENCECOUNT);
	assignExternal(gaPartyBuildingStatsOffsetBigResidenceCount, hS4,
	               Offsets::Values::APARTYBUILDINGSTATSOFFSETBIGRESIDENCECOUNT);

	assignExternal(gaPlayerStats, hS4, Offsets::Values::APLAYERSTATS); // incorrect naming 
	assignExternal(gaPartyInfoOffsetFreeSettlerBuilderDigger, hS4, Offsets::Values::APARTYINFOOFFSETFREESETTLERS);
	assignExternal(gaPartyInfoOffsetSettlersAddedTotal, hS4, Offsets::Values::APARTYINFOOFFSETSETTLERSADDEDTOTAL);
}

BOOL S4BetterResidenceBuildings::overwriteResidentialBuildingLogicTickVTable(S4API pS4API, HANDLE hS4Base, DWORD to)
{
	auto residentialBuildingVTableUpdateLogic = (void*)((uint32_t)hS4Base +
		Offsets::Functions::CRESIDENTIALBUILDING_VTABLE_UPDATELOGIC);
#if ASKBEFOREOVERWRITE == 1
    {
        SSTREAM ss;
        ss << "overwriting vtable addr at " << residentialBuildingVTableUpdateLogic << " to " << std::hex << newFnc << "?";
        if (Winutil::messageBoxA(ss.str(), PROJECT_NAME, nullptr, Winutil::MessageBoxTypes::YESNO) == IDNO)
            return true;
    }
#endif
	m_oldResidentialBuildingUpdateLogic = READ_AT(residentialBuildingVTableUpdateLogic);
	BOOL ret;
	{
		// this is potentially unsafe...? no other thread should WRITE this addr, but maybe read, and jesus knows what happens then
		// TODO: assure exe rdata is already initalized? dlls, especially these, should load after the exe has been loaded into memory right?
		// make page writable
		DWORD oldProtection;
		auto startPage = (LPVOID)((unsigned int)hS4Base + 0x1000 + 0xb14000);
		size_t pageSize = 0x231000;
		VirtualProtect(startPage, pageSize, PAGE_READWRITE, &oldProtection);
		//write new vtable address
		ret = WRITE_AT(residentialBuildingVTableUpdateLogic, to);
		// make page unwritable again
		VirtualProtect(startPage, pageSize, oldProtection, nullptr);
	}
	return ret;
}


ATTACH_VALUE S4BetterResidenceBuildings::onAttach()
{
#ifdef _DEBUG
	//Winutil::messageBoxA("YOU ARE USING A DEBUG VERSION OF " PROJECT_NAME, PROJECT_NAME);
#endif
	if (!createAPI())
	{
		m_lastErrorStr = "Call to S4ApiCreate returned nullptr";
		return ATTACH_VALUE::FAILED_COULD_NOT_CREATE_API_PTR;
	}
	m_hS4Base = GetModuleHandle(nullptr);
	HANDLE hS4 = m_hS4Base;
	pS4Api = m_pS4API;

	setupAdresses(hS4);

	if (overwriteResidentialBuildingLogicTickVTable(m_pS4API, m_hS4Base, (DWORD)newCResidentialBuildingUpdateLogic) !=
		TRUE)
	{
		m_lastErrorStr = "Could not overwrite vtable values";
		return ATTACH_VALUE::FAILED_COULD_NOT_OVERRIDE_VTABLE;
	}

	return ATTACH_VALUE::SUCCESS;
}

DETACH_VALUE S4BetterResidenceBuildings::onDetach()
{
	// we don't need to warn again, it's just a detach... unless there will be dynamic detach on runtime
	releaseAPI();
	overwriteResidentialBuildingLogicTickVTable(m_pS4API, m_hS4Base, m_oldResidentialBuildingUpdateLogic);
	return DETACH_VALUE::SUCCESS;
}

bool S4BetterResidenceBuildings::createAPI()
{
	m_pS4API = S4ApiCreate();
	if (m_pS4API == nullptr)
	{
		return false;
	}
	return true;
}

bool S4BetterResidenceBuildings::releaseAPI()
{
	if (m_pS4API)
	{
		m_pS4API->Release();
		m_pS4API = nullptr;
		return true;
	}
	return false;
}
