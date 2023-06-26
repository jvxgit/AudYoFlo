#include "CjvxFlexibleTextControlDevice.h"

static const char* lookupElementType[] =
{
	"JVX_CONTROL_DEVICE_UNDEFINED",
	"JVX_CONTROL_DEVICE_TEXT",
	"JVX_CONTROL_DEVICE_SEPERATOR",
	"JVX_CONTROL_DEVICE_REFERENCE_TOKEN",
	"JVX_CONTROL_DEVICE_REFERENCE_PROPERTY",
	"JVX_CONTROL_DEVICE_REFERENCE_COMMAND",
	"JVX_CONTROL_DEVICE_REFERENCE_SEQUENCE",
	"JVX_CONTROL_DEVICE_REFERENCE",
	"JVX_CONTROL_DEVICE_WILDCARD",
	"JVX_CONTROL_DEVICE_INVALIDATED"
};

struct oneMessage_hdr_ids
{
	oneMessage_hdr hdr;
	jvxSize id_cmd;
	jvxSize id_seq;
	jvxSize id_prop_override;
	jvxSize id_report_single;
};

// ======================================================================================================================

bool
CjvxFlexibleTextControlDevice::check_sep_character(char c, const std::list<oneDefinitionSeperatorChar> & lst_seps, std::string& replace)
{
	auto elm = lst_seps.begin();
	for (; elm != lst_seps.end(); elm++)
	{
		if (c == elm->sep_define)
		{
			replace = elm->replaceby;
			return true;
		}
	}
	return false;
}

jvxErrorType
CjvxFlexibleTextControlDevice::jvx_expandTokens(std::list<oneElementList>& args)
{
	jvxSize i;
	std::string hstr;
	jvxBool err = false;
	jvxSize hval;
	jvxSize hcnt = 0;
	auto elm = args.begin();
	for (; elm != args.end(); elm++)
	{
		if (elm->tp == JVX_CONTROL_DEVICE_TEXT)
		{

			std::string token = elm->txt;
			std::string collect;
			int state = 0;
			jvxBool previousWc = false;
			oneElementList nElm;
			nElm.orig = token;
			std::list<oneElementList> replTokens;
			for (i = 0; i < token.size(); i++)
			{
				char c = token[i];
				switch (state)
				{
				case 0:
					if (c == '\\')
					{
						state = 1;
					}
					else
					{
						if (c == '*')
						{
							// It is a wildcard
							if (previousWc == false)
							{
								if (collect.size())
								{
									nElm.txt = collect;
									nElm.tp = JVX_CONTROL_DEVICE_TEXT;
									collect.clear();
									replTokens.push_back(nElm);
								}

								nElm.txt = "*";
								nElm.tp = JVX_CONTROL_DEVICE_WILDCARD;
								replTokens.push_back(nElm);
							}
							else
							{
								// Skip secondary wildcards
							}
						}
						else
						{
							collect.push_back(c);
							previousWc = false;
						}
					}
					break;
				case 1:
					switch (c)
					{
						//case '\\':
						//	collect.push_back('\\');
						//	break;
						case 'r':
							collect.push_back('\r');
							state = 0;
							break;
						case 'n':
							collect.push_back('\n');
							state = 0;
							break;
						case 't':
							collect.push_back('\t');
							state = 0;
							break;
						case 'x':
							hstr = "0x";
							hcnt = 0;
							state = 2;
							break;
						default:

							collect.push_back(c);
							state = 0;
					}					
					break;
				case 2:
					if (
						(c >= 'a') && (c <= 'f') ||
						(c >= 'A') && (c <= 'F') ||
						(c >= '0') && (c <= '9'))
					{
						hstr.push_back(c);
						hcnt++;
						if (hcnt == 2)
						{
							err = false;
							hval = jvx_string2Size(hstr, err);
							collect.push_back((char)hval);
							state = 0;
						}
					}
					else
					{
						err = false;
						hval = jvx_string2Size(hstr, err);
						collect.push_back((char)hval);
						i--; // Reject current character
						state = 0;
					}
					break;
				}// switch (state)
			}// or (i = 0; i < token.size(); i++)
			
			if (state == 2)
			{
				err = false;
				hval = jvx_string2Size(hstr, err);
				collect.push_back((char)hval);
				state = 0;
			}
			if (collect.size())
			{
				nElm.txt = collect;
				nElm.tp = JVX_CONTROL_DEVICE_TEXT;
				collect.clear();
				replTokens.push_back(nElm);
			}

			args.insert(elm, replTokens.begin(), replTokens.end());
			elm->tp = JVX_CONTROL_DEVICE_INVALIDATED;
		}// if (elm->tp == JVX_CONTROL_DEVICE_TEXT)
	}
	while (1)
	{
		jvxBool foundany = false;
		elm = args.begin();
		while (elm != args.end())
		{
			if (elm->tp == JVX_CONTROL_DEVICE_INVALIDATED)
			{
				foundany = true;
				break;
			}
			elm++;
		}
		if (foundany)
		{
			args.erase(elm);
		}
		else
		{
			break;
		}
		
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFlexibleTextControlDevice::jvx_parseSequenceIntoToken(
	std::string input, std::list<oneElementList>& args,
	const std::list<oneDefinitionSeperatorChar> & lst_seps)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;
	args.clear();
	int state = 0;
	int posi = 0;
	jvxSize sz;
	std::string token;
	jvxBool isReference = false;
	std::string replaceby;
	oneElementList nElm;
	std::string work = input;
	std::string hstr;
	jvxBool err = false;
	jvxSize hval = 0;
	jvxSize hcnt = 0;

	for (i = 0; i < input.size(); i++)
	{
		char c = input[i];
		switch(state)
		{
		case 0:
			if(check_sep_character(c, lst_seps, replaceby))
			{
				if (token.size())
				{
					if (isReference)
					{
						nElm.tp = JVX_CONTROL_DEVICE_REFERENCE;
						nElm.txt = token;
					}
					else
					{
						nElm.tp = JVX_CONTROL_DEVICE_TEXT;
						nElm.txt = token;
					}
					args.push_back(nElm);
				}
				token = input.substr(i, 1);
				nElm.tp = JVX_CONTROL_DEVICE_SEPERATOR;
				nElm.txt = replaceby;
				args.push_back(nElm);
				token.clear();
				isReference = false;
			}
			else if (c == '$')
			{
				if (token.size())
				{
					if (isReference)
					{
						nElm.tp = JVX_CONTROL_DEVICE_REFERENCE;
						nElm.txt = token;
					}
					else
					{
						nElm.tp = JVX_CONTROL_DEVICE_TEXT;
						nElm.txt = token;
					}
					args.push_back(nElm);
					token.clear();
				}
			
				// This is a reference
				isReference = true;
			}
			else if (c == '\\')
			{
				if (isReference)
				{
					nElm.tp = JVX_CONTROL_DEVICE_REFERENCE;
					nElm.txt = token;
					args.push_back(nElm);
					token.clear();
					isReference = false;
				}
				state = 1;
			}
			else
			{
				token += input.substr(i, 1);
			}
			break;
		case 1:
			switch(c)
			{
			case '\"':
				state = 2;
				token += input.substr(i, 1);
				break;
			case 't':
				token += '\t';
				state = 0;
				break;
			case 'r':
				token += '\r';
				state = 0;
				break;
			case 'n':
				token += '\n';
				state = 0;
				break;
			case 'x':
				state = 4;
				hcnt = 0;
				hstr = "0x";
			default:
				token += input.substr(i, 1);
				state = 0;
				break;
			}
			break;
		case 2:
			if (c == '\\')
			{
				state = 3;
			}
			break;
		case 3:
			token += input.substr(i, 1);
			if (c == '\"')
			{
				state = 0;
			}
			else
			{
				state = 2;
			}
			break;
		case 4:
					
			if (
				(c >= 'a') && (c <= 'f') ||
				(c >= 'A') && (c <= 'F') ||
				(c >= '0') && (c <= '9'))
			{
				hstr.push_back(c);
				hcnt++;
				if (hcnt == 2)
				{
					err = false;
					hval = jvx_string2Size(hstr, err);
					token.push_back((char)hval);
					state = 0;
				}
			}
			else
			{
				err = false;
				hval = jvx_string2Size(hstr, err);
				token.push_back((char)hval);
				i--; // Reject current character
				state = 0;
			}
			break;
		default:
			break;				
		}
	}

	if (state == 4)
	{
		err = false;
		hval = jvx_string2Size(hstr, err);
		token.push_back((char)hval);
		state = 0;
	}

	if (token.size())
	{
		if (isReference)
		{
			nElm.tp = JVX_CONTROL_DEVICE_REFERENCE;
			nElm.txt = token;
		}
		else
		{
			nElm.tp = JVX_CONTROL_DEVICE_TEXT;
			nElm.txt = token;
		}
		args.push_back(nElm);
	}
	if (state == 0)
	{
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_PARSE_ERROR;
}




CjvxFlexibleTextControlDevice::CjvxFlexibleTextControlDevice()
{
	this->interact = NULL;
	verbose_out = false;
	timeout_slow_for_device_msecs = 0;
}

CjvxFlexibleTextControlDevice::~CjvxFlexibleTextControlDevice()
{
}

jvxErrorType 
CjvxFlexibleTextControlDevice::init(
	IjvxConfigProcessor* confProc, 
	const std::string& fname_config, 
	IjvxFlexibleTextControlDevice_interact* report, 
	CjvxProperties* prop_ref_arg,
	std::vector<std::string>& errs, jvxSize* des_mq_sz,
	jvxBool verbose_out_param,
	oneDefinitionSeperatorChar* elms_sep,
	jvxSize nums_seps)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize i;

	interact = report;
	fname = fname_config;	
	verbose_out = verbose_out_param;
	propRef = prop_ref_arg;

	if (des_mq_sz)
	{
		*des_mq_sz = sizeof(oneMessage_hdr_ids);
	}
	if (nums_seps == 0)
	{
		oneDefinitionSeperatorChar newElm;
		newElm.sep_define = ' ';
		newElm.replaceby = " ";
		lst_seps.push_back(newElm);
		newElm.sep_define = '~';
		newElm.replaceby = "";
		lst_seps.push_back(newElm);
	}
	else
	{
		for (i = 0; i < nums_seps; i++)
		{
			lst_seps.push_back(elms_sep[i]);
		}
	}

	if(confProc)
	{
		res = parse_input(confProc, errs);
		if (res == JVX_NO_ERROR)
		{
			res = process_input(propRef, errs);
		}
		if (res == JVX_NO_ERROR)
		{
			res = link_references(errs);
		}
	}

	monitor.num_incoming_messages = 0;
	monitor.num_skip_messages = 0;
	monitor.num_unmatched_messages = 0;
	monitor.num_unknown_messages = 0;

	monitor.quality = jvxFlexibleControlQualityIndicator::JVX_CONNECTION_QUALITY_NOT_CONNECTED;

	// Expose additional properties
	CjvxFlexibleTextControlDevice_genpcg::init_all();
	CjvxFlexibleTextControlDevice_genpcg::allocate_all();
	CjvxFlexibleTextControlDevice_genpcg::register_all(propRef);
	CjvxFlexibleTextControlDevice_genpcg::associate__flex_remote__monitor(propRef,
		&monitor.num_incoming_messages, 1,
		&monitor.num_skip_messages, 1,
		&monitor.num_unmatched_messages, 1,
		&monitor.num_unknown_messages, 1);

	return res;
}


jvxErrorType 
CjvxFlexibleTextControlDevice::terminate()
{
	interact = NULL;
	
	CjvxFlexibleTextControlDevice_genpcg::deassociate__flex_remote__monitor(propRef);
	CjvxFlexibleTextControlDevice_genpcg::unregister_all(propRef);
	CjvxFlexibleTextControlDevice_genpcg::deallocate_all();

	propRef = NULL;

	lst_proprefs.clear();
	lst_setprops.clear();
	lst_tokens.clear();
	lst_commands.clear();
	lst_sequences.clear();
	lst_unmatched.clear();
	lst_seps.clear();
	skip_tokens.clear();
	return JVX_NO_ERROR;
}

std::string
CjvxFlexibleTextControlDevice::repair_backslash(const std::string& in)
{
	jvxSize i;
	jvxSize state = 0;
	std::string out;
	std::string hstr;
	jvxSize hval;
	jvxSize hcnt;
	jvxBool err = false;
	for (i = 0; i < in.size(); i++)
	{
		char c = in[i];
		switch (state)
		{
		case 0:
			if (c == '\\')
			{
				state = 1;
			}
			else
			{
				out.push_back(c);
			}
			break;
		case 1:
			switch (c)
			{
			case '\\':
				out.push_back('\\');
				break;
			case 'r':
				out.push_back('\r');
				break;
			case 'n':
				out.push_back('\n');
				break;
			case 't':
				out.push_back('\t');
				break;
			case 'x':
				state = 2;
				hcnt = 0;
				hstr = "0x";
				break;
			default:
				out.push_back(c);

			}
			state = 0;
			break;
		case 2:

			if (
				(c >= 'a') && (c <= 'f') ||
				(c >= 'A') && (c <= 'F') ||
				(c >= '0') && (c <= '9'))
			{
				hstr.push_back(c);
				hcnt++;
				if (hcnt == 2)
				{
					err = false;
					hval = jvx_string2Size(hstr, err);
					out.push_back((char)hval);
					state = 0;
				}
			}
			else
			{
				err = false;
				hval = jvx_string2Size(hstr, err);
				out.push_back((char)hval);
				i--; // Reject current character
				state = 0;
			}

			break;
		}// switch (state)
	}// or (i = 0; i < token.size(); i++)

	if (state == 2)
	{
		err = false;
		hval = jvx_string2Size(hstr, err);
		out.push_back((char)hval);
		state = 0;
	}
	return out;
}

jvxErrorType
CjvxFlexibleTextControlDevice::parse_input(IjvxConfigProcessor* confProc, std::vector<std::string>& errs)
{
	jvxErrorType res = JVX_NO_ERROR, resL = JVX_NO_ERROR;
	jvxConfigData* myConfigHdl = NULL;
	jvxSize i, j,ii, iii;
	jvxConfigData* datPropRefs = NULL;
	jvxConfigData* datPropToken = NULL;
	jvxConfigData* datPropSCommand = NULL;
	jvxConfigData* datPropSeqs = NULL;
	jvxConfigData* datUnmatched = NULL;
	jvxConfigData* datSkip = NULL;
	jvxApiString strRet;
	jvxConfigSectionTypes tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
	jvxConfigData* datTmp = NULL, *datTmpTmp = NULL;
	jvxConfigData* datTout = NULL;
	jvxSize num = 0;
	std::string entry_name;
	jvxApiError errA;
	jvxBool detectedError = false;
	std::string errText;
	jvxValue val;

	errs.clear();
	res = confProc->parseFile(fname.c_str());
	if (res != JVX_NO_ERROR)
	{
		confProc->getParseError(&errA);
		errText = "Parse error in file <fname>: ";
		errText += errA.errorDescription.std_str();
		errText += ".";
		errs.push_back(errText);
		return res;
	}

	// Parsing successful
	confProc->getConfigurationHandle(&myConfigHdl);
	if (!myConfigHdl)
	{
		errText += "Unexpected error: failed to obtain config handle.";
		errs.push_back(errText);
		return res;
	}

	timeout_slow_for_device_msecs = 0;
	confProc->getReferenceEntryCurrentSection_name(myConfigHdl, &datTout, "TIMEOUT_SLOWDOWN_MSEC");
	if (datTout)
	{
		resL = confProc->getAssignmentValue(datTout, &val);
		if (resL == JVX_NO_ERROR)
		{
			val.toContent(&timeout_slow_for_device_msecs);
		}
	}

	skip_tokens_verbose = false;
	confProc->getReferenceEntryCurrentSection_name(myConfigHdl, &datSkip, "SKIP");
	if (datSkip)
	{
		datTmp = NULL;
		confProc->getReferenceEntryCurrentSection_name(datSkip, &datTmp, "TOKENS");
		if (datTmp)
		{
			tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
			confProc->getTypeCurrentEntry(datTmp, &tp);
			switch (tp)
			{
			case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
				confProc->getAssignmentString(datTmp, &strRet);
				skip_tokens.push_back(repair_backslash(strRet.std_str()));
				break;
			case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
				confProc->getNumberStrings(datTmp, &num);
				for (i = 0; i < num; i++)
				{
					confProc->getString_id(datTmp, &strRet, i);
					skip_tokens.push_back(repair_backslash(strRet.std_str()));
				}
				break;
			default:
				errText += "Found entry for <TOKENS> assignment in <SKIP> section which is of wrong type <.";
				errText += jvxConfigSection_str[tp].friendly;
				errText += ">.";
				errs.push_back(errText);
				detectedError = true;
			}
		}
		skip_tokens_verbose = check_verbose(datSkip, confProc);
	}

	confProc->getReferenceEntryCurrentSection_name(myConfigHdl, &datPropRefs, "PROPERTY_REFERENCES");
	if (datPropRefs)
	{
		num = 0;
		confProc->getNumberEntriesCurrentSection(datPropRefs, &num);
		for (i = 0; i < num; i++)
		{
			datTmp = NULL;
			confProc->getReferenceEntryCurrentSection_id(datPropRefs, &datTmp, i);
			if (datTmp)
			{
				jvxValue valS;
				jvxSize numVariants = 1;
				oneDefinitionPropRef nElm;

				confProc->getNameCurrentEntry(datTmp, &strRet);
				nElm.entry = strRet.std_str();

				nElm.length = check_length(datTmp, confProc);

				datTmpTmp = NULL;
				confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "VARIANTS");
				if (datTmpTmp)
				{
					resL = confProc->getAssignmentValue(datTmpTmp, &valS);
					if (resL == JVX_NO_ERROR)
					{
						valS.toContent(&numVariants);
					}
				}

				datTmpTmp = NULL;
				confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "CONVERSION_ID");
				if (datTmpTmp)
				{
					resL = confProc->getAssignmentValue(datTmpTmp, &val);
					if (resL == JVX_NO_ERROR)
					{
						val.toContent(&nElm.conversion_id);
					}
					else
					{
						errText += "Unexpected error: <CONVERSION_ID> assignment is of wrong type.";
						errs.push_back(errText);
						detectedError = true;
					}
				}

				// REFERENCE part
				datTmpTmp = NULL;
				confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "REFERENCE");
				if (datTmpTmp)
				{
					resL = confProc->getAssignmentString(datTmpTmp, &strRet);
					if (resL == JVX_NO_ERROR)
					{
						nElm.verbose = check_verbose(datTmp, confProc);
						std::string storeEntryElm = nElm.entry;
						std::string propname = strRet.std_str();
						jvxSize conversionid = nElm.conversion_id;
						for (ii = 0; ii < numVariants; ii++)
						{
							if (numVariants == 1)
							{
								nElm.prop_name = propname;
								lst_proprefs[nElm.entry] = nElm;
							}
							else
							{
								if (JVX_CHECK_SIZE_SELECTED(conversionid))
								{
									nElm.conversion_id = conversionid + ii;
								}
								nElm.prop_name = jvx_replaceStrInStr(propname, "$VARIANT", jvx_size2String(ii));
								nElm.entry = storeEntryElm + jvx_size2String(ii);
								lst_proprefs[nElm.entry] = nElm;
							}
							datTmpTmp = NULL;
							oneDefinitionCommandOnSet nElmCos;
							nElmCos.entry = propname;
							std::string storeEntryCos = nElmCos.entry;

							// COMMAND_ON_SET part
							confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "COMMAND_ON_SET");
							if (datTmpTmp)
							{
								resL = confProc->getAssignmentString(datTmpTmp, &strRet);
								if (resL == JVX_NO_ERROR)
								{
									nElmCos.verbose = nElm.verbose;
									if (numVariants == 1)
									{
										nElmCos.set_commands.push_back(strRet.std_str());
										lst_setprops[nElmCos.entry] = nElmCos;
									}
									else
									{
										nElmCos.entry = jvx_replaceStrInStr(storeEntryCos, "$VARIANT", jvx_size2String(ii));
										nElmCos.set_commands.push_back(jvx_replaceStrInStr(strRet.std_str(), "$VARIANT", jvx_size2String(ii)));
										auto elmCheck = lst_setprops.find(nElmCos.entry);
										if(elmCheck == lst_setprops.end())
										{
											lst_setprops[nElmCos.entry] = nElmCos;
										}
									}
								}
								else
								{
									jvxSize nStrings = 0;
									resL = confProc->getNumberStrings(datTmpTmp, &nStrings); 
									if (resL == JVX_NO_ERROR)
									{
										nElmCos.verbose = nElm.verbose;
										for (jvxSize iss = 0; iss < nStrings; iss++)
										{
											resL = confProc->getString_id(datTmpTmp, &strRet, iss);
											assert(resL == JVX_NO_ERROR);
											if (numVariants == 1)
											{
												nElmCos.set_commands.push_back(strRet.std_str());
												
											}
											else
											{
												nElmCos.entry = jvx_replaceStrInStr(storeEntryCos, "$VARIANT", jvx_size2String(ii));
												nElmCos.set_commands.push_back(jvx_replaceStrInStr(strRet.std_str(), "$VARIANT", jvx_size2String(ii)));
												
											}
										}
										if (numVariants == 1)
										{
											lst_setprops[nElmCos.entry] = nElmCos;
										}
										else
										{
											auto elmCheck = lst_setprops.find(nElmCos.entry);
											if (elmCheck == lst_setprops.end())
											{
												lst_setprops[nElmCos.entry] = nElmCos;
											}
										}
									}
									else
									{
										errText += "Unexpected error: <COMMAND_ON_SET> assignment is of wrong type..";
										errs.push_back(errText);
										detectedError = true;
									}
								}
							}
							else
							{
								// COMMAND_ON_SET part
								confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "SEQUENCE_ON_SET");
								if (datTmpTmp)
								{
									resL = confProc->getAssignmentString(datTmpTmp, &strRet);
									if (resL == JVX_NO_ERROR)
									{
										if (numVariants == 1)
										{
											nElmCos.set_sequence = strRet.std_str();
											lst_setprops[nElmCos.entry] = nElmCos;
										}
										else
										{
											
											nElmCos.entry = jvx_replaceStrInStr(storeEntryCos, "$VARIANT", jvx_size2String(ii));
											nElmCos.set_sequence = jvx_replaceStrInStr(strRet.std_str(), "$VARIANT",jvx_size2String(ii));
											auto elmCheck = lst_setprops.find(nElmCos.entry);
											if (elmCheck == lst_setprops.end())
											{
												lst_setprops[nElmCos.entry] = nElmCos;
											}
										}
									}
									else
									{
										errText += "Unexpected error: <SEQUENCE_ON_SET> assignment is of wrong type.";
										errs.push_back(errText);
										detectedError = true;
									}
								}
							}
						} // for (ii = 0; ii < numVariants; ii++)
					}
					else
					{
						errText += "Invalid assignment type for <REFERENCE> in <PROPERTY_REFERENCES> subsection <";
						errText += nElm.entry;
						errText += ">.";
						errs.push_back(errText);
						detectedError = true;
					}
				}
				else
				{
					// This is a property with no property reference
					nElm.prop_name = "";
					if (numVariants == 1)
					{
						lst_proprefs[nElm.entry] = nElm;
					}
					else
					{
						std::string storeEntry = nElm.entry;
						jvxSize convId = nElm.conversion_id;
						for (ii = 0; ii < numVariants; ii++)
						{
							nElm.entry = storeEntry + jvx_size2String(ii);
							if (JVX_CHECK_SIZE_SELECTED(convId))
							{
								nElm.conversion_id = convId + ii;
							}
							lst_proprefs[nElm.entry] = nElm;
						}
					}
				}
				
			} // if (datTmp) --- 
			else
			{
				errText += "Failed to get subsection for element #";
				errText += jvx_size2String(i);
				errText += " in <PROPERTY_REFERENCES> section.";
				errs.push_back(errText);
				detectedError = true;
			}
		} // for (i = 0; i < num; i++)
	}

	confProc->getReferenceEntryCurrentSection_name(myConfigHdl, &datPropToken, "TOKENS");
	if (datPropToken)
	{
		num = 0;
		confProc->getNumberEntriesCurrentSection(datPropToken, &num);
		for (i = 0; i < num; i++)
		{
			datTmp = NULL;
			confProc->getReferenceEntryCurrentSection_id(datPropToken, &datTmp, i);
			if (datTmp)
			{
				oneDefinitionToken nElm;

				confProc->getNameCurrentEntry(datTmp, &strRet);
				nElm.token = strRet.std_str();

				resL = confProc->getAssignmentString(datTmp, &strRet);
				if (resL == JVX_NO_ERROR)
				{
					nElm.expr = strRet.std_str();
					lst_tokens[nElm.token] = nElm;
				}
				else
				{
					errText += "Invalid type for token in <TOKENS> subsection <";
					errText += nElm.token;
					errText += ">.";
					errs.push_back(errText);
					detectedError = true;
				}
			}
			else
			{
				errText += "Failed to get subsection for element #";
				errText += jvx_size2String(i);
				errText += " in <TOKENS> section.";
				detectedError = true;
				errs.push_back(errText);
			}
		}
	}

	confProc->getReferenceEntryCurrentSection_name(myConfigHdl, &datPropSCommand, "COMMANDS");
	if (datPropSCommand)
	{
		num = 0;
		confProc->getNumberEntriesCurrentSection(datPropSCommand, &num);
		for (i = 0; i < num; i++)
		{
			jvxSize numVariants = 1;
			jvxValue valS;
			datTmp = NULL;
			confProc->getReferenceEntryCurrentSection_id(datPropSCommand, &datTmp, i);
			if (datTmp)
			{
				oneDefinitionCommand nElm;

				confProc->getNameCurrentEntry(datTmp, &strRet);
				nElm.entry = strRet.std_str();

				datTmpTmp = NULL;
				confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "VARIANTS");
				if (datTmpTmp)
				{
					resL = confProc->getAssignmentValue(datTmpTmp, &valS);
					if (resL == JVX_NO_ERROR)
					{
						valS.toContent(&numVariants);
					}
				}
				// COMMAND part
				datTmpTmp = NULL;
				confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "COMMAND");
				if (datTmpTmp)
				{
					resL = confProc->getAssignmentString(datTmpTmp, &strRet);
					if (resL == JVX_NO_ERROR)
					{
						nElm.command = strRet.std_str();
						nElm.verbose = check_verbose(datTmp, confProc);
						nElm.pre_timeout_msec = 0;
						nElm.post_timeout_msec = 0;

						datTmpTmp = NULL;
						confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "PRE_TIMEOUT_MSECS");
						if (datTmpTmp)
						{
							confProc->getAssignmentValue(datTmpTmp, &val);
							val.toContent(&nElm.pre_timeout_msec);
						}

						datTmpTmp = NULL;
						confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "POST_TIMEOUT_MSECS");
						if (datTmpTmp)
						{
							confProc->getAssignmentValue(datTmpTmp, &val);
							val.toContent(&nElm.post_timeout_msec);
						}

						datTmpTmp = NULL;
						confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "RESPONSE");
						if (datTmpTmp)
						{
							jvxSize numR;
							tp = JVX_CONFIG_SECTION_TYPE_UNKNOWN;
							confProc->getTypeCurrentEntry(datTmpTmp, &tp);
							switch (tp)
							{
							case JVX_CONFIG_SECTION_TYPE_ASSIGNMENTSTRING:
								resL = confProc->getAssignmentString(datTmpTmp, &strRet);
								assert(resL == JVX_NO_ERROR);
								nElm.responses.push_back(strRet.std_str());
								break;
							case JVX_CONFIG_SECTION_TYPE_STRINGLIST:
								confProc->getNumberStrings(datTmpTmp, &numR);
								for (j = 0; j < numR; j++)
								{
									confProc->getString_id(datTmpTmp, &strRet, j);
									nElm.responses.push_back(strRet.std_str());
								}
								break;
							default:
								errText += "Invalid type for token in <RESPONSE> subsection <";
								errText += nElm.entry;
								errText += ">, type <";
								errText += jvxConfigSection_str[tp].friendly;
								errText += ">.";
								errs.push_back(errText);
								detectedError = true;
							}
						}

						nElm.verbose = check_verbose(datTmp, confProc);
						nElm.callback_id = check_callback_id(datTmp, confProc);

						if (numVariants == 1)
						{
							lst_commands[nElm.entry] = nElm;
						}
						else
						{
							std::string storeEntry = nElm.entry;
							std::string storeCommand = nElm.command;
							std::list<std::string> storeResponses = nElm.responses;
							for (ii = 0; ii < numVariants; ii++)
							{
								nElm.entry = storeEntry + jvx_size2String(ii);
								nElm.command = jvx_replaceStrInStr(storeCommand, "$VARIANT", jvx_size2String(ii));
								nElm.responses.clear();
								auto elmV = storeResponses.begin();
								for (; elmV != storeResponses.end(); elmV++)
								{
									nElm.responses.push_back(jvx_replaceStrInStr(*elmV, "$VARIANT", jvx_size2String(ii)));
								}
								lst_commands[nElm.entry] = nElm;
							}
						}
					}
					else
					{
						errText += "Invalid type for token in <COMMAND> subsection <";
						errText += nElm.entry;
						errText += ">.";
						errs.push_back(errText);
						detectedError = true;
					}
				}
				else
				{
					errText += "No assignment <COMMAND> found in <COMMANDS> subsection <";
					errText += nElm.entry;
					errText += ">.";
					errs.push_back(errText);
					detectedError = true;
				}
			}
			else
			{
				errText += "Failed to get subsection for element #";
				errText += jvx_size2String(i);
				errText += " in <COMMANDS> section.";
				errs.push_back(errText);
				detectedError = true;
			}
		}
	}
	
	confProc->getReferenceEntryCurrentSection_name(myConfigHdl, &datPropSeqs, "SEQUENCES");
	if (datPropSeqs)
	{
		num = 0;
		confProc->getNumberEntriesCurrentSection(datPropSeqs, &num);
		for (i = 0; i < num; i++)
		{
			datTmp = NULL;
			confProc->getReferenceEntryCurrentSection_id(datPropSeqs, &datTmp, i);
			if (datTmp)
			{
				oneDefinitionSequence nElm;
				jvxSize numL = 0;

				confProc->getNameCurrentEntry(datTmp, &strRet);
				nElm.entry = strRet.std_str();

				nElm.verbose = check_verbose(datTmp, confProc);

				confProc->getNumberStrings(datTmp, &numL);
				for (j = 0; j < numL; j++)
				{
					resL = confProc->getString_id(datTmp, &strRet, j);
					if (resL == JVX_NO_ERROR)
					{
						oneElementList nElmLst;
						nElmLst.txt = strRet.std_str();
						if (nElmLst.txt[0] == '$')
						{
							nElmLst.tp = JVX_CONTROL_DEVICE_REFERENCE;
							nElm.lst_commands.push_back(nElmLst);
						}
						else
						{
							detectedError = true;
							errText = "Command specification <" + nElmLst.txt + "> in sequence entry ";
							errText += nElm.entry;
							errText += " is not a reference";
							errs.push_back(errText);
						}
					}
					else
					{
						assert(0);
					}
				}
				auto elm = lst_sequences.find(nElm.entry);
				if (elm == lst_sequences.end())
				{
					lst_sequences[nElm.entry] = nElm;
				}
				else
				{
					detectedError = true;
					errText = "Sequence entry <" + nElm.entry + ">: duplicate entry";
					errs.push_back(errText);
				}
			}
			else
			{
				errText += "Failed to get subsection for element #";
				errText += jvx_size2String(i);
				errText += " in <SEQUENCES> section.";
				errs.push_back(errText);
				detectedError = true;
			}
		}
	}

	confProc->getReferenceEntryCurrentSection_name(myConfigHdl, &datUnmatched, "UNMATCHED_MESSAGES");
	if (datUnmatched)
	{
		num = 0;
		confProc->getNumberEntriesCurrentSection(datUnmatched, &num);
		for (i = 0; i < num; i++)
		{
			confProc->getReferenceEntryCurrentSection_id(datUnmatched, &datTmp, i);
			if (datTmp)
			{
				oneDefinitionUnmatched nElm;
				confProc->getNameCurrentEntry(datTmp, &strRet);
				nElm.entry = strRet.std_str();

				nElm.verbose = check_verbose(datTmp, confProc);

				confProc->getAssignmentString(datTmp, &strRet);
				nElm.response = strRet.std_str();

				nElm.callback_id = check_callback_id(datTmp, confProc);
				lst_unmatched[nElm.entry] = nElm;
			}
		}
	}

	if (detectedError)
	{
		return JVX_ERROR_INTERNAL;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFlexibleTextControlDevice::process_input(CjvxProperties* propRef, std::vector<std::string>& errs)
{
	jvx::propertyAddress::CjvxPropertyAddressDescriptor addrProp(nullptr);
	jvxErrorType res = JVX_NO_ERROR;
	std::string errText;
	std::string token;
	jvxCallManagerProperties callGate;
	jvxBool errDetected = false;
	auto elm_proprefs = lst_proprefs.begin();
	for (; elm_proprefs != lst_proprefs.end(); elm_proprefs++)
	{
		elm_proprefs->second.descr.reset();
		if (!elm_proprefs->second.prop_name.empty())
		{
			addrProp.descriptor = elm_proprefs->second.prop_name.c_str();
			res = propRef->_description_property(callGate,
				elm_proprefs->second.descr, addrProp);
			if (res != JVX_NO_ERROR)
			{
				errText = "Failed to obtain property description for property <";
				errText += elm_proprefs->second.prop_name;
				errText += ">.";
				errs.push_back(errText);
				errDetected = true;
			}
		}
		else
		{
			// This is also allowed to do a "manual" conversion
		}
	}

	auto elm_setprops = lst_setprops.begin();
	for (; elm_setprops != lst_setprops.end(); elm_setprops++)
	{
		elm_setprops->second.descr.reset();
		addrProp.descriptor = elm_setprops->first.c_str();
		res = propRef->_description_property(callGate,
			elm_setprops->second.descr,
			addrProp);
		if (res != JVX_NO_ERROR)
		{
			errText = "Failed to obtain property description for property <";
			errText += elm_setprops->first;
			errText += ">.";
			errs.push_back(errText);
			errDetected = true;
		}

		elm_setprops->second.ref.tp = JVX_CONTROL_DEVICE_UNDEFINED;
		elm_setprops->second.ref.txt.clear();

		if (elm_setprops->second.set_commands.size() == 1)
		{
			if (!elm_setprops->second.set_commands[0].empty())
			{
				if (elm_setprops->second.set_commands[0][0] == '$')
				{
					elm_setprops->second.ref.tp = JVX_CONTROL_DEVICE_REFERENCE_COMMAND;
					elm_setprops->second.ref.txt = elm_setprops->second.set_commands[0].substr(1);
				}
				else
				{
					errText = "Invalid reference to command <";
					errText += elm_setprops->second.set_commands[0];
					errText += "> in property reference <";
					errText += elm_setprops->first;
					errText += ">. Maybe you forgot to use the reference start character $?";
					errDetected = true;
					errs.push_back(errText);
				}
			}
			else
			{
				if (!elm_setprops->second.set_sequence.empty())
				{
					if (elm_setprops->second.set_commands[0][0] == '$')
					{
						elm_setprops->second.ref.tp = JVX_CONTROL_DEVICE_REFERENCE_SEQUENCE;
						elm_setprops->second.ref.txt = elm_setprops->second.set_commands[0].substr(1);
					}
					else
					{
						errText = "Invalid reference to command <";
						errText += elm_setprops->second.set_commands[0];
						errText += "> in property reference <";
						errText += elm_setprops->first;
						errText += ">. Maybe you forgot to use the reference start character $?";
						errDetected = true;
						errs.push_back(errText);
					}
				}
			}
		}
		else
		{
			for (jvxSize iss = 0; iss < elm_setprops->second.set_commands.size(); iss++)
			{
				if (!elm_setprops->second.set_commands[iss].empty())
				{
					if (elm_setprops->second.set_commands[iss][0] == '$')
					{
						elm_setprops->second.ref.tp = JVX_CONTROL_DEVICE_REFERENCE_COMMAND;
						elm_setprops->second.ref.txtl.push_back(elm_setprops->second.set_commands[iss].substr(1));
					}
					else
					{
						errText = "Invalid reference to command <";
						errText += elm_setprops->second.set_commands[iss];
						errText += "> in property reference <";
						errText += elm_setprops->first;
						errText += ">. Maybe you forgot to use the reference start character $?";
						errDetected = true;
						errs.push_back(errText);
					}
				}
				else
				{
					if (!elm_setprops->second.set_sequence.empty())
					{
						if (elm_setprops->second.set_commands[iss][0] == '$')
						{
							elm_setprops->second.ref.tp = JVX_CONTROL_DEVICE_REFERENCE_SEQUENCE;
							elm_setprops->second.ref.txtl.push_back(elm_setprops->second.set_commands[iss].substr(1));
						}
						else
						{
							errText = "Invalid reference to command <";
							errText += elm_setprops->second.set_commands[iss];
							errText += "> in property reference <";
							errText += elm_setprops->first;
							errText += ">. Maybe you forgot to use the reference start character $?";
							errDetected = true;
							errs.push_back(errText);
						}
					}
				}
			}
		}
	}

	auto elm_commands = lst_commands.begin();
	for (; elm_commands != lst_commands.end(); elm_commands++)
	{
		std::list<oneElementList> args;
		std::string command = elm_commands->second.command;
		res = jvx_parseSequenceIntoToken(command, args, lst_seps);
		if (res == JVX_NO_ERROR)
		{
			elm_commands->second.cmd_ref_lst = args;
		}
		else
		{
			errText = "Error when parsing command <";
			errText += command;
			errText += "> into elements.";
			errDetected = true;
			errs.push_back(errText);
		}

		auto elmrespelm = elm_commands->second.responses.begin();
		for(; elmrespelm != elm_commands->second.responses.end(); elmrespelm++)
		{
			std::string response = *elmrespelm;
			args.clear();
			res = jvx_parseSequenceIntoToken(response, args, lst_seps);
			if (res == JVX_NO_ERROR)
			{
				elm_commands->second.resp_ref_lst.push_back(args);
			}
			else
			{
				errText = "Error when parsing response <";
				errText += command;
				errText += "> into elements.";
				errDetected = true;
				errs.push_back(errText);
			}
		}
	}

	auto elm_unmatched = lst_unmatched.begin();
	for (; elm_unmatched != lst_unmatched.end(); elm_unmatched++)
	{
		std::list<oneElementList> args;
		std::string descr = elm_unmatched->second.response;
		res = jvx_parseSequenceIntoToken(descr, args, lst_seps);
		if (res == JVX_NO_ERROR)
		{
			elm_unmatched->second.resp_ref_lst = args;
		}
		else
		{
			errText = "Error when parsing unmatched description <";
			errText += descr;
			errText += "> into elements.";
			errDetected = true;
			errs.push_back(errText);
		}
	}

	if (errDetected)
	{
		return JVX_ERROR_INTERNAL;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFlexibleTextControlDevice::link_references(std::vector<std::string>& errs)
{
	jvxBool errDetected = false;
	std::string token;
	std::string errText;


	jvxSize unique_cmd_ids = 1;
	auto elm_cmds = lst_commands.begin();
	for (; elm_cmds != lst_commands.end(); elm_cmds++)
	{
		elm_cmds->second.id = unique_cmd_ids++;

		auto elm_step_cmd = elm_cmds->second.cmd_ref_lst.begin();
		for (; elm_step_cmd != elm_cmds->second.cmd_ref_lst.end(); elm_step_cmd++)
		{
			if (elm_step_cmd->tp == JVX_CONTROL_DEVICE_REFERENCE)
			{
				// Can be token or property reference
				auto elm_props = lst_proprefs.find(elm_step_cmd->txt);
				if (elm_props != lst_proprefs.end())
				{
					elm_step_cmd->tp = JVX_CONTROL_DEVICE_REFERENCE_PROPERTY;
					elm_step_cmd->descr = elm_props->second.descr;
					elm_step_cmd->length = elm_props->second.length;
					elm_step_cmd->conversion_id = elm_props->second.conversion_id;
				}
				else
				{
					auto elm_token = lst_tokens.find(elm_step_cmd->txt);
					if (elm_token != lst_tokens.end())
					{
						elm_step_cmd->tp = JVX_CONTROL_DEVICE_TEXT;
						elm_step_cmd->txt = elm_token->second.expr;
					}
					else
					{
						errText = "Undefined reference to <";
						errText += elm_step_cmd->txt;
						errText += "> in token reference for command <";
						errText += elm_cmds->first;
						errText += ">.";
						errDetected = true;
						errs.push_back(errText);
					}
				}
			}
		}
		jvx_expandTokens(elm_cmds->second.cmd_ref_lst);

		auto elm_step_lst = elm_cmds->second.resp_ref_lst.begin();
		for (; elm_step_lst != elm_cmds->second.resp_ref_lst.end(); elm_step_lst++)
		{
			auto elm_step_resp = elm_step_lst->begin();
			for (; elm_step_resp != elm_step_lst->end(); elm_step_resp++)
			{
				if (elm_step_resp->tp == JVX_CONTROL_DEVICE_REFERENCE)
				{
					// Can be token or property reference
					auto elm_props = lst_proprefs.find(elm_step_resp->txt);
					if (elm_props != lst_proprefs.end())
					{
						elm_step_resp->tp = JVX_CONTROL_DEVICE_REFERENCE_PROPERTY;
						elm_step_resp->descr = elm_props->second.descr;
						elm_step_resp->length = elm_props->second.length;
						elm_step_resp->conversion_id = elm_props->second.conversion_id;
					}
					else
					{
						auto elm_token = lst_tokens.find(elm_step_resp->txt);
						if (elm_token != lst_tokens.end())
						{
							elm_step_resp->tp = JVX_CONTROL_DEVICE_TEXT;
							elm_step_resp->txt = elm_token->second.expr;
						}
						else
						{
							errText = "Undefined reference to <";
							errText += elm_step_resp->txt;
							errText += "> in token reference for command <";
							errText += elm_cmds->first;
							errText += ">.";
							errDetected = true;
							errs.push_back(errText);
						}
					}
				}
			}
			jvx_expandTokens(*elm_step_lst);
		}
	}

	jvxSize unique_seq_ids = 1;
	auto elm_seqs = lst_sequences.begin();
	for (; elm_seqs != lst_sequences.end(); elm_seqs++)
	{
		elm_seqs->second.id = unique_seq_ids++;

		auto elm_step_seq = elm_seqs->second.lst_commands.begin();
		for (; elm_step_seq != elm_seqs->second.lst_commands.end(); elm_step_seq++)
		{
			assert(elm_step_seq->tp == JVX_CONTROL_DEVICE_REFERENCE);

			// Can be command only
			std::string command = elm_step_seq->txt;
			if (command[0] == '$')
			{
				command = command.substr(1);
				auto elm_cmd = lst_commands.find(command);
				if (elm_cmd != lst_commands.end())
				{
					elm_step_seq->tp = JVX_CONTROL_DEVICE_REFERENCE_COMMAND;
					elm_step_seq->ref_id = elm_cmd->second.id;
					elm_step_seq->orig = elm_cmd->first;
					elm_step_seq->expects_response = (elm_cmd->second.resp_ref_lst.size() != 0);
				}
				else
				{
					errText = "Undefined reference to <";
					errText += elm_step_seq->txt;
					errText += "> in sequence definition <";
					errText += elm_seqs->first;
					errText += ">.";
					errDetected = true;
					errs.push_back(errText);
				}
			}
			else
			{
				errText = "Invalid reference to command <";
				errText += command;
				errText += "> in sequence definition <";
				errText += elm_seqs->first;
				errText += ">.";
				errDetected = true;
				errs.push_back(errText);
			}
		}
	}

	auto elm_setprops = lst_setprops.begin();
	for (; elm_setprops != lst_setprops.end(); elm_setprops++)
	{
		elm_setprops->second.ref.tp = JVX_CONTROL_DEVICE_UNDEFINED;
		elm_setprops->second.ref.txt.clear();
		elm_setprops->second.ref.txtl.clear();

		if (elm_setprops->second.set_commands.size() == 1)
		{
			if (!elm_setprops->second.set_commands[0].empty())
			{
				if (elm_setprops->second.set_commands[0][0] == '$')
				{
					token = elm_setprops->second.set_commands[0].substr(1);
					auto elm_cmd_search = lst_commands.find(token);
					if (elm_cmd_search != lst_commands.end())
					{
						elm_setprops->second.ref.tp = JVX_CONTROL_DEVICE_REFERENCE_COMMAND;
						elm_setprops->second.ref.txt = token;
						elm_setprops->second.ref.ref_id = elm_cmd_search->second.id;
						elm_setprops->second.ref.orig = token;
					}
					else
					{
						errText = "Undefined reference to command <";
						errText += token;
						errText += "> in property reference <";
						errText += elm_setprops->first;
						errText += ">.";
						errDetected = true;
						errs.push_back(errText);
					}
				}
				else
				{
					errText = "Invalid reference to command <";
					errText += elm_setprops->second.set_commands[0];
					errText += "> in property reference <";
					errText += elm_setprops->first;
					errText += ">.";
					errDetected = true;
					errs.push_back(errText);
				}
			}
			else
			{
				if (!elm_setprops->second.set_sequence.empty())
				{
					if (elm_setprops->second.set_commands[0][0] == '$')
					{
						token = elm_setprops->second.set_commands[0].substr(1);
						auto elm_seq_search = lst_sequences.find(token);
						if (elm_seq_search != lst_sequences.end())
						{
							elm_setprops->second.ref.tp = JVX_CONTROL_DEVICE_REFERENCE_SEQUENCE;
							elm_setprops->second.ref.txt = token;
							elm_setprops->second.ref.ref_id = elm_seq_search->second.id;

						}
						else
						{
							errText = "Undefined reference to sequence <";
							errText += token;
							errText += "> in property reference <";
							errText += elm_setprops->first;
							errText += ">.";
							errDetected = true;
							errs.push_back(errText);
						}
					}
					else
					{
						errText = "Invalid reference to command <";
						errText += elm_setprops->second.set_commands[0];
						errText += "> in property reference <";
						errText += elm_setprops->first;
						errText += ">.";
						errDetected = true;
						errs.push_back(errText);
					}
				}
			}
		}
		else
		{
			for (jvxSize iss = 0; iss < elm_setprops->second.set_commands.size(); iss++)
			{
				if (!elm_setprops->second.set_commands[iss].empty())
				{
					if (elm_setprops->second.set_commands[iss][0] == '$')
					{
						token = elm_setprops->second.set_commands[iss].substr(1);
						auto elm_cmd_search = lst_commands.find(token);
						if (elm_cmd_search != lst_commands.end())
						{
							elm_setprops->second.ref.tp = JVX_CONTROL_DEVICE_REFERENCE_COMMAND;
							elm_setprops->second.ref.txtl.push_back(token);
							elm_setprops->second.ref.ref_id = elm_cmd_search->second.id;
							elm_setprops->second.ref.orig = token;
						}
						else
						{
							errText = "Undefined reference to command <";
							errText += token;
							errText += "> in property reference <";
							errText += elm_setprops->first;
							errText += ">.";
							errDetected = true;
							errs.push_back(errText);
						}
					}
					else
					{
						errText = "Invalid reference to command <";
						errText += elm_setprops->second.set_commands[iss];
						errText += "> in property reference <";
						errText += elm_setprops->first;
						errText += ">.";
						errDetected = true;
						errs.push_back(errText);
					}
				}
				else
				{
					if (!elm_setprops->second.set_sequence.empty())
					{
						if (elm_setprops->second.set_commands[iss][0] == '$')
						{
							token = elm_setprops->second.set_commands[iss].substr(1);
							auto elm_seq_search = lst_sequences.find(token);
							if (elm_seq_search != lst_sequences.end())
							{
								elm_setprops->second.ref.tp = JVX_CONTROL_DEVICE_REFERENCE_SEQUENCE;
								elm_setprops->second.ref.txtl.push_back(token);
								elm_setprops->second.ref.ref_id = elm_seq_search->second.id;

							}
							else
							{
								errText = "Undefined reference to sequence <";
								errText += token;
								errText += "> in property reference <";
								errText += elm_setprops->first;
								errText += ">.";
								errDetected = true;
								errs.push_back(errText);
							}
						}
						else
						{
							errText = "Invalid reference to command <";
							errText += elm_setprops->second.set_commands[iss];
							errText += "> in property reference <";
							errText += elm_setprops->first;
							errText += ">.";
							errDetected = true;
							errs.push_back(errText);
						}
					}
				}
			}
		}
	}

	auto elm_unmatched = lst_unmatched.begin();
	for (; elm_unmatched != lst_unmatched.end(); elm_unmatched++)
	{
		auto elm_step_unma = elm_unmatched->second.resp_ref_lst.begin();
		for (; elm_step_unma != elm_unmatched->second.resp_ref_lst.end(); elm_step_unma++)
		{
			if (elm_step_unma->tp == JVX_CONTROL_DEVICE_REFERENCE)
			{
				// Can be token or property reference
				auto elm_props = lst_proprefs.find(elm_step_unma->txt);
				if (elm_props != lst_proprefs.end())
				{
					elm_step_unma->tp = JVX_CONTROL_DEVICE_REFERENCE_PROPERTY;
					elm_step_unma->descr = elm_props->second.descr;
					elm_step_unma->conversion_id = elm_props->second.conversion_id;
				}
				else
				{
					auto elm_token = lst_tokens.find(elm_step_unma->txt);
					if (elm_token != lst_tokens.end())
					{
						elm_step_unma->tp = JVX_CONTROL_DEVICE_TEXT;
						elm_step_unma->txt = elm_token->second.expr;
					}
					else
					{
						errText = "Undefined reference to <";
						errText += elm_step_unma->txt;
						errText += "> in unmatched description <";
						errText += elm_unmatched->first;
						errText += ">.";
						errDetected = true;
						errs.push_back(errText);
					}
				}
			}
		}
		jvx_expandTokens(elm_unmatched->second.resp_ref_lst);
	}

	if (errDetected)
	{
		return JVX_ERROR_INTERNAL;
	}
	return JVX_NO_ERROR;
}

void
CjvxFlexibleTextControlDevice::print()
{
	if (skip_tokens_verbose)
	{
		auto elmskip = skip_tokens.begin();
		for (; elmskip != skip_tokens.end(); elmskip++)
		{
			std::cout << "Skip <" << *elmskip << ">" << std::endl;
		}
	}

	auto elmc = lst_commands.begin();
	for (; elmc != lst_commands.end(); elmc++)
	{
		if (elmc->second.verbose)
		{
			std::cout << "Command <" << elmc->first << "[" << elmc->second.id << "]:" << std::endl;
			std::cout << "\tCallback id = " << elmc->second.callback_id << std::endl;
			std::cout << "\tComposition" << std::endl;
			print_ref_list(elmc->second.cmd_ref_lst);
			auto elmresp = elmc->second.resp_ref_lst.begin();
			for(; elmresp != elmc->second.resp_ref_lst.end(); elmresp++)
			{
				std::cout << "\tResponse" << std::endl;
				print_ref_list(*elmresp);
			}
			if (elmc->second.pre_timeout_msec)
			{
				std::cout << "\tPre timeout msecs = " << elmc->second.pre_timeout_msec << std::endl;
			}
			if (elmc->second.post_timeout_msec)
			{
				std::cout << "\tPost timeout msecs = " << elmc->second.post_timeout_msec << std::endl;
			}
		}
	}

	auto elmpr = lst_proprefs.begin();
	for (; elmpr != lst_proprefs.end(); elmpr++)
	{
		if (elmpr->second.verbose)
		{
			std::cout << "Property Reference <" << elmpr->first << ":" << elmpr->second.prop_name << " - " << 
				jvxPropertyCategoryType_txt(elmpr->second.descr.category) << ":" << 
				elmpr->second.descr.globalIdx << ", length = " << elmpr->second.length << 
				", conversion id = " << elmpr->second.conversion_id << "." << std::endl;
		}
	}

	auto elmseq = lst_sequences.begin();
	for (; elmseq != lst_sequences.end(); elmseq++)
	{
		if (elmseq->second.verbose)
		{
			std::cout << "Sequence <" << elmseq->first << ":" << std::endl;
			auto elmseqe = elmseq->second.lst_commands.begin();
			for (; elmseqe != elmseq->second.lst_commands.end(); elmseqe++)
			{
				std::cout << "\tCommand: " << elmseqe->txt << "[" << lookupElementType[elmseqe->tp] << "]" << std::endl;
			}
		}
	}

	auto elmsp = lst_setprops.begin();
	for (; elmsp != lst_setprops.end(); elmsp++)
	{
		if (elmsp->second.verbose)
		{
			std::cout << "Property <" << elmsp->first << " - " << jvxPropertyCategoryType_txt(elmsp->second.descr.category) << ":" << elmsp->second.descr.globalIdx << " -> " << elmsp->second.ref.txt << "[" << lookupElementType[elmsp->second.ref.tp] << "]" << std::endl;
		}
	}

	auto elmum = lst_unmatched.begin();
	for (; elmum != lst_unmatched.end(); elmum++)
	{
		if (elmum->second.verbose)
		{
			std::cout << "Unmatched rule <" << elmum->first << std::endl;
			print_ref_list(elmum->second.resp_ref_lst);
		}
	}
}

void
CjvxFlexibleTextControlDevice::print_ref_list(const std::list<oneElementList>& ref_lst)
{
	auto elmce = ref_lst.begin();
	for (; elmce != ref_lst.end(); elmce++)
	{
		std::cout << "\t\t#" << jvx_prepareStringForLogfile(elmce->txt) << "[" << lookupElementType[elmce->tp] << "]" << std::flush;
		if (JVX_CHECK_SIZE_SELECTED(elmce->length))
		{
			std::cout << ", length = " << elmce->length << std::flush;
		}
		std::cout << std::endl;
	}
}

jvxErrorType
CjvxFlexibleTextControlDevice::inform_identified(oneMessage_hdr* mess, jvxBool error_flag, jvxSize* idIdentify)
{
	jvxSize sz = sizeof(oneMessage_hdr_ids);
	if (mess->sz_elm == sz)
	{
		oneMessage_hdr_ids* ptrHead = (oneMessage_hdr_ids*)mess;

		// Store an identification id that will come back in the post_message_complete
		if (idIdentify)
			*idIdentify = ptrHead->id_report_single;

		if (JVX_CHECK_SIZE_SELECTED(ptrHead->id_seq))
		{
			auto elm_seq = lst_sequences.begin();

			for (; elm_seq != lst_sequences.end(); elm_seq++)
			{
				if (ptrHead->id_seq == elm_seq->second.id)
				{
					jvxSize cnt = 0;
					auto elm_c = elm_seq->second.lst_commands.begin();
					for (; elm_c != elm_seq->second.lst_commands.end(); elm_c++, cnt++)
					{
						if (ptrHead->id_cmd == elm_c->ref_id)
						{
							jvx_bitSet(elm_seq->second.returned_response_flags, cnt);
							if (error_flag)
							{
								jvx_bitSet(elm_seq->second.error_response_flags, cnt);
							}
							break;
						}
					}
					break;
				}
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFlexibleTextControlDevice::process_cancelled_message(oneMessage_hdr* mess, jvxSize* idIdentify)
{
	inform_identified(mess, true, idIdentify);
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxFlexibleTextControlDevice::process_incoming_message(const std::string& token, jvxSize* uId, jvxSize* messType, jvxSize* idIdentify)
{
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool identifiedMessage = false;
	jvxInt64 tStamp_sent = JVX_SIZE_UNSELECTED;

	if (interact)
	{
		std::string mess = __FUNCTION__;
		mess += ": Reported text token <";
		mess += jvx_prepareStringForLogfile(token);
		mess += ">";
		interact->report_message(mess);
	}

	this->monitor.num_incoming_messages++;

	// Check if it is a "skip token"
	auto elmskip = skip_tokens.begin();
	for (; elmskip != skip_tokens.end(); elmskip++)
	{
		if (token == *elmskip)
		{
			if (interact)
			{
				std::string mess = __FUNCTION__;
				mess += ": Found skip rule for incoming message <";
				mess += jvx_prepareStringForLogfile(token);
				mess += ">.";
				interact->report_message(mess);
				if (skip_tokens_verbose)
				{
					std::cout << mess << std::endl;
				}
			}

			this->monitor.num_skip_messages++;
			*uId = JVX_SIZE_DONTCARE;
			res = JVX_ERROR_EMPTY_LIST;
			return res;
		}
	}

	if (interact)
	{
		auto elm = lst_commands.begin();
		for(; elm != lst_commands.end(); elm++)
		{
			jvxSize id = elm->second.id;
			oneMessage_hdr* ptr_hdr_ret = NULL;
			resL = interact->device_check_pending_uid(id, (jvxSize)JVX_GENERIC_RS232_MESSAGE_OFFSET, &tStamp_sent, &ptr_hdr_ret);
			if (resL == JVX_NO_ERROR)
			{
				jvxBool matches_rule = false;

				// Check this message
				auto elmresp = elm->second.resp_ref_lst.begin();
				for (; elmresp != elm->second.resp_ref_lst.end(); elmresp++)
				{					
					matches_rule = parse_text_message(token, *elmresp, elm->second.callback_id);
					if (matches_rule)
					{
						inform_identified(ptr_hdr_ret, false, idIdentify);
						
						break;
					}
				}

				if (matches_rule)
				{
					if (interact)
					{
						if (JVX_CHECK_SIZE_SELECTED(elm->second.callback_id))
						{
							interact->trigger_callback(JVX_FLEXIBLE_CONTROL_EVENT_COMMAND_COMPLETE, elm->second.callback_id, NULL);
						}

						std::string mess = __FUNCTION__;
						mess += ": Incoming text token <";
						mess += jvx_prepareStringForLogfile(token);
						mess += "> matched with previous command < ";
						mess += elm->first;
						mess += ">, id <";
						mess += jvx_size2String(elm->second.id);
						mess += ">";
						interact->report_message(mess);

						if (elm->second.verbose)
						{
							std::cout << mess << std::endl;
						}
					}

					if (elm->second.post_timeout_msec)
					{
						if (interact)
						{
							std::string mess = __FUNCTION__;
							mess += ": Waiting for <";
							mess += jvx_size2String(elm->second.post_timeout_msec);
							mess += "> msecs due to command specific post-timeout before releasing channel.";
							interact->report_message(mess);
						}
						JVX_SLEEP_MS(elm->second.post_timeout_msec);
					}
					*uId = id;
					*messType = JVX_GENERIC_RS232_MESSAGE_OFFSET;
					identifiedMessage = true;

					
					break;
				}
			}
			else if (resL == JVX_ERROR_WRONG_STATE)
			{
				// Message exists but has not been sent yet. This should not be matched
			}
		}

		if (!identifiedMessage)
		{
			jvxBool foundunmatched = false;
			auto elmu = lst_unmatched.begin();
			for (; elmu != lst_unmatched.end(); elmu++)
			{
				jvxBool matches_rule = false;

				// Check the incoming text
				matches_rule = parse_text_message(token, elmu->second.resp_ref_lst, elmu->second.callback_id);

				if (matches_rule)
				{
					if (interact)
					{
						std::string mess = __FUNCTION__;
						mess += ": Incoming text token <";
						mess += jvx_prepareStringForLogfile(token);
						mess += "> matched with unmatched definition < ";
						mess += elmu->first;
						mess += ">";
						interact->report_message(mess);
					}

					this->monitor.num_unmatched_messages++;
					foundunmatched = true;
					break;
				}
			}
			if (!foundunmatched)
			{
				if (interact)
				{
					std::string mess = __FUNCTION__;
					mess += ": No matching rule found for incoming message <";
					mess += jvx_prepareStringForLogfile(token);
					mess += ">.";
					interact->report_message(mess);
					if (verbose_out)
					{
						std::cout << mess << std::endl;
					}
				}
				this->monitor.num_unknown_messages++;

				res = JVX_ERROR_ABORT;
			}
		}
	}
	return res;
}

jvxErrorType 
CjvxFlexibleTextControlDevice::translate_message_token_separator(oneMessage_hdr* mess, std::string& txtOut, const std::string& sepToken)
{
	jvxErrorType res = JVX_NO_ERROR;
	if (mess->tp == JVX_GENERIC_RS232_MESSAGE_OFFSET)
	{
		if (mess->sz_elm == sizeof(oneMessage_hdr_ids))
		{
			// Do something here
			oneMessage_hdr_ids* this_mess = (oneMessage_hdr_ids*)mess;
			jvxSize id_command = mess->uId;
			jvxBool foundit = false;
			auto elm = lst_commands.begin();
			for (; elm != lst_commands.end(); elm++)
			{
				if (elm->second.id == id_command)
				{
					res = command_2_string(elm->second.cmd_ref_lst, txtOut, elm->second.callback_id, this_mess->id_prop_override);
					//res = command_2_string(elm->second.cmd_ref_lst, txtOut, elm->second.callback_id);
					if (interact)
					{
						std::string mess = __FUNCTION__; 
						mess += ": Translating message with id <";
						mess += jvx_size2String(id_command);
						mess += "> request to text token <";
						mess += txtOut;
						mess += ">.";
						interact->report_message(mess);

						if (elm->second.verbose)
						{
							std::cout << mess << std::endl;
						}

					}

					if (timeout_slow_for_device_msecs)
					{
						if (interact)
						{
							std::string mess = __FUNCTION__;
							mess += ": Waiting for <";
							mess += jvx_size2String(timeout_slow_for_device_msecs);
							mess += "> msecs due to global timeout.";
							interact->report_message(mess);
						}
						JVX_SLEEP_MS(timeout_slow_for_device_msecs);
					}
					
					if (elm->second.pre_timeout_msec)
					{
						if (interact)
						{
							std::string mess = __FUNCTION__;
							mess += ": Waiting for <";
							mess += jvx_size2String(elm->second.pre_timeout_msec);
							mess += "> msecs due to command specific pre-timeout.";
							interact->report_message(mess);
						}
						JVX_SLEEP_MS(elm->second.pre_timeout_msec);
					}

					foundit = true;
					break;
				}
			}
			if (!foundit)
			{
				if (interact)
				{
					std::string mess = __FUNCTION__; 
					mess += ": Could not find the command with id <";
					mess += jvx_size2String(id_command);
					mess += ">.";
					interact->report_message(mess);
				}
			}
		}
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	if (res == JVX_NO_ERROR)
	{
		txtOut += sepToken;
	}
	return res;
}

jvxErrorType
CjvxFlexibleTextControlDevice::trigger_send_command(const std::string& cmd_name, jvxSize id_prop_override, jvxSize id_report_single)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	oneMessage_hdr_ids fld;
	memset(&fld, 0, sizeof(oneMessage_hdr_ids));

	fld.hdr.sz_elm = sizeof(oneMessage_hdr_ids);
	fld.hdr.tp = JVX_GENERIC_RS232_MESSAGE_OFFSET;

	auto elmc = lst_commands.begin();
	for (; elmc != lst_commands.end(); elmc++)
	{
		if (elmc->first == cmd_name)
		{
			fld.id_cmd = elmc->second.id;
			fld.id_seq = JVX_SIZE_UNSELECTED;
			fld.id_prop_override = id_prop_override;
			fld.id_report_single = id_report_single;
			fld.hdr.uId = fld.id_cmd;

			if (interact)
			{
				std::string mess = __FUNCTION__;
				mess += ": Triggering command <";
				mess += elmc->second.command;
				mess += "> with uid <";
				mess += jvx_size2String(fld.id_cmd);
				if (JVX_CHECK_SIZE_SELECTED(fld.id_prop_override))
				{
					mess += ">, and id_override <";
					mess += jvx_size2String(fld.id_prop_override);
				}
				mess += ">.";
				interact->report_message(mess);

				if (elmc->second.verbose)
				{
					std::cout << mess << std::endl;
				}
			}

			// CjvxGenericRS232TextDevice::activate_init_messages();
			res = interact->add_message_queue(reinterpret_cast<oneMessage_hdr*>(&fld), reinterpret_cast<jvxHandle*>(this), 
				(elmc->second.resp_ref_lst.size() != 0));
			if (res != JVX_NO_ERROR)
			{
				if (interact)
				{
					std::string mess = __FUNCTION__;
					mess += ": Failed to trigger device command <";
					mess += elmc->second.command;
					mess += ">, reason: ";
					mess += jvxErrorType_txt(res);
					interact->report_message(mess);

					if (verbose_out)
					{
						std::cout << mess << std::endl;
					}
				}
			}
		}
	}
	return res;
}

jvxErrorType
CjvxFlexibleTextControlDevice::trigger_send_seq(const std::string& seq_name, jvxBool track_response, jvxBool send_direct)
{
	jvxErrorType res = JVX_NO_ERROR;
	oneMessage_hdr_ids fld;
	memset(&fld, 0, sizeof(oneMessage_hdr_ids));
	fld.hdr.sz_elm = sizeof(oneMessage_hdr_ids);
	fld.hdr.tp = JVX_GENERIC_RS232_MESSAGE_OFFSET;

	if (interact)
	{
		std::string mess = __FUNCTION__; 
		mess += ": Triggering sequence <";
		mess += seq_name;
		mess += ">";
		interact->report_message(mess);
	}

	if (interact)
	{
		auto elm = lst_sequences.find(seq_name);
		if (elm != lst_sequences.end())
		{
			if (elm->second.status_operate == false)
			{
				if (track_response)
				{
					elm->second.status_operate = true;

					jvx_bitFClear(elm->second.returned_response_flags);
					jvx_bitFClear(elm->second.complete_response_flags);
					jvx_bitFClear(elm->second.error_response_flags);
				}
				auto elmc = elm->second.lst_commands.begin();
				jvxSize cnt = 0;
				for (; elmc != elm->second.lst_commands.end(); elmc++)
				{
					fld.id_cmd = elmc->ref_id;
					fld.id_seq = elm->second.id;
					fld.id_report_single = JVX_SIZE_UNSELECTED;
					fld.id_prop_override = JVX_SIZE_UNSELECTED;
					fld.hdr.uId = fld.id_cmd;

					if (elmc->expects_response)
					{
						jvx_bitSet(elm->second.complete_response_flags, cnt);						
					}
					if (interact)
					{
						std::string mess = __FUNCTION__;
						mess += ": Triggering command <";
						mess += elmc->orig;
						mess += "> with uid <";
						mess += jvx_size2String(fld.id_cmd);
						mess += ">.";
						interact->report_message(mess);
					}

					if (send_direct)
					{
						interact->send_direct(reinterpret_cast<oneMessage_hdr*>(&fld));
					}
					else
					{
						// CjvxGenericRS232TextDevice::activate_init_messages();
						res = interact->add_message_queue(reinterpret_cast<oneMessage_hdr*>(&fld),
							reinterpret_cast<jvxHandle*>(this), elmc->expects_response);
						if (res != JVX_NO_ERROR)
						{
							if (interact)
							{
								std::string mess = __FUNCTION__;
								mess += ": Failed to trigger device command <";
								mess += elmc->orig;
								mess += ">, reason: ";
								mess += jvxErrorType_txt(res);
								interact->report_message(mess);

								if (verbose_out)
								{
									std::cout << mess << std::endl;
								}
							}
						}
						cnt++;
					}
				}
				if (track_response)
				{
					if (!JVX_EVALUATE_BITFIELD(elm->second.complete_response_flags))
					{
						// If no response is expected-
						elm->second.status_operate = false;
						elm->second.returned_response_flags = 0;
						elm->second.complete_response_flags = 0;
					}
				}
			}
			else
			{
				res = JVX_ERROR_COMPONENT_BUSY;
				if (interact)
				{
					std::string mess = __FUNCTION__;
					mess += ": Failed to trigger sequence <";
					mess += seq_name;
					mess += "> in list of valid sequences.";
					interact->report_message(mess);
				}
			}
		}
		else
		{
			res = JVX_ERROR_ELEMENT_NOT_FOUND;
			if (interact)
			{
				std::string mess = __FUNCTION__;
				mess += ": Failed to trigger sequence <";
				mess += seq_name;
				mess += "> in list of valid sequences.";
				interact->report_message(mess);
			}
		}
	}
	return res;
}

jvxErrorType 
CjvxFlexibleTextControlDevice::check_set_status_seq(const std::string& seq_name, jvxBitField* error_bfld, jvxBitField* returned_bfld, jvxBitField* complete_bfld)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	auto elm = lst_sequences.find(seq_name);
	if ( elm != lst_sequences.end())
	{
		if (elm->second.status_operate == true)
		{
			if (error_bfld)
			{
				*error_bfld = elm->second.error_response_flags;
			}
			if (returned_bfld)
			{
				*returned_bfld = elm->second.returned_response_flags;
			}
			if (complete_bfld)
			{
				*complete_bfld = elm->second.complete_response_flags;
			}
			if (elm->second.returned_response_flags == elm->second.complete_response_flags)
			{				
				elm->second.status_operate = false;
				jvx_bitFClear(elm->second.returned_response_flags);
				jvx_bitFClear(elm->second.complete_response_flags);
				jvx_bitFClear(elm->second.error_response_flags);
				res = JVX_NO_ERROR;
			}
			else
			{
				res = JVX_ERROR_NOT_READY;
			}
		}
		else
		{
			res = JVX_ERROR_WRONG_STATE;
		}
	}
	return res;
}

void
CjvxFlexibleTextControlDevice::messages_on_init()
{
	trigger_send_seq("INIT", false);
}

void
CjvxFlexibleTextControlDevice::messages_on_observe()
{
	trigger_send_seq("OBSERVE", false);
}

jvxErrorType
CjvxFlexibleTextControlDevice::command_2_string(const std::list<oneElementList>& lst, std::string& txtOut, jvxSize callback_id, jvxSize ext_conversion_id)
{
	jvxErrorType res = JVX_NO_ERROR;
	txtOut.clear();
	std::string token;
	jvxSize conv_id;
	auto elm = lst.begin();
	for (; elm != lst.end(); elm++)
	{
		switch (elm->tp)
		{
		case JVX_CONTROL_DEVICE_TEXT:
		case JVX_CONTROL_DEVICE_SEPERATOR:

			txtOut += elm->txt;
			break;

		case JVX_CONTROL_DEVICE_REFERENCE_PROPERTY:

			conv_id = elm->conversion_id;
			if (JVX_CHECK_SIZE_SELECTED(ext_conversion_id))
			{
				conv_id = ext_conversion_id;
			}
			res = interact->convert_property_2_txt(&elm->descr, conv_id, token, callback_id, elm->length);
			if (res == JVX_ERROR_POSTPONE)
			{
				token = "<PROPERTY>";
				res = JVX_NO_ERROR;
			}
			txtOut += token;
			break;
		case JVX_CONTROL_DEVICE_WILDCARD:
			// Do not add, empty is also possible
			break;
		default:
			assert(0);
		}
		if (res != JVX_NO_ERROR)
		{
			break;
		}
	}
	return res;
}

jvxBool
CjvxFlexibleTextControlDevice::parse_text_message(std::string message, const std::list<oneElementList>& lst, jvxSize command_id)
{
	std::string tok1;
	std::string tok2;
	auto elm = lst.begin();
	auto elmfwd = elm;
	jvxSize idx0 = 0;
	jvxSize idx1 = 0;
	jvxSize idx2 = 0;
	jvxSize sz;
	std::list< reportOneProperty> report_props;

	for (; elm != lst.end(); elm++)
	{
		switch (elm->tp)
		{

		case JVX_CONTROL_DEVICE_TEXT:

			sz = elm->txt.size();
			tok1 = message.substr(idx0, sz);
			if (tok1 == elm->txt)
			{
				idx0 += sz;
				break;
			}
			else
			{
				// Mismatch
				return false;
			}
			break;

		case JVX_CONTROL_DEVICE_SEPERATOR:

			if (elm->txt.size() > 0)
			{
				sz = elm->txt.size();
				tok1 = message.substr(idx0, sz);
				if (tok1 == elm->txt)
				{
					idx0 += sz;
					break;
				}
				else
				{
					// Mismatch
					return false;
				}
			}
			break;

		case JVX_CONTROL_DEVICE_REFERENCE_PROPERTY:
		case JVX_CONTROL_DEVICE_WILDCARD:

			if (JVX_CHECK_SIZE_UNSELECTED(elm->length))
			{
				elmfwd = elm;
				elmfwd++;
				if (elmfwd == lst.end())
				{
					tok2 = message.substr(idx0, std::string::npos);

					if (elm->tp == JVX_CONTROL_DEVICE_REFERENCE_PROPERTY)
					{
						// Process property!!!
						reportOneProperty newP;
						newP.descr = elm->descr;
						newP.convertme = tok2;
						newP.conversion_id = elm->conversion_id;
						newP.txtref = elm->txt;
						report_props.push_back(newP);

						/*
						if (interact)
						{
							std::string mess = __FUNCTION__;
							mess += "Property text = <";
							mess += tok2;
							mess += "> to be passed to processor with conversion id <";
							mess += jvx_size2String(elm->conversion_id);
							mess += "> ->";

							
							jvxErrorType resL = interact->convert_txt_2_property(&elm->descr, elm->conversion_id, tok2, elm->txt, command_id);

							mess += jvxErrorType_txt(resL);
							mess += ".";

							interact->report_message(mess);
						}*/
					}
				}
				else
				{
					if (
						(elmfwd->tp == JVX_CONTROL_DEVICE_TEXT) ||
						((elmfwd->tp == JVX_CONTROL_DEVICE_SEPERATOR) && (elmfwd->txt.size())))
					{
						jvxBool foundit = false;
						idx1 = idx0;
						while (1)
						{
							idx2 = idx1 + elmfwd->txt.size() - 1;
							if (idx2 < message.size())
							{
								tok2 = message.substr(idx1, idx2 - idx1 + 1);
								if (tok2 == elmfwd->txt)
								{
									foundit = true;
									break;
								}
								idx1++;
							}
							else
							{
								return false;
							}
						}
						assert(foundit);

						// Text token BEFORE actual separator text
						tok1 = message.substr(idx0, idx1 - idx0);

						if (elm->tp == JVX_CONTROL_DEVICE_REFERENCE_PROPERTY)
						{
							reportOneProperty newP;
							newP.descr = elm->descr;
							newP.convertme = tok1;
							newP.conversion_id = elm->conversion_id;
							newP.txtref = elm->txt;
							report_props.push_back(newP);

							/*
							if (interact)
							{
								std::string mess = __FUNCTION__;
								mess += "Property text = <";
								mess += tok1;
								mess += "> to be passed to processor with conversion id <";
								mess += jvx_size2String(elm->conversion_id);
								mess += "> -> ";

								jvxErrorType resL = interact->convert_txt_2_property(&elm->descr, elm->conversion_id, tok1, elm->txt, command_id);

								mess += jvxErrorType_txt(resL);
								mess += ".";

								interact->report_message(mess);
							}
							*/
						}
						idx0 = idx2 + 1;
						elm = elmfwd;
					}
					else
					{
						std::cout << __FUNCTION__ << "::" << __LINE__ << ": undecodable response specification." << std::endl;
						assert(0);
					}
				} // else if (elmfwd == lst.end())
			} // if (JVX_CHECK_SIZE_UNSELECTED(elm->length))
			else
			{
				if (elm->tp == JVX_CONTROL_DEVICE_REFERENCE_PROPERTY)
				{
					tok2 = message.substr(idx0, elm->length);

					reportOneProperty newP;
					newP.descr = elm->descr;
					newP.convertme = tok2;
					newP.conversion_id = elm->conversion_id;
					newP.txtref = elm->txt;
					report_props.push_back(newP);
					
					// Process property!!!
					/*
					if (interact)
					{
						std::string mess = __FUNCTION__;
						mess += "Property text = <";
						mess += tok2;
						mess += "> to be passed to processor with conversion id <";
						mess += jvx_size2String(elm->conversion_id);
						mess += "> ->";

						jvxErrorType resL = interact->convert_txt_2_property(&elm->descr, elm->conversion_id, tok2, elm->txt, command_id);

						mess += jvxErrorType_txt(resL);
						mess += ".";

						interact->report_message(mess);
					}
					*/
					idx0 += elm->length;
				}
				else
				{
					assert(0);
				}
			}
			break;

		default:
			/*
			case JVX_CONTROL_DEVICE_UNDEFINED:
			case JVX_CONTROL_DEVICE_REFERENCE_TOKEN:
			case JVX_CONTROL_DEVICE_REFERENCE_COMMAND:
			case JVX_CONTROL_DEVICE_REFERENCE_SEQUENCE:
			case JVX_CONTROL_DEVICE_REFERENCE:
			case JVX_CONTROL_DEVICE_INVALIDATED:
			*/
			assert(0);
		}
	}

	// New implementation which modifies propertis ONLY if message was completely parsed successfully
	auto elm_rep = report_props.begin();
	for (; elm_rep != report_props.end(); elm_rep++)
	{
		if (interact)
		{
			std::string mess = __FUNCTION__;
			mess += "Property text = <";
			mess += elm_rep->convertme;
			mess += "> passed to processor matching token <";
			mess += elm_rep->txtref;
			mess += ">, conversion id < ";
			mess += jvx_size2String(elm_rep->conversion_id);
			mess += ">, command id <";
			mess += jvx_size2String(command_id);
			mess += ">";

			jvxErrorType resL = interact->convert_txt_2_property(&elm_rep->descr, elm_rep->conversion_id, 
				elm_rep->convertme, elm_rep->txtref, command_id);

			mess += " -- Return value from processor: ";
			mess += jvxErrorType_txt(resL);
			mess += ".";

			interact->report_message(mess);
		}
	}
	return true;
}

jvxErrorType 
CjvxFlexibleTextControlDevice::property_set(jvxPropertyCategoryType category, jvxSize handleIdx, jvxSize id_prop_command)
{
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;
	jvxSize i;
	auto elm = lst_setprops.begin();

	for (; elm != lst_setprops.end(); elm++)
	{
		if (
			(elm->second.descr.category == category) &&
			(elm->second.descr.globalIdx == handleIdx))
		{
			switch (elm->second.ref.tp)
			{
			case JVX_CONTROL_DEVICE_REFERENCE_COMMAND:

				if (elm->second.ref.txtl.size())
				{
					for (i = 0; i < elm->second.ref.txtl.size(); i++)
					{
						res = trigger_send_command(elm->second.ref.txtl[i], id_prop_command);
					}
				}
				else
				{
					res = trigger_send_command(elm->second.ref.txt, id_prop_command);
				}
				break;

			case JVX_CONTROL_DEVICE_REFERENCE_SEQUENCE:
				if (elm->second.ref.txtl.size())
				{
					for (i = 0; i < elm->second.ref.txtl.size(); i++)
					{
						res = trigger_send_seq(elm->second.ref.txtl[i], false);
					}
				}
				else
				{
					res = trigger_send_seq(elm->second.ref.txt, false);
				}
				break;

			default:
				assert(0);

			}
			break;
		}
	}
	return res;
}

jvxSize 
CjvxFlexibleTextControlDevice::check_callback_id(jvxConfigData* datTmp, IjvxConfigProcessor* confProc)
{
	jvxSize id = JVX_SIZE_UNSELECTED;

	jvxConfigData* datTmpTmp = NULL;
	jvxValue val;

	confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "CALLBACK_ID");
	if (datTmpTmp)
	{
		jvxErrorType resL = confProc->getAssignmentValue(datTmpTmp, &val);
		if (resL == JVX_NO_ERROR)
		{
			val.toContent(&id);
		}
	}
	return id;
}

jvxBool
CjvxFlexibleTextControlDevice::check_verbose(jvxConfigData* datTmp, IjvxConfigProcessor* confProc)
{
	jvxConfigData* datTmpTmp = NULL;
	jvxApiString strRet;

	confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "VERBOSE");
	if (datTmpTmp)
	{
		jvxErrorType resL = confProc->getAssignmentString(datTmpTmp, &strRet);
		if (resL == JVX_NO_ERROR)
		{
			if (strRet.std_str() == "yes")
			{
				return true;
			}
		}
	}
	return false;
}

jvxSize
CjvxFlexibleTextControlDevice::check_length(jvxConfigData* datTmp, IjvxConfigProcessor* confProc)
{
	jvxConfigData* datTmpTmp = NULL;
	jvxValue val;
	jvxSize ll = JVX_SIZE_UNSELECTED;;

	confProc->getReferenceEntryCurrentSection_name(datTmp, &datTmpTmp, "LENGTH");
	if (datTmpTmp)
	{
		jvxErrorType resL = confProc->getAssignmentValue(datTmpTmp, &val);
		if (resL == JVX_NO_ERROR)
		{
			val.toContent(&ll);
		}
	}
	return ll;
}

jvxErrorType 
CjvxFlexibleTextControlDevice::cleared_messages()
{
	auto elm = lst_sequences.begin();
	for (; elm != lst_sequences.end(); elm++)
	{
		if (elm->second.status_operate == true)
		{
			elm->second.status_operate = false;
			jvx_bitFClear(elm->second.returned_response_flags);
			jvx_bitFClear(elm->second.complete_response_flags);
			jvx_bitFClear(elm->second.error_response_flags);
		}
	}
	return JVX_NO_ERROR;
}

void 
CjvxFlexibleTextControlDevice::report_observer_timeout()
{
	std::cout << "Observe the module to detect stagnation." << std::endl;
	if (interact)
	{
		interact->decide_quality(&monitor.quality, monitor.num_incoming_messages,
			monitor.num_skip_messages, monitor.num_unmatched_messages, monitor.num_unknown_messages);
		translate__flex_remote__monitor__device_state_to(monitor.quality);
	}
}
