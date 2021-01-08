#pragma once
#include "S4DataTypes.h"
#include <cstdint>
#include "macros.h"
struct S4ResidenceBuilding {
    // vtable 
    virtual void dummy00();
    virtual void dummy04();
    virtual void dummy08();
    virtual void dummy0C(); // <- LOGICUPDATE, but whatever
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
    virtual void updateGUI(S4Building* pBuilding, uint32_t);

    // Class allocated memory, remember [0,3] bytes are vtable ptr
    uint8_t residenceBuildingState; // 4h
    uint8_t tickCounterSinceLastRelease; //5h
    OFFSETDATA(1, 0x172);
    S4UnknownInformationStructResidenceBuilding* pUnknownStructResidenceBuilding;//0x178h
    uint8_t releasedInhabitants;
};