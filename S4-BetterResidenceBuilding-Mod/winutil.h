#pragma once
// winutil from unreleased OBS SPA plugin
#include <iostream>
#include <sstream>
#include <string_view>
#include <string>
#include <vector>
#include <map>
#include <variant>

#include <Windows.h>
#include <TlHelp32.h>

typedef std::wstring WSTRING;
typedef std::string STRING8; // prevent collision with typedef STRING _STRING

typedef std::wstring_view WSTRINGVIEW;
typedef std::string_view STRINGVIEW;

typedef std::wstringstream WSSTREAM;
typedef std::stringstream SSTREAM;

typedef std::wistringstream WISSTREAM;
typedef std::istringstream ISSTREAM;


#define WINDOWS_UTIL_UFS(function,suffix) function##suffix
#ifdef UNICODE
#define WINDOWS_UTIL_UFAW_OBJ(tname,aname,wname) static auto& tname = wname;
#define WINDOWS_UTIL_UFAW_FUNC(tname,aname,wname) static const constexpr auto& tname = wname;
#define WINDOWS_UTIL_TFUNC(funcname) static const constexpr auto& funcname = WINDOWS_UTIL_UFS(funcname,W);

typedef WSSTREAM TSSTREAM;
typedef WISSTREAM TISSTREAM;
typedef WSTRINGVIEW TSTRINGVIEW;
typedef WSTRING TSTRING;

#define tsplitpath_s _wsplitpath_s // overloaded function :(
#else
#define WINDOWS_UTIL_UFAW_OBJ(tname,aname,wname) static auto& tname = aname;
#define WINDOWS_UTIL_UFAW_FUNC(tname,aname,wname) static const constexpr auto& tname = aname;
#define WINDOWS_UTIL_TFUNC(funcname) static const constexpr auto& funcname = WINDOWS_UTIL_UFS(funcname,A);

typedef SSTREAM TSSTREAM;
typedef ISSTREAM TISSTREAM;
typedef STRINGVIEW TSTRINGVIEW;
typedef STRING8 TSTRING;

#define tsplitpath_s _splitpath_s // overloaded function :(
#endif

namespace Winutil {
	WINDOWS_UTIL_UFAW_OBJ(tcout, std::cout, std::wcout)
		WINDOWS_UTIL_UFAW_OBJ(tcin, std::cin, std::wcin)
		WINDOWS_UTIL_UFAW_FUNC(tstrcmp, strcmp, wcscmp)

		/* Wrapper class for HANDLE with utility functions */
		class CHandle {

		bool m_canbeinherited = false;
		bool m_protectedfromclose = false;

		void updateFlags() {
			DWORD flags = 0;
			GetHandleInformation(m_handle, &flags);
		}
		protected:
			HANDLE m_handle = nullptr;
		public:
			CHandle() : m_handle(nullptr) {	}
			CHandle(HANDLE h) : m_handle(h) {
				updateFlags();
			}
			virtual ~CHandle() {}
			HANDLE* operator&() { return &m_handle; }

			operator HANDLE() const {
				return m_handle;
			}
			CHandle& operator=(HANDLE h) {
				m_handle = h;
				updateFlags();
				return *this;
			}

			/* Checks if handle is a valid handle */
			bool isValid() { // 0 or -1(invalid_handle_value) are not valid
				return (m_handle != nullptr &&  // example: on CreateToolhelp32Snapshot
					m_handle != INVALID_HANDLE_VALUE); // example: on CreateFile
			}

			/* Closes the Handle */
			void close() {
				if (isValid())
					CloseHandle(m_handle);
			}

			bool canBeInherited() {
				return m_canbeinherited;
			}
			bool protectedFromClose() {
				return m_protectedfromclose;
			}
			void canBeInherited(bool b) {
				if (!isValid())
					return;

				m_canbeinherited = b;
				SetHandleInformation(m_handle, (b ? 1 : 0) << (HANDLE_FLAG_INHERIT - 1), (b ? 1 : 0) << (HANDLE_FLAG_INHERIT - 1));
			}
			void protectedFromClose(bool b) {
				if (!isValid())
					return;

				m_protectedfromclose = b;
				SetHandleInformation(m_handle, (b ? 1 : 0) << (HANDLE_FLAG_PROTECT_FROM_CLOSE - 1), (b ? 1 : 0) << (HANDLE_FLAG_PROTECT_FROM_CLOSE - 1));
			}
	};
	/* A HANDLE wrapper to automatically close handle if out of scope if handle is valid handle. similar to std::lock_guard

		NON COPYABLE
	*/
	class CHandleGuard : public CHandle {
		bool m_canClose = false;
	public:
		// INITIATION CONSTRUCTOR
		CHandleGuard() : CHandle() {
			m_canClose = false;
		}
		CHandleGuard(HANDLE h) : CHandle(h) {
			m_canClose = isValid();
		}
		// DELETED COPY CONSTRUCTOR/ASSIGNMENTS
		CHandleGuard(const CHandleGuard&) = delete;
		CHandleGuard& operator=(HANDLE h) {
			if (m_canClose)
				CloseHandle(m_handle);
			m_handle = h;
			m_canClose = isValid();
			return *this;
		}

		// MOVE CONSTRUCTOR
		CHandleGuard(CHandleGuard&& g) = default;

		virtual ~CHandleGuard() {
			if (m_canClose)
				CloseHandle(m_handle);
		}
	};

	static const constexpr DWORD PID_NOT_FOUND = -1;

	STRING8 GetLastErrorAsStringA();
	WSTRING GetLastErrorAsStringW();
	STRING8 GetErrorAsStringA(DWORD err);
	WSTRING GetErrorAsStringW(DWORD err);
	/*
		Returns the first process ID found of the given process executable name
				if none found, returns PID_NOT_FOUND
	*/
	DWORD getFirstProcessIDByExeW(WSTRING exe);
	/*
		Returns the first process ID found of the given process executable name
				if none found, returns PID_NOT_FOUND
		NOTE: It is recommended to use the WSTRING version since this version was only kept for old models.
	*/
	DWORD getFirstProcessIDByExeA(STRING8 exe);
	/*
		Returns the process executable name + extension on success

		On error returns ""
	*/
	STRING8 getExeByProcessIDA(DWORD pid);
	/*
		Returns the process executable name + extension on success

		On error returns L""
	*/
	WSTRING getExeByProcessIDW(DWORD pid);

	BOOL isMainWindow(HWND handle);
	HWND findMainWindow(DWORD process_id);
	/*
		Returns TRUE if the given process id's running process is running under 64-bit architecture

			\process arch	|
			pc-arch\	x64	|	x32
			--------|-------|-------
				x64 |	TRUE|	X
			--------|-------|--------
				x32 |  FALSE|	FALSE
						^- runs under WOW64 however, will still return false

		Can throw std::exception if bad process id or no rights to open process
	*/
	BOOL processIs64Bit(DWORD pid);

	enum class MessageBoxTypes : UINT {
		DEFAULT = 0,
		/* The message box contains one push button: OK. This is the default. */
		OK = MB_OK,
		/* The message box contains two push buttons: OK and Cancel. */
		OKCANCEL = MB_OKCANCEL,
		/* Adds a Help button to the message box. When the user clicks the Help button or presses F1, the system sends a WM_HELP message to the owner. */
		HELP = MB_HELP,
		/* The message box contains two push buttons: Retry and Cancel. */
		RETRYCANCEL = MB_RETRYCANCEL,
		/* The message box contains two push buttons: Yes and No. */
		YESNO = MB_YESNO,
		/* The message box contains three push buttons: Yes, No, and Cancel. */
		YESNOCANCEL = MB_YESNOCANCEL,
		/* The message box contains three push buttons: Abort, Retry, and Ignore. */
		ABORTRETRYIGNORE = MB_ABORTRETRYIGNORE,
		/* The message box contains three push buttons: Cancel, Try Again, Continue. Use this message box type instead of MB_ABORTRETRYIGNORE. */
		CANCELTRYCONTINUE = MB_CANCELTRYCONTINUE
	};
#ifdef ERROR
#define TMPVAR_347890nm2fffnda ERROR
#undef ERROR
#endif
	/* *I do not agree to some of these descriptions. see https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-messagebox in case of correction */
	enum class MessageBoxIcons : UINT {
		DEFAULT = 0,
		/*  An exclamation-point icon appears in the message box. */
		EXCLAMATION = MB_ICONEXCLAMATION,
		/*  An exclamation-point icon appears in the message box. */
		WARNING = MB_ICONWARNING,
		/*  An icon consisting of a lowercase letter i in a circle appears in the message box. */
		INFORMATION = MB_ICONINFORMATION,
		/*  An icon consisting of a lowercase letter i in a circle appears in the message box. */
		ASTERISK = MB_ICONASTERISK,
		/* A question-mark icon appears in the message box. The question-mark message icon is no longer recommended because it

		does not clearly represent a specific type of message and because the phrasing of a message as a question could apply to any message type.

		In addition, users can confuse the message symbol question mark with Help information. Therefore, do not use this question mark message

		symbol in your message boxes. The system continues to support its inclusion only for backward compatibility. */
		QUESTION = MB_ICONQUESTION,
		/* A stop-sign icon appears in the message box. */
		STOP = MB_ICONSTOP,
		/* A stop-sign icon appears in the message box. */
		ERROR = MB_ICONERROR,
		/* A stop-sign icon appears in the message box. */
		HAND = MB_ICONHAND
	};
	enum class MessageBoxDefaultButtons : UINT {
		DEFAULT = 0,
		/*The first button is the default button.

		MB_DEFBUTTON1 is the default unless MB_DEFBUTTON2, MB_DEFBUTTON3, or MB_DEFBUTTON4 is specified.*/
		DEFBUTTON1 = MB_DEFBUTTON1,
		/* The second button is the default button. */
		DEFBUTTON2 = MB_DEFBUTTON2,
		/* The third  button is the default button. */
		DEFBUTTON3 = MB_DEFBUTTON3,
		/* The fourth  button is the default button. */
		DEFBUTTON4 = MB_DEFBUTTON4,
	};
	enum class MessageBoxModalty : UINT {
		DEFAULT = 0,
		/*The user must respond to the message box before continuing work in the window identified by the hWnd parameter. However, the user can move to the windows of other threads and work in those windows.

		Depending on the hierarchy of windows in the application, the user may be able to move to other windows within the thread. All child windows of the parent of the message box are automatically disabled, but pop-up windows are not.

		MB_APPLMODAL is the default if neither MB_SYSTEMMODAL nor MB_TASKMODAL is specified.*/
		APPL = MB_APPLMODAL,
		/* Same as APPL except that the message box has the WS_EX_TOPMOST style.

		Use system-modal message boxes to notify the user of serious,

		potentially damaging errors that require immediate attention (for example, running out of memory).

		This flag has no effect on the user's ability to interact with windows other than those associated with hWnd*/
		SYSTEM = MB_SYSTEMMODAL,
		/*Same as MB_APPLMODAL except that all the top-level windows belonging to the current thread are disabled if the hWnd parameter is NULL.

		Use this flag when the calling application or library does not have a window handle available but still needs to prevent input to other windows

		in the calling thread without suspending other threads. */
		TASK = MB_TASKMODAL
	};
	enum class MessageBoxOther : UINT {
		DEFAULT = 0,
		/*Same as desktop of the interactive window station. For more information, see Window Stations.

		If the current input desktop is not the default desktop, MessageBox does not return until the user switches to the default desktop.*/
		DEFAULT_DESKTOP_ONLY = MB_DEFAULT_DESKTOP_ONLY,
		/* The text is right-justified. */
		RIGHT = MB_RIGHT,
		/* Displays message and caption text using right-to-left reading order on Hebrew and Arabic systems */
		RTLREADING = MB_RTLREADING,
		/* The message box becomes the foreground window. Internally, the system calls the SetForegroundWindow function for the message box. */
		SETFOREGROUND = MB_SETFOREGROUND,
		/* The message box is created with the WS_EX_TOPMOST window style. */
		TOPMOST = MB_TOPMOST,
		/*The caller is a service notifying the user of an event. The function displays a message box on the current active desktop, even if there is no user logged on to the computer.

		Terminal Services: If the calling thread has an impersonation token, the function directs the message box to the session specified in the impersonation token.

		If this flag is set, the hWnd parameter must be NULL. This is so that the message box can appear on a desktop other than the desktop corresponding to the hWnd.

		For information on security considerations in regard to using this flag, see Interactive Services. In particular, be aware that this flag can produce interactive content on a locked desktop and should therefore be used for only a very limited set of scenarios, such as resource exhaustion.*/
		SERVICE_NOTIFICATION = MB_SERVICE_NOTIFICATION
	};
#ifdef TMPVAR_347890nm2fffnda
#define ERROR TMPVAR_347890nm2fffnda
#undef TMPVAR_347890nm2fffnda
#endif
	typedef std::variant<MessageBoxOther, std::initializer_list<MessageBoxOther>> MessageBoxOtherSingleOrList; // it could have also been done with or'ing all MBOthers but this is as clear as it gets 
	/*

		Returns:
		If a message box has a Cancel button, the function returns the IDCANCEL value if either the ESC key is pressed or the Cancel button is selected. If the message box has no Cancel button, pressing ESC has no effect.

		If the function fails, the return value is zero. To get extended error information, call GetLastError.

		If the function succeeds, the return value is one of the following menu-item values.
	*/
	int messageBoxA(const STRING8& message, const STRING8& title,
		HWND window = nullptr,
		MessageBoxTypes type = MessageBoxTypes::DEFAULT,
		MessageBoxIcons mbi = MessageBoxIcons::DEFAULT,
		MessageBoxDefaultButtons defaultbuttons = MessageBoxDefaultButtons::DEFAULT,
		MessageBoxModalty modalty = MessageBoxModalty::DEFAULT,
		MessageBoxOtherSingleOrList others = MessageBoxOther::DEFAULT
	);
	/*

		Returns:
		If a message box has a Cancel button, the function returns the IDCANCEL value if either the ESC key is pressed or the Cancel button is selected. If the message box has no Cancel button, pressing ESC has no effect.

		If the function fails, the return value is zero. To get extended error information, call GetLastError.

		If the function succeeds, the return value is one of the following menu-item values.
	*/
	int messageBoxW(const WSTRING& message, const WSTRING& title,
		HWND window = nullptr,
		MessageBoxTypes type = MessageBoxTypes::DEFAULT,
		MessageBoxIcons mbi = MessageBoxIcons::DEFAULT,
		MessageBoxDefaultButtons defaultbuttons = MessageBoxDefaultButtons::DEFAULT,
		MessageBoxModalty modalty = MessageBoxModalty::DEFAULT,
		MessageBoxOtherSingleOrList others = MessageBoxOther::DEFAULT
	);
	//HANDLE OpenLivingProcess(DWORD desiredAccess, BOOL inheritHandle,DWORD pid) {
	//	OpenProcess(desiredAccess, inheritHandle, pid);
	//}
	/*
		Returns true if <pid> contains module with name <moduleName> (e.g. "HookTarget32.dll")

		throws if process not found or function failure
	*/
	bool processContainsModuleA(DWORD pid, STRING8 moduleName);
	/*
		Returns true if <pid> contains module with name <moduleName> (e.g. "HookTarget32.dll")

		throws if process not found or function failure
	*/
	bool processContainsModuleW(DWORD pid, WSTRING moduleName);
	/*
		Returns true if <pid> contains module with name <moduleName> (e.g. "HookTarget32.dll")

		throws if process not found or function failure
	*/
	bool processContainsAnyModulesA(DWORD pid, std::initializer_list<STRING8> moduleNames);
	/*
		Returns true if <pid> contains module with name <moduleName> (e.g. "HookTarget32.dll")

		throws if process not found or function failure
	*/
	bool processContainsAnyModulesW(DWORD pid, std::initializer_list<WSTRING> moduleNames);

	/* Functions with STRING8 or WSTRING suffixes, generalized to current compiler character option */
	WINDOWS_UTIL_TFUNC(messageBox)
		WINDOWS_UTIL_TFUNC(GetLastErrorAsString)
		WINDOWS_UTIL_TFUNC(GetErrorAsString)
		WINDOWS_UTIL_TFUNC(getExeByProcessID)
		WINDOWS_UTIL_TFUNC(getFirstProcessIDByExe)
		WINDOWS_UTIL_TFUNC(processContainsModule)
		WINDOWS_UTIL_TFUNC(processContainsAnyModules)


		class CWindowsUtilException : public std::exception {
		DWORD m_errCode = 0;
		TSTRING m_errStr;
		TSTRING m_whatErrStr;
		public:
			CWindowsUtilException(const char* what) : std::exception(what), m_errCode(GetLastError()), m_errStr(GetLastErrorAsString()) {
				TSSTREAM ss;
				ss << std::exception::what() << " " << errorString() << std::endl;
				m_whatErrStr = ss.str();
			}

			/*
				Returns getlasterror code
			*/
			DWORD error() const {
				return m_errCode;
			}

			/*
				Returns getlasterror string
			*/
			TSTRING errorString() const {
				return m_errStr;
			}

			/*
				Returns what() + getlasterror string
			*/
			TSTRING whatAndError() const {
				return m_whatErrStr;
			}

			virtual ~CWindowsUtilException() {

			}
	};
	/*
		If a process (e.g. via PID) could not be found, use this exception
	*/
	class CProcessNotFoundException : public CWindowsUtilException {

	public:
		CProcessNotFoundException(const char* what) : CWindowsUtilException(what) {

		}
		virtual ~CProcessNotFoundException() {

		}
	};
	/*
		If a winapi function that returns a BOOL on if it fails or not returns FALSE, this exception should be thrown
	*/
	class CWinapiFunctionFailure : public CWindowsUtilException {

	public:
		CWinapiFunctionFailure(const char* what) : CWindowsUtilException(what) {

		}
		virtual ~CWinapiFunctionFailure() {

		}
	};
}
