#include "pch.h"
#include "S4Detours.h"
#include "S4ResidenceBuilding.h"
#include "S4Building.h"
#include "S4UnknownInformationStructResidenceBuilding.h"
#include "S4Functions.h"
#include "S4Vars.h"
#include "S4Util.h"
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
            if (pThis->tickCounterSinceLastRelease >= 60) { // roughly the amount of seconds
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

            if (CanSpawnSettlerAtInSector(&x, &y, sectorID)) { // not dynamic, but its always 2?
                if (AddSettler(x, y, partyID, pThis->pUnknownStructResidenceBuilding->settlerTypeSpawn, 0) > 0) {
                    /* Update player counts */
                    uint32_t index = partyID * (0x1128 / sizeof(uint32_t));
                    gaPartyInfoOffsetFreeSettlers[index]++;
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
    }
}