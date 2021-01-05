#pragma once
#include <cstdint>
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
    virtual void PostAIEvent(int32_t aiType, int32_t partyID, int32_t objectID, int32_t unknown);
};