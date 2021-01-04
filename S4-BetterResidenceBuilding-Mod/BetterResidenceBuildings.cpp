#include "pch.h"
#include "BetterResidenceBuildings.h"

S4API SBetterResidenceBuildings::m_pS4API = nullptr;
Winutil::CHandle SBetterResidenceBuildings::m_hS4Base = nullptr;
std::string SBetterResidenceBuildings::m_lastErrorStr;

#include <processthreadsapi.h>
#include <bitset>

#include "Offsets.h"
#include "FunctionTypes.h"

static_assert(sizeof(uint8_t) == 1, "uint8_t is not 1 byte size");
static_assert(sizeof(uint8_t*) == 4, "pointer size does not equal 4 bytes, build this project with x86 settings");

// theres probably a prettier way to do this and a more compiler friendly way
#define OFFSETDATA(t,bytes) uint8_t unknown##t[bytes]

template<typename T>
void assignExternal(T& var, HANDLE baseHandle, uint32_t offset) {
    var = reinterpret_cast<T>((uint32_t)baseHandle + offset);
}
void showTextMessage(S4API pS4, STRING8 s) {
    pS4->ShowTextMessage(s.c_str(), 9, 0);
}
FPRegisterForLogicUpdate RegisterForLogicUpdate;
FPAddSettler AddSettler;
FPAdjustSpotForSettler AdjustPosForSettler;

// DEBUG
S4API pS4Api;

struct S4AIMain {
    // fill non-used vtable to simulate actual once so we dont have to use assembly lol im doing this because I dont want to see assembly code anymore
    virtual void dummy00();
    virtual void dummy04();
    virtual void dummy08();
    virtual void dummy0C();
    virtual void dummy10();
    virtual void dummy14();
    virtual void dummy18();
    virtual void dummy1C();
    virtual void dummy20();
    virtual void dummy24();
    virtual void dummy28();
    virtual void PostAIEvent(DWORD aiType, DWORD partyID, DWORD objectID, DWORD unknown);
};
struct S4Building {
    OFFSETDATA(1,0x8); // 0 -> 8h
    uint16_t buildingID; // 8h -> Ah
    OFFSETDATA(2, 10);// Ah -> 0x14
    uint32_t guiFlags; // 14h
    OFFSETDATA(3, 0x8); // 18h
    uint8_t playerFlags; // 20h
    OFFSETDATA(4, 0xF);
    uint32_t logicQueueNumber; // 30h -> 34h
    OFFSETDATA(5, 0x10);
    uint32_t buildingPosXY;

};
// this is probably a generalized struct with information about every building (?) hence the 1 billion 0x0 fields and the sometimes important data
struct S4UnknownInformationStructResidenceBuilding {
    OFFSETDATA(1, 0x1de);
    /* Settler Type that spawns from building */
    uint8_t settlerTypeSpawn; // 1de
    OFFSETDATA(2, 0x1);
    uint8_t runsTillReleaseSettler; // 1e0
    OFFSETDATA(3, 0xF);
    uint32_t maxInhabitantsCounts; // 1f0
};
struct S4ResidenceBuilding {
    // vtable 
    virtual void dummy00();
    virtual void dummy04();
    virtual void dummy08();
    virtual void dummy0C();
    virtual void dummy10();
    virtual void dummy14();
    virtual void dummy18();
    virtual void dummy1C();
    virtual void dummy20();
    virtual void dummy24();
    virtual void dummy28();
    virtual void dummy2C();
    virtual void dummy30();
    virtual void dummy34();
    virtual void dummy38();
    virtual void dummy3C();
    virtual void dummy40();
    virtual void dummy44();
    virtual void dummy48();
    virtual void dummy4C();
    virtual void dummy50();
    virtual void dummy54();
    virtual void updateGUI(S4Building* pBuilding,uint32_t);

    // Class allocated memory, remember [0,3] bytes are vtable ptr
    uint8_t residenceBuildingState; // 4h
    uint8_t tickCounterSinceLastRelease; //5h
    OFFSETDATA(1, 0x172);
    S4UnknownInformationStructResidenceBuilding* pUnknownStructResidenceBuilding;//0x178h
    uint8_t releasedInhabitants;
};

// Global vars in .EXE
//vars
uint32_t* pgPartiesInMap;
uint32_t* pgCurrentPartyID;
//ptrs (/arrays)
uint32_t* gaPlayerStats;
int32_t* gaPartyInfoOffsetFreeSettlers;
int32_t* gaPartyInfoOffsetSettlersAddedTotal;

#define DBGPRINTLINE(var) { SSTREAM ss; ss << #var << " - " << std::hex <<  (uint32_t)var; showTextMessage(pS4Api, ss.str()); ss.str("");ss.clear(); } //clear buf + error flags

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

            uint32_t settlerCountLeft = 0, settlerCountMiddle = 0, settlerCountRight = 0; // probably just compiler optimisation
            uint32_t partyID = pBuilding->playerFlags >> 4;
            constexpr size_t sizeOfPlayerStatElement = 0x10c;
            uint32_t* paSettlerCountsPartyArray = gaPlayerStats + (partyID * sizeOfPlayerStatElement) / sizeof(uint32_t); // 0x1545660 table addr, 0x10c sizeof
            for (int i = 16; i > 0; i--) {
                settlerCountLeft += paSettlerCountsPartyArray[-1];
                settlerCountMiddle += paSettlerCountsPartyArray[0];
                settlerCountRight += paSettlerCountsPartyArray[1];
                paSettlerCountsPartyArray = paSettlerCountsPartyArray + 3; // should be + 0xC
            }
            uint32_t totalSettlerCount = settlerCountLeft + settlerCountMiddle + settlerCountRight;
            uint32_t maxSettlersPerParty = 0xFFFF;

            /* Determine max settlers per party (0xFFFF is a modded value to remove settler restriction) */
            if (*pgPartiesInMap < 9) {
                maxSettlersPerParty = 0xFFFF;
            }
            else {
                maxSettlersPerParty = 20000 / *pgPartiesInMap;
            }
            if (totalSettlerCount < maxSettlersPerParty) {
                /* Check if settler can be spawned, spawn if so*/
                DWORD x = pBuilding->buildingPosXY & 0xFFFF;
                DWORD y = pBuilding->buildingPosXY >> 0x10;
                if (AdjustPosForSettler(&x, &y, 2)) { // not dynamic, but its always 2?
                    if (AddSettler(x, y, partyID, pThis->pUnknownStructResidenceBuilding->settlerTypeSpawn, 0) > 0) {
                        /* Update player counts */
                        uint32_t index = partyID * (0x1128 / sizeof(uint32_t));
                        gaPartyInfoOffsetFreeSettlers[index]++;
                        gaPartyInfoOffsetSettlersAddedTotal[index]++;
                        pThis->releasedInhabitants++;

                        if (pThis->pUnknownStructResidenceBuilding->maxInhabitantsCounts <= pThis->releasedInhabitants) {
                            /* signal AI of party house is done. This is the last part of code that gets called of this function per building originally */
                            S4AIMain* pAIMain = nullptr;
                            uint32_t buildingID = static_cast<uint32_t>(pBuilding->buildingID);
                            assignExternal(pAIMain, GetModuleHandle(nullptr), Offsets::Values::PAIMAIN);

                            pAIMain->PostAIEvent(8, partyID, buildingID, 0);

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
            if (pThis->tickCounterSinceLastRelease >= 60) {
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
            if (AdjustPosForSettler(&x, &y, 2)) { // not dynamic, but its always 2?
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
void assignConstantAddresses(HANDLE hS4) {

    assignExternal(RegisterForLogicUpdate, hS4, Offsets::Functions::REGISTERFORLOGICUPDATE);
    assignExternal(AddSettler, hS4, Offsets::Functions::ADDSETTLER);
    assignExternal(AdjustPosForSettler, hS4, Offsets::Functions::ADJUSTPOSFORSETTLER);

    assignExternal(pgPartiesInMap, hS4, Offsets::Values::PARTIESINMAP);
    assignExternal(pgCurrentPartyID, hS4, Offsets::Values::CURRENTPARTYID);

    assignExternal(gaPlayerStats, hS4, Offsets::Values::APLAYERSTATS);
    assignExternal(gaPartyInfoOffsetFreeSettlers, hS4, Offsets::Values::APARTYINFOOFFSETFREESETTLERS);
    assignExternal(gaPartyInfoOffsetSettlersAddedTotal, hS4, Offsets::Values::APARTYINFOOFFSETSETTLERSADDEDTOTAL);
}
#define ASKBEFOREOVERWRITE 0
BOOL overwriteVTable(S4API pS4API, HANDLE hS4Base) {
   

    void* residentialBuildingVTableUpdateLogic = (void*)((uint32_t)hS4Base + Offsets::Functions::CRESIDENTIALBUILDING_VTABLE_UPDATELOGIC);
    DWORD newFnc = (DWORD)newCResidentialBuildingUpdateLogic;
#if ASKBEFOREOVERWRITE == 1
    {
        SSTREAM ss;
        ss << "overwriting vtable addr at " << residentialBuildingVTableUpdateLogic << " to " << std::hex << newFnc << "?";
        if (Winutil::messageBoxA(ss.str(), PROJECT_NAME, nullptr, Winutil::MessageBoxTypes::YESNO) == IDNO)
            return true;
    }
#endif

    BOOL ret;
    {
        // this is potentially unsafe, no other thread should WRITE this addr, but maybe read, and jesus knows what happens then
        // make page writable
        DWORD oldProtection;
        LPVOID startPage = (LPVOID)((unsigned int)hS4Base + 0x1000 + 0xb14000);
        size_t pageSize = 0x231000;
        VirtualProtect(startPage, pageSize, PAGE_READWRITE, &oldProtection);
        //write new vtable address
        ret = WRITE_AT(residentialBuildingVTableUpdateLogic, newFnc);
        // make page unwritable again
        VirtualProtect(startPage, pageSize, oldProtection, nullptr);
    }
    return ret;

}


ATTACH_VALUE SBetterResidenceBuildings::onAttach()
{
#ifdef _DEBUG
    //Winutil::messageBoxA("YOU ARE USING A DEBUG VERSION OF " PROJECT_NAME, PROJECT_NAME);
#endif
    if (!createAPI()) {
        m_lastErrorStr = "Call to S4ApiCreate returned nullptr";
        return ATTACH_VALUE::FAILED_COULD_NOT_CREATE_API_PTR;
    }
    m_hS4Base = GetModuleHandle(nullptr);
    HANDLE hS4 = m_hS4Base;
    pS4Api = m_pS4API;
    //Winutil::messageBoxA("Bruh", "BRUHH");
    assignConstantAddresses(hS4);

    if (overwriteVTable(m_pS4API, m_hS4Base) != TRUE)
    {
        m_lastErrorStr = "Could not overwrite vtable values";
        return ATTACH_VALUE::FAILED_COULD_NOT_OVERRIDE_VTABLE;
    }

    return ATTACH_VALUE::SUCCESS;
}

DETACH_VALUE SBetterResidenceBuildings::onDetach()
{
    // we don't need to warn again, it's just a detach... unless there will be dynamic detach on runtime
    releaseAPI();
    return DETACH_VALUE::SUCCESS;
}

bool SBetterResidenceBuildings::createAPI()
{
    m_pS4API = S4ApiCreate();
    if (m_pS4API == nullptr) {
        return false;
    }
    return true;
}

bool SBetterResidenceBuildings::releaseAPI()
{
    if (m_pS4API) {
        m_pS4API->Release();
        m_pS4API = nullptr;
        return true;
    }
    return false;
}
