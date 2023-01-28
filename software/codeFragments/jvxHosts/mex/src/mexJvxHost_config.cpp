#include "mexJvxHost.h"
#include "jvxHost_config.h"
#include "realtimeViewer_helpers.h"

jvxErrorType 
mexJvxHost::configurationFromFile(std::string fName, IjvxHost* theHost,IjvxToolsHost* theTools, IjvxReport* theReport)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string content;
	jvxConfigData* datTmp1 = NULL, *datTmp2 = NULL;
	jvxApiString fldStr;
	IjvxConfiguration* cfg = NULL;
	std::string secName;
	std::vector<std::string> warnings;
	IjvxObject* cryptObj = NULL;
	IjvxCrypt* cryptRef = NULL;
	jvxCallManagerConfiguration callConf;
	theTools->reference_tool(JVX_COMPONENT_CRYPT, &cryptObj, 0, NULL);
	if(cryptObj)
	{
		cryptObj->request_specialization(reinterpret_cast<jvxHandle**>(cryptRef), NULL, NULL);
	}

	//res = jvx_readContentFromFile(fName, content, cryptRef);
	
	callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
	if(cryptRef)
	{
		theTools->return_reference_tool(JVX_COMPONENT_CRYPT, cryptObj);
	}

	if(res == JVX_NO_ERROR)
	{
		IjvxObject* obj = NULL;
		IjvxConfigProcessor* cfgProc = NULL;

		theTools->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &obj, 0, NULL);
		if(obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if(cfgProc)
			{
				//res = cfgProc->parseTextField(content.c_str(), fName.c_str(), 0);
				res = cfgProc->parseFile(fName.c_str());
				if(res == JVX_NO_ERROR)
				{
					datTmp1 = NULL;
					res = cfgProc->getConfigurationHandle(&datTmp1);
					if((res == JVX_NO_ERROR) && (datTmp1))
					{
						cfgProc->getNameCurrentEntry(datTmp1, &fldStr);

						secName = fldStr.std_str();
						if (JVX_QT_HOST_1_SECTION_MAIN == secName)
						{
							// Yes, I found the right section..

							// Obtain all host specific contents here.

							// Next, pass the reserved subsection to used host
							datTmp2 = NULL;
							cfgProc->getReferenceSubsectionCurrentSection_name(datTmp1, &datTmp2, JVX_QT_HOST_1_SECTION_COMPONENTS);
							if (datTmp2)
							{
								res = theHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
								if ((res == JVX_NO_ERROR) && cfg)
								{
									cfg->put_configuration(&callConf, cfgProc, datTmp2, fName.c_str(), 0);
									theHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);
								}
								else
								{
									theReport->report_simple_text_message("Failed to obtain configuration interface from host.", JVX_REPORT_PRIORITY_WARNING);
									res = JVX_ERROR_INTERNAL;
								}
							}
							else
							{
								theReport->report_simple_text_message("Failed to obtain subsection for involved host from config file.", JVX_REPORT_PRIORITY_WARNING);
								res = JVX_ERROR_ELEMENT_NOT_FOUND;
							}

							/*
							 * Realtime viewer does not make sense in Matlab host. However, we handle it here to keep those entries in the
							 * configuration file.*/
							viewer_plots._clear();
							viewer_plots._put_configuration(&callConf, cfgProc, datTmp1, theHost, fName.c_str(), 0, warnings, static_putConfiguration, "plots");
							viewer_props._clear();
							viewer_props._put_configuration(&callConf, cfgProc, datTmp1, theHost, fName.c_str(), 0, warnings, static_putConfiguration, "props");
						}
						else
						{
							theReport->report_simple_text_message("Failed to obtain section for jvx-qt-host-1 from config file.", JVX_REPORT_PRIORITY_ERROR);
							res = JVX_ERROR_ELEMENT_NOT_FOUND;
						}

						cfgProc->removeHandle(datTmp1);
						datTmp1 = NULL;
					}
					else
					{
						res = JVX_ERROR_ELEMENT_NOT_FOUND;
					}
				}
				else
				{
					jvxApiError err;
					switch (res)
					{
					case JVX_ERROR_OPEN_FILE_FAILED:
						theReport->report_simple_text_message(("Opening config file <" + fName + "> failed.").c_str(), JVX_REPORT_PRIORITY_ERROR);
						break;
					case JVX_ERROR_PARSE_ERROR:
						theReport->report_simple_text_message("Parse error in config file:", JVX_REPORT_PRIORITY_ERROR);
						cfgProc->getParseError(&err);
						theReport->report_simple_text_message(err.errorDescription.c_str(), JVX_REPORT_PRIORITY_ERROR);
						break;
					default:
						theReport->report_simple_text_message(("Error in reading configuration from file <" + fName + ">.").c_str(), JVX_REPORT_PRIORITY_ERROR);
					}

					res = JVX_ERROR_INTERNAL;
				}
			}
			else
			{
				theReport->report_simple_text_message("Failed to config file processor tool specialization.", JVX_REPORT_PRIORITY_ERROR);
				res = JVX_ERROR_INTERNAL;
			}
			theTools->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, obj);
			cfgProc = NULL;
		}
		else
		{
			theReport->report_simple_text_message("Failed to access config file processor tool.", JVX_REPORT_PRIORITY_ERROR);
			res = JVX_ERROR_UNSUPPORTED;
		}
		obj = NULL;
	}
	else
	{
		res = JVX_ERROR_OPEN_FILE_FAILED;
	}

	if(res == JVX_NO_ERROR)
	{
		theReport->report_simple_text_message(("Successfully used configuration file " + fName).c_str(), JVX_REPORT_PRIORITY_SUCCESS);
	}
	else
	{
		theReport->report_simple_text_message(("Error when using configuration file " + fName + ": " + jvxErrorType_txt(res)).c_str(), JVX_REPORT_PRIORITY_WARNING);
	}
	return(res);
}

jvxErrorType 
mexJvxHost::configurationToFile(std::string& fName, IjvxHost* theHost, IjvxToolsHost* theTools, IjvxReport* theReport)
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string content;
	jvxCallManagerConfiguration callConf;
	callConf.configModeFlags = JVX_CONFIG_MODE_FULL;

	if(res == JVX_NO_ERROR)
	{
		IjvxObject* obj = NULL;
		IjvxConfigProcessor* cfgProc = NULL;
		IjvxConfiguration* cfg = NULL;
		jvxConfigData* datTmp1 = NULL, * datTmp2 = NULL;
		jvxApiString fldStr;

		theTools->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR,  &obj, 0, NULL);
		if(obj)
		{
			obj->request_specialization((jvxHandle**)&cfgProc, NULL, NULL);
			if(cfgProc)
			{
				cfg = NULL;
				theHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle**)&cfg);
				if(cfg)
				{
					datTmp1 = NULL;
					cfgProc->createEmptySection(&datTmp1, JVX_QT_HOST_1_SECTION_MAIN);
	
					// Add all specific parameters here

					cfgProc->createEmptySection(&datTmp2, JVX_QT_HOST_1_SECTION_COMPONENTS);
					if(datTmp2)
					{
						cfg->get_configuration(&callConf, cfgProc, datTmp2);
					}
					cfgProc->addSubsectionToSection(datTmp1, datTmp2);
					theHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, (jvxHandle*)cfg);

					viewer_plots._get_configuration(&callConf, cfgProc, datTmp1, theHost, static_getConfiguration, "plots");
					viewer_props._get_configuration(&callConf, cfgProc, datTmp1, theHost, static_getConfiguration, "props");

					res = cfgProc->printConfiguration(datTmp1, &fldStr, JVX_QT_HOST_1_PRINT_COMPACT_FORM);
					if(res == JVX_NO_ERROR)
					{
						content = fldStr.std_str();
					}
					cfgProc->removeHandle(datTmp1);
				}
			}
			else
			{
				theReport->report_simple_text_message("Failed to config file processor tool specialization.", JVX_REPORT_PRIORITY_ERROR);
				res = JVX_ERROR_INTERNAL;
			}
			theTools->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, obj);
			cfgProc = NULL;
		}
		else
		{
			theReport->report_simple_text_message("Failed to access config file processor tool.", JVX_REPORT_PRIORITY_ERROR);
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
		}
		obj = NULL;
	}

	if(res == JVX_NO_ERROR)
	{
		res = jvx_writeContentToFile(fName, content);
		if(res == JVX_NO_ERROR)
		{
			theReport->report_simple_text_message(("Successfully written configuration file " + fName).c_str(), JVX_REPORT_PRIORITY_SUCCESS);
		}
		else
		{
			theReport->report_simple_text_message(("Failed to write configuration content to file " + fName).c_str(), JVX_REPORT_PRIORITY_ERROR);
		}
	}
	return(res);
}

jvxErrorType 
mexJvxHost::set_config_entry(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i,j;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	std::string content;
	std::vector<std::string> content_list;
	jvxDataFormat form = JVX_DATAFORMAT_NONE;
	jvxHandle* fldDat = NULL;
	jvxValue val;
	jvxApiValueList valLst;
	jvxInt32 dimX;
	jvxInt32 dimY;
	jvxConfigData* datTmp1 = NULL, *datTmp2 = NULL;
	jvxApiString fldStr;
	jvxApiStringList fldStrLst;
	jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	std::string entryname;
	jvxBool isDouble = false;
	jvxSize paramId = 0;
	IjvxConfigurationExtender* cfgExt = NULL;

	if(involvedComponents.theHost.hFHost)
	{
		if(nrhs == 3)
		{
			paramId = 1;
			if(mxIsChar(prhs[paramId]))
			{
				 entryname = jvx_mex_2_cstring(prhs[paramId]);

				 paramId++;
				 resL = mexArgument2String(content, prhs, paramId,nrhs);
				 if(resL == JVX_NO_ERROR)
				 {
					 tp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING;
				 }
				 else
				 {
					 resL = mexArgument2StringList(content_list, prhs, paramId,nrhs);
					 if(resL == JVX_NO_ERROR)
					 {
						 tp = JVX_CONFIG_SECTION_TYPE_STRINGLIST;
					 }
					 else
					 {
						if(mxIsDouble(prhs[paramId]))
						{
							tp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
							fldDat = mxGetData(prhs[paramId]);
							isDouble = true;
							form = JVX_DATAFORMAT_DATA;
						}
						if(mxIsSingle(prhs[paramId]))
						{
							tp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
							fldDat = mxGetData(prhs[paramId]);
							isDouble = false;
							form = JVX_DATAFORMAT_DATA;
						}
						if(mxIsInt32(prhs[paramId]))
						{
							tp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
							fldDat = mxGetData(prhs[paramId]);
							form = JVX_DATAFORMAT_32BIT_LE;
						}
						if(mxIsInt64(prhs[paramId]))
						{
							tp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
							fldDat = mxGetData(prhs[paramId]);
							form = JVX_DATAFORMAT_64BIT_LE;
						}
						if(mxIsInt16(prhs[paramId]))
						{
							tp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
							fldDat = mxGetData(prhs[paramId]);
							form = JVX_DATAFORMAT_16BIT_LE;
						}
						if(mxIsInt8(prhs[paramId]))
						{
							tp = JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE;
							fldDat = mxGetData(prhs[1]);
							form = JVX_DATAFORMAT_8BIT;
						}
						if(tp == JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE)
						{
							dimY = (jvxInt32)mxGetM(prhs[paramId]);
							dimX = (jvxInt32)mxGetN(prhs[paramId]);
							if(!(
								(dimY == 1) && (dimX == 1)))
							{
								tp = JVX_CONFIG_SECTION_TYPE_VALUELIST;
							}
						}
					 }
				 }
			}
		}

		res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle**)&cfgExt);
		if ((res == JVX_NO_ERROR) && cfgExt)
		{
			switch (tp)
			{
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
				switch (form)
				{
				case JVX_DATAFORMAT_DATA:
					if (isDouble)
					{
						val = jvxValue((jvxData)(*((double*)fldDat)));
					}
					else
					{
						val = jvxValue((jvxData)(*((float*)fldDat)));
					}
					break;
				case JVX_DATAFORMAT_32BIT_LE:
					val = jvxValue(*((jvxInt32*)fldDat));
					break;
				case JVX_DATAFORMAT_16BIT_LE:
					val = jvxValue(*((jvxInt16*)fldDat));
					break;
				case JVX_DATAFORMAT_64BIT_LE:
					val = jvxValue(*((jvxInt64*)fldDat));
					break;
				case JVX_DATAFORMAT_8BIT:
					val = jvxValue(*((jvxInt8*)fldDat));
					break;
				}
				res = cfgExt->set_configuration_entry(entryname.c_str(), &val, tp, 0);
				break;
			case JVX_CONFIG_SECTION_TYPE_VALUELIST:
				valLst.create(dimX);

				// Clear all entries in this list
				res = cfgExt->set_configuration_entry(entryname.c_str(), NULL, tp, -1);
				for (i = 0; i < dimY; i++)
				{
					switch (form)
					{
					case JVX_DATAFORMAT_DATA:
						if (isDouble)
						{
							for (j = 0; j < dimX; j++)
							{
								valLst.set_elm_at(j, jvxValue((jvxData)((double*)fldDat)[i*dimY + j]));
							}
						}
						else
						{
							for (j = 0; j < dimX; j++)
							{
								valLst.set_elm_at(j, jvxValue((jvxData)((float*)fldDat)[i*dimY + j]));
							}
						}
						break;
					case JVX_DATAFORMAT_32BIT_LE:
						for (j = 0; j < dimX; j++)
						{
							valLst.set_elm_at(j, jvxValue(((jvxInt32*)fldDat)[i*dimY + j]));
						}
						break;
					case JVX_DATAFORMAT_16BIT_LE:
						for (j = 0; j < dimX; j++)
						{
							valLst.set_elm_at(j,jvxValue(((jvxInt16*)fldDat)[i*dimY + j]));
						}
						break;
					case JVX_DATAFORMAT_64BIT_LE:
						for (j = 0; j < dimX; j++)
						{
							valLst.set_elm_at(j, jvxValue(((jvxInt64*)fldDat)[i*dimY + j]));
						}
						break;
					case JVX_DATAFORMAT_8BIT:
						for (j = 0; j < dimX; j++)
						{
							valLst.set_elm_at(j, jvxValue(((jvxInt8*)fldDat)[i*dimY + j]));
						}
						break;
					}
					res = cfgExt->set_configuration_entry(entryname.c_str(), &valLst, tp, JVX_SIZE_INT32(i));
				}
				break;
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
				fldStr.assign_const(content.c_str(), content.size());
				res = cfgExt->set_configuration_entry(entryname.c_str(), &fldStr, tp, 0);
				break;
			case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
				fldStrLst.assign( content_list);
				res = cfgExt->set_configuration_entry(entryname.c_str(), &fldStrLst, tp, 0);
				break;
			default:
				res = JVX_ERROR_INVALID_ARGUMENT;
			}
			involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle*)cfgExt);
			cfgExt = NULL;
		}
		else
		{
			// No access to configuration extender available
			assert(0);
			res = JVX_ERROR_UNSUPPORTED;
		}

		if(res == JVX_NO_ERROR)
		{
			if(nlhs > 0)
			{
				this->mexReturnBool(plhs[0], true);
			}
			mexFillEmpty(plhs,nlhs,1);
		}
		else
		{
			if(nlhs > 0)
				this->mexReturnBool(plhs[0], false);
			if(nlhs > 1)
			{
				this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + jvxErrorType_descr(res), res);
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
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
mexJvxHost::get_config_entry(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxSize i,j;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT(__FUNCTION__);
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;
	std::string content;
	std::vector<std::string> content_list;
	jvxDataFormat form = JVX_DATAFORMAT_NONE;
	jvxHandle* fldDat = NULL;
	jvxData valD;
	jvxValue valV;
	jvxInt32 dimX;
	jvxInt32 dimY;
	jvxConfigData* datTmp1 = NULL, *datTmp2 = NULL;
	jvxApiString fldStr;
	jvxApiStringList fldStrLst;
	jvxApiValueList valL;
	jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	std::string entryname;
	jvxBool isDouble = false;
	std::vector<std::vector<jvxData>> tmpData;
	std::vector<jvxData> tmpVector;
	jvxSize cnt = 0;
	jvxSize maxSize = 0;
	jvxSize paramId = 0;
	IjvxConfigurationExtender* cfgExt = NULL;
	if(involvedComponents.theHost.hFHost)
	{
		if(nrhs == 2)
		{
			paramId = 1;
			if(mxIsChar(prhs[paramId]))
			{
				entryname = jvx_mex_2_cstring(prhs[paramId]);
				tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;

				res = involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle**)&cfgExt);
				if ((res == JVX_NO_ERROR) && cfgExt)
				{
					res = cfgExt->get_configuration_entry(entryname.c_str(), NULL, &tp, 0);
					switch (tp)
					{
					case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
						fldDat = reinterpret_cast<jvxHandle*>(&fldStr);
						break;
					case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:
						fldDat = reinterpret_cast<jvxHandle*>(&valV);// Dirty hack...
						break;
					case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
						fldDat = reinterpret_cast<jvxHandle*>(&fldStrLst);
						break;
					case JVX_CONFIG_SECTION_TYPE_VALUELIST:
						fldDat = reinterpret_cast<jvxHandle*>(&valL);
						break;
					}
					res = cfgExt->get_configuration_entry(entryname.c_str(), fldDat, &tp, 0);
					if (res == JVX_NO_ERROR)
					{
						switch (tp)
						{
						case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
							content = fldStr.std_str();
							if (nlhs > 0)
							{
								this->mexReturnBool(plhs[0], true);
							}
							if (nlhs > 1)
							{
								mexReturnString(plhs[1], content);
							}
							break;

						case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTVALUE:

							if (nlhs > 0)
							{
								this->mexReturnBool(plhs[0], true);
							}
							if (nlhs > 1)
							{
								valV.toContent(&valD);
								mexReturnData(plhs[1], valD);
							}
							break;

						case JVX_CONFIG_SECTION_TYPE_STRINGLIST:

							for (i = 0; i < fldStrLst.ll(); i++)
							{
								content_list.push_back(fldStrLst.std_str_at(i));
							}
							if (nlhs > 0)
							{
								this->mexReturnBool(plhs[0], true);
							}
							if (nlhs > 1)
							{
								mexReturnStringCell(plhs[1], content_list);
							}
							break;
						case JVX_CONFIG_SECTION_TYPE_VALUELIST:

							cnt = 1;
							do
							{
								tmpVector.clear();
								if (valL.ll() > maxSize)
								{
									maxSize = (valL.ll());
								}
								for (i = 0; i < valL.ll(); i++)
								{
									valL.elm_at(i).toContent(&valD);
									tmpVector.push_back(valD);
								}
								tmpData.push_back(tmpVector);
								res = cfgExt->get_configuration_entry(entryname.c_str(), fldDat, &tp, cnt);
								cnt++;
							} while (res == JVX_NO_ERROR);

							dimY = JVX_SIZE_INT32(tmpData.size());
							dimX = JVX_SIZE_INT32(maxSize);
							if (nlhs > 0)
							{
								this->mexReturnBool(plhs[0], true);
							}
							if (dimX * dimY)
							{
								if (nlhs > 1)
								{
									SZ_MAT_TYPE ndim = 2;
									SZ_MAT_TYPE dims[2] = { 1, 1 };

									dims[0] = dimY;
									dims[1] = dimX;
									plhs[1] = mxCreateNumericArray(ndim, dims, JVX_DATA_MEX_CONVERT, mxREAL);
									jvxData* ptrWrite = (jvxData*)mxGetData(plhs[1]);
									for (i = 0; i < dimY; i++)
									{
										for (j = 0; j < dimX; j++)
										{
											valD = 0.0;
											if (i < tmpData.size())
											{
												if (j < tmpData[i].size())
												{
													valD = tmpData[i][j];
												}
											}

											ptrWrite[i*dimY + j] = valD;
										}
									}
								}
							}
							else
							{
								this->mexFillEmpty(plhs, 2, 1);
							}
							break;
						}
						this->mexFillEmpty(plhs, nlhs, 2);
					}
					else
					{
						if (nlhs > 0)
							this->mexReturnBool(plhs[0], false);
						if (nlhs > 1)
						{
							this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Failed to return config entry", res);
						}
					}
					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_CONFIGURATION_EXTENDER, (jvxHandle*)cfgExt);
					cfgExt = NULL;
				}
				else
				{
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Configuration extension not accessible.", JVX_ERROR_UNSUPPORTED);
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
			this->mexReturnBool(plhs[0], false);
		if(nlhs > 1)
		{
			this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Wrong state for call: Host reference is not valid", JVX_ERROR_WRONG_STATE);
		}
		mexFillEmpty(plhs, nlhs, 2);
	}
	return(JVX_NO_ERROR);
}

jvxErrorType 
mexJvxHost::export_config_struct(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("mexJvxHost::info_property");
	jvxState theState = JVX_STATE_NONE;
	std::string txt;
	jvxCallManagerConfiguration callConf;
	int foffset = 2;

	callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
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

			if (parametersOk)
			{
				theState = JVX_STATE_NONE;
				involvedHost.hHost->state_selected_component(tpAll[tpC], &theState);
				if (theState > JVX_STATE_INIT)
				{
					IjvxConfiguration* iface = NULL;
					involvedHost.hHost->request_hidden_interface_selected_component(tpAll[tpC], JVX_INTERFACE_CONFIGURATION, reinterpret_cast<jvxHandle**>(&iface));
					if (iface)
					{
						IjvxToolsHost* theTools = NULL;
						involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theTools));
						if (theTools)
						{
							IjvxObject* theCfgObj = NULL;
							theTools->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &theCfgObj, 0, NULL);
							if (theCfgObj)
							{
								IjvxConfigProcessor* theProc = NULL;
								theCfgObj->request_specialization(reinterpret_cast<jvxHandle**>(&theProc), NULL, NULL);
								if (theProc)
								{
									jvxHandle* dataEmpty = NULL;
									theProc->createEmptySection(&dataEmpty, "THE_CONFIG_SECTION");
									callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
									jvxErrorType resL = iface->get_configuration(&callConf, theProc, dataEmpty);
									if (resL == JVX_NO_ERROR)
									{
										if (nlhs > 0)
											this->mexReturnBool(plhs[0], true);
										if (dataEmpty)
										{
											if (nlhs > 1)
												this->mexReturnStructSection(plhs[1], dataEmpty, theProc);
											theProc->removeHandle(dataEmpty);
										}
										else
										{
											foffset = 1;
										}
									}
									else
									{
										if (nlhs > 0)
											this->mexReturnBool(plhs[0], false);
										if (nlhs > 1)
											this->mexReturnAnswerNegativeResult(plhs[1], "Failed to return config description from component in <get_configuration>.", resL);
									}
								}
								else
								{
									if (nlhs > 0)
										this->mexReturnBool(plhs[0], false);
									if (nlhs > 1)
										this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain handle to config processor specialization.", JVX_ERROR_INTERNAL);
								}

								theTools->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, theCfgObj);
							}
							else
							{
								if (nlhs > 0)
									this->mexReturnBool(plhs[0], false);
								if (nlhs > 1)
									this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain handle to config processor object.", JVX_ERROR_INTERNAL);
							}

							involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theTools));
						}
						else
						{
							if (nlhs > 0)
								this->mexReturnBool(plhs[0], false);
							if (nlhs > 1)
								this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain handle to tools host.", JVX_ERROR_INTERNAL);
						}

						involvedHost.hHost->return_hidden_interface_selected_component(tpAll[tpC], JVX_INTERFACE_CONFIGURATION, reinterpret_cast<jvxHandle*>(iface));
					}
					else
					{
						if (nlhs > 0)
							this->mexReturnBool(plhs[0], false);
						if (nlhs > 1)
							this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain component configuration interface.", JVX_ERROR_INTERNAL);
					}

				}
				else
				{
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Component is not in state init or higher.", JVX_ERROR_INVALID_ARGUMENT);
					}

				}
			}
			else
			{
				if (nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_INVALID_ARGUMENT);
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
	mexFillEmpty(plhs, nlhs, foffset);
	return(JVX_NO_ERROR);
}


jvxErrorType
mexJvxHost::export_config_string(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("mexJvxHost::info_property");
	jvxState theState = JVX_STATE_NONE;
	std::string txt;
	jvxApiString fldStr;
	jvxCallManagerConfiguration callConf;
	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
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

			if (parametersOk)
			{
				theState = JVX_STATE_NONE;
				involvedHost.hHost->state_selected_component(tpAll[tpC], &theState);
				if (theState > JVX_STATE_INIT)
				{
					IjvxConfiguration* iface = NULL;
					involvedHost.hHost->request_hidden_interface_selected_component(tpAll[tpC], JVX_INTERFACE_CONFIGURATION, reinterpret_cast<jvxHandle**>(&iface));
					if (iface)
					{
						IjvxToolsHost* theTools = NULL;
						involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theTools));
						if (theTools)
						{
							IjvxObject* theCfgObj = NULL;
							theTools->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &theCfgObj, 0, NULL);
							if (theCfgObj)
							{
								IjvxConfigProcessor* theProc = NULL;
								theCfgObj->request_specialization(reinterpret_cast<jvxHandle**>(&theProc), NULL, NULL);
								if (theProc)
								{
									jvxHandle* dataEmpty = NULL;
									theProc->createEmptySection(&dataEmpty, "THE_CONFIG_SECTION");
									callConf.configModeFlags = JVX_CONFIG_MODE_FULL;
									jvxErrorType resL = iface->get_configuration(&callConf, theProc, dataEmpty);
									if (resL == JVX_NO_ERROR)
									{
										theProc->printConfiguration(dataEmpty, &fldStr, false);
										txt = fldStr.std_str();
										if (dataEmpty)
										{
											theProc->removeHandle(dataEmpty);
										}

										if (nlhs > 0)
											this->mexReturnBool(plhs[0], true);
										if (nlhs > 1)
											this->mexReturnString(plhs[1], txt);
									}
									else
									{
										if (nlhs > 0)
											this->mexReturnBool(plhs[0], false);
										if (nlhs > 1)
											this->mexReturnAnswerNegativeResult(plhs[1], "Failed to return config description from component in <get_configuration>.", resL);
									}
								}
								else
								{
									if (nlhs > 0)
										this->mexReturnBool(plhs[0], false);
									if (nlhs > 1)
										this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain handle to config processor specialization.", JVX_ERROR_INTERNAL);
								}

								theTools->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, theCfgObj);
							}
							else
							{
								if (nlhs > 0)
									this->mexReturnBool(plhs[0], false);
								if (nlhs > 1)
									this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain handle to config processor object.", JVX_ERROR_INTERNAL);
							}

							involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theTools));
						}
						else
						{
							if (nlhs > 0)
								this->mexReturnBool(plhs[0], false);
							if (nlhs > 1)
								this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain handle to tools host.", JVX_ERROR_INTERNAL);
						}

						involvedHost.hHost->return_hidden_interface_selected_component(tpAll[tpC], JVX_INTERFACE_CONFIGURATION, reinterpret_cast<jvxHandle*>(iface));
					}
					else
					{
						if (nlhs > 0)
							this->mexReturnBool(plhs[0], false);
						if (nlhs > 1)
							this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain component configuration interface.", JVX_ERROR_INTERNAL);
					}

				}
				else
				{
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if (nlhs > 1)
					{
						this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Component is not in state init or higher.", JVX_ERROR_INVALID_ARGUMENT);
					}
				}
			}
			else
			{
				if (nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_INVALID_ARGUMENT);
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
	mexFillEmpty(plhs, nlhs, 2);
	return(JVX_NO_ERROR);
}

jvxErrorType
mexJvxHost::convert_string_config_struct(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
	jvxErrorType res = JVX_NO_ERROR;
	std::string internalErrorMessage = ERROR_MESSAGE_REPORT("mexJvxHost::info_property");
	jvxState theState = JVX_STATE_NONE;
	std::string txt;
	std::string configStr;
	int foffset = 2;

	if (involvedComponents.theHost.hFHost)
	{
		// Make sure everything is as expected
		involvedComponents.theHost.hFHost->state(&theState);
		if (theState == JVX_STATE_ACTIVE)
		{
			jvxComponentType tpC = JVX_COMPONENT_UNKNOWN;
			bool parametersOk = true;
			jvxInt32 valI = -1;
			jvxInt32 paramId = 0;

			paramId = 1;
			res = mexArgument2String(configStr, prhs, paramId, nrhs);
			if (res != JVX_NO_ERROR)
			{
				parametersOk = false;
				MEX_PARAMETER_ERROR("config-string", paramId, "char");
			}

			if (parametersOk)
			{
				IjvxToolsHost* theTools = NULL;
				involvedComponents.theHost.hFHost->request_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle**>(&theTools));
				if (theTools)
				{
					IjvxObject* theCfgObj = NULL;
					theTools->reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, &theCfgObj, 0, NULL);
					if (theCfgObj)
					{
						IjvxConfigProcessor* theProc = NULL;
						theCfgObj->request_specialization(reinterpret_cast<jvxHandle**>(&theProc), NULL, NULL);
						if (theProc)
						{
							jvxErrorType resL = JVX_NO_ERROR;
							jvxHandle* datParsed = NULL;
							jvxApiError myErr;
							resL = theProc->parseTextField(configStr.c_str(), "mex input", 1);
							switch (resL)
							{
							case JVX_NO_ERROR:
								theProc->getConfigurationHandle(&datParsed);
								if (nlhs > 0)
									this->mexReturnBool(plhs[0], true);
								if (datParsed)
								{
									if (nlhs > 1)
										this->mexReturnStructSection(plhs[1], datParsed, theProc);
									theProc->removeHandle(datParsed);
								}
								else
								{
									foffset = 1;
								}
								break;
							case JVX_ERROR_PARSE_ERROR:
								if (nlhs > 0)
									this->mexReturnBool(plhs[0], false);
								if (nlhs > 1)
								{
									theProc->getParseError(&myErr);
									this->mexReturnAnswerNegativeResult(plhs[1], ((std::string)"Parse error when parsing configuration: " + myErr.errorDescription.std_str() + "."), resL);
								}
							}
						}
						else
						{
							if (nlhs > 0)
								this->mexReturnBool(plhs[0], false);
							if (nlhs > 1)
								this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain handle to config processor specialization.", JVX_ERROR_INTERNAL);
						}

						theTools->return_reference_tool(JVX_COMPONENT_CONFIG_PROCESSOR, theCfgObj);
					}
					else
					{
						if (nlhs > 0)
							this->mexReturnBool(plhs[0], false);
						if (nlhs > 1)
							this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain handle to config processor object.", JVX_ERROR_INTERNAL);
					}

					involvedComponents.theHost.hFHost->return_hidden_interface(JVX_INTERFACE_TOOLS_HOST, reinterpret_cast<jvxHandle*>(theTools));
				}
				else
				{
					if (nlhs > 0)
						this->mexReturnBool(plhs[0], false);
					if (nlhs > 1)
						this->mexReturnAnswerNegativeResult(plhs[1], "Failed to obtain handle to tools host.", JVX_ERROR_INTERNAL);
				}
			}
			else
			{
				if (nlhs > 0)
					this->mexReturnBool(plhs[0], false);
				if (nlhs > 1)
				{
					this->mexReturnAnswerNegativeResult(plhs[1], internalErrorMessage + "Invalid input argument(s).", JVX_ERROR_INVALID_ARGUMENT);
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
	mexFillEmpty(plhs, nlhs, foffset);
	return(JVX_NO_ERROR);
}
