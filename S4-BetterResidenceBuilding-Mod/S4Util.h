#pragma once
#include <cstdint>
#include <S4ModApi.h>
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

    uint32_t countTotalSettlersParty(uint32_t partyID);
}