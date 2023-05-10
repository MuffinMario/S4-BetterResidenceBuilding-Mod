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
#include "S4Detours.h"
#include "S4ResidenceBuilding.h"
#include "S4Building.h"
#include "S4UnknownInformationStructResidenceBuilding.h"
#include "S4Functions.h"
#include "S4Util.h"
#include "S4AIMain.h"
#include "macros.h"
// * modded
void __fastcall originalCResidentialBuildingUpdateLogic(S4ResidenceBuilding* pThis, void* PLACEHOLDER_IGNORE,
                                                        S4Building* pBuilding)
{
	// we will rebuild the original function 
	/* Need:
	    Building Interface class with
	        0x14 -> dword guiFlagsOrMore
	        0x20 -> byte  playerFlags (>>4 = party ID)
	        0x30 -> dword logicQueueNumber -- from registerforlogicupdate()
	        0x58 -> void updateGUI(CBuilding*,1)

	        // TODO check, its ECX and not EDI (pBuilding, ECX) or ESI (pThis, [EBP + 8])
	        0x1e0 -> byte runsTilReleaseSettler -- logic function calls until machine state = 3 (push settler out of house)

	    CResidenceBuilding:
	        0x4 -> byte buildingState
	        0x5 -> byte tickRunsSinceSettlerRelease



	    [Functions]
	        uint32 RegisterForLogicUpdate(CBuilding*,1)
	        bool IsSpaceForSettlers(x,y,type)
	        uint32 AddSettler(x,y,partyID,[[this->0x178] + 0x1de]
	        pThis = base ptr -> implementation instance offset 0x58 = UpdateGUI(CBuilding*,1)
	        gAIMain -> 0x2c CAIMain::PostAIEvent()

	    TODO: find out what
	        ESI (pThis) -> 0x178 & 0x17c is
	        */


	if (pBuilding->guiFlags & 0x200)
	{
		pThis->updateGUI(pBuilding, 1);
	}
	if (pThis->residenceBuildingState == 2)
	{
		/* residence building is not ready to spawn new settler, increase BUSY-logic-calls-in-a-row counter and register for logic update queue*/
		const auto ticks = pThis->tickCounterSinceLastRelease;
		if (const auto runsTillRelease = pThis->pUnknownStructResidenceBuilding->runsTillReleaseSettler; ticks <=
			runsTillRelease)
		{
			pThis->tickCounterSinceLastRelease++;
			const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0xe, pBuilding->buildingID);
			pBuilding->logicQueueNumber = queueLogicUpdateNumber;
			return;
		}

		if (pThis->releasedInhabitants < pThis->pUnknownStructResidenceBuilding->maxInhabitantsCounts)
		{
			// these two are one instruction in assembler
			pThis->tickCounterSinceLastRelease = 0;
			pThis->residenceBuildingState = 3;

			const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0xe, pBuilding->buildingID);
			pBuilding->logicQueueNumber = queueLogicUpdateNumber;
		}
	}
	else
	{
		if (pThis->residenceBuildingState == 3 &&
			pThis->releasedInhabitants < pThis->pUnknownStructResidenceBuilding->maxInhabitantsCounts)
		{
			/* Count total players */
			const uint32_t partyId = S4Util::getPartyID(pBuilding->playerFlags);
			const uint32_t totalSettlerCount = S4Util::countTotalSettlersParty(partyId);

			uint32_t maxSettlersPerParty = 0xFFFF;

			/* Determine max settlers per party (0xFFFF is a modded value to remove settler restriction, this was probably litze's stuff :) ) */
			if (*pgPartiesInMap < 9)
			{
				maxSettlersPerParty = 0xFFFF;
			}
			else
			{
				maxSettlersPerParty = 20000 / *pgPartiesInMap;
			}
			if (totalSettlerCount < maxSettlersPerParty)
			{
				/* Check if settler can be spawned, spawn if so*/
				DWORD x = S4Util::getX(pBuilding->buildingPosXY);
				DWORD y = S4Util::getY(pBuilding->buildingPosXY);
				//DBGPRINTLINE("adjust settler if...");
				// retreive sector id of building position
				if (const uint16_t sectorId = S4Util::getSectorID(x, y); CanSpawnSettlerAtInSector(&x, &y, sectorId))
				{
					// not dynamic, but its always 2?
					if (AddSettler(x, y, partyId, pThis->pUnknownStructResidenceBuilding->settlerTypeSpawn, 0) > 0)
					{
						/* Update player counts */
						const uint32_t index = partyId * (0x1128 / sizeof(int32_t));
						gaPartyInfoOffsetFreeSettlerBuilderDigger[index]++;
						gaPartyInfoOffsetSettlersAddedTotal[index]++;
						pThis->releasedInhabitants++;

						if (pThis->pUnknownStructResidenceBuilding->maxInhabitantsCounts <= pThis->releasedInhabitants)
						{
							/* signal AI of party house is done. This is the last part of code that gets called of this function per building originally */
							const auto buildingId = static_cast<int32_t>(pBuilding->buildingID);
							pgAIMain->PostAIEvent(8, partyId, buildingId, 0);

							/* MODDED: add another logic update request, this function will not take care of this house anymore */
							const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x1F, pBuilding->buildingID);
							pBuilding->logicQueueNumber = queueLogicUpdateNumber;
							pThis->tickCounterSinceLastRelease = 0;
							pThis->residenceBuildingState = 2;

							/* Update GUI */
							// function ends if user is not inside the house (UI wise of course)
							if ((pBuilding->guiFlags >> 9 & 1) == 0 || partyId != *pgCurrentPartyID)
								return;
							pThis->updateGUI(pBuilding, 1);

							return;
						}
					}
				}
			}
			if (pThis->pUnknownStructResidenceBuilding->runsTillReleaseSettler == 0)
			{
				const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x1F, pBuilding->buildingID);
				pBuilding->logicQueueNumber = queueLogicUpdateNumber;
				return;
			}
			const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0xE, pBuilding->buildingID);
			pBuilding->logicQueueNumber = queueLogicUpdateNumber;
			pThis->tickCounterSinceLastRelease = 0;
			pThis->residenceBuildingState = 2;
		}
	}
}
