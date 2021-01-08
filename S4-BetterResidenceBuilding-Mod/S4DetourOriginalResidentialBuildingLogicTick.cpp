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
void __fastcall originalCResidentialBuildingUpdateLogic(S4ResidenceBuilding* pThis, void* PLACEHOLDER_IGNORE, S4Building* pBuilding) {
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


    if (pBuilding->guiFlags & 0x200) {
        pThis->updateGUI(pBuilding, 1);
    }
    if (pThis->residenceBuildingState == 2) {
        /* residence building is not ready to spawn new settler, increase BUSY-logic-calls-in-a-row counter and register for logic update queue*/
        auto ticks = pThis->tickCounterSinceLastRelease;
        auto runsTillRelease = pThis->pUnknownStructResidenceBuilding->runsTillReleaseSettler;
        if (ticks <= runsTillRelease) {
            pThis->tickCounterSinceLastRelease++;
            auto queueLogicUpdateNumber = RegisterForLogicUpdate(0xe, pBuilding->buildingID);
            pBuilding->logicQueueNumber = queueLogicUpdateNumber;
            return;
        }

        if (pThis->releasedInhabitants < pThis->pUnknownStructResidenceBuilding->maxInhabitantsCounts) {

            // these two are one instruction in assembler
            pThis->tickCounterSinceLastRelease = 0;
            pThis->residenceBuildingState = 3;

            auto queueLogicUpdateNumber = RegisterForLogicUpdate(0xe, pBuilding->buildingID);
            pBuilding->logicQueueNumber = queueLogicUpdateNumber;
            return;
        }
    }
    else {
        if (pThis->residenceBuildingState == 3 &&
            pThis->releasedInhabitants < pThis->pUnknownStructResidenceBuilding->maxInhabitantsCounts) {
            /* Count total players */
            uint32_t partyID = S4Util::getPartyID(pBuilding->playerFlags);
            uint32_t totalSettlerCount = S4Util::countTotalSettlersParty(partyID);
               
            uint32_t maxSettlersPerParty = 0xFFFF;

            /* Determine max settlers per party (0xFFFF is a modded value to remove settler restriction, this was probably litze's stuff :) ) */
            if (*pgPartiesInMap < 9) {
                maxSettlersPerParty = 0xFFFF;
            }
            else {
                maxSettlersPerParty = 20000 / *pgPartiesInMap;
            }
            if (totalSettlerCount < maxSettlersPerParty) {
                /* Check if settler can be spawned, spawn if so*/
                DWORD x = S4Util::getX(pBuilding->buildingPosXY);
                DWORD y = S4Util::getY(pBuilding->buildingPosXY);
                //DBGPRINTLINE("adjust settler if...");
                // retreive sector id of building position
                uint16_t sectorID = S4Util::getSectorID(x, y);
                if (CanSpawnSettlerAtInSector(&x, &y, sectorID)) { // not dynamic, but its always 2?
                    if (AddSettler(x, y, partyID, pThis->pUnknownStructResidenceBuilding->settlerTypeSpawn, 0) > 0) {
                        /* Update player counts */
                        uint32_t index = partyID * (0x1128 / sizeof(uint32_t));
                        gaPartyInfoOffsetFreeSettlerBuilderDigger[index]++;
                        gaPartyInfoOffsetSettlersAddedTotal[index]++;
                        pThis->releasedInhabitants++;

                        if (pThis->pUnknownStructResidenceBuilding->maxInhabitantsCounts <= pThis->releasedInhabitants) {
                            /* signal AI of party house is done. This is the last part of code that gets called of this function per building originally */
                            uint32_t buildingID = static_cast<uint32_t>(pBuilding->buildingID);
                            pgAIMain->PostAIEvent(8, partyID, buildingID, 0);

                            /* MODDED: add another logic update request, this function will not take care of this house anymore */
                            auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x1F, pBuilding->buildingID);
                            pBuilding->logicQueueNumber = queueLogicUpdateNumber;
                            pThis->tickCounterSinceLastRelease = 0;
                            pThis->residenceBuildingState = 2;

                            /* Update GUI */
                            // function ends if user is not inside the house (UI wise of course)
                            if ((pBuilding->guiFlags >> 9 & 1) == 0 || partyID != *pgCurrentPartyID)
                                return;
                            pThis->updateGUI(pBuilding, 1);

                            return;
                        }
                    }
                }
            }
            if (pThis->pUnknownStructResidenceBuilding->runsTillReleaseSettler == 0) {
                auto queueLogicUpdateNumber = RegisterForLogicUpdate(0x1F, pBuilding->buildingID);
                pBuilding->logicQueueNumber = queueLogicUpdateNumber;
                return;
            }
            auto queueLogicUpdateNumber = RegisterForLogicUpdate(0xE, pBuilding->buildingID);
            pBuilding->logicQueueNumber = queueLogicUpdateNumber;
            pThis->tickCounterSinceLastRelease = 0;
            pThis->residenceBuildingState = 2;
            return;
        }
    }
}