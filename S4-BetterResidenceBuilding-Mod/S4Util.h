#pragma once
#include <cstdint>
#include <S4ModApi.h>
#include "S4MemoryOffsets.h"
#include "winutil.h"
#include "S4Vars.h"
/* Utility functions for S4 to save myself time and space */
namespace S4Util {
    /* YYYY.YYYY YYYY.YYYY XXXX.XXXX XXXX.XXXX*/
    uint32_t getX(uint32_t pos);
    /* YYYY.YYYY YYYY.YYYY XXXX.XXXX XXXX.XXXX*/
    uint32_t getY(uint32_t pos);

    uint32_t getSectorID(uint32_t x, uint32_t y);

    void showTextMessage(S4API pS4, STRING8 s);

    /* PPPP.???? */
    uint32_t getPartyID(uint8_t playerFlags);

    // todo maybe use original s4 function for this instead?
    race_t getRace(uint32_t partyID);
    
    enum class PARTY_BUILDING_TYPE {
        SMALL_RESIDENCE,
        MEDIUM_RESIDENCE,
        BIG_RESIDENCE
    };
    // todo get entire array instead of all single, maybe use original s4 function for this instead?
    uint32_t getBuildingCount(uint32_t partyID, PARTY_BUILDING_TYPE buildingType);
    enum class RACE_INFO {
        SMALL_RESIDENCE_INHABITANTS,
        MEDIUM_RESIDENCE_INHABITANTS,
        BIG_RESIDENCE_INHABITANTS
    };
    uint32_t getRaceInfo(race_t race, RACE_INFO raceInfo);

    uint32_t getInitialBedCount(uint32_t partyID);

    uint32_t getResidenceBedCount(uint32_t partyID);
    uint32_t getBedCount(uint32_t partyID);

    uint32_t countTotalSettlersParty(uint32_t partyID);
}