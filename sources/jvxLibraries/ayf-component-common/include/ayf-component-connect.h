#ifndef _AYF_WEAVER_CONNECT_H__
#define _AYF_WEAVER_CONNECT_H__

#ifndef AYF_PROJECT_POSTFIX
#error "No entry defined for postfix in AYF_PROJECT_POSTFIX"
#endif

#define AYF_FUNCTIONNAME_REDEFINE_HELPER(prefix, postfix) prefix##_##postfix
#define AYF_FUNCTIONNAME_REDEFINE(prefix, postfix) AYF_FUNCTIONNAME_REDEFINE_HELPER(prefix, postfix)

#define AYF_MODULE_NAME_HELPER(prefix, middle, postfix) prefix ## middle ## postfix
#define AYF_MODULE_NAME(prefix, middle, postfix) AYF_MODULE_NAME_HELPER(prefix, middle, postfix)

#define AYF_MASK_NAME_HELPER(middle, var) MASK_Binaurics ## middle ## _ ## var
#define AYF_MASK_NAME(middle, var) AYF_MASK_NAME_HELPER(middle, var)

// AYF_FUNCTIONNAME_REDEFINE_HELPER(middle, var)

#include "jvx_system.h"
#include "ayf_component_lib_types.h"

#ifdef __cplusplus
extern "C" {
#endif

	// void AYF_FUNCTIONNAME_REDEFINE(ayf_aw_preinit, AYF_PROJECT_POSTFIX)();
	// void AYF_FUNCTIONNAME_REDEFINE(ayf_aw_preterminate, AYF_PROJECT_POSTFIX)();
	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_preInitModule, AYF_PROJECT_POSTFIX)(
		jvxHandle** instance, int numInChans, int numOutChans, int bSize, int sRate, ayfBufferInterleaveType ilTp,
		int passthroughMode, jvxHandle* ptrConstructorFunction, int* ayfIdentsPtr, int ayfIdentsNum,
		void_pvoid_callback fptr, void* priv);

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_delayedInitModule, AYF_PROJECT_POSTFIX)(jvxHandle** instance, jvxHandle** preinstance, const char* fNameIniPtr, const char* fNameDllProxy);

	/**
	 * Init the connected AYF module. The arguments act as follows:
	 * void** instance:  
	 * int numInChans, int numOutChans, int bSize, int sRate		: Audio parameter for signal processing chaining
	 * ayfBufferInterleaveType ilBufferTp							: Interleaved or non-interleaved
	 * int passthroughMode, 
	 * jvxHandle* ptrConstructorFunction, 
	 * int* ayfIdentsPtr, int ayfIdentsNum, 
	 * const char* fNameIniPtr, const char* fNameDllProxy			: Options for embedding proxy: the name of the inin file and the name of the dll for embedding_proxy
	 * void_pvoid_callback, void* priv								: Multipurpose callback. May be called in different situations
	 
	 */
	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_initModule, AYF_PROJECT_POSTFIX)(
		void** instance, int numInChans, 
		int numOutChans, int bSize, int sRate,
		ayfBufferInterleaveType ilBufferTp,
		int passthroughMode, jvxHandle* ptrConstructorFunction, 
		int* ayfIdentsPtr, int ayfIdentsNum, 
		const char* fNameIniPtr, const char* fNameDllProxy,
		void_pvoid_callback, void* priv);

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_termModule, AYF_PROJECT_POSTFIX)(void* instance);
	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_process_il, AYF_PROJECT_POSTFIX)(
		jvxHandle* instance, jvxData* inInterleaved, 
		int numSamplesIn, int numChannelsIn, jvxData* outInterleaved, 
		int numSamlesOut, int numChannelsOut);

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_process_nil, AYF_PROJECT_POSTFIX)(
		jvxHandle* instance, jvxData** inInterleaved,
		int numSamplesIn, int numChannelsIn, jvxData** outInterleaved,
		int numSamlesOut, int numChannelsOut);

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_reportTxtInput, AYF_PROJECT_POSTFIX)(jvxHandle* instance, const char* txtFldToken);
	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_reportTxtStatus, AYF_PROJECT_POSTFIX)(jvxHandle* instance, int nStat, char* fldRespond, int szRespond, int* newStatOnReturn);

	//jvxErrorType ayf_aw_requestTxtOutput(jvxHandle* instance, char* txtFldToken, int sz);
	//jvxErrorType ayf_aw_requestTxtStatus(jvxHandle* instance, int* nStat);
	
#ifdef __cplusplus
}
#endif

#endif
