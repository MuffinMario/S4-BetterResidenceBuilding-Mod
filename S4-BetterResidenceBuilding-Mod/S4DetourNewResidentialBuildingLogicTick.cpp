#include "pch.h"
#include "S4Detours.h"
#include "S4ResidenceBuilding.h"
#include "S4Building.h"
#include "S4UnknownInformationStructResidenceBuilding.h"
#include "S4Functions.h"
#include "S4Vars.h"
#include "S4Util.h"
extern S4API pS4Api;
// ecx                          edx                         first stack param
void __fastcall newCResidentialBuildingUpdateLogic(S4ResidenceBuilding* pThis, void* PLACEHOLDER_IGNORE, S4Building* pBuilding) {
    if (pThis->releasedInhabitants < pThis->pUnknownStructResidenceBuilding->maxInhabitantsCounts)
    {
        //DBGPRINTLINE("original function");
        originalCResidentialBuildingUpdateLogic(pThis, PLACEHOLDER_IGNORE, pBuilding);
    }
    else
    {
        //DBGPRINTLINE("bruh function");
        if (pThis->residenceBuildingState == 2) {
            pThis->tickCounterSinceLastRelease++;
            uint32_t ticksRequired = -1;
            if (pThis->tickCounterSinceLastRelease >= 180) { // roughly the amount of seconds
                auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x1, pBuilding->buildingID);
                pBuilding->logicQueueNumber = queueLogicUpdateNumber;
                pThis->tickCounterSinceLastRelease = 0;
                pThis->residenceBuildingState = 3;
            }
            else {
                auto queueLogicUpdateNumber = RegisterForLogicUpdate(0xE, pBuilding->buildingID);
                pBuilding->logicQueueNumber = queueLogicUpdateNumber;
            }
        }
        else if (pThis->residenceBuildingState == 3) {
            uint32_t partyID = pBuilding->playerFlags >> 4;
            DWORD x = pBuilding->buildingPosXY & 0xFFFF;
            DWORD y = pBuilding->buildingPosXY >> 0x10;

            uint16_t sectorID = S4Util::getSectorID(x, y);
            /* This WILL create collisions with new residences. THIS HAS TO BE REVAMPED if interest spikes idk */
            uint32_t index = partyID * (0x1128 / sizeof(uint32_t));
            // frick finding this myself, unlike building count, this has settlers have no state difference
            uint32_t freesettlers = pS4Api->SettlersAmount(S4_OBJECT_SETTLER_CARRIER,partyID);//gaPartyInfoOffsetFreeSettlerBuilderDigger[index];
            uint32_t beds = S4Util::getBedCount(partyID);

            //delimit max settlers spawned this way to carriers - 5
            const uint32_t SETTLERBUFFERLIMIT = 5;
            //DBGPRINTLINE(freesettlers);
            //DBGPRINTLINE(beds);
            if (beds > freesettlers + SETTLERBUFFERLIMIT) {
                if (CanSpawnSettlerAtInSector(&x, &y, sectorID)) { 
                    if (AddSettler(x, y, partyID, pThis->pUnknownStructResidenceBuilding->settlerTypeSpawn, 0) > 0) {
                        /* Update player counts */
                        gaPartyInfoOffsetFreeSettlerBuilderDigger[index]++;
                        gaPartyInfoOffsetSettlersAddedTotal[index]++;
                        //pThis->releasedInhabitants++;

                        /* Queue agane */
                        auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x1F, pBuilding->buildingID);
                        pBuilding->logicQueueNumber = queueLogicUpdateNumber;
                        pThis->tickCounterSinceLastRelease = 0;
                        pThis->residenceBuildingState = 2;

                        /* Update GUI */
                        // function ends if user is not inside the house (UI wise of course)
                        if ((pBuilding->guiFlags >> 9 & 1) == 0 || partyID != *pgCurrentPartyID)
                            return;
                        pThis->updateGUI(pBuilding, 1);
                    }
                }
            }
            else {
                // reset minute timer if bed limit has reached. prevent a huge flood of waiting settlers (rethink?)
                auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x1F, pBuilding->buildingID);
                pBuilding->logicQueueNumber = queueLogicUpdateNumber;
                pThis->tickCounterSinceLastRelease = 0;
                pThis->residenceBuildingState = 2;
            }
        }
    }
}