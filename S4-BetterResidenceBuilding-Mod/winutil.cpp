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

#include "pch.h"
#include "winutil.h"
#include <TlHelp32.h>
#include <Psapi.h>

#define SAFE_HANDLE CHandleGuard

namespace Winutil
{
	STRING8 GetErrorAsStringA(DWORD err)
	{
		if (err == 0)
			return STRING8(); //No error message has been recorded

		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);

		STRING8 message(messageBuffer, size);
		SSTREAM ss;
		ss << " (Code: " << err << ")";
		message.append(ss.str());
		//Free the buffer.
		LocalFree(messageBuffer);

		return message;
	}

	WSTRING GetErrorAsStringW(DWORD err)
	{
		if (err == 0)
			return WSTRING(); //No error message has been recorded

		LPWSTR messageBuffer = nullptr;
		size_t size = FormatMessageW(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&messageBuffer, 0, nullptr);

		WSTRING message(messageBuffer, size);
		WSSTREAM ss;
		ss << " (Code: " << err << ")";
		message.append(ss.str());
		//Free the buffer.
		LocalFree(messageBuffer);

		return message;
	}

	STRING8 GetLastErrorAsStringA()
	{
		//Get the error message, if any.
		DWORD errorMessageID = GetLastError();

		return GetErrorAsStringA(errorMessageID);
	}

	WSTRING GetLastErrorAsStringW()
	{
		//Get the error message, if any.
		DWORD errorMessageID = GetLastError();

		return GetErrorAsStringW(errorMessageID);
	}

	DWORD getFirstProcessIDByExeW(WSTRING exe)
	{
		PROCESSENTRY32W entry;
		entry.dwSize = sizeof(PROCESSENTRY32W);

		SAFE_HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		LPCWSTR exestr = exe.c_str();
		if (Process32FirstW(snapshot, &entry) == TRUE)
		{
			do
			{
				if (wcscmp(entry.szExeFile, exestr) == 0)
				{
					return entry.th32ProcessID;
				}
			}
			while (Process32NextW(snapshot, &entry) == TRUE);
		}
		return PID_NOT_FOUND; //make this an exception, eh?
	}
#undef PROCESSENTRY32
#undef Process32First
#undef Process32Next
	DWORD getFirstProcessIDByExeA(STRING8 exe)
	{
		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);

		SAFE_HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		LPCSTR exestr = exe.c_str();

		if (Process32First(snapshot, &entry) == TRUE)
		{
			do
			{
				if (strcmp(entry.szExeFile, exestr) == 0)
				{
					return entry.th32ProcessID;
				}
			}
			while (Process32Next(snapshot, &entry) == TRUE);
		}
		return PID_NOT_FOUND;
	}

	STRING8 getExeByProcessIDA(DWORD pid)
	{
		/* WINAPI DOCUMENTATION:
		The file Psapi.dll is installed in the %windir%\System32 directory.
		If there is another copy of this DLL on your computer, it can lead to the following error when running applications on your
		system: "The procedure entry point GetProcessImageFileName could not be located in the dynamic link library PSAPI.DLL." To
		work around this problem, locate any versions that are not in the %windir%\System32 directory and delete or rename them, then restart.
		*/
		SAFE_HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, pid);
		if (process == nullptr)
			return STRING8();

		constexpr unsigned PATHSIZE = MAX_PATH + 1;
		CHAR deviceBuf[PATHSIZE] = {0},
		     dirBuf[PATHSIZE] = {0},
		     fileBuf[PATHSIZE] = {0},
		     extBuf[PATHSIZE] = {0};
		SSTREAM ss;

		CHAR buf[PATHSIZE] = {0};
		if (!GetProcessImageFileNameA(process, buf, PATHSIZE - 1))
		{
			return STRING8();
		}
		//split path from buffer
		_splitpath_s(buf, deviceBuf, dirBuf, fileBuf, extBuf);
		ss << fileBuf << extBuf;
		return ss.str();
	}

	WSTRING getExeByProcessIDW(DWORD pid)
	{
		/* WINAPI DOCUMENTATION:
		The file Psapi.dll is installed in the %windir%\System32 directory.
		If there is another copy of this DLL on your computer, it can lead to the following error when running applications on your
		system: "The procedure entry point GetProcessImageFileName could not be located in the dynamic link library PSAPI.DLL." To
		work around this problem, locate any versions that are not in the %windir%\System32 directory and delete or rename them, then restart.
		*/
		HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, TRUE, pid);
		if (process == nullptr)
			return WSTRING();

		constexpr unsigned PATHSIZE = MAX_PATH + 1;
		WCHAR deviceBuf[PATHSIZE] = {0},
		      dirBuf[PATHSIZE] = {0},
		      fileBuf[PATHSIZE] = {0},
		      extBuf[PATHSIZE] = {0};
		WSSTREAM ss;

		WCHAR buf[PATHSIZE] = {0};
		if (!GetProcessImageFileNameW(process, buf, PATHSIZE - 1))
		{
			return WSTRING();
		}
		//split path from buffer
		_wsplitpath_s(buf, deviceBuf, dirBuf, fileBuf, extBuf);
		ss << fileBuf << extBuf;
		return ss.str();
	}

	//local struct
	struct handle_data
	{
		DWORD process_id;
		HWND window_handle;
	};

	//local function
	BOOL CALLBACK enum_windows_callback(HWND handle, LPARAM lParam)
	{
		handle_data& data = *(handle_data*)lParam;
		unsigned long process_id = 0;
		GetWindowThreadProcessId(handle, &process_id);
		if (data.process_id != process_id || !isMainWindow(handle))
			return TRUE;
		data.window_handle = handle;
		return FALSE;
	}

	BOOL isMainWindow(HWND handle)
	{
		return GetWindow(handle, GW_OWNER) == static_cast<HWND>(nullptr) && IsWindowVisible(handle);
	}

	HWND findMainWindow(DWORD process_id)
	{
		handle_data data;
		data.process_id = process_id;
		data.window_handle = nullptr;
		EnumWindows(enum_windows_callback, (LPARAM)&data);
		return data.window_handle;
	}

	/*
		Returns whether the process id is running in 64-bit mode by first checking if it is running under WOW64 and then checking the system architecture
	*/
	BOOL processIs64Bit(DWORD pid)
	{
		auto is64BitMachine = [](LPSYSTEM_INFO info) -> BOOL
		{
			return info->wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
				info->wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64 ||
				info->wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64;
		};
		//"The handle must have the PROCESS_QUERY_INFORMATION or PROCESS_QUERY_LIMITED_INFORMATION access right" - IsWow64Process doc
		SAFE_HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_QUERY_LIMITED_INFORMATION, TRUE, pid);
		BOOL isWOW64 = FALSE;

		if (process == nullptr)
		{
			throw CProcessNotFoundException("Process not found");
		}
		if (IsWow64Process(process, &isWOW64) == FALSE)
		{
			return FALSE;
			// System does not contain this function, I can assume this only happens on very old (not all) 32-bit windows systems
		}

		if (isWOW64)
		{
			// process is running under WOW64
			return FALSE; // process is not 64-bit running on 64-bit
		}
		// process runs in native environment
		//a) process is 64-bit running on 64-bit
		//b) process is 32-bit running on 32-bit
		_SYSTEM_INFO info;
		GetNativeSystemInfo(&info);
		return is64BitMachine(&info);
	}

	/*

	Returns:
	If a message box has a Cancel button, the function returns the IDCANCEL value if either the ESC key is pressed or the Cancel button is selected. If the message box has no Cancel button, pressing ESC has no effect.

	If the function fails, the return value is zero. To get extended error information, call GetLastError.

	If the function succeeds, the return value is one of the following menu-item values.
	*/
	int messageBoxA(const STRING8& message, const STRING8& title, HWND window, MessageBoxTypes type,
	                MessageBoxIcons mbi, MessageBoxDefaultButtons defaultbuttons, MessageBoxModalty modalty,
	                MessageBoxOtherSingleOrList others)
	{
		UINT typeParam = static_cast<UINT>(type) | static_cast<UINT>(mbi) | static_cast<UINT>(defaultbuttons)
			| static_cast<UINT>(modalty);
		try
		{
			auto other = std::get<MessageBoxOther>(others);
			typeParam |= static_cast<UINT>(other);
		}
		catch (const std::bad_variant_access& bva)
		{
			// bad overhead tbh
			auto otherlist = std::get<std::initializer_list<MessageBoxOther>>(others);
			for (auto other : otherlist)
				typeParam |= static_cast<UINT>(other);
		}

		return MessageBoxA(window, message.c_str(), title.c_str(), typeParam);
	}

	int messageBoxW(const WSTRING& message, const WSTRING& title, HWND window, MessageBoxTypes type,
	                MessageBoxIcons mbi, MessageBoxDefaultButtons defaultbuttons, MessageBoxModalty modalty,
	                MessageBoxOtherSingleOrList others)
	{
		UINT typeParam = static_cast<UINT>(type) | static_cast<UINT>(mbi) | static_cast<UINT>(defaultbuttons)
			| static_cast<UINT>(modalty);
		try
		{
			auto other = std::get<MessageBoxOther>(others);
			typeParam |= static_cast<UINT>(other);
		}
		catch (const std::bad_variant_access& bva)
		{
			// bad overhead tbh
			auto otherlist = std::get<std::initializer_list<MessageBoxOther>>(others);
			for (auto other : otherlist)
				typeParam |= static_cast<UINT>(other);
		}
		return MessageBoxW(window, message.c_str(), title.c_str(), typeParam);
	}

	bool processContainsModuleA(DWORD pid, STRING8 moduleName)
	{
		SAFE_HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
		if (!process)
			throw CProcessNotFoundException(
				"Could not OpenProcess with information PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE");
		HMODULE aMods[1024];
		DWORD cbNeeded = 0;

		if (EnumProcessModulesEx(process, aMods, sizeof(aMods), &cbNeeded, LIST_MODULES_ALL) == FALSE)
			throw CWinapiFunctionFailure("EnumProcessModulesEx failed.");
		size_t modSize = cbNeeded / sizeof(HMODULE);
		for (size_t i = 0; i < modSize; i++)
		{
			CHAR currentModuleName[MAX_PATH];
			if (!GetModuleBaseNameA(process, aMods[i], currentModuleName, MAX_PATH))
			{
				throw CWinapiFunctionFailure("GetModuleBaseNameA failed.");
			}
			if (moduleName == currentModuleName)
			{
				return true;
			}
		}

		return false;
	}

	bool processContainsModuleW(DWORD pid, WSTRING moduleName)
	{
		SAFE_HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
		if (!process)
			throw CProcessNotFoundException(
				"Could not OpenProcess with information PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE");
		HMODULE aMods[1024];
		DWORD cbNeeded = 0;

		if (EnumProcessModulesEx(process, aMods, sizeof(aMods), &cbNeeded, LIST_MODULES_ALL) == FALSE)
			throw CWinapiFunctionFailure("EnumProcessModulesEx failed.");
		size_t modSize = cbNeeded / sizeof(HMODULE);
		for (size_t i = 0; i < modSize; i++)
		{
			WCHAR currentModuleName[MAX_PATH];
			if (!GetModuleBaseNameW(process, aMods[i], currentModuleName, MAX_PATH))
			{
				throw CWinapiFunctionFailure("GetModuleBaseNameA failed.");
			}
			if (moduleName == currentModuleName)
			{
				return true;
			}
		}

		return false;
	}

	/*
	Returns true if <pid> contains at least one of the module of <moduleNames>

	throws if process not found or function failure
	*/
	bool processContainsAnyModulesA(DWORD pid, std::initializer_list<STRING8> moduleNames)
	{
		for (auto& mod : moduleNames)
		{
			if (processContainsModuleA(pid, mod))
				return true;
		}
		return false;
	}

	/*
	Returns true if <pid> contains at least one of the module of <moduleNames>

	throws if process not found or function failure
	*/
	bool processContainsAnyModulesW(DWORD pid, std::initializer_list<WSTRING> moduleNames)
	{
		for (auto& mod : moduleNames)
		{
			if (processContainsModuleW(pid, mod))
				return true;
		}
		return false;
	}
}
