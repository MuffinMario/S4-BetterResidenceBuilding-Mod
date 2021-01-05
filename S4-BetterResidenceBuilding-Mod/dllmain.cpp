// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.
#include "pch.h"
#include "BetterResidenceBuildings.h"

#pragma comment(lib,"S4ModApi")

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if(S4BetterResidenceBuildings::onAttach() != ATTACH_VALUE::SUCCESS)
            Winutil::messageBoxA(S4BetterResidenceBuildings::getLastErrorStr().c_str(), "S4BetterResidenceBuildings, onAttach");
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (S4BetterResidenceBuildings::onDetach() != DETACH_VALUE::SUCCESS)
        {
            Winutil::messageBoxA(S4BetterResidenceBuildings::getLastErrorStr().c_str(), "S4BetterResidenceBuildings, onDetach");
        }
        break;
    }
    return TRUE;
}

