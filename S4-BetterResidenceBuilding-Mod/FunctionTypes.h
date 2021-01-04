#pragma once
#include <Windows.h>
struct S4Building;
struct S4ResidenceBuilding;
typedef DWORD(__stdcall* FPRegisterForLogicUpdate)(DWORD tickDelta, DWORD objectID);
typedef uint8_t(__stdcall* FPAdjustSpotForSettler)(DWORD* x, DWORD* y, DWORD type);
typedef DWORD(__stdcall* FPAddSettler)(DWORD x, DWORD y, DWORD partyID,DWORD settlerType,DWORD unknown);

//thiscalls?
typedef void(__stdcall* FPS4ResidenceBuildingGUIUpdateGUI)(S4Building*, uint32_t);
typedef void(__stdcall* FPS4AIMainPostAIEvent)(DWORD aiType,DWORD partyID,DWORD objectID,DWORD unknown);