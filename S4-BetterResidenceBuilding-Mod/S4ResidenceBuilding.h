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
#include "S4DataTypes.h"
#include <cstdint>
#include "macros.h"

struct S4ResidenceBuilding
{
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
	S4UnknownInformationStructResidenceBuilding* pUnknownStructResidenceBuilding; //0x178h
	uint8_t releasedInhabitants;
};
