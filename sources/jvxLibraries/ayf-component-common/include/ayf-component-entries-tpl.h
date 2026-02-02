#include "ayf-component-connect.h"

// ===============================================================================
// 
// Keep track of allocated instances
std::list<CayfComponentLibContainer*> AYF_FUNCTIONNAME_REDEFINE(lstAllocated, AYF_PROJECT_POSTFIX);

struct preInitModuleData
{
	int numInChans = 0;
	int numOutChans = 0;
	int bSize = 0;
	int sRate = 0;
	int passthroughMode = 0;
	jvxHandle* ptrConstructorFunction;
	int* ayfIdentsPtr = nullptr;
	int ayfIdentsNum = 0;
	void_pvoid_callback moduleCbPtr = nullptr;
	void* moduleCbPriv = nullptr;
	ayfBufferInterleaveType ilTp = ayfBufferInterleaveType::AYF_BUFFERS_INTERLEAVED;
};

extern "C"
{
#if 0
	// These two functions are forwarded from dllMain via InitInstance and ExitInstance
	void AYF_FUNCTIONNAME_REDEFINE(ayf_aw_preinit, AYF_PROJECT_POSTFIX)()
	{
		// This function is called when the module is first loaded, that is, at app start time!
		// Unfortunately, we are not allowed to hook up any loadLibrary here, due to
		// https://learn.microsoft.com/de-de/windows/win32/dlls/dynamic-link-library-best-practices
	}
	
	void ayf_aw_preterminate()
	{		

	}
#endif
	// ==================================================================================================
	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_preInitModule, AYF_PROJECT_POSTFIX)(
		jvxHandle** instance, int numInChans, int numOutChans, int bSize, int sRate, ayfBufferInterleaveType ilTp,
		int passthroughMode, jvxHandle* ptrConstructorFunction, int* ayfIdentsPtr, int ayfIdentsNum,
		void_pvoid_callback fptr, void* priv)
	{
		preInitModuleData* preInitDataReturn = nullptr;
		JVX_SAFE_ALLOCATE_OBJECT(preInitDataReturn, preInitModuleData);
		preInitDataReturn->numInChans = numInChans;
		preInitDataReturn->numOutChans = numOutChans;
		preInitDataReturn->bSize = bSize;
		preInitDataReturn->sRate = sRate;
		preInitDataReturn->passthroughMode = passthroughMode;
		preInitDataReturn->ptrConstructorFunction = ptrConstructorFunction;
		preInitDataReturn->moduleCbPtr = fptr;
		preInitDataReturn->moduleCbPriv = priv;
		preInitDataReturn->ilTp = ilTp;

		if (ayfIdentsNum)
		{
			preInitDataReturn->ayfIdentsNum = ayfIdentsNum;
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(preInitDataReturn->ayfIdentsPtr, int, preInitDataReturn->ayfIdentsNum);
			memcpy(preInitDataReturn->ayfIdentsPtr, ayfIdentsPtr, sizeof(int) * preInitDataReturn->ayfIdentsNum);
		}		
		*instance = reinterpret_cast<jvxHandle*>(preInitDataReturn);
		return JVX_NO_ERROR;

	}

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_initModule, AYF_PROJECT_POSTFIX)(
		jvxHandle** instance, int numInChans, int numOutChans, int bSize, int sRate, ayfBufferInterleaveType ilType,  
		int passthroughMode, jvxHandle* ptrConstructorFunction, int* ayfIdentsPtr, int ayfIdentsNum, 
		const char* fNameIniPtr, const char* fNameDllProxy,
		void_pvoid_callback fptr, void* priv)
	{
		jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
		if (instance)
		{
			std::string modName = AYF_BINDING_MODULE_NAME;
			
			ayfHostBindingReferences* bindOnReturn = nullptr;
			std::string alternativePath = JVX_GET_CURRENT_MODULE_PATH(ptrConstructorFunction);
			alternativePath = jvx_extractDirectoryFromFilePath(alternativePath);

			jvxErrorType resL = CayfComponentLib::populateBindingRefs(modName, alternativePath, bindOnReturn, fNameIniPtr, fNameDllProxy);
			if(resL == JVX_NO_ERROR)
			{
				if (bindOnReturn->bindType == ayfHostBindingType::AYF_HOST_BINDING_NONE)
				{
					if (fptr)
					{						
						fptr(ayfVoidPvoidDefinition::AYF_VOID_PVOID_ID_REPORT_NO_BINDING, priv, nullptr);
					}
				}
			}
			CayfComponentLibContainer* libCont = nullptr;
			JVX_SAFE_ALLOCATE_OBJECT(libCont, AYF_MODULE_CONTAINER_TYPE);
			libCont->linkBinding( bindOnReturn);

			libCont->startBinding(modName, numInChans, 
				numOutChans, bSize, sRate, passthroughMode,
				ayfIdentsPtr, ayfIdentsNum, fptr, priv);

			AYF_FUNCTIONNAME_REDEFINE(lstAllocated, AYF_PROJECT_POSTFIX).push_back(libCont);

			*instance = reinterpret_cast<jvxHandle*>(libCont);

			res = JVX_NO_ERROR;
		}
		return res;
	}

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_delayedInitModule, AYF_PROJECT_POSTFIX)(jvxHandle** instance, jvxHandle** preinstance, const char* fNameIniPtr, const char* fNameDllProxy)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (preinstance)
		{
			preInitModuleData* preInitDataReturn = (preInitModuleData*)*preinstance;

			jvxErrorType res = AYF_FUNCTIONNAME_REDEFINE(ayf_cc_initModule, AYF_PROJECT_POSTFIX)(instance, preInitDataReturn->numInChans, preInitDataReturn->numOutChans,
				preInitDataReturn->bSize, preInitDataReturn->sRate, preInitDataReturn->ilTp, 
				preInitDataReturn->passthroughMode, preInitDataReturn->ptrConstructorFunction, preInitDataReturn->ayfIdentsPtr, preInitDataReturn->ayfIdentsNum, 
				fNameIniPtr, fNameDllProxy,
				preInitDataReturn->moduleCbPtr, preInitDataReturn->moduleCbPriv);

			// Release the pre-allocated fields
			if (preInitDataReturn->ayfIdentsPtr)
			{
				JVX_SAFE_DELETE_FIELD(preInitDataReturn->ayfIdentsPtr);
			}
			JVX_SAFE_DELETE_OBJECT(preInitDataReturn);
			*preinstance = nullptr;
		}
		return res;
	}

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_termModule, AYF_PROJECT_POSTFIX)(jvxHandle* instance)
	{
		jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
		if (instance)
		{
			CayfComponentLibContainer* libCont = reinterpret_cast<CayfComponentLibContainer*>(instance);

			libCont->stopBinding();

			for (auto elm = AYF_FUNCTIONNAME_REDEFINE(lstAllocated, AYF_PROJECT_POSTFIX).begin(); elm != AYF_FUNCTIONNAME_REDEFINE(lstAllocated, AYF_PROJECT_POSTFIX).end(); elm++)
			{
				if (*elm == libCont)
				{
					AYF_FUNCTIONNAME_REDEFINE(lstAllocated, AYF_PROJECT_POSTFIX).erase(elm);
					break;
				}
			}
			JVX_SAFE_DELETE_OBJECT(libCont);

			// Remember: static call!!
			CayfComponentLib::unpopulateBindingRefs();
			res = JVX_NO_ERROR;
		}
		return res;
	}

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_process_il, AYF_PROJECT_POSTFIX)(jvxHandle* instance, jvxData* inInterleaved, int numSamplesIn, int numChannelsIn, jvxData* outInterleaved, int numSamlesOut, int numChannelsOut)
	{
		jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
		if (instance)
		{			
			CayfComponentLibContainer* libCont = reinterpret_cast<CayfComponentLibContainer*>(instance);
			libCont->process_one_buffer_interleaved(inInterleaved, numSamplesIn, numChannelsIn, outInterleaved, numSamlesOut, numChannelsOut);
		}
		return res;
	}

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_process_nil, AYF_PROJECT_POSTFIX)(jvxHandle* instance, jvxData** inInterleaved, int numSamplesIn, int numChannelsIn, jvxData** outInterleaved, int numSamlesOut, int numChannelsOut)
	{
		jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
		if (instance)
		{
			assert(false);
			/*
			CayfComponentLibContainer* libCont = reinterpret_cast<CayfComponentLibContainer*>(instance);
			libCont->process_one_buffer_interleaved(inInterleaved, numSamplesIn, numChannelsIn, outInterleaved, numSamlesOut, numChannelsOut);
			*/
		}
		return res;
	}

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_reportTxtInput, AYF_PROJECT_POSTFIX)(jvxHandle* instance, const char* txtFldToken)
	{
		jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
		if (instance)
		{
			CayfComponentLibContainer* libCont = reinterpret_cast<CayfComponentLibContainer*>(instance);
			std::string txtToken = txtFldToken;
			if (!txtToken.empty())
			{
				libCont->lock();
				CayfComponentLib* ptrRet = libCont->processorRef;
				res = JVX_ERROR_NOT_READY;
				if (ptrRet)
				{
					res = ptrRet->add_text_message_token(txtToken);
				}
				libCont->unlock();
			}
		}
		return res;
	}

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_reportTxtStatus, AYF_PROJECT_POSTFIX)(jvxHandle* instance, int nStat, char* fldRespond, int szRespond, int* newStatOnReturn)
	{
		jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
		if (instance)
		{
			CayfComponentLibContainer* libCont = reinterpret_cast<CayfComponentLibContainer*>(instance);
			
			libCont->lock();
			CayfComponentLib* ptrRet = libCont->processorRef;
			jvxSize szRespondSz = szRespond;
			res = JVX_ERROR_NOT_READY;
			if (ptrRet)
			{
				res = ptrRet->new_text_message_status(nStat, fldRespond, szRespondSz, newStatOnReturn);
			}
			libCont->unlock();
		}
		return res;
	}
}
