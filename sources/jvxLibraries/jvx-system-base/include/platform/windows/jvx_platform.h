#ifndef __JVX_PLATFORM_H___WINDOWS__
#define __JVX_PLATFORM_H___WINDOWS__

// Pragmas for windows 
#pragma warning( disable : 4065)

#include <winsock2.h>
#include <ws2tcpip.h>
#include <ws2bth.h>
#include <afunix.h>
#include <windows.h>
#include <string>
#include <cmath>
#include <ctime>
#include <direct.h>
#include <vector>
#include <list>
#include <cassert>
#include <Setupapi.h>
#include <tlhelp32.h>
#include <conio.h>

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <inttypes.h>
#include <shlobj.h>

#include "jvx_system.h"
#include "../jvx_platform_common.h"

//==============================================================
// Datatype definitions
//==============================================================

// Some more specific datatypes (not just processing)
#define JVX_TIME_USEC long
#define JVX_TIME_SEC long

#define JVX_SYSTEM_DESCRIPTOR_PLATFORM JVX_SYSTEM_WINDOWS

#define JVX_ISINF(value) (value == std::numeric_limits<jvxData>::infinity())

#define JVX_ISNEGINF(value) (value == -std::numeric_limits<jvxData>::infinity())

#define JVX_ISNAN(value) ((value == std::numeric_limits<jvxData>::quiet_NaN()) || (value == std::numeric_limits<jvxData>::signaling_NaN()))

#define JVX_ISNEGNAN(value) ((value == -std::numeric_limits<jvxData>::quiet_NaN()) || (value == -std::numeric_limits<jvxData>::signaling_NaN()))

// Define the interface type, in Mac, interface keyword is unknown and reused by someone
#define JVX_INTERFACE interface

#define JVX_CONVERT_BITFIELD_STRING_FUNC _strtoui64
#define JVX_STRTOSIZE strtoumax

//===============================================
// Misc stuff
//===============================================
#define JVX_UNBUFFERED_CHAR_STATE
#define JVX_PREPARE_UNBUFFERED_CHAR
//#define JVX_GET_UNBUFFERED_CHAR _getch <- this function is buggy as of current version of Windows SDK (14.11.2018, HK)
#define JVX_GET_UNBUFFERED_CHAR _getwch
#define JVX_POSTPROCESS_UNBUFFERED_CHAR

// ===================================================

#define JVX_CHDIR(a) _chdir(a)

#define JVX_SEPARATOR_DIR "\\"
#define JVX_SEPARATOR_DIR_THE_OTHER "/"

#define JVX_SEPARATOR_DIR_CHAR '\\'
#define JVX_SEPARATOR_DIR_CHAR_THE_OTHER '/'

#define JVX_COMPONENT_DIR "rtpComponents"

#define JVX_RS232_HANDLE HANDLE

#define JVX_INIT_LOCALE(a)

#define JVX_CONSOLE_CLEAR_SCREEN system("cls")

//===============================================
//===============================================

#define JVX_INVALID_FILE_DESCRIPTOR_VALUE JVX_INVALID_HANDLE_VALUE
#define JVX_FILE_DESCRIPTOR_HANDLE HANDLE 

//===============================================
// Everything for access to dynamic link libraries
//===============================================
//
#define JVX_DLL_EXPORT __declspec(dllexport)

#define JVX_HMODULE HMODULE

#define JVX_HMODULE_INVALID nullptr

#define JVX_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE

#define JVX_LOADLIBRARYA(a) LoadLibraryA(a)

#define JVX_LOADLIBRARYA_PATH(a,b) LoadLibraryA(((std::string)b+ JVX_SEPARATOR_DIR + a).c_str())

#define JVX_LOADLIBRARY(a) LoadLibrary(a)

JVX_HMODULE JVX_LOADLIBRARY_PATHVEC(std::string fName, std::vector<std::string> PathVec);

JVX_HMODULE JVX_LOADLIBRARY_PATH(std::string fName, std::string dir);
//#define JVX_LOADLIBRARY_PATH(a,b) LoadLibraryA(((std::string)b+ JVX_SEPARATOR_DIR + a).c_str())

#define JVX_GETPROCADDRESS(hMod,nm) GetProcAddress(hMod, nm)

#define JVX_UNLOADLIBRARY(hMod) FreeLibrary(hMod)

//===============================================
// Everything for network stuff
//===============================================

// We use a specific socket definition here since pcap redefines socket as a uint
#define JVX_SOCKET UINT_PTR
// SOCKET

#define socklen_t int
#define JVX_CLOSE_SOCKET closesocket
#define JVX_SOCKADDR SOCKADDR
#define JVX_SOCKADDR_IN SOCKADDR_IN
#define JVX_INVALID_SOCKET_HANDLE  (SOCKET)(~0)

// Some defines for the new poll function
#define JVX_POLL_SOCKET_STRUCT WSAPOLLFD /* struct pollfd */
#define JVX_POLL_SOCKET_INF(arg1, arg2) WSAPoll(arg1, arg2, INFINITE) /* poll */
#define JVX_POLL_SOCKET WSAPoll /* poll */
#define JVX_POLL_SOCKET_RDNORM POLLRDNORM  /* POLLIN */
#define JVX_POLL_SOCKET_ERR POLLERR
#define JVX_POLL_SOCKET_HUP POLLHUP
#define JVX_POLL_SOCKET_NVAL POLLNVAL
#define JVX_SOCKET_POLL_INIT_DEFAULT (POLLIN)
#define JVX_SHUTDOWN_SOCKET shutdown

#define JVX_SOCKET_SD_RECEIVE SD_RECEIVE
#define JVX_SOCKET_SD_SEND SD_SEND
#define JVX_SOCKET_SD_BOTH SD_BOTH

#define JVX_SOCKET_REVENT SHORT

#define JVX_SOCKET_NUM_BYTES u_long
#define JVX_IOCTRL_SOCKET_FIONREAD(sock, ptrRead) ioctlsocket(sock, FIONREAD, ptrRead)

//===============================================
// Everything for directory/file access
//===============================================

#define JVX_GETCURRENTDIRECTORY(destBuf, szBytes) _getcwd(destBuf, szBytes)/*GetCurrentDirectory(b, a)*/

#define JVX_HANDLE_DIR HANDLE

#define JVX_DIR_FIND_DATAA WIN32_FIND_DATAA
#define JVX_FINDFIRSTFILEA(a,b1,b2,c) FindFirstFileA( ((std::string)b1 + JVX_SEPARATOR_DIR +"*" +b2).c_str(), &c )
#define JVX_FINDNEXTFILEA(a,b,c) FindNextFileA( a, &b )

#define JVX_DIR_FIND_DATA WIN32_FIND_DATA

#define INIT_SEARCH_DIR_DEFAULT {0}
#define JVX_FINDFIRSTFILE_WC(a,b1,b2,c) FindFirstFile( ((std::string)b1 + JVX_SEPARATOR_DIR +"*" +b2).c_str(), &c )
#define JVX_FINDFIRSTFILE_S(path, file, handle) FindFirstFile((path + std::string(JVX_SEPARATOR_DIR) + file).c_str(), &handle )
#define JVX_FINDNEXTFILE(a,b,c) FindNextFile( a, &b )

#define JVX_GETFILENAMEDIR(a) a.cFileName

#define JVX_FINDCLOSE(a) FindClose( a)

#define JVX_DIR_NAME(fWithPath) jvx_pathExprFromFilePath(fWithPath, JVX_SEPARATOR_DIR_CHAR)

//===============================================
// Everything for rs232 variables
//===============================================
#define JVX_HANDLE_PORT HANDLE

//===============================================
// Everything for environment variables
//===============================================

#define JVX_GETENVIRONMENTVARIABLE(a,b,c) GetEnvironmentVariableA(a, b, c)

#define JVX_SETENVIRONMENTVARIABLE(a, b, overwrite_not_in_mswin) SetEnvironmentVariableA(a, b)

#define JVX_PASTE_ENV_VARIABLE_ATT(a, b, c) (std::string)a + JVX_SEPARATOR_DIR + c + ";" + b;

#define JVX_PASTE_ENV_VARIABLE_ADD_PATH_FRONT(a, b) (std::string)b + ";" + a;

#define JVX_PASTE_ENV_VARIABLE_NEW(a,b) (std::string)a + JVX_SEPARATOR_DIR + b;


//! Define the overall calling conventions for all C++ functions!
#define JVX_CALLINGCONVENTION /*__stdcall*/

//===============================================
// Some important names
//===============================================

#define JVX_DLL_EXTENSION ".dll"

#define JVX_MEX_EXTENSION ".mexw32"

#define JVX_DYN_LIB_PATH_VARIABLE "PATH"

#define JVX_LOAD_DLL_DIR ((std::string)JVX_SEPARATOR_DIR "loadDLLs")

std::string JVX_TIME();
std::string JVX_DATE();

// Expression for "INFINITE" in maths
#define JVX_MATH_INFINITE std::numeric_limits<jvxData>::infinity()

// Expression for "NOT A NUMBER" in maths
#define JVX_MATH_NAN std::numeric_limits<jvxData>::quiet_NaN()

#define JVX_FSEEK _fseeki64

#define JVX_FTELL _ftelli64

#define JVX_GUID GUID

//! Define the GUID for access to the COMPORT, taken from somewhere in the internet
#ifndef GUID_CLASS_COMPORT_DEFINE_GUID
#define GUID_CLASS_COMPORT_DEFINE_GUID DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, \
	0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
#endif

#define JVX_PROCESS_HANDLE HANDLE

#define JVX_GET_CURRENT_PROCESS_HANDLE(hdl) hdl = GetCurrentProcess()

#define JVX_GET_PROCESS_AFFINITY_RESULT BOOL

#define JVX_GET_PROCESS_AFFINITY_SUCCESS TRUE

#define JVX_AFFINITY_MASK DWORD_PTR

#define JVX_GET_PROCESS_AFFINITY_MASK(res, hdl, procAff, sysAff) res = GetProcessAffinityMask(hdl, &procAff, &sysAff)

#define JVX_SET_PROCESS_AFFINITY_MASK(res, hdl, procAff) res = SetProcessAffinityMask(hdl, procAff)

#define JVX_GET_PROCESS_ID(id, hdl) id = GetProcessId(hdl)

#define JVX_PROCESS_ID DWORD

#define JVX_OPEN_PROCESS(hdl, id) hdl = OpenProcess( PROCESS_ALL_ACCESS, FALSE, id );

#define JVX_CLOSE_PROCESS(hdl) CloseHandle(hdl)

#define JVX_SIGNAL_INSTALL signal
#define JVX_SIGNAL_TYPE _crt_signal_t

#define JVX_FUNCTION_RETURN_ADDRESS _ReturnAddress()

std::string JVX_GET_CURRENT_MODULE_PATH(void* ptrIdentify);
std::string JVX_GET_USER_DESKTOP_PATH();
std::string JVX_GET_USER_APPDATA_PATH();
std::string JVX_DISPLAY_LAST_ERROR_LOAD(std::string fName);

#define JVX_THREADID_SIZE(threadId) (jvxSize)threadId
/*
#define JVX_GET_FUNCTION_RETURN_ADDRESS __builtin_extract_return_addr(__builtin_return_address(0))
JVX_GET_CURRENT_MODULE_PATH 

https://man7.org/linux/man-pages/man3/dladdr.3.html 

int dladdr(void *addr, Dl_info *info);
string get_path( )
{
		char arg1[20];
		char exepath[PATH_MAX + 1] = {0};

		sprintf( arg1, "/proc/%d/exe", getpid() );
		readlink( arg1, exepath, 1024 );
		return string( exepath );
}
*/

// Compile for MATLAB version MATLAB 2007 A plus
// 100: 2007A
// 200: 2008A
// 250: 2008b
// 300: 2009A
// 350: 2009B
// 400: 2010A
// 450: 2010B
// 500: 2011A
#define _MATLAB_MEXVERSION 500

#ifndef __func__
#define __func__ __FUNCTION__
#endif

//#ifndef isnan
//#define isnan(x) ((x) != (x))
//#endif

#endif
