// /*
// * S4-BetterResidenceBuilding-Mod
// Version 0.2, May 2023
// Copyright (C) 2023 MuffinMario https://github.com/MuffinMario https://muffinmar.io/
// This software and associated documentation files (the "Software") are licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License ("CC BY-NC-SA License").
// You are free to:
// - Share: Copy and redistribute the Software in any medium or format.
// - Adapt: Remix, transform, and build upon the Software.
//  
// Under the following terms:
// 1. Attribution: You must give appropriate credit, provide a link to the license, and indicate if changes were made. 
// You must give credit to the original authors of the Software and all contributors. 
// You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
// 2. NonCommercial: You may not use the Software for commercial purposes without obtaining explicit permission from the licensor.
// 3. ShareAlike: If you remix, transform, or build upon the Software, you must distribute your contributions under the same license as the original.
// The above licensing terms apply to all parts of the Software and any modifications made to it.
// This license does not grant you any patent rights, trademark rights, or any other intellectual property rights. The Software is provided "as is," without any warranty or guarantee, expressed or implied.
// For more details about the CC BY-NC-SA License, please visit: https://creativecommons.org/licenses/by-nc-sa/4.0/
// */

#pragma once
#include <cstdint>
#include <Windows.h>
#include "S4DataTypes.h"
using FPRegisterForLogicUpdate = DWORD(__stdcall*)(DWORD priority, DWORD objectID);
using FPAdjustSpotForSettler = uint8_t(__stdcall*)(DWORD* x, DWORD* y, DWORD sectorID);
using FPAddSettler = DWORD(__stdcall*)(DWORD x, DWORD y, DWORD partyID, DWORD settlerType, DWORD unknown);

//thiscalls?. # revisioned to be implemented in a "pseudo vtable" instead
//typedef void(__stdcall* FPS4ResidenceBuildingGUIUpdateGUI)(S4Building*, uint32_t);
//typedef void(__stdcall* FPS4AIMainPostAIEvent)(DWORD aiType,DWORD partyID,DWORD objectID,DWORD unknown);
