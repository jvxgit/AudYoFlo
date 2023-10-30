#include "CayfTinyMexComponentHost.h"
#include "jvxTconfigProcessor.h"

// *funcInit = jvxCuTArtHeadTracker_init;
// *funcTerm = jvxCuTArtHeadTracker_terminate;

#define MEX_PARAMETER_ERROR(p, id, exp) this->report_simple_text_message(((std::string)"Parameter <" + p + ", #" + jvx_int2String(id) + "> is not of correct type. A parameter of type <" + exp + "> is expected.").c_str(), JVX_REPORT_PRIORITY_ERROR);

CayfTinyMexComponentHost::CayfTinyMexComponentHost()
{
	jvxTconfigProcessor_init(&theGlobalConfigProcessor);
	CjvxPropertiesToMatlabConverter::converter = this;
}

CayfTinyMexComponentHost::~CayfTinyMexComponentHost()
{
	if (theObjectMain)
	{
		shutdownCore();
	}

	jvxTconfigProcessor_terminate(theGlobalConfigProcessor);
}

// ===============================================================================================================================

jvxErrorType
CayfTinyMexComponentHost::request_hidden_interface(jvxInterfaceType tp, jvxHandle** retPtr)
{
	switch (tp)
	{
	case JVX_INTERFACE_TOOLS_HOST:
		if (retPtr)
		{
			*retPtr = static_cast<IjvxToolsHost*>(this);
		}
		return JVX_NO_ERROR;
		break;
	default:
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CayfTinyMexComponentHost::return_hidden_interface(jvxInterfaceType tp, jvxHandle* retPtr)
{
	switch (tp)
	{
	case JVX_INTERFACE_TOOLS_HOST:
		if (retPtr == static_cast<IjvxToolsHost*>(this))
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
		break;
	default:
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CayfTinyMexComponentHost::object_hidden_interface(IjvxObject** objRef)
{
	if (objRef) *objRef = nullptr;
	return JVX_NO_ERROR;
}

// ======================================================================================================
jvxErrorType
CayfTinyMexComponentHost::number_tools(const jvxComponentIdentification& cpId, jvxSize* num)
{
	if (num) *num = 0;
	switch (cpId.tp)
	{
	case JVX_COMPONENT_CONNECTION:
	case JVX_COMPONENT_CONFIG_PROCESSOR:
	
		JVX_PTR_SAFE_ASSIGN(num, 1);
		break;
	default:
		break;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CayfTinyMexComponentHost::identification_tool(const jvxComponentIdentification&, jvxSize idx, jvxApiString* description, jvxApiString* descriptor, jvxBool* multipleInstances)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType
CayfTinyMexComponentHost::reference_tool(const jvxComponentIdentification& cpId, IjvxObject** theObject,
	jvxSize filter_id, const char* filter_descriptor, jvxBitField filter_stateMask,
	IjvxReferenceSelector* decider)
{
	jvxErrorType res = JVX_ERROR_UNSUPPORTED;
	switch (cpId.tp)
	{
	case JVX_COMPONENT_CONFIG_PROCESSOR:
		JVX_PTR_SAFE_ASSIGN(theObject, theGlobalConfigProcessor);
		res = JVX_NO_ERROR;
		break;
	default:
		break;
	}
	if (res != JVX_NO_ERROR)
	{
		std::cout << __FUNCTION__ << ": Warning: Failed to deliver reference to <" << jvxComponentType_txt(cpId.tp) << ">." << std::endl;
	}
	return res;
}

jvxErrorType 
CayfTinyMexComponentHost::return_reference_tool(const jvxComponentIdentification& cpId, IjvxObject* theObject)
{
	switch (cpId.tp)
	{
	case JVX_COMPONENT_CONFIG_PROCESSOR:
		if (theObject == theGlobalConfigProcessor)
		{
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ELEMENT_NOT_FOUND;
		break;
	}
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CayfTinyMexComponentHost::instance_tool(jvxComponentType, IjvxObject** theObject, jvxSize filter_id, const char* filter_descriptor)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CayfTinyMexComponentHost::return_instance_tool(jvxComponentType tp, IjvxObject* theObject, jvxSize filter_id, const char* filter_descriptor)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CayfTinyMexComponentHost::request_reference_object(IjvxObject** obj)
{
	return JVX_ERROR_UNSUPPORTED;
}

jvxErrorType 
CayfTinyMexComponentHost::return_reference_object(IjvxObject* obj) 
{
	return JVX_ERROR_UNSUPPORTED;
}

// ===============================================================================================================================

void 
CayfTinyMexComponentHost::print_usage()
{
	mexPrintf("%s\n", __FUNCTION__);
}

void
CayfTinyMexComponentHost::command(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	// If calling the function without input arguments, the setup is returned
	if (nrhs == 0)
	{
		print_usage();
		if (nlhs > 0)
			this->mexReturnBool(plhs[0], true);
		if (nlhs > 1)
			this->mexFillEmpty(plhs, nlhs, 1);
	}
	else
	{
		jvxBool dedicatedReturn = false;
		jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

		// Step I: Extract command ID from input argument
		const mxArray* arr = prhs[0];
		if (mxIsChar(arr))
		{
			std::string token = jvx_mex_2_cstring(arr);
			if (token == "init")
			{
				res = this->init_component(nlhs, plhs, nrhs, prhs, dedicatedReturn, 1, 0);
			}
			else if (token == "term")
			{
				res = this->term_component(nlhs, plhs, nrhs, prhs, dedicatedReturn, 1, 0);
			}
			else if (token == "props")
			{
				res = this->props_component(nlhs, plhs, nrhs, prhs, dedicatedReturn, 1, 0);
			}
			else if (token == "get_prop")
			{
				res = this->get_props_component(nlhs, plhs, nrhs, prhs, dedicatedReturn, 1, 0);
			}
			else if (token == "set_prop")
			{
				res = this->set_props_component(nlhs, plhs, nrhs, prhs, dedicatedReturn, 1, 0);
			}
			else
			{
				std::string err = "Invalid command <";
				err += token;
				err += ">.";
				mexErrMsgTxt(err.c_str());
				this->mexFillEmpty(plhs, nlhs, 0);
				dedicatedReturn = true;
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		if (!dedicatedReturn)
		{
			if (res == JVX_NO_ERROR)
			{
				if (nlhs > 0)
					this->mexReturnBool(plhs[0], true);
				if (nlhs > 1)
					this->mexFillEmpty(plhs, nlhs, 1);
			}
			else
			{
				if (nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if (nlhs > 1)
				{
					std::string errDescr = "Error: <";
					errDescr += jvxErrorType_descr(res);
					errDescr += ">.";
					this->mexReturnAnswerNegativeResult(plhs[1], errDescr, res);
				}

			}
		}
	}
}

jvxErrorType
CayfTinyMexComponentHost::init_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool& dedicatedReturn, int offsetlhs, int offsetrhs)
{
	mexPrintf("%s\n", __FUNCTION__);
	if(theObjectMain == nullptr)
	{
#ifdef JVX_OS_WINDOWS
		AllocConsole();
		freopen("conin$", "r", stdin);
		freopen("conout$", "w", stdout);
		freopen("conout$", "w", stderr);
		printf("Attached Debugging Window:\n");
		consoleAttached = true;
#endif
		if(isTech)
		{
			init_tech_component();
			if (theObjectMain)
			{
				theTech = castFromObject<IjvxTechnology>(theObjectMain);
				if (theTech)
				{
					jvxErrorType res = theTech->initialize(this);
					JVX_RUN_ON_NO_ERROR(res, theTech->select());
					JVX_RUN_ON_NO_ERROR(res, theTech->activate());
					JVX_RUN_ON_NO_ERROR(res, theTech->request_device(0, &theDev));
					JVX_RUN_ON_NO_ERROR(res, theDev->initialize(this));
					JVX_RUN_ON_NO_ERROR(res, theDev->select());
					JVX_RUN_ON_NO_ERROR(res, theDev->activate());

					jvx_initPropertyReferenceTriple(&theTriple);
					theTriple.theObj = theDev;
					theDev->request_specialization(&theTriple.theHdl, &theTriple.cpId, nullptr);
					theDev->request_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle**>(&theTriple.theProps));

					return JVX_NO_ERROR;
				}
				return JVX_ERROR_INVALID_SETTING;
			}
		}
		else
		{
			assert(0);
		}
		return JVX_ERROR_UNEXPECTED;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CayfTinyMexComponentHost::term_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool& dedicatedReturn, int offsetlhs, int offsetrhs)
{
	mexPrintf("%s\n", __FUNCTION__);
	if(theObjectMain != nullptr)
	{
		shutdownCore();
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

void
CayfTinyMexComponentHost::shutdownCore()
{
	if (theTech)
	{
		if (theDev)
		{
			theDev->return_hidden_interface(JVX_INTERFACE_PROPERTIES, reinterpret_cast<jvxHandle*>(theTriple.theProps));
			theTriple.theObj = nullptr;
			theTriple.theHdl = nullptr;
			jvx_initPropertyReferenceTriple(&theTriple);

			theDev->deactivate();
			theDev->unselect();
			theDev->terminate();
			theTech->return_device(theDev);
		}
		theDev = nullptr;

		theTech->deactivate();
		theTech->unselect();
		theTech->terminate();
	}
	theTech = nullptr;
	if (theObjectMain)
	{
		if (isTech)
		{
			term_tech_component();
		}
		else
		{
			assert(0);
		}		
		theObjectMain = nullptr;
	}
#ifdef JVX_OS_WINDOWS
	if (consoleAttached)
	{
		std::cout << "Running FreeConsole to shutdown verbose output." << std::endl;
		// https://github.com/microsoft/terminal/pull/14544
		// https://devblogs.microsoft.com/commandline/windows-command-line-introducing-the-windows-pseudo-console-conpty/
		// ClosePseudoConsoleTimeout(0);
		BOOL freeResult = FreeConsole();
		if (!freeResult)
		{
			DWORD lErr = GetLastError();
		}
	}
	consoleAttached = false;
#endif
}

jvxErrorType
CayfTinyMexComponentHost::props_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool& dedicatedReturn, int offsetlhs, int offsetrhs)
{
	if (theObjectMain != nullptr)
	{
		if (nlhs > 0)
		{
			this->mexReturnStructProperties(plhs[0], theTriple);
		}
		if (nlhs > 1)
		{
			this->mexFillEmpty(plhs, nlhs, 1);
		}
		dedicatedReturn = true;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

jvxErrorType
CayfTinyMexComponentHost::get_props_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool& dedicatedReturn, int offsetlhs, int offsetrhs)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (theObjectMain != nullptr)
	{
		std::string errMessage;
		std::string descriptor;
		jvxSize paramId = 1;
		jvxInt32 valI;
		jvxSize offset = 0;
		res = mexArgument2String(descriptor, prhs, paramId, nrhs);
		if (res == JVX_NO_ERROR)
		{
			jvxSize fillEmptyCnt = 0;
			paramId = 2;
			res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if (res == JVX_NO_ERROR)
			{
				offset = (jvxSize)valI;
			}

			if (nrhs > 0)
			{
				fillEmptyCnt++;
				res = this->mexGetPropertyCore(plhs[0], theTriple, descriptor, offset, errMessage);
				if (res != JVX_NO_ERROR)
				{
					mexErrMsgTxt(errMessage.c_str());

					// In case of negative, return an empty value
					this->mexFillEmpty(plhs, 1, 0);

					if (nlhs > 1)
					{
						fillEmptyCnt++;
						this->mexReturnAnswerNegativeResult(plhs[1], errMessage, res);
					}
					else
					{
						mexPrintf("%s\n", errMessage.c_str());
					}
				}
			}
			this->mexFillEmpty(plhs, nlhs, fillEmptyCnt);
		}
		else
		{
			mexErrMsgTxt("Argument to identify property secriptor must be a string!");
		}
		dedicatedReturn = true;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_WRONG_STATE;
}

	jvxErrorType
		CayfTinyMexComponentHost::set_props_component(int nlhs, mxArray * plhs[], int nrhs, const mxArray * prhs[], jvxBool & dedicatedReturn, int offsetlhs, int offsetrhs)
	{
		jvxErrorType res = JVX_NO_ERROR;
		if (theObjectMain != nullptr)
		{
			std::string errMessage;
			std::string descriptor;
			jvxSize paramId = 1;
			jvxInt32 valI;
			jvxSize offset = 0;
			res = mexArgument2String(descriptor, prhs, paramId, nrhs);
			if (res == JVX_NO_ERROR)
			{
				jvxSize fillEmptyCnt = 0;
				if (nrhs > 0)
				{
					fillEmptyCnt++;
					res = this->mexSetPropertyCore(plhs[0], prhs, 2, nrhs, theTriple, descriptor, errMessage);
					if (res != JVX_NO_ERROR)
					{
						mexErrMsgTxt(errMessage.c_str());

						// In case of negative, return an empty value
						this->mexFillEmpty(plhs, 1, 0);

						if (nlhs > 1)
						{
							fillEmptyCnt++;
							this->mexReturnAnswerNegativeResult(plhs[1], errMessage, res);
						}
						else
						{
							mexPrintf("%s\n", errMessage.c_str());
						}
					}
				}
				this->mexFillEmpty(plhs, nlhs, fillEmptyCnt);
			}
			else
			{
				mexErrMsgTxt("Argument to identify property secriptor must be a string!");
			}
			dedicatedReturn = true;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	}