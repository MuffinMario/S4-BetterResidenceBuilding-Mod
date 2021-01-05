#include "pch.h"
#include "S4Util.h"



/* YYYY.YYYY YYYY.YYYY XXXX.XXXX XXXX.XXXX*/
uint32_t S4Util::getX(uint32_t pos) {
    return pos & 0xFFFF;
}

/* YYYY.YYYY YYYY.YYYY XXXX.XXXX XXXX.XXXX*/
uint32_t S4Util::getY(uint32_t pos) {
    return pos >> 0x10;
}

uint32_t S4Util::getSectorID(uint32_t x, uint32_t y) {
    uint32_t mapsize = *pgMapsize;
    uint16_t* aFieldTypes = *pgpaMapFieldTypes;
    uint16_t fieldTypeAtPos = aFieldTypes[mapsize * y + x];
    return *(gaFieldIDType + pgaFieldIDTypeStructSize * fieldTypeAtPos);
}

void S4Util::showTextMessage(S4API pS4, STRING8 s) {
    pS4->ShowTextMessage(s.c_str(), 9, 0);
}


/* PPPP.???? */
uint32_t S4Util::getPartyID(uint8_t playerFlags) {
    return playerFlags >> 4;
}

uint32_t S4Util::countTotalSettlersParty(uint32_t partyID) {
    uint32_t settlerCountLeft = 0, settlerCountMiddle = 0, settlerCountRight = 0; // probably just compiler optimisation
    constexpr size_t sizeOfPlayerStatElement = 0x10c;
    uint32_t* paSettlerCountsPartyArray = gaPlayerStats + (partyID * sizeOfPlayerStatElement) / sizeof(uint32_t); // 0x1545660 table addr, 0x10c sizeof
    for (int i = 16; i > 0; i--) {
        settlerCountLeft += paSettlerCountsPartyArray[-1];
        settlerCountMiddle += paSettlerCountsPartyArray[0];
        settlerCountRight += paSettlerCountsPartyArray[1];
        paSettlerCountsPartyArray = paSettlerCountsPartyArray + 3; // should be + 0xC
    }
    return settlerCountLeft + settlerCountMiddle + settlerCountRight;
}
