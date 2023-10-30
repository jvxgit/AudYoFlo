#include "mexJvxHost.h"
#include "realtimeViewer_helpers.h"
#include "codeFragments/matlab_c/HjvxMex2CConverter.h"

jvxErrorType 
mexJvxHost::info_properties(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("mexJvxHost::info_property");
	jvxState theState = JVX_STATE_NONE;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvx_propertyReferenceTriple theTriple;
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
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
				jvx_initPropertyReferenceTriple(&theTriple);
				jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);

				if(theTriple.theProps)
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					if(nlhs > 1)
					{
						mexReturnStructProperties(plhs[1], theTriple);
					}
					mexFillEmpty(plhs,nlhs,2);

					jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], true);
					}
					mexFillEmpty(plhs, nlhs,1);
				}
			}
			else
			{
				if(nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if(nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_INVALID_ARGUMENT);
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
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
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
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
	mexJvxHost::list_property_tags(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i,j,num = 0;
	std::vector<std::string> tags;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::list_property_tags");
	jvxState theState = JVX_STATE_NONE;


	std::string outTxt;
	jvxBool readonly = true;

	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE) 
		{
			std::string cpString;
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;

			std::string descString;

			jvx_propertyReferenceTriple theTriple;

			jvxCallManagerProperties callGate;
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2String(cpString, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				for(i = 0; i < JVX_COMPONENT_ALL_LIMIT; i++)
				{
					if(
						(cpString == jvxComponentType_txtf(i))||
						(cpString == jvxComponentType_txt(i)))
					{
						tpC = (jvxComponentType)i;
						break;
					}
				}
			}

			if(parametersOk)
			{
				if(tpC == JVX_COMPONENT_UNKNOWN)
				{
					for(i = JVX_COMPONENT_UNKNOWN+1; i < JVX_COMPONENT_ALL_LIMIT; i++)
					{
						outTxt = "--> Component selector = <";
						outTxt += jvxComponentType_txtf(i);
						outTxt += ">";
						report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_NONE);

						jvx_initPropertyReferenceTriple(&theTriple);
						jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[(jvxComponentType)i]);

						if(jvx_isValidPropertyReferenceTriple(&theTriple))
						{
							num = 0;
							theTriple.theProps->number_properties(callGate, &num);
							for(j = 0; j < num; j++)
							{
								jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus theDescr;
								jvx::propertyAddress::CjvxPropertyAddressLinear ident(j);

								res = theTriple.theProps->description_property(callGate, theDescr, ident);
								if(res == JVX_NO_ERROR)
								{
									std::string outTxt = "----> Property #" + jvx_size2String(j);
									outTxt += "<";

									outTxt += theDescr.descriptor.std_str();
									outTxt +=  ":";
									outTxt += theDescr.name.std_str();
									outTxt +=  ":";
									outTxt += theDescr.description.std_str();
	
									outTxt +=  ">";
									outTxt += ":";
									outTxt += +jvxPropertyCategoryType_str[theDescr.category].full;

									outTxt += ":states(";

									if(theDescr.allowedStateMask & JVX_STATE_INIT)
										outTxt += "I";
									if(theDescr.allowedStateMask & JVX_STATE_SELECTED)
										outTxt += "S";
									if(theDescr.allowedStateMask & JVX_STATE_ACTIVE)
										outTxt += "A";
									if(theDescr.allowedStateMask & JVX_STATE_PREPARED)
										outTxt += "P";
									if(theDescr.allowedStateMask & JVX_STATE_PROCESSING)
										outTxt += "R";
									outTxt += ")";

									outTxt += ":threads(";
									if(theDescr.allowedThreadingMask & JVX_THREADING_ASYNC)
										outTxt += "A";
									if(theDescr.allowedThreadingMask & JVX_THREADING_SYNC)
										outTxt += "S";
									if(theDescr.allowedThreadingMask & JVX_THREADING_DONTCARE)
										outTxt += "D";
									if(theDescr.allowedThreadingMask & JVX_THREADING_INIT)
										outTxt += "I";
									outTxt += ")";

									outTxt += ":";
									outTxt += jvxDataFormat_str[theDescr.format].full;

									outTxt += ": " + jvx_size2String(theDescr.num);

									outTxt += ": ";
									outTxt += jvxPropertyAccessType_str[theDescr.accessType].full;

									outTxt += ":";
									outTxt += jvxPropertyDecoderHintType_str[theDescr.decTp].full;

									outTxt += ":";
									if(theDescr.isValid)
										outTxt += "*";
									else
										outTxt += "-";

									outTxt += ":";
									outTxt += jvx_uint322Hexstring(theDescr.accessFlags);

									report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_NONE);

								}
							}// for 
						}
					}
				}
				else
				{
					outTxt = "Component selector = <";
					outTxt += jvxComponentType_txtf(tpAll[tpC].tp);
					outTxt += ">";
					report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_NONE);

					jvx_initPropertyReferenceTriple(&theTriple);
					jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);

					if(jvx_isValidPropertyReferenceTriple(&theTriple))
					{
						theTriple.theProps->number_properties(callGate, &num);
						for(j = 0; j < num; j++)
						{
							jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
							jvx::propertyAddress::CjvxPropertyAddressLinear ident(j);
							res = theTriple.theProps->description_property(callGate, theDescr, ident);
							if(res == JVX_NO_ERROR)
							{
								std::string outTxt = "--> Property #" + jvx_size2String(j);
								
								outTxt += "<";
								
								outTxt += theDescr.descriptor.std_str();
								outTxt +=  ":";
								outTxt += theDescr.name.std_str();
								outTxt +=  ":";
								outTxt += theDescr.description.std_str();
								
								outTxt +=  ">";
								outTxt += ":";
								outTxt += jvxPropertyCategoryType_str[theDescr.category].full;

								outTxt += ":states(";

								if(theDescr.allowedStateMask & JVX_STATE_INIT)
									outTxt += "I";
								if(theDescr.allowedStateMask & JVX_STATE_SELECTED)
									outTxt += "S";
								if(theDescr.allowedStateMask & JVX_STATE_ACTIVE)
									outTxt += "A";
								if(theDescr.allowedStateMask & JVX_STATE_PREPARED)
									outTxt += "P";
								if(theDescr.allowedStateMask & JVX_STATE_PROCESSING)
									outTxt += "R";
								outTxt += ")";

								outTxt += ":threads(";
								if(theDescr.allowedThreadingMask & JVX_THREADING_ASYNC)
									outTxt += "A";
								if(theDescr.allowedThreadingMask & JVX_THREADING_SYNC)
									outTxt += "S";
								if(theDescr.allowedThreadingMask & JVX_THREADING_DONTCARE)
									outTxt += "D";
								if(theDescr.allowedThreadingMask & JVX_THREADING_INIT)
									outTxt += "I";
								outTxt += ")";

								outTxt += ":";
								outTxt += jvxDataFormat_str[theDescr.format].full;

								outTxt += ": " + jvx_size2String(theDescr.num);

								outTxt += ": ";
								outTxt += jvxPropertyAccessType_str[theDescr.accessType].full;

								outTxt += ":";
								outTxt += jvxPropertyDecoderHintType_str[theDescr.decTp].full;
								report_simple_text_message(outTxt.c_str(), JVX_REPORT_PRIORITY_NONE);
							}
						}
					}
				}
			}
			else
			{
				if (nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_ID_OUT_OF_BOUNDS);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
	}
	mexFillEmpty(plhs, nlhs, 0);
	return(JVX_NO_ERROR);
}



jvxErrorType 
mexJvxHost::get_property_idx(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize fillEmptyCnt = 2;

	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::get_property_idx");
	jvxState theState = JVX_STATE_NONE;
	jvxCallManagerProperties callGate;
	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			jvx_propertyReferenceTriple theTriple;

			jvxSize idxProp = 0;
			jvxBool readonly = true;
			
			jvxSize offset = 0;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
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
			res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				idxProp = (jvxSize) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("property_index", paramId, "numeric");
				parametersOk = false;
			}

			if (spec_offset)
			{
				paramId = 3;
				res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
				if (res == JVX_NO_ERROR)
				{
					offset = (jvxSize)valI;
				}
				else
				{
					MEX_PARAMETER_ERROR("property_offset", paramId, "numeric");
					parametersOk = false;
				}
			}

			if(parametersOk)
			{
				jvx_initPropertyReferenceTriple(&theTriple);
				jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);

				if(theTriple.theProps)
				{
					jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
					jvx::propertyAddress::CjvxPropertyAddressLinear ident(idxProp);

					res = theTriple.theProps->description_property(callGate, theDescr, ident);						
					if(res == JVX_NO_ERROR && (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK))
					{
						switch(theDescr.format)
						{
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_U8BIT:
						case JVX_DATAFORMAT_U16BIT_LE:
						case JVX_DATAFORMAT_U32BIT_LE:
						case JVX_DATAFORMAT_U64BIT_LE:
						case JVX_DATAFORMAT_DATA:
							
							if(nlhs > 1)
							{
								resL = mexReturnPropertyNumerical(plhs[1], theDescr.globalIdx, theDescr.category, theDescr.format, theDescr.num, 
									theDescr.decTp, theTriple, offset);
							}
							if (resL == JVX_NO_ERROR)
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}
							}
							else
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], false);
								}
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Component request returned error message <" +
									jvxErrorType_descr(resL) + ">.", res);
							}
							fillEmptyCnt = 2;
							break;
						case JVX_DATAFORMAT_SIZE:
							
							if (nlhs > 1)
							{
								mexReturnPropertyNumericalSize(plhs[1], theDescr.globalIdx, theDescr.category, theDescr.format, theDescr.num,
									theDescr.decTp, theTriple, offset);
							}
							if (resL == JVX_NO_ERROR)
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}
							}
							else
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], false);
								}
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Component request returned error message <" +
									jvxErrorType_descr(resL) + ">.", res);
							}
							fillEmptyCnt = 2;
							break;
						case JVX_DATAFORMAT_SELECTION_LIST:
						case JVX_DATAFORMAT_STRING:
						case JVX_DATAFORMAT_STRING_LIST:
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
							if (nlhs > 1)
							{
								resL = this->mexReturnPropertyOthers(plhs[1], theDescr.globalIdx, theDescr.category, theDescr.format, theDescr.num,
									theDescr.decTp, theTriple, offset);
							}
							if (resL == JVX_NO_ERROR)
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}
							}
							else
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], false);
								}
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Component request returned error message <" +
									jvxErrorType_descr(resL) + ">.", res);
							}
							fillEmptyCnt = 2;
							break;
						default:
							if(nlhs > 0)
							{
								this->mexReturnBool(plhs[0], false);
							}
							if(nlhs > 1)
							{
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Request for unsupported property type.", JVX_ERROR_UNSUPPORTED);
							}
							fillEmptyCnt = 2;
						}
					}
					else
					{
						if(nlhs > 0)
							this->mexReturnBool(plhs[0], false);
						if(nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to host reference failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						fillEmptyCnt = 2;
					}
					jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Internal Error.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					fillEmptyCnt = 2;
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
				fillEmptyCnt = 2;
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
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			fillEmptyCnt = 2;
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
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		fillEmptyCnt = 2;
	}

	mexFillEmpty(plhs, nlhs, fillEmptyCnt);

	return(JVX_NO_ERROR);
}

jvxErrorType 
mexJvxHost::get_property_uniqueid(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize offsetFillEmpty = 2;

	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::get_property_uniqueid");
	jvxState theState = JVX_STATE_NONE;
	jvxCallManagerProperties callGate;
	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE) 
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;

			jvx_propertyReferenceTriple theTriple;

			jvxSize num = 0;
			jvxPropertyCategoryType catProperty_user = JVX_PROPERTY_CATEGORY_UNKNOWN;
			jvxSize hdlIdx_user = 0;

			jvxBool readonly = true;
			jvxSize offset = 0;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
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
			res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				catProperty_user = (jvxPropertyCategoryType) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("category", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 3;
			res = mexArgument2Index<jvxInt32>(valI,prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				hdlIdx_user = (jvxSize) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("property_index", paramId, "numeric");
				parametersOk = false;
			}

			if (spec_offset)
			{
				paramId = 4;
				res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
				if (res == JVX_NO_ERROR)
				{
					offset = (jvxSize)valI;
				}
				else
				{
					MEX_PARAMETER_ERROR("property_offset", paramId, "numeric");
					parametersOk = false;
				}
			}

			if(parametersOk)
			{
				jvx_initPropertyReferenceTriple(&theTriple);
				jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);

				if(theTriple.theProps)
				{
					jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
					// Map given unique id to one specific property. We need the format and other infos before accessing the content
					theTriple.theProps->number_properties(callGate, &num);
					bool foundit = false;

					for(i = 0;i < num; i++)
					{
						jvx::propertyAddress::CjvxPropertyAddressLinear ident(i);
						res = theTriple.theProps->description_property(callGate,theDescr, ident);
						bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_ID(i), theTriple.theProps);
						assert(rr);
						if((theDescr.globalIdx == hdlIdx_user) && (theDescr.category == catProperty_user))
						{
							foundit = true;
							break;
						}
					}

					if(foundit)
					{
						switch(theDescr.format)
						{
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_U8BIT:
						case JVX_DATAFORMAT_U16BIT_LE:
						case JVX_DATAFORMAT_U32BIT_LE:
						case JVX_DATAFORMAT_U64BIT_LE:
						case JVX_DATAFORMAT_DATA:
							if (nlhs > 1)
							{
								resL = mexReturnPropertyNumerical(plhs[1], theDescr.globalIdx, theDescr.category, theDescr.format, 
									theDescr.num, theDescr.decTp, theTriple, offset);
							}
							if (resL == JVX_NO_ERROR)
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}
							}
							else
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], false);
								}
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Component request returned error message <" +
									jvxErrorType_descr(resL) + ">.", res);
							}
							offsetFillEmpty = 2;
							break;
						case JVX_DATAFORMAT_SIZE:
							if (nlhs > 1)
							{
								resL = mexReturnPropertyNumericalSize(plhs[1], theDescr.globalIdx, theDescr.category, theDescr.format,
									theDescr.num, theDescr.decTp, theTriple, offset);
							}
							
							if (resL == JVX_NO_ERROR)
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}
							}
							else
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], false);
								}
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Component request returned error message <" +
									jvxErrorType_descr(resL) + ">.", res);
							}
							offsetFillEmpty = 2;
							break;

						case JVX_DATAFORMAT_SELECTION_LIST:
						case JVX_DATAFORMAT_STRING:
						case JVX_DATAFORMAT_STRING_LIST:
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
							if (nlhs > 1)
							{
								resL = this->mexReturnPropertyOthers(plhs[1], theDescr.globalIdx, theDescr.category, theDescr.format,
									theDescr.num, theDescr.decTp, theTriple, offset);
							}
							if (resL == JVX_NO_ERROR)
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}
							}
							else
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], false);
								}
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Component request returned error message <" +
									jvxErrorType_descr(resL) + ">.", res);
							}
							offsetFillEmpty = 2;
							break;
						default:
							if(nlhs > 0)
							{
								this->mexReturnBool(plhs[0], false);
							}
							if(nlhs > 1)
							{
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Request for unsupported property type.", JVX_ERROR_UNSUPPORTED);
							}
							offsetFillEmpty = 2;
						}
					}
					else
					{
						if(nlhs > 0)
							this->mexReturnBool(plhs[0], false);
						if(nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Did not find property with specified unique id/category.", JVX_ERROR_ELEMENT_NOT_FOUND);
						}
						offsetFillEmpty = 2;
					}
					jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Internal error.", JVX_ERROR_INTERNAL);
					}
					offsetFillEmpty = 2;
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
				offsetFillEmpty = 2;
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
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			offsetFillEmpty = 2;
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
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		offsetFillEmpty = 2;
	}

	// Fill the remaining fields
	mexFillEmpty(plhs, nlhs, offsetFillEmpty);

	return(JVX_NO_ERROR);
}




jvxErrorType 
mexJvxHost::get_property_descriptor(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxSize fillEmptyCnt = 2;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::get_property_descriptor");
	jvxState theState = JVX_STATE_NONE;
	jvxCallManagerProperties callGate;
	jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor idProp(nullptr);
	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE) 
		{
			std::string cpString;
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;

			std::string descString;

			jvx_propertyReferenceTriple theTriple;

			jvxSize num = 0;
			jvxBool readonly = true;
			jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
			jvxSize offset = 0;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
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
			res = mexArgument2String(descString, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{				
				MEX_PARAMETER_ERROR("descriptor", paramId, "string");
				parametersOk = false;
			}

			if (spec_offset)
			{
				paramId = 3;
				res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
				if (res == JVX_NO_ERROR)
				{
					offset = (jvxSize)valI;
				}
				else
				{
					MEX_PARAMETER_ERROR("property_offset", paramId, "numeric");
					parametersOk = false;
				}
			}
			if(parametersOk)
			{
				jvx_initPropertyReferenceTriple(&theTriple);
				jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				
				if (theTriple.theProps)
				{
					// This
					if (nlhs > 1)
					{
						std::string errMessOnReturn;
						res = mexGetPropertyCore(plhs[1], theTriple, descString, offset, errMessOnReturn);
						if (res == JVX_NO_ERROR)
						{
							this->mexReturnBool(plhs[0], true);							
						}
						else
						{
							assert(plhs[0] == nullptr);
							this->mexReturnBool(plhs[0], false);
							this->mexReturnAnswerNegativeResult(plhs[1], errMessOnReturn, res );
						}
						fillEmptyCnt = 2;
					}
					else
					{
						if (nlhs > 0)
						{
							// Not enough output args
							this->mexReturnBool(plhs[0], false);
						}
					}
					jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Could not find property entry references for component type " + cpString +".", JVX_ERROR_ELEMENT_NOT_FOUND);
					}
					fillEmptyCnt = 2;
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
				fillEmptyCnt = 2;
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
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			fillEmptyCnt = 2;
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
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		fillEmptyCnt = 2;
	}
	mexFillEmpty(plhs, nlhs, fillEmptyCnt);
	return(JVX_NO_ERROR);
}



jvxErrorType 
mexJvxHost::set_property_idx(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::set_property_idx");
	jvxState theState = JVX_STATE_NONE;
	jvxCallManagerProperties callGate;
	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			jvx_propertyReferenceTriple theTriple;

			jvxSize idxProp = 0;
			jvxBool readonly = true;
			jvxSize offset = 0;
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
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
			res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				idxProp = (jvxSize) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("property_index", paramId, "numeric");
				parametersOk = false;
			}

			if (spec_offset)
			{
				paramId = 3;
				res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
				if (res == JVX_NO_ERROR)
				{
					offset = (jvxSize)valI;
				}
				else
				{
					MEX_PARAMETER_ERROR("property_offset", paramId, "numeric");
					parametersOk = false;
				}
			}

			paramId++;

			if(nrhs <= paramId)
			{
				MEX_PARAMETER_ERROR("data_field", paramId, "specific");
				parametersOk = false;
			}
			if(parametersOk)
			{
				jvx_initPropertyReferenceTriple(&theTriple);
				jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);

				if(theTriple.theProps)
				{
					jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
					jvx::propertyAddress::CjvxPropertyAddressLinear ident(idxProp);

					res = theTriple.theProps->description_property(callGate, theDescr, ident);
					if(JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_ID(idxProp), theTriple.theProps))
					{
						switch(theDescr.format)
						{
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_U8BIT:
						case JVX_DATAFORMAT_U16BIT_LE:
						case JVX_DATAFORMAT_U32BIT_LE:
						case JVX_DATAFORMAT_U64BIT_LE:
						case JVX_DATAFORMAT_DATA:

							res = copyDataToComponentNumerical(prhs[paramId], theTriple, theDescr.category, theDescr.format, theDescr.num, theDescr.globalIdx, offset, &callGate.access_protocol);
							if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(theDescr.globalIdx, theDescr.category), theTriple.theProps))
							{
								if(nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}						
								mexFillEmpty(plhs, nlhs, 1);
							}
							else
							{
								if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified data field is not of correct format.", JVX_ERROR_INVALID_ARGUMENT);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
								else
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Tranferring data to componente failed, reason: " + 
											jvxAccessProtocol_txt(callGate.access_protocol), JVX_ERROR_INVALID_ARGUMENT);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
							}
							break;
						case JVX_DATAFORMAT_SIZE:

							res = copyDataToComponentNumericalSize(prhs[paramId], theTriple, theDescr.category, theDescr.format, theDescr.num, theDescr.globalIdx, offset, &callGate.access_protocol);
							if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(theDescr.globalIdx, theDescr.category), theTriple.theProps))
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}
								mexFillEmpty(plhs, nlhs, 1);
							}
							else
							{
								if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified data field is not of correct format.", JVX_ERROR_INVALID_ARGUMENT);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
								else
								{
									if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
									{
										if (nlhs > 0)
										{
											this->mexReturnBool(plhs[0], false);
										}
										if (nlhs > 1)
										{
											this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified data field is not of correct format.", JVX_ERROR_INVALID_ARGUMENT);
										}
										mexFillEmpty(plhs, nlhs, 2);
									}
									else
									{
										if (nlhs > 0)
										{
											this->mexReturnBool(plhs[0], false);
										}
										if (nlhs > 1)
										{
											this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Tranferring data to componente failed, reason: " +
												jvxAccessProtocol_txt(callGate.access_protocol), JVX_ERROR_INVALID_ARGUMENT);
										}
										mexFillEmpty(plhs, nlhs, 2);
									}
								}
							}
							break;
						case JVX_DATAFORMAT_SELECTION_LIST:
						case JVX_DATAFORMAT_STRING:
						case JVX_DATAFORMAT_STRING_LIST:
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
							res = copyDataToComponentOthers(&prhs[paramId], nrhs- paramId, theTriple, theDescr.category, theDescr.format, theDescr.num, theDescr.globalIdx, offset, &callGate.access_protocol);
							if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(theDescr.globalIdx, theDescr.category), theTriple.theProps))
							{
								if(nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}
								mexFillEmpty(plhs, nlhs, 1);
							}
							else
							{
								if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified data field is not of correct format.", JVX_ERROR_INVALID_ARGUMENT);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
								else
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Tranferring data to componente failed, reason: " +
											jvxAccessProtocol_txt(callGate.access_protocol), JVX_ERROR_INVALID_ARGUMENT);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
							}
							break;
						default:
							if(nlhs > 0)
							{
								this->mexReturnBool(plhs[0], false);
							}
							if(nlhs > 1)
							{
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Request for unsupported property type.", JVX_ERROR_UNSUPPORTED);
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
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to member function <IjvxProperties::description_property> failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
					jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified component does not provide interface for properties.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
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
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
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
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
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
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
mexJvxHost::set_property_uniqueid(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::set_property_uniqueid");
	jvxState theState = JVX_STATE_NONE;
	jvxCallManagerProperties callGate;
	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;

			jvx_propertyReferenceTriple theTriple;

			jvxSize num = 0;
			jvxPropertyCategoryType catProperty_user = JVX_PROPERTY_CATEGORY_UNKNOWN;
			jvxSize hdlIdx_user = 0;

			jvxBool readonly = true;
			jvxSize offset = 0;
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
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
			res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				catProperty_user = (jvxPropertyCategoryType) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("category", paramId, "numeric");
				parametersOk = false;
			}

			paramId = 3;
			res = mexArgument2Index<jvxInt32>(valI,prhs, paramId, nrhs);
			if(res == JVX_NO_ERROR)
			{
				hdlIdx_user = (jvxSize) valI;
			}
			else
			{
				MEX_PARAMETER_ERROR("property_index", paramId, "numeric");
				parametersOk = false;
			}

			if (spec_offset)
			{
				paramId = 4;
				res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
				if (res == JVX_NO_ERROR)
				{
					offset = (jvxSize)valI;
				}
				else
				{
					MEX_PARAMETER_ERROR("property_offset", paramId, "numeric");
					parametersOk = false;
				}
			}
			paramId++;
			if(parametersOk)
			{
				jvx_initPropertyReferenceTriple(&theTriple);
				jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);

				if(theTriple.theProps)
				{
					jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;

					// Map given unique id to one specific property. We need the format and other infos before accessing the content
					theTriple.theProps->number_properties(callGate, &num);
					bool foundit = false;

					for(i = 0;i < num; i++)
					{
						jvx::propertyAddress::CjvxPropertyAddressLinear ident(i);
						res = theTriple.theProps->description_property(callGate,theDescr, ident);
						bool rr = JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_ID(i), theTriple.theProps);
						assert(rr);
						if((theDescr.globalIdx == hdlIdx_user) && (theDescr.category == catProperty_user))
						{
							foundit = true;
							break;
						}
					}

					if(foundit)
					{

						switch(theDescr.format)
						{
						case JVX_DATAFORMAT_8BIT:
						case JVX_DATAFORMAT_16BIT_LE:
						case JVX_DATAFORMAT_32BIT_LE:
						case JVX_DATAFORMAT_64BIT_LE:
						case JVX_DATAFORMAT_U8BIT:
						case JVX_DATAFORMAT_U16BIT_LE:
						case JVX_DATAFORMAT_U32BIT_LE:
						case JVX_DATAFORMAT_U64BIT_LE:
						case JVX_DATAFORMAT_DATA:

							res = copyDataToComponentNumerical(prhs[paramId], theTriple, theDescr.category, theDescr.format, 
								theDescr.num, theDescr.globalIdx, offset, &callGate.access_protocol);
							if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(theDescr.globalIdx, theDescr.category), theTriple.theProps))
							{
								if(nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}						
								mexFillEmpty(plhs, nlhs, 1);
							}
							else
							{
								if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to copy property data to property.", res);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
								else
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Tranferring data to componente failed, reason: " +
											jvxAccessProtocol_txt(callGate.access_protocol), JVX_ERROR_INVALID_ARGUMENT);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
							}
							break;
						case JVX_DATAFORMAT_SIZE:

							res = copyDataToComponentNumericalSize(prhs[paramId], theTriple, theDescr.category, theDescr.format,
								theDescr.num, theDescr.globalIdx, offset, &callGate.access_protocol);
							if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(theDescr.globalIdx, theDescr.category), theTriple.theProps))
							{
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}
								mexFillEmpty(plhs, nlhs, 1);
							}
							else
							{
								if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to copy property data to property.", res);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
								else
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Tranferring data to componente failed, reason: " +
											jvxAccessProtocol_txt(callGate.access_protocol), JVX_ERROR_INVALID_ARGUMENT);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
							}
							break;
						case JVX_DATAFORMAT_SELECTION_LIST:
						case JVX_DATAFORMAT_STRING:
						case JVX_DATAFORMAT_STRING_LIST:
						case JVX_DATAFORMAT_VALUE_IN_RANGE:
							res = copyDataToComponentOthers(&prhs[paramId], nrhs- paramId, theTriple, theDescr.category, theDescr.format,
								theDescr.num, theDescr.globalIdx, offset, &callGate.access_protocol);
							if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, JVX_PROPERTY_DESCRIBE_IDX_CAT(theDescr.globalIdx, theDescr.category), theTriple.theProps))
							{
								if(nlhs > 0)
								{
									this->mexReturnBool(plhs[0], true);
								}
								mexFillEmpty(plhs, nlhs, 1);
							}
							else
							{
								if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to copy property data to property.", res);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
								else
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], false);
									}
									if (nlhs > 1)
									{
										this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Tranferring data to componente failed, reason: " +
											jvxAccessProtocol_txt(callGate.access_protocol), JVX_ERROR_INVALID_ARGUMENT);
									}
									mexFillEmpty(plhs, nlhs, 2);
								}
							}
							break;
						default:
							if(nlhs > 0)
							{
								this->mexReturnBool(plhs[0], false);
							}
							if(nlhs > 1)
							{
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Request for unsupported property type.", JVX_ERROR_UNSUPPORTED);
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
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to member function <IjvxProperties::description_property> failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
					jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified component does not provide interface for properties.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
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
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
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
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
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
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
mexJvxHost::set_property_descriptor(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::set_property_descriptor");
	jvxState theState = JVX_STATE_NONE;

	jvx::propertyDescriptor::CjvxPropertyDescriptorCore descr;
	jvx::propertyAddress::CjvxPropertyAddressDescriptor idProp(nullptr);

	jvxCallManagerProperties callGate;
	if(involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if(theState == JVX_STATE_ACTIVE)
		{
			std::string cpString;
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;

			std::string descString;

			jvx_propertyReferenceTriple theTriple;

			jvxSize num = 0;
			jvxBool readonly = true;
			jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
			jvxSize offset = 0;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
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
			res = mexArgument2String(descString, prhs, paramId, nrhs);
			if(res != JVX_NO_ERROR)
			{				
				MEX_PARAMETER_ERROR("category", paramId, "numeric");
				parametersOk = false;
			}

			if (spec_offset)
			{
				paramId = 3;
				res = mexArgument2Index<jvxInt32>(valI, prhs, paramId, nrhs);
				if (res == JVX_NO_ERROR)
				{
					offset = (jvxSize)valI;
				}
				else
				{
					MEX_PARAMETER_ERROR("property_offset", paramId, "numeric");
					parametersOk = false;
				}
			}

			paramId++;
			if(parametersOk)
			{
				jvx_initPropertyReferenceTriple(&theTriple);
				jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);

				if (theTriple.theProps)
				{
					// Map given unique id to one specific property. We need the format and other infos before accessing the content
					idProp.descriptor = descString.c_str();
					res = theTriple.theProps->description_property(callGate, descr, idProp);
					if (res != JVX_NO_ERROR)
					{
						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if (nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Access to property failed.", res);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
					else
					{
					  if(res == JVX_NO_ERROR && (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK))
						{

							switch (descr.format)
							{
							case JVX_DATAFORMAT_8BIT:
							case JVX_DATAFORMAT_16BIT_LE:
							case JVX_DATAFORMAT_32BIT_LE:
							case JVX_DATAFORMAT_64BIT_LE:
							case JVX_DATAFORMAT_U8BIT:
							case JVX_DATAFORMAT_U16BIT_LE:
							case JVX_DATAFORMAT_U32BIT_LE:
							case JVX_DATAFORMAT_U64BIT_LE:
							case JVX_DATAFORMAT_DATA:
							case JVX_DATAFORMAT_SIZE:
								res = copyDataToComponentNumerical(prhs[paramId], theTriple, descr.format, descr.num, descString.c_str(), offset, &callGate.access_protocol);
								if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, descString, theTriple.theProps))
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], true);
									}
									mexFillEmpty(plhs, nlhs, 1);
								}
								else
								{
									if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
									{
										if (nlhs > 0)
										{
											this->mexReturnBool(plhs[0], false);
										}
										if (nlhs > 1)
										{
											this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to copy property data to property.", res);
										}
										mexFillEmpty(plhs, nlhs, 2);
									}
									else
									{
										if (nlhs > 0)
										{
											this->mexReturnBool(plhs[0], false);
										}
										if (nlhs > 1)
										{
											this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Tranferring data to componente failed, reason: " +
												jvxAccessProtocol_txt(callGate.access_protocol), JVX_ERROR_INVALID_ARGUMENT);
										}
										mexFillEmpty(plhs, nlhs, 2);
									}
								}
								break;
							/*
							case JVX_DATAFORMAT_SIZE:

								res = copyDataToComponentNumericalSize(prhs[paramId], theTriple, format, numElms, descString.c_str(), offset, callGate.access_protocol);
								if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, descString, theTriple.theProps))
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], true);
									}
									mexFillEmpty(plhs, nlhs, 1);
								}
								else
								{
									if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
									{
										if (nlhs > 0)
										{
											this->mexReturnBool(plhs[0], false);
										}
										if (nlhs > 1)
										{
											this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to copy property data to property.", res);
										}
										mexFillEmpty(plhs, nlhs, 2);
									}
									else
									{
										if (nlhs > 0)
										{
											this->mexReturnBool(plhs[0], false);
										}
										if (nlhs > 1)
										{
											this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Tranferring data to componente failed, reason: " +
												jvxAccessProtocol_txt(callGate.access_protocol), JVX_ERROR_INVALID_ARGUMENT);
										}
										mexFillEmpty(plhs, nlhs, 2);
									}
								}
								break;
								*/
							case JVX_DATAFORMAT_SELECTION_LIST:
							case JVX_DATAFORMAT_STRING:
							case JVX_DATAFORMAT_STRING_LIST:
							case JVX_DATAFORMAT_VALUE_IN_RANGE:
								res = copyDataToComponentOthers(&prhs[paramId], nrhs - paramId, theTriple, descr.format, descr.num, descString.c_str(), offset, &callGate.access_protocol);
								if (JVX_CHECK_PROPERTY_ACCESS_OK(res, callGate.access_protocol, descString, theTriple.theProps))
								{
									if (nlhs > 0)
									{
										this->mexReturnBool(plhs[0], true);
									}
									mexFillEmpty(plhs, nlhs, 1);
								}
								else
								{
									if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
									{
										if (nlhs > 0)
										{
											this->mexReturnBool(plhs[0], false);
										}
										if (nlhs > 1)
										{
											this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to copy property data to property.", res);
										}
										mexFillEmpty(plhs, nlhs, 2);
									}
									else
									{
										if (nlhs > 0)
										{
											this->mexReturnBool(plhs[0], false);
										}
										if (nlhs > 1)
										{
											this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Tranferring data to componente failed, reason: " +
												jvxAccessProtocol_txt(callGate.access_protocol), JVX_ERROR_INVALID_ARGUMENT);
										}
										mexFillEmpty(plhs, nlhs, 2);
									}
								}
								break;
							default:
								if (nlhs > 0)
								{
									this->mexReturnBool(plhs[0], false);
								}
								if (nlhs > 1)
								{
									this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Request for unsupported property type.", JVX_ERROR_UNSUPPORTED);
								}
								mexFillEmpty(plhs, nlhs, 2);
							}
						}
						else
						{
							if (nlhs > 0)
							{
								this->mexReturnBool(plhs[0], false);
							}
							if (nlhs > 1)
							{
								this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to member function <IjvxProperties::description_property> failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
							}
							mexFillEmpty(plhs, nlhs, 2);
						}
					}
					jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				}
				else
				{
					if(nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if(nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified component does not provide interface for properties.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
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
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
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
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
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
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}


jvxErrorType
mexJvxHost::start_property_group(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::set_property_descriptor");
	jvxState theState = JVX_STATE_NONE;
	jvxCallManagerProperties callGate;
	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			std::string cpString;
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;

			std::string descString;

			jvx_propertyReferenceTriple theTriple;

			jvxSize num = 0;
			jvxDataFormat format = JVX_DATAFORMAT_NONE;

			jvxSize numElms = 0;
			jvxBool readonly = true;
			jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
			jvxSize offset = 0;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
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

			
			if (parametersOk)
			{
				jvx_initPropertyReferenceTriple(&theTriple);
				jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);

				if (theTriple.theProps)
				{
					res = theTriple.theProps->reset_property_group(callGate);

					// Map given unique id to one specific property. We need the format and other infos before accessing the content
					res = theTriple.theProps->start_property_group(callGate);

					if(res == JVX_NO_ERROR && (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK))
					{

						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if (nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to member function <IjvxProperties::description_property> failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
					jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				}
				else
				{
					if (nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified component does not provide interface for properties.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::stop_property_group(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::set_property_descriptor");
	jvxState theState = JVX_STATE_NONE;
	jvxCallManagerProperties callGate;
	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			std::string cpString;
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;

			std::string descString;

			jvx_propertyReferenceTriple theTriple;

			jvxSize num = 0;
			jvxDataFormat format = JVX_DATAFORMAT_NONE;

			jvxSize numElms = 0;
			jvxBool readonly = true;
			jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
			jvxSize offset = 0;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
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


			if (parametersOk)
			{
				jvx_initPropertyReferenceTriple(&theTriple);
				jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);

				if (theTriple.theProps)
				{
					// Map given unique id to one specific property. We need the format and other infos before accessing the content
					res = theTriple.theProps->stop_property_group(callGate);

					if(res == JVX_NO_ERROR && (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK))
					{

						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if (nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to member function <IjvxProperties::description_property> failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
					jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				}
				else
				{
					if (nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified component does not provide interface for properties.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::status_property_group(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[], jvxBool spec_offset)
{
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("jvxJvxHost::set_property_descriptor");
	jvxState theState = JVX_STATE_NONE;
	jvxCallManagerProperties callGate;
	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			std::string cpString;
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;

			std::string descString;

			jvx_propertyReferenceTriple theTriple;

			jvxSize num = 0;
			jvxDataFormat format = JVX_DATAFORMAT_NONE;

			jvxSize numElms = 0;
			jvxBool readonly = true;
			jvxPropertyDecoderHintType decHtTp = JVX_PROPERTY_DECODER_NONE;
			jvxSize offset = 0;

			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;
			jvxSize  startCnt = JVX_SIZE_UNSELECTED;

			paramId = 1;
			res = mexArgument2Type<jvxComponentType>(tpC, prhs, paramId, nrhs, JVX_COMPONENT_UNKNOWN, JVX_COMPONENT_ALL_LIMIT, jvxComponentType_str());
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


			if (parametersOk)
			{
				jvx_initPropertyReferenceTriple(&theTriple);
				jvx_getReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);

				if (theTriple.theProps)
				{
					// Map given unique id to one specific property. We need the format and other infos before accessing the content
					res = theTriple.theProps->status_property_group(callGate, &startCnt);

					if(res == JVX_NO_ERROR && (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK))
					{

						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], true);
						}
						mexFillEmpty(plhs, nlhs, 1);
					}
					else
					{
						if (nlhs > 0)
						{
							this->mexReturnBool(plhs[0], false);
						}
						if (nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Call to member function <IjvxProperties::description_property> failed.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
						}
						mexFillEmpty(plhs, nlhs, 2);
					}
					jvx_returnReferencePropertiesObject(involvedHost.hHost, &theTriple, tpAll[tpC]);
				}
				else
				{
					if (nlhs > 0)
					{
						this->mexReturnBool(plhs[0], false);
					}
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Specified component does not provide interface for properties.", JVX_ERROR_CALL_SUB_COMPONENT_FAILED);
					}
					mexFillEmpty(plhs, nlhs, 2);
				}
			}
			else
			{
				if (nlhs > 0)
				{
					this->mexReturnBool(plhs[0], false);
				}
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Mandatory input arguments not specified properly.", JVX_ERROR_INVALID_ARGUMENT);
				}
				mexFillEmpty(plhs, nlhs, 2);
			}
		}
		else
		{
			if (nlhs > 0)
			{
				this->mexReturnBool(plhs[0], false);
			}
			if (nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host needs to be in active state", JVX_ERROR_WRONG_STATE);
			}
			mexFillEmpty(plhs, nlhs, 2);
		}
	}
	else
	{
		if (nlhs > 0)
		{
			this->mexReturnBool(plhs[0], false);
		}
		if (nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}
// ===================================================================================
// ===================================================================================

jvxErrorType 
mexJvxHost::copyDataToComponentNumerical(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, jvxPropertyCategoryType cat, jvxDataFormat format, 
	jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxCallManagerProperties callGate;
	numTypeConvert inputConvert = { 0 };

	switch(format)
	{
	case JVX_DATAFORMAT_DATA:
		if(mxIsData(prhs))
		{
			if(mxGetM(prhs) == 1)
			{
				if(mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		if(mxIsInt64(prhs))
		{
			if(mxGetM(prhs) == 1)
			{
				if(mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		if(mxIsInt32(prhs))
		{
			if(mxGetM(prhs) == 1)
			{
				if(mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		if(mxIsInt16(prhs))
		{
			if(mxGetM(prhs) == 1)
			{
				if(mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_8BIT:
		if(mxIsInt8(prhs))
		{
			if(mxGetM(prhs) == 1)
			{
				if(mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_U64BIT_LE:
		if (mxIsUint64(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_U32BIT_LE:
		if (mxIsUint32(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		if (mxIsUint16(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	case JVX_DATAFORMAT_U8BIT:
		if (mxIsUint8(prhs))
		{
			if (mxGetM(prhs) == 1)
			{
				if (mxGetN(prhs) == numElms)
				{
					data = mxGetData(prhs);
				}
			}
		}
		break;
	}

	if (!data)
	{
		/*
		jvxData singleDat = 0;
		jvxInt8 singleInt8 = 0;
		jvxInt16 singleInt16 = 0;
		jvxInt32 singleInt32 = 0;
		jvxInt64 singleInt64 = 0;
		jvxInt8 singleInt8 = 0;
		jvxUInt16 singleUInt16 = 0;
		jvxUInt32 singleUInt32 = 0;
		jvxUInt64 singleUInt64 = 0;
		*/
		if (numElms == 1)
		{
			if(convertSingleNumericalUnion(format, inputConvert, prhs) == JVX_NO_ERROR)
			{
				data = &inputConvert;
			}
		}
	}

	if (data)
	{
		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(uniqueId, cat);
		jvx::propertyDetail::CjvxTranferDetail trans(true);

		res = theTriple.theProps->set_property(callGate,
			jPRG(data, numElms, format),ident, trans);
		if (accProt)
			*accProt = callGate.access_protocol;
	}
	else
	{
		res =JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType
mexJvxHost::copyDataToComponentNumericalSize(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, jvxPropertyCategoryType cat, 
	jvxDataFormat format, jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxSize* dataSz = NULL;
	jvxCallManagerProperties callGate;
	assert(format == JVX_DATAFORMAT_SIZE);
	if (mxIsInt32(prhs))
	{
		if (mxGetM(prhs) == 1)
		{
			if (mxGetN(prhs) == numElms)
			{
				data = mxGetData(prhs);
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(dataSz, jvxSize, numElms);
				for (i = 0; i < numElms; i++)
				{
					dataSz[i] = JVX_INT_SIZE(((jvxInt32*)data)[i]);
				}
			}
		}
	}

	if (dataSz)
	{
		jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(uniqueId, cat);
		jvx::propertyDetail::CjvxTranferDetail trans(true);


		res = theTriple.theProps->set_property(callGate,
			jPRG(dataSz, numElms, format), ident, trans);
		if (accProt)
			*accProt = callGate.access_protocol;
		JVX_DSP_SAFE_DELETE_FIELD(dataSz);
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType 
mexJvxHost::copyDataToComponentNumerical(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, jvxDataFormat format, jvxSize numElms, 
	const char* descr, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */

	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxCallManagerProperties callGate;
	jvxData* data_dat = NULL;
	jvxSize* data_sz = NULL;
	jvxInt64* data_int64 = NULL;
	jvxInt32* data_int32 = NULL;
	jvxInt16* data_int16 = NULL;
	jvxInt8* data_int8 = NULL;
	jvxUInt64* data_uint64 = NULL;
	jvxUInt32* data_uint32 = NULL;
	jvxUInt16* data_uint16 = NULL;
	jvxUInt8* data_uint8 = NULL;
	jvxBool formatFits = false;
	jvxHandle* data_setprop = NULL;

	if (mxGetM(prhs) == 1)
	{
		if (mxGetN(prhs) == numElms)
		{
			formatFits = true;
		}
	}

	if (formatFits)
	{
		switch (format)
		{
		case JVX_DATAFORMAT_DATA:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_dat, jvxData, numElms);
			data_setprop = data_dat;
			break;
		case JVX_DATAFORMAT_SIZE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_sz, jvxSize, numElms);
			data_setprop = data_sz;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_int64, jvxInt64, numElms);
			data_setprop = data_int64;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_int32, jvxInt32, numElms);
			data_setprop = data_int32;
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_int16, jvxInt16, numElms);
			data_setprop = data_int16;
			break;
		case JVX_DATAFORMAT_8BIT:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_int8, jvxInt8, numElms);
			data_setprop = data_int8;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_uint64, jvxUInt64, numElms);
			data_setprop = data_uint64;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_uint32, jvxUInt32, numElms);
			data_setprop = data_uint32;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_uint16, jvxUInt16, numElms);
			data_setprop = data_uint16;
			break;
		case JVX_DATAFORMAT_U8BIT:
			JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(data_uint8, jvxUInt8, numElms);
			data_setprop = data_uint8;
			break;
		}

		convert_mat_buf_c_buf_1_x_N(data_setprop, format, numElms, prhs);

		if (data_setprop)
		{
			res = jvx_set_property(theTriple.theProps, data_setprop, offset, numElms, format, true, descr, callGate);
			if (accProt)
				*accProt = callGate.access_protocol;
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}

		switch (format)
		{
		case JVX_DATAFORMAT_DATA:
			JVX_DSP_SAFE_DELETE_FIELD(data_dat);
			break;
		case JVX_DATAFORMAT_SIZE:
			JVX_DSP_SAFE_DELETE_FIELD(data_sz);
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_int64);
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_int32);
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_int16);
			break;
		case JVX_DATAFORMAT_8BIT:
			JVX_DSP_SAFE_DELETE_FIELD(data_int8);
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_uint64);
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_uint32);
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			JVX_DSP_SAFE_DELETE_FIELD(data_uint16);
			break;
		case JVX_DATAFORMAT_U8BIT:
			JVX_DSP_SAFE_DELETE_FIELD(data_uint8);
			break;
		}
	}
	return(res);
}

#define JVX_CONVERSION_LOOP(failedTransfer, dat_src, jvx_dest, N, TPConvert, FConvert ) \
	if (dat_src) \
	{ \
		jvxSize i; \
		failedTransfer = false; \
		for (i = 0; i < N; i++) \
		{ \
			TPConvert tmp = (TPConvert)dat_src[i]; \
			jvx_dest[i] = FConvert(tmp); \
		} \
	}

#define JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_src, jvx_dest, N, TPConvert, FConvert, LMax, LMin ) \
	if (dat_src) \
	{ \
		jvxSize i; \
		failedTransfer = false; \
		for (i = 0; i < N; i++) \
		{ \
			TPConvert tmp = (TPConvert)dat_src[i]; \
			tmp = JVX_MIN(tmp, LMax); \
			tmp = JVX_MAX(tmp, LMin); \
			jvx_dest[i] = FConvert(tmp); \
		} \
	}

jvxErrorType
mexJvxHost::convert_mat_buf_c_buf_1_x_N(jvxHandle* data_setprops, jvxDataFormat format, jvxSize N, const mxArray* prhs)
{
	double* dat_dbl = NULL;
	float* dat_flt = NULL;

	jvxInt64* dat_int64 = NULL;
	jvxInt32* dat_int32 = NULL;
	jvxInt16* dat_int16 = NULL;
	jvxInt8* dat_int8 = NULL;
	jvxUInt64* dat_uint64 = NULL;
	jvxUInt32* dat_uint32 = NULL;
	jvxUInt16* dat_uint16 = NULL;
	jvxUInt8* dat_uint8 = NULL;

	jvxData* jvx_data = NULL;
	jvxInt64* jvx_int64 = NULL;
	jvxInt32* jvx_int32 = NULL;
	jvxInt16* jvx_int16 = NULL;
	jvxInt8* jvx_int8 = NULL;
	jvxUInt64* jvx_uint64 = NULL;
	jvxUInt32* jvx_uint32 = NULL;
	jvxUInt16* jvx_uint16 = NULL;
	jvxUInt8* jvx_uint8 = NULL;
	jvxSize* jvx_sz = NULL;

	jvxSize i;

	if (mxIsDouble(prhs))
	{
		dat_dbl = (double*)mxGetData(prhs);
	}
	else if (mxIsSingle(prhs))
	{
		dat_flt = (float*)mxGetData(prhs);
	}
	else if (mxIsInt64(prhs))
	{
		dat_int64 = (jvxInt64*)mxGetData(prhs);
	}
	else if (mxIsInt32(prhs))
	{
		dat_int32 = (jvxInt32*)mxGetData(prhs);
	}
	else if (mxIsInt16(prhs))
	{
		dat_int16 = (jvxInt16*)mxGetData(prhs);
	}
	else if (mxIsInt8(prhs))
	{
		dat_int8 = (jvxInt8*)mxGetData(prhs);
	}
	else if (mxIsUint64(prhs))
	{
		dat_uint64 = (jvxUInt64*)mxGetData(prhs);
	}
	else if (mxIsUint32(prhs))
	{
		dat_uint32 = (jvxUInt32*)mxGetData(prhs);
	}
	else if (mxIsUint16(prhs))
	{
		dat_uint16 = (jvxUInt16*)mxGetData(prhs);
	}
	else if (mxIsUint8(prhs))
	{
		dat_uint8 = (jvxUInt8*)mxGetData(prhs);
	}

	// Now, convert
	switch (format)
	{
	case JVX_DATAFORMAT_DATA:
		jvx_data = (jvxData*)data_setprops;
		break;
	case JVX_DATAFORMAT_64BIT_LE:
		jvx_int64 = (jvxInt64*)data_setprops;
		break;
	case JVX_DATAFORMAT_32BIT_LE:
		jvx_int32 = (jvxInt32*)data_setprops;
		break;
	case JVX_DATAFORMAT_16BIT_LE:
		jvx_int16 = (jvxInt16*)data_setprops;
		break;
	case JVX_DATAFORMAT_8BIT:
		jvx_int8 = (jvxInt8*)data_setprops;
		break;
	case JVX_DATAFORMAT_U64BIT_LE:
		jvx_uint64 = (jvxUInt64*)data_setprops;
		break;
	case JVX_DATAFORMAT_U32BIT_LE:
		jvx_uint32 = (jvxUInt32*)data_setprops;
		break;
	case JVX_DATAFORMAT_U16BIT_LE:
		jvx_uint16 = (jvxUInt16*)data_setprops;
		break;
	case JVX_DATAFORMAT_U8BIT:
		jvx_uint8 = (jvxUInt8*)data_setprops;
		break;
	case JVX_DATAFORMAT_SIZE:
		jvx_sz = (jvxSize*)data_setprops;
		break;
	}

	jvxBool failedTransfer = true;

	if (jvx_data)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_data, N, double, (jvxData));
		}
		else if(dat_flt)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_data, N, jvxData, );
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_data, N, jvxData, );
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_data, N, jvxData, );
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_data, N, jvxData, );
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_data, N, jvxData, );
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_data, N, jvxData, );
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_data, N, jvxData, );
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_data, N, jvxData, );
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_data, N, jvxData, );
		}
	}
	else if (jvx_sz)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_sz, N, jvxData, JVX_DATA2SIZE);
		}
	}
	else if (jvx_int64)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_int64, N, jvxInt64, );
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_int64, N, jvxData, JVX_DATA2INT64);
		}
	}
	else if (jvx_int32)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_int32, N, jvxInt32, );
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_int32, N, jvxData, JVX_DATA2INT32);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int32, N, jvxData, JVX_DATA2INT32);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_int32, N, jvxData, JVX_DATA2INT32, 0x7FFFFFFF, -0x7FFFFFFF);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_int32, N, jvxData, JVX_DATA2INT32);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_int32, N, jvxData, JVX_DATA2INT32);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_int32, N, jvxData, JVX_DATA2INT32);
		}
	}
	else if (jvx_int16)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_int16, N, jvxInt16, );
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int16, N, jvxData, JVX_DATA2INT16);
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint16, jvx_int16, N, jvxData, JVX_DATA2INT16, 0x7FFF, -0x7FFF);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_int16, N, jvxData, JVX_DATA2INT16);
		}
	}
	else if (jvx_int8)
	{
		if (dat_dbl)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_flt)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_int64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_int32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_int16)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int16, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_int8)
		{
			JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_int8, N, jvxInt8, );
		}
		else if (dat_uint64)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_uint32)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_uint16)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint16, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
		else if (dat_uint8)
		{
			JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint8, jvx_int8, N, jvxData, JVX_DATA2INT8, 0x7F, -0x7F);
		}
	}

	else if (jvx_uint64)
	{
	if (dat_dbl)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_dbl, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
	}
	else if (dat_flt)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_flt, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
	}
	else if (dat_int64)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_int64, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
	}
	else if (dat_int32)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_int32, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
	}
	else if (dat_int16)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
	}
	else if (dat_int8)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
	}
	else if (dat_uint64)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_uint64, jvx_uint64, N, jvxUInt64, );
	}
	else if (dat_uint32)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
	}
	else if (dat_uint16)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
	}
	else if (dat_uint8)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint64, N, jvxData, JVX_DATA2UINT64);
	}
	}
	else if (jvx_uint32)
	{
	if (dat_dbl)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
	}
	else if (dat_flt)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
	}
	else if (dat_int64)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
	}
	else if (dat_int32)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
	}
	else if (dat_int16)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_int16, jvx_uint32, N, jvxData, JVX_DATA2UINT32);
	}
	else if (dat_int8)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int8, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
	}
	else if (dat_uint64)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_uint32, N, jvxData, JVX_DATA2UINT32, 0xFFFFFFFF, 0);
	}
	else if (dat_uint32)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_uint32, jvx_uint32, N, jvxUInt32, );
	}
	else if (dat_uint16)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_uint32, N, jvxData, JVX_DATA2UINT32);
	}
	else if (dat_uint8)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint32, N, jvxData, JVX_DATA2UINT32);
	}
	}
	else if (jvx_int16)
	{
	if (dat_dbl)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
	}
	else if (dat_flt)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
	}
	else if (dat_int64)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
	}
	else if (dat_int32)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
	}
	else if (dat_int16)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int16, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
	}
	else if (dat_int8)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_int8, jvx_uint16, N, jvxData, JVX_DATA2UINT16);
	}
	else if (dat_uint64)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
	}
	else if (dat_uint32)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_uint16, N, jvxData, JVX_DATA2UINT16, 0xFFFF, 0);
	}
	else if (dat_uint16)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_uint16, jvx_uint16, N, jvxUInt16, );
	}
	else if (dat_uint8)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint16, N, jvxData, JVX_DATA2UINT16);
	}
	}
	else if (jvx_uint8)
	{
	if (dat_dbl)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_dbl, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
	}
	else if (dat_flt)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_flt, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
	}
	else if (dat_int64)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int64, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
	}
	else if (dat_int32)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int32, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
	}
	else if (dat_int16)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int16, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
	}
	else if (dat_int8)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_int8, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
	}
	else if (dat_uint64)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint64, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
	}
	else if (dat_uint32)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint32, jvx_uint8, N, jvxData, JVX_DATA2UINT8, 0xFF, 0);
	}
	else if (dat_uint16)
	{
		JVX_CONVERSION_LOOP_LIM(failedTransfer, dat_uint16, jvx_uint8, N, jvxData, JVX_DATA2INT8, 0xFF, 0);
	}
	else if (dat_uint8)
	{
		JVX_CONVERSION_LOOP(failedTransfer, dat_uint8, jvx_uint8, N, jvxUInt8, );
	}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
mexJvxHost::copyDataToComponentNumericalSize(const mxArray* prhs, jvx_propertyReferenceTriple& theTriple, jvxDataFormat format, 
	jvxSize numElms, const char* descr, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxSize i;
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxSize* dataSz = NULL;
	jvxCallManagerProperties callGate;
	assert(format == JVX_DATAFORMAT_SIZE);
	if (mxIsInt32(prhs))
	{
		if (mxGetM(prhs) == 1)
		{
			if (mxGetN(prhs) == numElms)
			{
				data = mxGetData(prhs);
				JVX_DSP_SAFE_ALLOCATE_FIELD_CPP_Z(dataSz, jvxSize, numElms);
				for(i = 0; i < numElms; i++)
				{ 
					dataSz[i] = JVX_INT_SIZE(((jvxInt32*)data)[i]);
				}
			}
		}
	}

	if (dataSz)
	{
		res = jvx_set_property(theTriple.theProps, dataSz, offset, numElms, format, true, descr, callGate);
		if (accProt)
		{
			*accProt = callGate.access_protocol;
		}

		JVX_DSP_SAFE_DELETE_FIELD(dataSz);
	}
	else
	{
		res = JVX_ERROR_INVALID_ARGUMENT;
	}
	return(res);
}

jvxErrorType 
mexJvxHost::copyDataToComponentOthers(const mxArray** prhs, int nrhs, jvx_propertyReferenceTriple& theTriple, 
	jvxPropertyCategoryType cat, jvxDataFormat format, jvxSize numElms, jvxSize uniqueId, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxCallManagerProperties callGate;
	jvxApiString fldStr;
	std::string token;
	std::vector<std::string> tokenLst;
	jvxSize numEntries = 0;
	const mxArray* arr = NULL;
	bool errorDetected = false;
	jvxApiStringList fldStrLst;
	jvxSelectionList selList;
	jvxInt64 valI64 =0;
	jvxData valD;
	jvxValueInRange valR;

	jvx::propertyAddress::CjvxPropertyAddressGlobalId ident(uniqueId, cat);
	jvx::propertyDetail::CjvxTranferDetail trans(true);

	switch(format)
	{
	case JVX_DATAFORMAT_STRING:
		if(mxIsChar(prhs[0]))
		{
			token = jvx_mex_2_cstring(prhs[0]);
			fldStr.assign_const(token.c_str(), token.size());
			res = theTriple.theProps->set_property(callGate,
				jPRG(&fldStr, 1, format), ident, trans);
			if (accProt)
			{
				*accProt = callGate.access_protocol;
			}

		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_DATAFORMAT_STRING_LIST:
		res = mexArgument2String(tokenLst, numElms, prhs, 0, 1);
		if(res == JVX_NO_ERROR)
		{
			fldStrLst.assign(tokenLst);
			res = theTriple.theProps->set_property(callGate,
				jPRG(&fldStrLst, 1, format), ident, trans);
			if (accProt)
			{
				*accProt = callGate.access_protocol;
			}

		}
		break;
	case JVX_DATAFORMAT_SELECTION_LIST:
		
		res = mexArgument2String(token, prhs, 0, nrhs);
		if(res == JVX_NO_ERROR)
		{
			jvxBool err = false;
			selList.bitFieldSelected() = jvx_string2BitField(token, err);
			if (err)
			{
				res = JVX_ERROR_PARSE_ERROR;
			}
			else
			{
				res = mexArgument2String(token, prhs, 1, nrhs);
			}
			if(res == JVX_NO_ERROR)
			{
				err = false;
				selList.bitFieldExclusive  = jvx_string2BitField(token, err);
				if (err)
				{
					res = JVX_ERROR_PARSE_ERROR;
				}
				else
				{
					res = mexArgument2StringList(tokenLst, prhs, 2, nrhs);
				}
				if(res == JVX_NO_ERROR)
				{
					selList.strList.assign( tokenLst);
					trans.contentOnly = false;
					res = theTriple.theProps->set_property(callGate,
						jPRG(&selList, 1, format), ident, trans);
					if (accProt)
					{
						*accProt = callGate.access_protocol;
					}

				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = theTriple.theProps->set_property(callGate,
					jPRG(&selList, 1, format), ident, trans);
				if (accProt)
				{
					*accProt = callGate.access_protocol;
				}

			}
		}
		break;
	case JVX_DATAFORMAT_VALUE_IN_RANGE:
		res = mexArgument2Index<jvxData>(valD, prhs, 0, nrhs);
		if(res == JVX_NO_ERROR)
		{
			valR.val() = valD;
			res = mexArgument2Index<jvxData>(valD, prhs, 1, nrhs);
			if(res == JVX_NO_ERROR)
			{
				valR.minVal = valD;
				res = mexArgument2Index<jvxData>(valD, prhs, 2, nrhs);
				if(res == JVX_NO_ERROR)
				{
					valR.maxVal = valD;
					trans.contentOnly = false;
					res = theTriple.theProps->set_property(callGate,
						jPRG(&valR, 1, format), 
						ident, trans);
					if (accProt)
					{
						*accProt = callGate.access_protocol;
					}

				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = theTriple.theProps->set_property(callGate,
					jPRG(&valR, 1, format), ident, trans);
				if (accProt)
				{
					*accProt = callGate.access_protocol;
				}

			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	}
	return(res);
}

jvxErrorType 
mexJvxHost::copyDataToComponentOthers(const mxArray** prhs, int nrhs, jvx_propertyReferenceTriple& theTriple, jvxDataFormat format, jvxSize numElms, 
	const char* descr, jvxSize offset, jvxAccessProtocol* accProt)
{
	/* For numeric properties, expect a simple buffer of values */
	jvxErrorType res = JVX_NO_ERROR;
	jvxHandle* data = NULL;
	jvxCallManagerProperties callGate;
	jvxBitField btfld;
	jvxApiString fldStr;
	std::string token;
	std::vector<std::string> tokenLst;
	jvxSize numEntries = 0;
	const mxArray* arr = NULL;
	bool errorDetected = false;
	jvxApiStringList fldStrLst;
	jvxSelectionList selList;
	jvxInt64 valI64 =0;
	jvxData valD;
	jvxValueInRange valR;

	switch(format)
	{
	case JVX_DATAFORMAT_STRING:
		if(mxIsChar(prhs[0]))
		{
			token = jvx_mex_2_cstring(prhs[0]);
			fldStr.assign_const(token.c_str(), token.size());
			res = jvx_set_property(theTriple.theProps, &fldStr, offset, 1, format, true, descr, callGate);
			if (accProt)
			{
				*accProt = callGate.access_protocol;
			}

		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	case JVX_DATAFORMAT_STRING_LIST:
		res = mexArgument2String(tokenLst, numElms, prhs, 0, 1);
		if(res == JVX_NO_ERROR)
		{
			fldStrLst.assign( tokenLst);
			res = jvx_set_property(theTriple.theProps, &fldStrLst, offset, 1, format, true, descr, callGate);
			if (accProt)
			{
				*accProt = callGate.access_protocol;
			}

		}
		break;
	case JVX_DATAFORMAT_SELECTION_LIST:
		res = mexArgument2String(token, prhs, 0, nrhs);
		if (res == JVX_NO_ERROR)
		{
			jvxBool err = false;
			btfld = jvx_string2BitField(token, err);
			if (err)
			{
				res = JVX_ERROR_PARSE_ERROR;
			}
		}
		else
		{
			res = mexArgument2Index<jvxInt64>(valI64, prhs, 0, nrhs);
			btfld = (jvxBitField)valI64;
		}
		selList.bitFieldSelected() = btfld;
		if(res == JVX_NO_ERROR)
		{
			res = mexArgument2String(token, prhs, 1, nrhs);
			if (res == JVX_NO_ERROR)
			{
				jvxBool err = false;
				btfld = jvx_string2BitField(token, err);
				if (err)
				{
					res = JVX_ERROR_PARSE_ERROR;
				}
			}
			else
			{
				res = mexArgument2Index<jvxInt64>(valI64, prhs, 1, nrhs);
				btfld = (jvxBitField)valI64;
			}
			selList.bitFieldExclusive = btfld;
			
			if(res == JVX_NO_ERROR)
			{
				res = mexArgument2StringList(tokenLst, prhs, 2, nrhs);
				if(res == JVX_NO_ERROR)
				{
					selList.strList.assign(tokenLst);
					res = jvx_set_property(theTriple.theProps, &selList, offset, 1, format, false, descr, callGate);
					if (accProt)
					{
						*accProt = callGate.access_protocol;
					}

				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = jvx_set_property(theTriple.theProps, &selList, offset, 1, format, true, descr, callGate);
				if (accProt)
				{
					*accProt = callGate.access_protocol;
				}

			}
		}
		break;
	case JVX_DATAFORMAT_VALUE_IN_RANGE:
		res = mexArgument2Index<jvxData>(valD, prhs, 0, nrhs);
		if(res == JVX_NO_ERROR)
		{
			valR.val() = valD;
			res = mexArgument2Index<jvxData>(valD, prhs, 1, nrhs);
			if(res == JVX_NO_ERROR)
			{
				valR.minVal = valD;
				res = mexArgument2Index<jvxData>(valD, prhs, 2, nrhs);
				if(res == JVX_NO_ERROR)
				{
					valR.maxVal = valD;
					res = jvx_set_property(theTriple.theProps, &valR, offset, 1, format, false, descr, callGate);
					if (accProt)
					{
						*accProt = callGate.access_protocol;
					}

				}
				else
				{
					res = JVX_ERROR_INVALID_ARGUMENT;
				}
			}
			else
			{
				res = jvx_set_property(theTriple.theProps, &valR, offset, 1, format, true, descr, callGate);
				if (accProt)
				{
					*accProt = callGate.access_protocol;
				}
			}
		}
		else
		{
			res = JVX_ERROR_INVALID_ARGUMENT;
		}
		break;
	}
	return(res);
}

jvxErrorType
mexJvxHost::convertSingleNumericalUnion(jvxDataFormat format, numTypeConvert& inputConvert, const mxArray* prhs)
{
	jvxErrorType res = JVX_ERROR_INVALID_FORMAT;
	void* data = mxGetData(prhs);
	if ((mxGetM(prhs) == 1) && (mxGetN(prhs) == 1))
	{
		res = JVX_NO_ERROR;
		switch (format)
		{
		case JVX_DATAFORMAT_DATA:
			if (mxIsData(prhs)) inputConvert.singleDat = (jvxData) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleDat = (jvxData) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleDat = (jvxData) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleDat = (jvxData) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleDat = (jvxData) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleDat = (jvxData) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleDat = (jvxData) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleDat = (jvxData) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleDat = (jvxData) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;

		case JVX_DATAFORMAT_8BIT:
			if (mxIsData(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleInt8 = (jvxInt8) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_16BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleInt16 = (jvxInt16) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_32BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleInt32 = (jvxInt32) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_64BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleInt64 = (jvxInt64) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;

		case JVX_DATAFORMAT_U8BIT:
			if (mxIsData(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleUInt8 = (jvxUInt8) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_U16BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleUInt16 = (jvxUInt16) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_U32BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleUInt32 = (jvxUInt32) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;
		case JVX_DATAFORMAT_U64BIT_LE:
			if (mxIsData(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxData*)data;
			else if (mxIsInt8(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxInt8*)data;
			else if (mxIsInt16(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxInt16*)data;
			else if (mxIsInt32(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxInt32*)data;
			else if (mxIsInt64(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxInt64*)data;
			else if (mxIsUint8(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxUInt8*)data;
			else if (mxIsUint16(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxUInt16*)data;
			else if (mxIsUint32(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxUInt32*)data;
			else if (mxIsUint64(prhs)) inputConvert.singleUInt64 = (jvxUInt64) * (jvxUInt64*)data;
			else res = JVX_ERROR_INVALID_ARGUMENT;
			break;

		default:
			return JVX_ERROR_INVALID_ARGUMENT;

		}
	}
	return res;
}
