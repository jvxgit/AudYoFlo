/*
 *****************************************************
 * Filename: rtp_platform.h
 *****************************************************
 * Project: RTProc-ESP (Echtzeit-Software-Plattform) *
 *****************************************************
 * Description:                                      *
 *****************************************************
 * Developed by JAVOX SOLUTIONS GMBH, 2012           *
 *****************************************************
 * COPYRIGHT BY JAVOX SOLUTION GMBH                  *
 *****************************************************
 * Contact: rtproc@javox-solutions.com               *
 *****************************************************
*/

/*
 *  platform.h
 *
 */

#ifndef __JVX_PLATFORM_H___LINUX__
#define __JVX_PLATFORM_H___LINUX__

#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <malloc.h>
#include <alloca.h>
#include <netinet/tcp.h>
#include <sys/poll.h>
#include <errno.h>
#include <dirent.h>
#include <cstring>
#include <string>
#include <dlfcn.h>
#include <pthread.h>
#include <vector>
#include <list>
#include <stdlib.h> // for getenv() under linux
#include <limits.h>
#include <float.h>
#include <inttypes.h>
#include <cmath>
#include <sys/utsname.h>
#include <fnmatch.h>
#include <termios.h>
#include <signal.h>
#include <memory>
#include <sys/wait.h>
#include <spawn.h>

// Make sure that threads behave as desired
#define JVX_DEBUG_THREADS

#define OUTPUT_DEBUG

//#define OS_SUPPORTS_RS232
//#define OS_SUPPORTS_BTH

#define JVX_RT_THREAD_CREATION
#define JVX_MY_STACK_SIZE (1024*1024)      /* 100 kB is enough for now. */

//==============================================================
// Datatype definitions
//==============================================================
#include "jvx_system.h"

#include "../jvx_platform_common.h"

// Some additional datatypes (timeval)
#define JVX_TIME_USEC suseconds_t
#define JVX_TIME_SEC time_t

#define JVX_CONVERT_BITFIELD_STRING_FUNC strtoull

#define JVX_SYSTEM_DESCRIPTOR_PLATFORM JVX_SYSTEM_LINUX

// Define the interface type, in Mac, interface keyword is unknown
#define JVX_INTERFACE class

#define JVX_ISINF(value) (std::isinf(value) == 1)

#define JVX_ISNEGINF(value) (std::isinf(value) == -1)

/*
#if (__cplusplus < 201703L)
#define byte char
#endif
*/

#define JVX_DLL_EXPORT __attribute__((visibility("default")))

//! Define the overall calling conventions for all C++ functions!
#define JVX_CALLINGCONVENTION

#define JVX_HMODULE void*

#define JVX_HMODULE_INVALID NULL

#define JVX_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE

//===============================================
//===============================================

#define JVX_INVALID_FILE_DESCRIPTOR_VALUE 0
#define JVX_FILE_DESCRIPTOR_HANDLE int

#define TCHAR char

#define MAX_PATH 1024

#define JVX_GETCURRENTDIRECTORY(a, b) getcwd(a, b)

#define JVX_SEPARATOR_DIR "/"
#define JVX_SEPARATOR_DIR_THE_OTHER "\\"

#define JVX_SEPARATOR_DIR_CHAR '/'
#define JVX_SEPARATOR_DIR_CHAR_THE_OTHER '\\'

#define JVX_COMPONENT_DIR "jvxComponents"

#define JVX_DIR_NAME(fWithPath) jvx_pathExprFromFilePath(fWithPath, JVX_SEPARATOR_DIR_CHAR)

#define JVX_CONSOLE_CLEAR_SCREEN system("clear")

#define JVX_UNBUFFERED_CHAR_STATE struct termios _jvx_unbuf_old, _jvx_unbuf_new;
#define JVX_PREPARE_UNBUFFERED_CHAR \
	tcgetattr(STDIN_FILENO,&_jvx_unbuf_old);\
	_jvx_unbuf_new=_jvx_unbuf_old; \
	_jvx_unbuf_new.c_lflag &=(~ICANON & ~ECHO); \
	tcsetattr(STDIN_FILENO,TCSANOW,&_jvx_unbuf_new);
	
#define JVX_GET_UNBUFFERED_CHAR getchar
#define JVX_POSTPROCESS_UNBUFFERED_CHAR tcsetattr(STDIN_FILENO,TCSANOW,&_jvx_unbuf_old);
 
//===============================================
// Everything for network stuff
//===============================================

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cassert>
#define JVX_SOCKET int
#define JVX_CLOSE_SOCKET close
#define INVALID_SOCKET -1
#define JVX_INVALID_SOCKET_HANDLE -1
#define SOCKET_ERROR   -1
//#define JVX_SOCKET_LEN socklen_t <- setup in Windows platform.h
#define JVX_SOCKADDR sockaddr
#define JVX_SOCKADDR_IN sockaddr_in

#define JVX_POLL_SOCKET_STRUCT struct pollfd
#define JVX_POLL_SOCKET poll
#define JVX_POLL_SOCKET_INF(arg1, arg2) poll(arg1, arg2, -1)
#define JVX_POLL_SOCKET_RDNORM POLLIN
#define JVX_POLL_SOCKET_ERR POLLERR
#define JVX_POLL_SOCKET_HUP POLLHUP
#define JVX_POLL_SOCKET_NVAL POLLNVAL

#define JVX_SHUTDOWN_SOCKET shutdown

#define JVX_SOCKET_SD_RECEIVE 0
#define JVX_SOCKET_SD_SEND 1
#define JVX_SOCKET_SD_BOTH 2

#define JVX_SOCKET_REVENT short

#define JVX_SOCKET_POLL_INIT_DEFAULT POLLIN

#define JVX_SOCKET_NUM_BYTES u_long
#define JVX_IOCTRL_SOCKET_FIONREAD(sock, ptrRead) ioctl(sock, FIONREAD, ptrRead)

JVX_STATIC_INLINE void JVX_INIT_LOCALE(const char* newL)
{
  // char *oldLocale = setlocale(LC_NUMERIC, NULL);
  char *locale = setlocale(LC_NUMERIC, newL);
  std::cout << "Set LC_NUMERIC to " << locale << std::endl;
}

#define JVX_LOADLIBRARYA(a) dlopen(a, RTLD_LAZY)

JVX_STATIC_INLINE void* JVX_LOADLIBRARYA_PATH(std::string fName, std::string dir)
{
	if((dir == ""))
	{
		return(dlopen(fName.c_str(), RTLD_LAZY));
	}
	return(dlopen((dir+ JVX_SEPARATOR_DIR +fName).c_str(), RTLD_LAZY));
}

#define JVX_LOADLIBRARY(a) dlopen(a, RTLD_LAZY)

JVX_STATIC_INLINE void* JVX_LOADLIBRARY_PATHVEC(std::string fName, std::vector<std::string> PathVec)
{
	for (size_t i = 0; i < PathVec.size(); i++) {
		if((PathVec[i] != ""))
		{
			std::string nmLib = PathVec[i]+ JVX_SEPARATOR_DIR +fName;
#ifdef OUTPUT_DEBUG
			std::cout << "Trying to open library " << nmLib << std::endl;
#endif
			JVX_HMODULE dll_loaded = dlopen((nmLib).c_str(), RTLD_LAZY);
			if ( dll_loaded )
			{
				return( dll_loaded );
			}
			else
			{
#ifdef OUTPUT_DEBUG
				std::cout << "Error trying to open so: " << dlerror() << std::endl;
#endif
			}
		}
	}
	return(dlopen(fName.c_str(), RTLD_LAZY));
}

JVX_STATIC_INLINE void* JVX_LOADLIBRARY_PATH(std::string fName, std::string dir)
{
	if((dir == ""))
	{
		return(dlopen(fName.c_str(), RTLD_LAZY));
	}
	return(dlopen((dir+ JVX_SEPARATOR_DIR +fName).c_str(), RTLD_LAZY));
}

#define JVX_GETPROCADDRESS(a, b) dlsym(a,b)

#define JVX_UNLOADLIBRARY(a) dlclose(a)

#define JVX_CREATE_PROCESS_RESULT int

#define JVX_CREATE_PROCESS_HANDLE pid_t
JVX_CREATE_PROCESS_RESULT JVX_CREATE_PROCESS_WITH_ARGS(JVX_CREATE_PROCESS_HANDLE& procHandle, const std::string& cmd, const std::list<std::string>& args); 

#define JVX_TERMINATE_PROCESS(procHandle, exitCode) kill(procHandle, SIGKILL)
JVX_STATIC_INLINE JVX_WAIT_RESULT JVX_WAIT_FOR_PROCESS_COMPLETE(JVX_CREATE_PROCESS_HANDLE hdlProc) 
{
	int status = 0;
	
	// Wait for process to complete
	return waitpid(hdlProc, &status, 0);
	
	// https://linux.die.net/man/2/waitid
	// WIFEXITED(status);
}

// Error indicated by errno error code
#define JVX_CREATE_PROCESS_FAILED 1 
#define JVX_CREATE_PROCESS_SUCCESS 0

#define JVX_HANDLE_ void*

#define INVALID_HANDLE_VALUE NULL


#define JVX_HANDLE_DIR DIR*

#define JVX_DIR_FIND_DATA struct dirent*

#define INIT_SEARCH_DIR_DEFAULT NULL

JVX_STATIC_INLINE JVX_HANDLE_DIR
JVX_FINDFIRSTFILE_WC(JVX_HANDLE_DIR dirH, const char* dirName, const char* fNameExt, JVX_DIR_FIND_DATA& fData)
{
	//std::cout << "Dir: " << dirName << std::endl;
	dirH = opendir (dirName);
	fData = NULL;
	if(dirH)
	{
		while(true)
		{
			fData = readdir (dirH);
			if(fData)
			{
				std::string str = fData->d_name;

				//std::cout << str << std::endl;
				size_t posStr = str.rfind(".");
				if(posStr != std::string::npos)
				{
					std::string ext = str.substr(posStr, std::string::npos);
					if(ext == fNameExt)
					{
						break;
					}
				}
                else
                {
                    // This rule for files without any extension
                    if((std::string)"" == fNameExt)
                    {
                        break;
                    }
                }

			}
			else
			{
				return NULL;
			}
		}
	}
	return(dirH);
}

JVX_STATIC_INLINE JVX_HANDLE_DIR
JVX_FINDFIRSTFILE_S(const char* dirName, const char* fName, JVX_DIR_FIND_DATA& fData)
{
	//std::cout << "Dir: " << dirName << std::endl;
	
        JVX_HANDLE_DIR dirH = opendir (dirName);
	fData = NULL;
	if(dirH)
	{
		while(true)
		{
			fData = readdir (dirH);
			if(!fData)
			{
				return NULL;
			}
			else
			{
				//int fnmatch(const char *pattern, const char *string, int flags);
				if(fnmatch(fName, fData->d_name, 0) == 0)
				//if(fData->d_name == (std::string)fName)
				{
					break;
				}
			}
		}
	}
	return(dirH);
}

/*
 * Important hint: always specify filename extension INCLUDING the ".", e.g. ".txt" to
 * list all *.txt files
 */
JVX_STATIC_INLINE bool
JVX_FINDNEXTFILE(DIR*& dirH, JVX_DIR_FIND_DATA& fData, const char* fNameExt)
{
	fData = NULL;

	bool found = false;
	while(!found)
	{
		if(dirH)
		{
			fData = readdir (dirH);
			if(fData)
			{
				std::string str = fData->d_name;

				// std::cout << __FUNCTION__ << ": Found file <" << str << ">." << std::endl;

				size_t posStr = str.rfind(".");
                if(posStr != std::string::npos)
                {
                    std::string ext = str.substr(posStr, std::string::npos);
				
		// std::cout << __FUNCTION__ << ": Comparing file extension <" << ext << "> vs <" << fNameExt << ">." << std::endl;
                 
		   if(ext == fNameExt)
                    {
                        found = true;
                        break;
                    }
                }
                else
                {
                    // This rule for files without any extension
                    if((std::string)"" == fNameExt)
                    {
                        found = true;
                        break;
                    }
                }
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return found;
}

#define JVX_GETFILENAMEDIR(searchResult) searchResult->d_name
#define JVX_TESTFILEATTRIBUTEDIRECTORY(searchHdl) ((searchHdl->d_type == DT_DIR) || (searchHdl->d_type == DT_LNK))

#define JVX_FINDCLOSE( a) closedir(a)

#define JVX_CHDIR(a) chdir(a)

#include <stdio.h>
#include <assert.h>

#define JVX_FUNCTION_RETURN_ADDRESS __builtin_extract_return_addr(__builtin_return_address(0))

std::string JVX_GET_CURRENT_MODULE_PATH(void* ptrIdentify);
std::string JVX_GET_USER_DESKTOP_PATH();
std::string JVX_FILENAME_FROM_FILE(FILE* hFile) ;

JVX_STATIC_INLINE std::string
JVX_DISPLAY_LAST_ERROR_LOAD(std::string fName)
{
	std::string errMess = strerror (errno);
	return errMess;
}

#define JVX_THREADID_SIZE(threadId) (jvxSize)threadId

#define JVX_DLL_EXTENSION ".so"

#define JVX_MEX_EXTENSION ".mexmaci64"

#define JVX_DYN_LIB_PATH_VARIABLE "LD_LIBRARY_PATH"

#define JVX_PASTE_ENV_VARIABLE_ATT(a, b, c) (std::string)a + SEPARATOR_DIR + c + ":" + b;

#define JVX_PASTE_ENV_VARIABLE_NEW(a,b) (std::string)a + JVX_SEPARATOR_DIR + b;

#define JVX_LOAD_DLL_DIR ((std::string)JVX_SEPARATOR_DIR "loadDLLs")

JVX_STATIC_INLINE std::string JVX_DATE()
{
  char cptime[50];
	// At August 14 2014, this function should output Aug_14_2014
	time_t now = time(NULL);
	strftime(cptime, 50, "%b/%d/%Y", localtime(&now)); //uses short month name
	std::string strTime = cptime;
	return strTime;
}

JVX_STATIC_INLINE std::string JVX_TIME()
{
  char cptime[50];
	// At 03:21:59, this function should return 03_21_59
	time_t now = time(NULL);
	strftime(cptime, 50, "%T", localtime(&now));
	std::string strTime = cptime;
	return strTime;
}

// Expression for "INFINITE" in maths
#define JVX_MATH_INFINITE INFINITY
//std::numeric_limits<jvxData>::infinity()

// Expression for "NOT A NUMBER" in maths
#define JVX_MATH_NAN NAN
//std::numeric_limits<jvxData>::quiet_NaN()

#define JVX_GUID void

#define JVX_FSEEK fseeko64

#define JVX_FTELL ftello64

#define JVX_SIGNAL_INSTALL signal
#define JVX_SIGNAL_TYPE __sighandler_t

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

static void show_new_pagefault_count(const char* logtext, 
   			      const char* allowed_maj,
   			      const char* allowed_min)
   {
   	static int last_majflt = 0, last_minflt = 0;
   	struct rusage usage;
   
   	getrusage(RUSAGE_SELF, &usage);
   
   	printf("%-30.30s: Pagefaults, Major:%ld (Allowed %s), " \
   	       "Minor:%ld (Allowed %s)\n", logtext,
   	       usage.ru_majflt - last_majflt, allowed_maj,
   	       usage.ru_minflt - last_minflt, allowed_min);
   	
   	last_majflt = usage.ru_majflt; 
   	last_minflt = usage.ru_minflt;
   }
   
   static void prove_thread_stack_use_is_safe(int stacksize)
   {
   	char* buffer = (char*)alloca(stacksize);
   	int i;
   
   	/* Prove that this thread is behaving well */
   	for (i = 0; i < stacksize; i += sysconf(_SC_PAGESIZE)) {
   		/* Each write to this buffer shall NOT generate a 
   			pagefault. */
   		buffer[i] = i;
   	}
   
   	show_new_pagefault_count("Caused by using thread stack", "0", "0");
   }

static void reserve_process_memory(int size)
   {
   	int i;
   	char *buffer;
   
   	buffer = (char*)malloc(size);
   
   	/* Touch each page in this piece of memory to get it mapped into RAM */
   	for (i = 0; i < size; i += sysconf(_SC_PAGESIZE)) {
   		/* Each write to this buffer will generate a pagefault.
   		   Once the pagefault is handled a page will be locked in
   		   memory and never given back to the system. */
   		buffer[i] = 0;
   	}
   
   	/* buffer will now be released. As Glibc is configured such that it 
   	   never gives back memory to the kernel, the memory allocated above is
   	   locked for this process. All malloc() and new() calls come from
   	   the memory pool reserved and locked above. Issuing free() and
   	   delete() does NOT make this locking undone. So, with this locking
   	   mechanism we can build C++ applications that will never run into
   	   a major/minor pagefault, even with swapping enabled. */
  	free(buffer);
  }

JVX_STATIC_INLINE void JVX_PREPARE_RT_START(jvxSize pre_allocation_size)
{
	std::cout << "Doing some stuff to prepare realtime processing" << std::endl;
	/*
	https://rt.wiki.kernel.org/index.php/Threaded_RT-application_with_memory_locking_and_stack_handling_example
	*/

	//show_new_pagefault_count("Initial count", ">=0", ">=0");

	// Prevent all page faults to become visible immediately in future thread
	if(mlockall(MCL_CURRENT | MCL_FUTURE) != 0)
	{
		std::cout << "mlockall failed!" << std::endl;
	}
	/* Turn off malloc trimming.*/
   	mallopt(M_TRIM_THRESHOLD, -1);
   
   	/* Turn off mmap usage. */
   	mallopt(M_MMAP_MAX, 0);

	//show_new_pagefault_count("mlockall() generated", ">=0", ">=0");

	if(pre_allocation_size)
	{
		reserve_process_memory(pre_allocation_size);
	}

	//show_new_pagefault_count("malloc() and touch generated", 
   	//			 ">=0", ">=0");
	
	/* Now allocate the memory for the 2nd time and prove the number of
   	   pagefaults are zero */
   	//reserve_process_memory(PRE_ALLOCATION_SIZE);
   	//show_new_pagefault_count("2nd malloc() and use generated", 
   	//			 "0", "0");
  }

JVX_STATIC_INLINE void JVX_PREPARE_RT_STOP()
{
	munlockall();
}
 
#endif
