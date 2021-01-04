#pragma once
#include <cstdint>

namespace Offsets {
	namespace {
		//ghidra
		constexpr uint32_t BASEOFFSET = 0x400000;
	}
	namespace Functions {
		constexpr uint32_t CRESIDENTIALBUILDING_VTABLE = 0x1059010 - BASEOFFSET;
		constexpr uint32_t CRESIDENTIALBUILDING_VTABLE_UPDATELOGIC = CRESIDENTIALBUILDING_VTABLE + 0xC;

		constexpr uint32_t REGISTERFORLOGICUPDATE = 0x56D5D0 - BASEOFFSET;
		constexpr uint32_t ADDSETTLER = 0x582600 - BASEOFFSET;
		constexpr uint32_t ADJUSTPOSFORSETTLER = 0x5857c0 - BASEOFFSET;

		constexpr uint32_t CRESIDENTIALBUILDING_UPDATELOGIC = 0x54DCA0 - BASEOFFSET;
	}
	namespace Values {
		constexpr uint32_t PARTIESINMAP = 0x1294828 - BASEOFFSET;
		constexpr uint32_t APLAYERSTATS = 0x1545660 - BASEOFFSET;
		constexpr uint32_t CURRENTPARTYID = 0x0129482c - BASEOFFSET;

		// pointers actually
		constexpr uint32_t PAIMAIN = 0x106B148;
		constexpr uint32_t APARTYINFOOFFSETFREESETTLERS = 0x11abf48 - BASEOFFSET;
		constexpr uint32_t APARTYINFOOFFSETSETTLERSADDEDTOTAL = 0x11ac050 - BASEOFFSET;
	}
}