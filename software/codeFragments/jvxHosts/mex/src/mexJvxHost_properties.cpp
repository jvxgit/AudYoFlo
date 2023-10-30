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

