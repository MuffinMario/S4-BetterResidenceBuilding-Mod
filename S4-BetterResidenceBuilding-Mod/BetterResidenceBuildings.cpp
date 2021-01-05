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


// DEBUG
S4API pS4Api;

void S4BetterResidenceBuildings::assignConstantAddresses(HANDLE hS4) {

    // function addresses we need
    assignExternal(RegisterForLogicUpdate, hS4, Offsets::Functions::REGISTERFORLOGICUPDATE);
    assignExternal(AddSettler, hS4, Offsets::Functions::ADDSETTLER);
    assignExternal(CanSpawnSettlerAtInSector, hS4, Offsets::Functions::ADJUSTPOSFORSETTLER);

    // all of this for sector id :bruh:
    assignExternal(pgMapsize, hS4, Offsets::Values::MAPSIZE);
    assignExternal(gaFieldIDType, hS4, Offsets::Values::AMAPFIELDIDTYPE_IGUESS);
    assignExternal(pgpaMapFieldTypes, hS4, Offsets::Values::PAFIELDIDTYPE);

    assignExternal(pgPartiesInMap, hS4, Offsets::Values::PARTIESINMAP);
    assignExternal(pgCurrentPartyID, hS4, Offsets::Values::CURRENTPARTYID);

    assignExternal(pgAIMain, hS4, Offsets::Values::PAIMAIN);

    assignExternal(gaPlayerStats, hS4, Offsets::Values::APLAYERSTATS);
    assignExternal(gaPartyInfoOffsetFreeSettlers, hS4, Offsets::Values::APARTYINFOOFFSETFREESETTLERS);
    assignExternal(gaPartyInfoOffsetSettlersAddedTotal, hS4, Offsets::Values::APARTYINFOOFFSETSETTLERSADDEDTOTAL);
}
BOOL S4BetterResidenceBuildings::overwriteVTable(S4API pS4API, HANDLE hS4Base) {
   

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
        // this is potentially unsafe...? no other thread should WRITE this addr, but maybe read, and jesus knows what happens then
        // TODO: assure exe rdata is already initalized? dlls, especially these, should load after the exe has been loaded into memory right?
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


ATTACH_VALUE S4BetterResidenceBuildings::onAttach()
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

    assignConstantAddresses(hS4);

    if (overwriteVTable(m_pS4API, m_hS4Base) != TRUE)
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
    return DETACH_VALUE::SUCCESS;
}

bool S4BetterResidenceBuildings::createAPI()
{
    m_pS4API = S4ApiCreate();
    if (m_pS4API == nullptr) {
        return false;
    }
    return true;
}

bool S4BetterResidenceBuildings::releaseAPI()
{
    if (m_pS4API) {
        m_pS4API->Release();
        m_pS4API = nullptr;
        return true;
    }
    return false;
}
