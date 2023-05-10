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

#include <array>

#include "S4Detours.h"
#include "S4ResidenceBuilding.h"
#include "S4Building.h"
#include "S4UnknownInformationStructResidenceBuilding.h"
#include "S4Functions.h"
#include "S4Vars.h"
#include "S4Util.h"

/* config */
const std::map<S4_BUILDING_ENUM, uint8_t> g_residenceBuildingWaitingTime{
	{
		{S4_BUILDING_RESIDENCEBIG, 6}, // 3 min 
		{S4_BUILDING_RESIDENCEMEDIUM, 12}, // 6 min
		{S4_BUILDING_RESIDENCESMALL, 24}, // 12 min
	}
};


extern S4API pS4Api;
// ecx                          edx                         first stack param
void __fastcall newCResidentialBuildingUpdateLogic(S4ResidenceBuilding* pThis, void* PLACEHOLDER_IGNORE,
                                                   S4Building* pBuilding)
{
	if (pThis->releasedInhabitants < pThis->pUnknownStructResidenceBuilding->maxInhabitantsCounts)
	{
		//DBGPRINTLINE("original function");
		originalCResidentialBuildingUpdateLogic(pThis, PLACEHOLDER_IGNORE, pBuilding);
	}
	else
	{
		/*
		 *	Regular spawning of residents depends on building type
		 *	residents will spawn in N x 30s cycles with N being predefined and depending on building type
		 *	logic follows:
		 *	WAIT 30S BEGIN (2) -> (WAIT 30S LOOP (N+2) -> WAIT 30S (N+1) -> ... -> WAIT 30S (4) ) -> WAIT END / SPAWN (3) -> WAIT 30S BEGIN (2) repeat
		 *
		 *
		 *	we choose intial state as ID 2 becuase the original residence building logic follows the same principle, so we can latch on easily.
		 **/
		// 30s counter state WAIT BEGIN
		if (pThis->residenceBuildingState == 2)
		{
			pThis->tickCounterSinceLastRelease++;
			uint32_t ticksRequired = -1;
			// roughly the amount of seconds
			if (pThis->tickCounterSinceLastRelease >= 30)
			{
				const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x5, pBuilding->buildingID);
				pBuilding->logicQueueNumber = queueLogicUpdateNumber;

				pThis->tickCounterSinceLastRelease = 0;

				const auto waitingTime = g_residenceBuildingWaitingTime.at(pBuilding->buildingType);
				pThis->residenceBuildingState = waitingTime + 2;
				pThis->residenceBuildingState = max(3U, pThis->residenceBuildingState);
			}
			else
			{
				const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0xE, pBuilding->buildingID);
				pBuilding->logicQueueNumber = queueLogicUpdateNumber;
			}
		}
		else if (pThis->residenceBuildingState == 3) // WAIT END
		{
			const uint32_t partyId = pBuilding->playerFlags >> 4;
			DWORD x = pBuilding->buildingPosXY & 0xFFFF;
			DWORD y = pBuilding->buildingPosXY >> 0x10;

			const uint32_t sectorId = S4Util::getSectorID(x, y);
			/* This WILL create collisions with new residences. THIS HAS TO BE REVAMPED if interest spikes idk */
			const uint32_t index = partyId * (0x1128 / sizeof(uint32_t));
			// frick finding this myself, unlike building count, this has settlers have no state difference
			const uint32_t freeSettlers = pS4Api->SettlersAmount(S4_SETTLER_CARRIER, partyId);
			//gaPartyInfoOffsetFreeSettlerBuilderDigger[index];
			const uint32_t beds = S4Util::getBedCount(partyId);

			//delimit max settlers spawned this way to beds - 10

			if (constexpr uint32_t settlerBufferLimit = 10; beds > freeSettlers + settlerBufferLimit)
			{
				if (CanSpawnSettlerAtInSector(&x, &y, sectorId))
				{
					if (AddSettler(x, y, partyId, pThis->pUnknownStructResidenceBuilding->settlerTypeSpawn, 0) > 0)
					{
						/* Update player counts */
						gaPartyInfoOffsetFreeSettlerBuilderDigger[index]++;
						gaPartyInfoOffsetSettlersAddedTotal[index]++;
						//pThis->releasedInhabitants++;

						/* Queue agane */
						const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x1F, pBuilding->buildingID);
						pBuilding->logicQueueNumber = queueLogicUpdateNumber;
						pThis->tickCounterSinceLastRelease = 0;
						pThis->residenceBuildingState = 2;

						/* Update GUI */
						// function ends if user is not inside the house (UI wise of course)
						if ((pBuilding->guiFlags >> 9 & 1) == 0 || partyId != *pgCurrentPartyID)
							return;
						pThis->updateGUI(pBuilding, 1);
					}
				}
			}
			else
			{
				// reset minute timer if bed limit has reached. prevent a huge flood of waiting settlers (rethink?)
				const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x1F, pBuilding->buildingID);
				pBuilding->logicQueueNumber = queueLogicUpdateNumber;
				pThis->tickCounterSinceLastRelease = 0;
				pThis->residenceBuildingState = 2; // revert back to loop start
			}
		}
		else if (pThis->residenceBuildingState > 3) // 30 second state WAIT LOOP
		{
			pThis->tickCounterSinceLastRelease++;
			uint32_t ticksRequired = -1;
			// roughly the amount of seconds
			if (pThis->tickCounterSinceLastRelease >= 30)
			{
				const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x5, pBuilding->buildingID);
				pBuilding->logicQueueNumber = queueLogicUpdateNumber;

				pThis->tickCounterSinceLastRelease = 0;
				pThis->residenceBuildingState--; // decrement 30s state to next 30s state or END state (spawn)
			}
			else
			{
				const auto queueLogicUpdateNumber = RegisterForLogicUpdate(0xE, pBuilding->buildingID);
				pBuilding->logicQueueNumber = queueLogicUpdateNumber;
			}
		}
	}
}
