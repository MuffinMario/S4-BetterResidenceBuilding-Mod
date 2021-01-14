#pragma once
#include <Windows.h>
#include <string>
#include "S4ModApi.h"
#include "winutil.h"
enum class ATTACH_VALUE {
	SUCCESS = 0,
	FAILED_COULD_NOT_CREATE_API_PTR,
	FAILED_COULD_NOT_OVERRIDE_VTABLE,
	FAILED_UNKNOWN
};
enum class DETACH_VALUE {
	SUCCESS = 0,
	FAILED_UNKNOWN
};
struct S4BetterResidenceBuildings
{
	static ATTACH_VALUE onAttach();
	static DETACH_VALUE onDetach();

	static const std::string getLastErrorStr() {
		return m_lastErrorStr;
	}
private:
	static std::string m_lastErrorStr;

	static S4API m_pS4API;
	static Winutil::CHandle m_hS4Base;
	
	static DWORD m_oldResidentialBuildingUpdateLogic;

	static bool createAPI();
	static bool releaseAPI();

	
	static void assignConstantAddresses(HANDLE hS4);
	static BOOL overwriteResidentialBuildingLogicTickVTable(S4API pS4API, HANDLE hS4Base,DWORD to);
};

