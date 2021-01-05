#pragma once
#include <cstdint>
#include <Windows.h>
#include "S4DataTypes.h"
typedef DWORD(__stdcall* FPRegisterForLogicUpdate)(DWORD tickDelta, DWORD objectID);
typedef uint8_t(__stdcall* FPAdjustSpotForSettler)(DWORD* x, DWORD* y, DWORD sectorID);
typedef DWORD(__stdcall* FPAddSettler)(DWORD x, DWORD y, DWORD partyID,DWORD settlerType,DWORD unknown);

//thiscalls?. # revisioned to be implemented in a "pseudo vtable" instead
//typedef void(__stdcall* FPS4ResidenceBuildingGUIUpdateGUI)(S4Building*, uint32_t);
//typedef void(__stdcall* FPS4AIMainPostAIEvent)(DWORD aiType,DWORD partyID,DWORD objectID,DWORD unknown);