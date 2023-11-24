#include "mexJvxHost.h"
#include "realtimeViewer_helpers.h"
#include "CjvxMatlabToCConverter.h"

jvxErrorType
mexJvxHost::select_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::select_component>");

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC;
			jvxInt32 idx = 0;
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;

			res = CjvxMatlabToCConverter::mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				MEX_PARAMETER_INVALID_VALUE_ERROR("component_type", paramId, valI, JVX_COMPONENT_ALL_LIMIT);
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				MEX_PARAMETER_ERROR("component_type", paramId, "numeric");
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_SETTING:
				MEX_PARAMETER_OBJ_NOT_FOUND("component_type", paramId, "jvxComponentType");
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			paramId = 2;
			res = CjvxMatlabToCConverter::mexArgument2Index<jvxInt32>(idx, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{
				MEX_PARAMETER_ERROR("component_index", paramId, "numeric");
				parametersOk = false;
			}

			if(parametersOk)
			{
				res = this->involvedHost.hHost->select_component(tpAll[tpC], idx);
				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], true);
					mexFillEmpty(plhs, nlhs, 1);
					return(JVX_NO_ERROR);
				}
				else
				{
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to host reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);					
				}
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}	
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}	
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::activate_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::activate_component>");

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				MEX_PARAMETER_INVALID_VALUE_ERROR("component_type", paramId, valI, JVX_COMPONENT_ALL_LIMIT);
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				MEX_PARAMETER_ERROR("component_type", paramId, "numeric");
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_SETTING:
				MEX_PARAMETER_OBJ_NOT_FOUND("component_type", paramId, "jvxComponentType");
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if(parametersOk)
			{
				res = this->involvedHost.hHost->activate_selected_component(tpAll[tpC]);
				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], true);

					mexFillEmpty(plhs, nlhs, 1);
				}
				else
				{
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to host reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}				
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}	
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}	
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::isready_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;

	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::isready_component>");

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				MEX_PARAMETER_INVALID_VALUE_ERROR("component_type", paramId, valI, JVX_COMPONENT_ALL_LIMIT);
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				MEX_PARAMETER_ERROR("component_type", paramId, "numeric");
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_SETTING:
				MEX_PARAMETER_OBJ_NOT_FOUND("component_type", paramId, "jvxComponentType");
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if(parametersOk)
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], true);

				this->mexReturnStructComponentReady(plhs[1], tpAll[tpC]);
				
				mexFillEmpty(plhs, nlhs, 2);
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}	
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}	
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::status_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;

	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::status_component>");

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				MEX_PARAMETER_INVALID_VALUE_ERROR("component_type", paramId, valI, JVX_COMPONENT_ALL_LIMIT);
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				MEX_PARAMETER_ERROR("component_type", paramId, "numeric");
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_SETTING:
				MEX_PARAMETER_OBJ_NOT_FOUND("component_type", paramId, "jvxComponentType");
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if(parametersOk)
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], true);

				this->mexReturnStructComponentStatus(plhs[1], tpAll[tpC]);
				
				mexFillEmpty(plhs, nlhs, 2);
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}	
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}	
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::commands_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;

	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::status_component>");

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				MEX_PARAMETER_INVALID_VALUE_ERROR("component_type", paramId, valI, JVX_COMPONENT_ALL_LIMIT);
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				MEX_PARAMETER_ERROR("component_type", paramId, "numeric");
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_SETTING:
				MEX_PARAMETER_OBJ_NOT_FOUND("component_type", paramId, "jvxComponentType");
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if(parametersOk)
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], true);

				this->mexReturnStructCommandsComponents(plhs[1], tpAll[tpC]);
				
				mexFillEmpty(plhs, nlhs, 2);
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}	
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}	
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::conditions_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;

	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::status_component>");

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				MEX_PARAMETER_INVALID_VALUE_ERROR("component_type", paramId, valI, JVX_COMPONENT_ALL_LIMIT);
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				MEX_PARAMETER_ERROR("component_type", paramId, "numeric");
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_SETTING:
				MEX_PARAMETER_OBJ_NOT_FOUND("component_type", paramId, "jvxComponentType");
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if(parametersOk)
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], true);

				this->mexReturnStructConditionsComponents(plhs[1], tpAll[tpC]);
				
				mexFillEmpty(plhs, nlhs, 2);
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}	
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}	
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::rel_jumps_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;

	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::status_component>");

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				MEX_PARAMETER_INVALID_VALUE_ERROR("component_type", paramId, valI, JVX_COMPONENT_ALL_LIMIT);
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				MEX_PARAMETER_ERROR("component_type", paramId, "numeric");
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_SETTING:
				MEX_PARAMETER_OBJ_NOT_FOUND("component_type", paramId, "jvxComponentType");
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if(parametersOk)
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], true);

				this->mexReturnStructRelativeJumpsComponents(plhs[1], tpAll[tpC]);
				
				mexFillEmpty(plhs, nlhs, 2);
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}	
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}	
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::deactivate_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;

	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::deactivate_component>");

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			jvxInt32 idx = 0;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				MEX_PARAMETER_INVALID_VALUE_ERROR("component_type", paramId, valI, JVX_COMPONENT_ALL_LIMIT);
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				MEX_PARAMETER_ERROR("component_type", paramId, "numeric");
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_SETTING:
				MEX_PARAMETER_OBJ_NOT_FOUND("component_type", paramId, "jvxComponentType");
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if(parametersOk)
			{
				res = this->involvedHost.hHost->deactivate_selected_component(tpAll[tpC]);
				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], true);
					mexFillEmpty(plhs, nlhs, 1);
				}
				else
				{
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to host reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}	
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::unselect_component(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize num = 0;

	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("<jvxJvxHost::unselect_component>");

	jvxState theState = JVX_STATE_NONE;
	if(involvedComponents.theHost.hFHost)
	{
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			jvxInt32 idx = 0;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = CjvxMatlabToCConverter::mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
			switch (res)
			{
			case JVX_NO_ERROR:
				break;
			case JVX_ERROR_ID_OUT_OF_BOUNDS:
				MEX_PARAMETER_INVALID_VALUE_ERROR("component_type", paramId, valI, JVX_COMPONENT_ALL_LIMIT);
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_ARGUMENT:
				MEX_PARAMETER_ERROR("component_type", paramId, "numeric");
				parametersOk = false;
				break;
			case JVX_ERROR_INVALID_SETTING:
				MEX_PARAMETER_OBJ_NOT_FOUND("component_type", paramId, "jvxComponentType");
				parametersOk = false;
				break;
			default:
				this->report_simple_text_message(((std::string)"Problem reading parameter #" + jvx_int2String(paramId) + ", error reason <" + jvxErrorType_descr(res) + ">").c_str(), JVX_REPORT_PRIORITY_ERROR);
				break;
			}

			if(parametersOk)
			{
				res = this->involvedHost.hHost->unselect_selected_component(tpAll[tpC]);
				if(res == JVX_NO_ERROR)
				{
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], true);
					mexFillEmpty(plhs, nlhs, 1);
				}
				else
				{
					if(nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to host reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in state active", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}	
	}
	else
	{
		if(nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid.", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}	
	return(JVX_NO_ERROR);
}
