#include "ayf-component-connect.h"

// ===============================================================================
// 
// Keep track of allocated instances
std::list<CayfComponentLibContainer*> AYF_FUNCTIONNAME_REDEFINE(lstAllocated, AYF_PROJECT_POSTFIX);

struct preInitModuleData
{
	struct ayfInitParamStruct initParam;
	jvxHandle* ptrConstructorFunction;
	struct ayfInitConnectStruct initStr;
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
		jvxHandle** instance, struct ayfInitParamStruct* paramStr, jvxHandle* ptrConstructorFunction, struct ayfInitConnectStruct* str)
	{
		preInitModuleData* preInitDataReturn = nullptr;
		JVX_SAFE_ALLOCATE_OBJECT(preInitDataReturn, preInitModuleData);
		preInitDataReturn->initParam = *paramStr;

		preInitDataReturn->ptrConstructorFunction = ptrConstructorFunction;

		// Copy init data
		preInitDataReturn->initStr = *str;

		*instance = reinterpret_cast<jvxHandle*>(preInitDataReturn);
		return JVX_NO_ERROR;

	}

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_initModule, AYF_PROJECT_POSTFIX)(
		jvxHandle** instance, struct ayfInitParamStruct* paramStr, 
		jvxHandle* ptrConstructorFunction, struct ayfInitConnectStruct* str)
	{
		jvxErrorType res = JVX_ERROR_INVALID_ARGUMENT;
		if (instance)
		{
			std::string modName = AYF_BINDING_MODULE_NAME;
			
			ayfHostBindingReferences* bindOnReturn = nullptr;
			std::string alternativePath = JVX_GET_CURRENT_MODULE_PATH(ptrConstructorFunction);
			alternativePath = jvx_extractDirectoryFromFilePath(alternativePath);

			jvxErrorType resL = CayfComponentLib::populateBindingRefs(modName, alternativePath, bindOnReturn, str->fNameIniPtr, str->fNameDllProxr);
			if(resL == JVX_NO_ERROR)
			{
				if (bindOnReturn->bindType == ayfHostBindingType::AYF_HOST_BINDING_NONE)
				{
					if (str->fptr)
					{						
						str->fptr(ayfVoidPvoidDefinition::AYF_VOID_PVOID_ID_REPORT_NO_BINDING, str->priv, nullptr);
					}
				}
			}
			CayfComponentLibContainer* libCont = nullptr;
			JVX_SAFE_ALLOCATE_OBJECT(libCont, AYF_MODULE_CONTAINER_TYPE);
			libCont->linkBinding( bindOnReturn);

			libCont->startBinding(modName, paramStr->numInChans,
				paramStr->numOutChans, paramStr->bSize, 
				paramStr->sRate, paramStr->passthroughMode,
				str->ayfIdentsPtr, 2, str->fptr, str->priv);

			AYF_FUNCTIONNAME_REDEFINE(lstAllocated, AYF_PROJECT_POSTFIX).push_back(libCont);

			*instance = reinterpret_cast<jvxHandle*>(libCont);

			res = JVX_NO_ERROR;
		}
		return res;
	}

	jvxErrorType AYF_FUNCTIONNAME_REDEFINE(ayf_cc_delayedInitModule, AYF_PROJECT_POSTFIX)(jvxHandle** instance, jvxHandle** preinstance, struct ayfInitConnectStruct* str)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (preinstance)
		{
			preInitModuleData* preInitDataReturn = (preInitModuleData*)*preinstance;

			// Copy these arguments
			preInitDataReturn->initStr.fNameIniPtr = str->fNameIniPtr;
			preInitDataReturn->initStr.fNameDllProxr = str->fNameDllProxr;

			jvxErrorType res = AYF_FUNCTIONNAME_REDEFINE(ayf_cc_initModule, AYF_PROJECT_POSTFIX)(instance, &preInitDataReturn->initParam, 
				preInitDataReturn->ptrConstructorFunction, &preInitDataReturn->initStr);
			
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
