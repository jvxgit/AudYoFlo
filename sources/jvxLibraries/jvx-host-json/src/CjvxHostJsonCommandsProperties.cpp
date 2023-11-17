#include "CjvxHostJsonCommandsProperties.h"
#include "CjvxHostJsonDefines.h"

extern "C"
{
#include "jvx-crc.h"
}

/**
 * Argument filter_purpose: can be either "list" or an expression composed of one among
 * "min", "core", "control", "full", "fullp" in combination with | "x" to show also the extended 
 * information.
 * Argument filter_exprs: Filter expression what to filter the returned expression with. 
 * The filter expression ma arise from a [] expression with "," to separate entries if 
 * multiple or a direct specification if one 
 */
jvxErrorType
CjvxHostJsonCommandsProperties::show_property_list(jvxComponentIdentification tp,
	const std::string& filter_purpose, const std::string& filter_prop_str,  
	CjvxJsonElementList& jelmlst_ret, std::string& errTxt)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxSize num, i, j;
	jvxCallManagerProperties callGate;
	jvx_propertyReferenceTriple theTriple;
	jvx_initPropertyReferenceTriple(&theTriple);
	std::vector<std::string> filter_exprs;

	//jvx_getReferencePropertiesObject(hHost, &theTriple, tp);
	this->requestReferencePropertiesObject(theTriple, tp);

	if (jvx_isValidPropertyReferenceTriple(&theTriple))
	{
		jvxSize showCnt = 0;
		CjvxJsonElementList jelml_allprops;
		CjvxJsonElement jelm_allprops;
		jvxBool errBracketsNotFound = false;
		if (!filter_prop_str.empty())
		{
			std::string propBr = jvx_parseStringFromBrackets(filter_prop_str, errBracketsNotFound, '[', ']');
			if (errBracketsNotFound)
			{
				filter_exprs.push_back(filter_prop_str);
			}
			else
			{
				filter_exprs.clear();
				jvx::helper::parseStringListIntoTokens(propBr, filter_exprs);
			}
		}

		res = theTriple.theProps->number_properties(callGate, &num);
		if (num > 0)
		{
			if (filter_purpose == "list")
			{
				CjvxJsonArray jelmarr;
				for (i = 0; i < num; i++)
				{
					jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
					jvx::propertyAddress::CjvxPropertyAddressLinear ident(i);
					jvxBool showThis = false;

					res = jvx_getPropertyDescription(theTriple.theProps, theDescr, ident, callGate);

					if (filter_exprs.empty())
					{
						showThis = true;
					}
					else
					{
						showThis = false;
						for (auto elm : filter_exprs)
						{
							std::string token = elm;
							if (jvx_compareStringsWildcard(token, theDescr.descriptor.std_str()))
							{
								showThis = true;
							}
						}
					}

					if (showThis)
					{
						CjvxJsonArrayElement jelmarr_elm;
						jelmarr_elm.makeString(theDescr.descriptor.std_str());
						jelmarr.addConsumeElement(jelmarr_elm);
					}
				}
				jelm_allprops.makeArray("properties", jelmarr);
			}
			else
			{
				jvxSize ii;
				jvxBool withExtendedInfo = false;
				std::vector<std::string> locArgs;
				jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus theDescr;

				jvx::propertyDescriptor::CjvxPropertyDescriptorMin* dmin = nullptr;
				jvx::propertyDescriptor::CjvxPropertyDescriptorCore* dcore = nullptr;
				jvx::propertyDescriptor::CjvxPropertyDescriptorControl* dctrl = nullptr;
				jvx::propertyDescriptor::CjvxPropertyDescriptorFull* dfull = nullptr;
				jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus* dfullp = nullptr;

				jvx::helper::parseStringListIntoTokens(filter_purpose, locArgs, '|');

				for (ii = 0; ii < locArgs.size(); ii++)
				{
					if (locArgs[ii] == "x")
					{
						withExtendedInfo = true;
					}
					else if (locArgs[ii] == "min")
					{
						dmin = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorMin>(&theDescr);
					}
					else if (locArgs[ii] == "core")
					{
						dmin = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorMin>(&theDescr);
						dcore = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorCore>(&theDescr);
					}
					else if (locArgs[ii] == "control")
					{
						dmin = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorMin>(&theDescr);
						dcore = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorCore>(&theDescr);
						dctrl = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorControl>(&theDescr);
					}
					else if (locArgs[ii] == "full")
					{
						dmin = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorMin>(&theDescr);
						dcore = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorCore>(&theDescr);
						dctrl = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorControl>(&theDescr);
						dfull = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorFull>(&theDescr);
					}
					else if (locArgs[ii] == "fullp")
					{
						dmin = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorMin>(&theDescr);
						dcore = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorCore>(&theDescr);
						dctrl = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorControl>(&theDescr);
						dfull = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorFull>(&theDescr);
						dfullp = castPropDescriptor< jvx::propertyDescriptor::CjvxPropertyDescriptorFullPlus>(&theDescr);
					}
					else
					{
						res = JVX_ERROR_PARSE_ERROR;
						errTxt = "The filter purpose is not correct. It should be a combination from [min,core,control,full,fullp]|[x].";
					}
				}
				if (res == JVX_NO_ERROR)
				{
					for (i = 0; i < num; i++)
					{
						theDescr.reset();

						jvx::propertyAddress::CjvxPropertyAddressLinear ident(i);
						CjvxJsonElementList jelmlst_onepro;
						CjvxJsonElement jelm_onepro;
						jvxBool showThis = false;
						std::string stateExpr;

						jvxApiString theExtendedPropsStr;

						// Obtain the property descriptor
						res = jvx_getPropertyDescription(theTriple.theProps, theDescr, ident, callGate);

						if (filter_exprs.empty())
						{
							showThis = true;
						}
						else
						{
							showThis = false;
							for (auto elm : filter_exprs)
							{
								std::string token = elm;
								if (jvx_compareStringsWildcard(token, theDescr.descriptor.std_str()))
								{
									showThis = true;
								}
							}
						}

						if (showThis)
						{
							if (dmin)
							{
								JVX_CREATE_PROPERTY_CATEGORY(jelm_onepro, jvxPropertyCategoryType_str[dmin->category].friendly);
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								JVX_CREATE_PROPERTY_GLOBALID(jelm_onepro, (int)dmin->globalIdx);
								jelmlst_onepro.addConsumeElement(jelm_onepro);
							}

							if (dcore)
							{
								JVX_CREATE_PROPERTY_FORMAT(jelm_onepro, jvxDataFormat_txt(dcore->format));
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								JVX_CREATE_PROPERTY_NUM(jelm_onepro, (int)dcore->num);
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								JVX_CREATE_PROPERTY_ACCESS_TYPE(jelm_onepro, jvxPropertyAccessType_str[dcore->accessType].friendly);
								jelmlst_onepro.addConsumeElement(jelm_onepro);


								JVX_CREATE_PROPERTY_DECODER_TYPE(jelm_onepro, jvxPropertyDecoderHintType_str[dcore->decTp].friendly);
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								JVX_CREATE_PROPERTY_CONTEXT(jelm_onepro, jvxPropertyContext_str[dcore->ctxt].friendly);
								jelmlst_onepro.addConsumeElement(jelm_onepro);
							}

							if (dctrl)
							{
								for (ii = 0; ii < JVXSTATE_NUM; ii++)
								{
									if (jvx_bitTest(dctrl->allowedStateMask, ii))
									{
										if (!stateExpr.empty())
										{
											stateExpr += ",";
										}
										stateExpr += jvxState_dec((jvxSize)1 << ii);
									}
								}
								stateExpr = "[" + stateExpr + "]";
								JVX_CREATE_PROPERTY_ALLOWED_STATE_MASK(jelm_onepro, stateExpr); // jvx_uint642Hexstring(theDescr.allowedStateMask)
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								JVX_CREATE_PROPERTY_ALLOWED_THREADING_MASK(jelm_onepro, jvx_uint162Hexstring(theDescr.allowedThreadingMask));
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								// this variable is not a bool. However, the reader expects a bool here!! Need to be fixed!
								JVX_CREATE_PROPERTY_INVALIDATE_STATE_SWITCH(jelm_onepro, dctrl->invalidateOnStateSwitch);
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								JVX_CREATE_PROPERTY_INVALIDATE_TEST(jelm_onepro, dctrl->invalidateOnTest); // JVX_PROPERTY_INVALIDATE_INACTIVE todo
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								JVX_CREATE_PROPERTY_INSTALLABLE(jelm_onepro, dctrl->installable);
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								if (dctrl->isValid)
								{
									JVX_CREATE_PROPERTY_VALID(jelm_onepro, JVX_INDICATE_TRUE_SHORT);
								}
								else
								{
									JVX_CREATE_PROPERTY_VALID(jelm_onepro, JVX_INDICATE_FALSE_SHORT);
								}
								jelmlst_onepro.addConsumeElement(jelm_onepro);
							}

							if (dfull)
							{
								JVX_CREATE_COMPONENT_DESCRIPTOR(jelm_onepro, dfull->descriptor.std_str());
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								JVX_CREATE_COMPONENT_DESCRIPTION(jelm_onepro, dfull->description.std_str());
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								JVX_CREATE_COMPONENT_NAME(jelm_onepro, dfull->name.std_str());
								jelmlst_onepro.addConsumeElement(jelm_onepro);
							}

							if (dfullp)
							{
								JVX_CREATE_ACCESS_FLAGS_RWCD(jelm_onepro, jvx_uint642Hexstring(dfullp->accessFlags));
								jelmlst_onepro.addConsumeElement(jelm_onepro);

								JVX_CREATE_CFG_MODE(jelm_onepro, jvx_uint162Hexstring(dfullp->configModeFlags));
								jelmlst_onepro.addConsumeElement(jelm_onepro);

							}

							if (withExtendedInfo)
							{
								jvxErrorType resL = theTriple.theProps->get_property_extended_info(callGate,
									&theExtendedPropsStr, JVX_PROPERTY_INFO_SHOWHINT,
									jPAGID(theDescr.globalIdx, theDescr.category));
								if (resL == JVX_NO_ERROR)
								{
									JVX_CREATE_PROPERTY_SHOWHINT(jelm_onepro, theExtendedPropsStr.std_str());
									jelmlst_onepro.addConsumeElement(jelm_onepro);
								}
								resL = theTriple.theProps->get_property_extended_info(callGate,
									&theExtendedPropsStr, JVX_PROPERTY_INFO_MOREINFO,
									jPAGID(theDescr.globalIdx, theDescr.category));
								if (resL == JVX_NO_ERROR)
								{
									JVX_CREATE_PROPERTY_MOREINFO(jelm_onepro, theExtendedPropsStr.std_str());
									jelmlst_onepro.addConsumeElement(jelm_onepro);
								}
								resL = theTriple.theProps->get_property_extended_info(callGate,
									&theExtendedPropsStr, JVX_PROPERTY_INFO_ORIGIN, jPAGID(theDescr.globalIdx,
										theDescr.category));
								if (resL == JVX_NO_ERROR)
								{
									JVX_CREATE_PROPERTY_ORIGIN(jelm_onepro, theExtendedPropsStr.std_str());
									jelmlst_onepro.addConsumeElement(jelm_onepro);
								}
							}
							jelm_onepro.makeSection(theDescr.descriptor.std_str(), jelmlst_onepro);
							jelml_allprops.addConsumeElement(jelm_onepro);
						}
					}
					jelm_allprops.makeSection("properties", jelml_allprops);
				}
			}
			jelmlst_ret.addConsumeElement(jelm_allprops);
		} // if (num > 0)

		// jvx_returnReferencePropertiesObject(hHost, &theTriple, tp);
		this->returnReferencePropertiesObject(theTriple, tp);
	}
	else
	{
		/*
		* No reference is ok - just no property returned
		errTxt = "The property reference could not be obtained for component <" + jvxComponentIdentification_txt(tp) + ", reason: ";
		errTxt += jvxErrorType_descr(res);
		JVX_CREATE_ERROR_RETURN(jelmlst_ret, res, errTxt);
		*/
	}
	return res;
}











jvxErrorType
CjvxHostJsonCommandsProperties::show_property_component(
	jvxComponentIdentification tp,
	const std::string identificationTarget,
	const std::vector<std::string>& args,
	jvxSize off,
	CjvxJsonElementList& jelmret,
	jvxBool content_only,
	jvxBool compact, std::string& errTxt)
{
	jvxCallManagerProperties callGate;
	jvxErrorType res = JVX_NO_ERROR;
	jvxErrorType resL = JVX_NO_ERROR;	

	jvx_propertyReferenceTriple theTriple;
	std::string value;
	std::string valuePP;
	jvxSize idS;
	jvxSize offsetS = 0;
	jvxSize numElms = JVX_SIZE_UNSELECTED;
	jvxBool err = false;
	jvxBool contentOnlyBool = false;
	jvxContext ctxt;
	jvxContext* ctxt_ptr = NULL;
	jvxSize offset = 0;
	std::string ctxtStr;
	std::vector<std::string> elms;
	CjvxJsonArray jelm_arr;
	CjvxJsonArrayElement jelml_arr;

	std::string identificationTargetl;
	std::string offsetStartl;
	std::string numElementsl;
	std::string contentOnlyl;
	std::string contextl;

	if (args.size() > off + 1)
	{
		offsetStartl = (args)[off + 1];
	}
	if (args.size() > off + 2)
	{
		numElementsl = (args)[off + 2];
	}
	if (args.size() > off + 3)
	{
		contentOnlyl = (args)[off + 3];
	}
	if (args.size() > off + 4)
	{
		contextl = (args)[off + 4];
	}

	if (jvx_check_yes(contentOnlyl))
	{
		contentOnlyBool = true;
	}

	if (content_only)
	{
		contentOnlyBool = true;
	}

	if (!contextl.empty())
	{
		resL = jvx_string2Context(contextl, &ctxt);
		if (resL == JVX_NO_ERROR)
		{
			ctxt_ptr = &ctxt;
		}
	}

	if (!offsetStartl.empty())
	{
		offsetS = jvx_string2Size(offsetStartl, err);
		if (err)
		{
			res = JVX_ERROR_PARSE_ERROR;
			errTxt = "Failed to convert property offset to index for property with descriptor <" + identificationTarget + ">  for component <" + jvxComponentIdentification_txt(tp) + ", reason: ";
			errTxt += jvxErrorType_descr(res);
			return res;
		}
	}
	if (!numElementsl.empty())
	{
		numElms = jvx_string2Size(numElementsl, err);
		if (err)
		{
			res = JVX_ERROR_PARSE_ERROR;
			errTxt = "Failed to convert property offset to number elements for property with descriptor <" + identificationTarget + ">  for component <" + jvxComponentIdentification_txt(tp) + ", reason: ";
			errTxt += jvxErrorType_descr(res);
			return res;
		}
	}
	/*
	if (jvx_check_no(contentOnly))
	{
		contentOnlyBool = false;
	}
	*/
	jvx_initPropertyReferenceTriple(&theTriple);

	this->requestReferencePropertiesObject(theTriple, tp);

	if (jvx_isValidPropertyReferenceTriple(&theTriple))
	{
		jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
		idS = jvx_string2Size(identificationTarget, err);
		if (!err)
		{
			jvx::propertyAddress::CjvxPropertyAddressLinear ident(idS);
			// Variant I: address property by an index
			res = theTriple.theProps->description_property(callGate,
				theDescr, ident);							
		}
		else
		{
			jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(identificationTarget.c_str());
			res = theTriple.theProps->description_property(callGate, theDescr, ident);
		}
		if (res == JVX_NO_ERROR)
		{
			jvxBool noEntry = false;
			callGate.context = ctxt_ptr;
			callGate.on_get.prop_access_type = &theDescr.accessType;

			res = jvx::helper::properties::toString(theTriple, callGate, theDescr, value, valuePP, config_show_ref.numdigits, config_show_ref.outputPropertyFieldsBase64,
				offsetS, numElms, contentOnlyBool, &noEntry);
			if (res == JVX_NO_ERROR)
			{
				CjvxJsonElement jelm;

				// Avoid problems with escape and non-utf8 chars
				value = jvx::helper::filterEscapes(jvx::helper::asciToUtf8(value));

				if (config_show_ref.get_property_compact || compact)
				{
					jvxUInt32 bf = 0;
					jvxUInt16 checksum = jvx_crc16_ccitt(identificationTarget.c_str(), identificationTarget.size());

					if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
					{

						if (offset > 0)
						{
							JVX_CREATE_PROPERTY_OFFSET_SHORT(jelm, 0);
							jelmret.addConsumeElement(jelm);
						}
						/* Numer should be visible from property string */
						// JVX_CREATE_PROPERTY_NUM_SHORT(jelm, theDescr.num);
						// jelmret.addConsumeElement(jelm);

						bf = (jvxUInt32)theDescr.format & 0x1F; // 5 bits

						bf = bf << 5;
						bf = bf | ((jvxUInt32)theDescr.decTp & 0x1F); // another 5 bits

						bf = bf << 4;
						bf = bf | ((jvxUInt32)theDescr.accessType & 0xF); // another 4 bits

						bf = bf << 1;
						if (theDescr.isValid)
						{
							bf = bf | 0x1; // another 1 bit
						}

						bf = bf | checksum << 16;

						JVX_CREATE_PROPERTY_PROPS_SHORT(jelm, jvx_uint322Hexstring(bf));
						jelmret.addConsumeElement(jelm);

						if (ctxt_ptr)
						{
							jvx_context2String(ctxtStr, ctxt_ptr);
							jelm.makeAssignmentString("cc", ctxtStr);
							jelmret.addConsumeElement(jelm);
						}

						jelm.makeAssignmentString("p", value);
						jelmret.addConsumeElement(jelm);

						switch (theDescr.format)
						{
						case JVX_DATAFORMAT_SELECTION_LIST:
						case JVX_DATAFORMAT_STRING_LIST:

							if (!noEntry)
							{
								jvx::helper::parseStringListIntoTokens(valuePP, elms);
								for (auto elm : elms)
								{
									jelml_arr.makeString(elm);
									jelm_arr.addConsumeElement(jelml_arr);
								}
								jelm.makeArray("pf", jelm_arr);
								jelmret.addConsumeElement(jelm);
							}

							break;
						default:
							if (!valuePP.empty())
							{
								jelm.makeAssignmentString("pf", value);
								jelmret.addConsumeElement(jelm);
							}
							break;
						}
					}
					else
					{
						jelm.makeAssignmentString("a", jvxAccessProtocol_txt(callGate.access_protocol));
						jelmret.addConsumeElement(jelm);
					}
				}
				else
				{
					JVX_CREATE_PROPERTY_DESCRIPTOR(jelm, identificationTarget);
					jelmret.addConsumeElement(jelm);

					if (callGate.access_protocol == JVX_ACCESS_PROTOCOL_OK)
					{
						JVX_CREATE_PROPERTY_FORMAT(jelm, jvxDataFormat_txt(theDescr.format));
						jelmret.addConsumeElement(jelm);
						JVX_CREATE_PROPERTY_OFFSET(jelm, 0);
						jelmret.addConsumeElement(jelm);
						JVX_CREATE_PROPERTY_NUM(jelm, JVX_SIZE_INT32(theDescr.num));
						jelmret.addConsumeElement(jelm);
						JVX_CREATE_PROPERTY_DECODER_TYPE(jelm, jvxPropertyDecoderHintType_txt(theDescr.decTp));
						jelmret.addConsumeElement(jelm);
						JVX_CREATE_PROPERTY_ACCESS_TYPE(jelm, jvxPropertyAccessType_txt(theDescr.accessType));
						jelmret.addConsumeElement(jelm);
						if (theDescr.isValid)
						{
							JVX_CREATE_PROPERTY_VALID(jelm, JVX_INDICATE_TRUE_SHORT);
						}
						else
						{
							JVX_CREATE_PROPERTY_VALID(jelm, JVX_INDICATE_TRUE_SHORT);
						}
						jelmret.addConsumeElement(jelm);
						if (ctxt_ptr)
						{
							jvx_context2String(ctxtStr, ctxt_ptr);
							JVX_CREATE_PROPERTY_CALL_CONTEXT(jelm, ctxtStr);
							jelmret.addConsumeElement(jelm);
						}


						switch (theDescr.format)
						{
						case JVX_DATAFORMAT_SELECTION_LIST:
							jelm.makeAssignmentString("property", value);
							jelmret.addConsumeElement(jelm);
#if __cplusplus >= 201703L
							[[fallthrough]];
#endif

						case JVX_DATAFORMAT_STRING_LIST:
							
							// Empty lists should not produce an entry here. Otherwise, there is no chance to distinguish
							// between "" and empty
							if (!noEntry)
							{
								jvx::helper::parseStringListIntoTokens(valuePP, elms);
								for (auto elm : elms)
								{
									jelml_arr.makeString(elm);
									jelm_arr.addConsumeElement(jelml_arr);
								}
								jelm.makeArray("property_fld", jelm_arr);
								jelmret.addConsumeElement(jelm);
							}
							break;
						default:
							jelm.makeAssignmentString("property", value);
							jelmret.addConsumeElement(jelm);

							if (!valuePP.empty())
							{
								jelm.makeAssignmentString("property_fld", valuePP);
								jelmret.addConsumeElement(jelm);
							}
							break;
						}
					}
					else
					{
						JVX_CREATE_ACCESS_PROTOCOL(jelm, jvxAccessProtocol_txt(callGate.access_protocol));
						jelmret.addConsumeElement(jelm);
					}
				}
			}
			else
			{
				errTxt = "Failed to convert property content to expression for property with descriptor <" + identificationTarget + ">  for component <" + jvxComponentIdentification_txt(tp) + ", reason: ";
				errTxt += jvxErrorType_descr(res);
			}
		}
		else
		{
			errTxt = "The property value with descriptor <" + identificationTarget + "> could not be obtained for component <" + jvxComponentIdentification_txt(tp) + ", reason: ";
			errTxt += jvxErrorType_descr(res);
		}
		this->returnReferencePropertiesObject(theTriple, tp);
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		errTxt = "The property value with descriptor <" + identificationTarget + "> can not be found for component <" + jvxComponentIdentification_txt(tp) + ".";
		errTxt += jvxErrorType_descr(res);
	}
	return res;
}

jvxErrorType
CjvxHostJsonCommandsProperties::show_property_component_list(
	jvxComponentIdentification tp,
	const std::string& args,
	CjvxJsonElementList& jelmret,
	jvxBool content_only,
	jvxBool compact, std::string& errTxt)
{
	jvxErrorType resP = JVX_NO_ERROR;
	CjvxJsonElementList jelmlst_props;
	CjvxJsonElementList jelmret_local;
	CjvxJsonElement jelm_entry;
	std::vector<std::string> tokens;
	jvx::helper::parseStringListIntoTokens(args, tokens); // , ';');
	for (auto elm : tokens)
	{
		std::string addElement = elm;
		std::vector<std::string> argsIn;
		jvxBool errBracketsNotFound = false;
		std::string argsBr = jvx_parseStringFromBrackets(elm, errBracketsNotFound, '(', ')');
		if (errBracketsNotFound)
		{
			// Is it a wildcard?
			size_t pos = elm.find("*");
			if (pos != std::string::npos)
			{
				jvx_propertyReferenceTriple theTriple;
				jvx_initPropertyReferenceTriple(&theTriple);
				this->requestReferencePropertiesObject(theTriple, tp);
				if (jvx_isValidPropertyReferenceTriple(&theTriple))
				{
					jvxSize i;
					jvxSize num = 0;
					jvxCallManagerProperties callGate;
					theTriple.theProps->number_properties(callGate, &num);
					for (i = 0; i < num; i++)
					{
						jvx::propertyDescriptor::CjvxPropertyDescriptorFull theDescr;
						jvx::propertyAddress::CjvxPropertyAddressLinear ident(i);
						theTriple.theProps->description_property(callGate,
							theDescr, ident);
						if (jvx_compareStringsWildcard(elm, theDescr.descriptor.std_str()))
						{
							attach_single_property_json_list(tp, theDescr.descriptor.std_str(), argsIn, 0, jelmlst_props, content_only, compact);
						}
					}
					this->returnReferencePropertiesObject(theTriple, tp);
				}
			}
			else
			{
				attach_single_property_json_list(tp, elm, argsIn, 0, jelmlst_props, content_only, compact);
				//resP = show_property_component(tp, elm, argsIn, 0, jelmret_local);
			}
		}
		else
		{
			jvx::helper::parseStringListIntoTokens(argsBr, argsIn);
			std::string idTarget = "";
			if (argsIn.size() > 0)
			{
				idTarget = argsIn[0];
				attach_single_property_json_list(tp, idTarget, argsIn, 0, jelmlst_props, content_only, compact);
				//resP = show_property_component(tp, idTarget, argsIn, 1, jelmret_local);
				//addElement = idTarget;
			}
			else
			{
				attach_single_property_json_list(tp, argsBr, argsIn, 0, jelmlst_props, content_only, compact);
				// resP = show_property_component(tp, elm, argsIn, 1, jelmret_local);

			}
		}

		/*
		jelm_entry.makeAssignmentString("return_code", jvxErrorType_str[resP].friendly);
		jelmret_local.insertConsumeElementFront(jelm_entry);

		jvxBool alreadyThere = false;
		jvxSize num = 0, i = 0;
		CjvxJsonElement* jelm_entry_ptr = nullptr;
		jelmlst_props.reference_element(&jelm_entry_ptr, elm);
		if (jelm_entry_ptr == nullptr)
		{
			// Add to property list
			jelm_entry.makeSection(elm, jelmret_local);
			jelmlst_props.addConsumeElement(jelm_entry);
		}
		else
		{
			jelmret_local.deleteAll();
		}
		*/
	}

	// Put the list into a "generic" section named properties
	jelm_entry.makeSection("properties", jelmlst_props);
	jelmret.addConsumeElement(jelm_entry);

	return JVX_NO_ERROR;
}

void
CjvxHostJsonCommandsProperties::attach_single_property_json_list(jvxComponentIdentification tp,
	const std::string& propDescr, std::vector<std::string>& argsIn, jvxSize off, CjvxJsonElementList& jelmlst_props,
	jvxBool content_only,
	jvxBool compact)
{
	CjvxJsonElementList jelmret_local;
	CjvxJsonElement jelm_entry;
	std::string attach = propDescr;
	std::string errTxt;
	jvxErrorType resP = show_property_component(tp, propDescr, argsIn, off, jelmret_local, content_only, compact, errTxt);
	if (resP != JVX_NO_ERROR)
	{
		JVX_CREATE_ERROR_NO_RETURN(jelmret_local, resP, errTxt);
	}
	jelm_entry.makeAssignmentString("return_code", jvxErrorType_str[resP].friendly);
	jelmret_local.insertConsumeElementFront(jelm_entry);

	if (propDescr.empty())
	{
		attach = "-no name-";
	}
	jvxBool alreadyThere = false;
	jvxSize num = 0, i = 0;
	CjvxJsonElement* jelm_entry_ptr = nullptr;
	jelmlst_props.reference_element(&jelm_entry_ptr, attach, CjvxJsonElement::JVX_JSON_ASSIGNMENT_VALUE_NON, 0, false);
	if (jelm_entry_ptr == nullptr)
	{
		// Add to property list
		jelm_entry.makeSection(attach, jelmret_local);
		jelmlst_props.addConsumeElement(jelm_entry);
	}
	else
	{
		jelmret_local.deleteAll();
	}
}

jvxErrorType
CjvxHostJsonCommandsProperties::act_set_property_component_single(
	const jvx_propertyReferenceTriple& theTriple, jvxCallManagerProperties& callGate,
	const std::string& identificationTarget,
	const std::string& loadTarget, const std::string& offsetStart, CjvxJsonElementList& jelmret, jvxBool reportSet, std::string& errTxt)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvx::propertyDescriptor::CjvxPropertyDescriptorCore theDescr;
	std::string value;
	std::string loadTargetPP;
	jvxSize idS;
	jvxSize offsetS = 0;
	jvxBool err = false;
	idS = jvx_string2Size(identificationTarget, err);
	if (!err)
	{
		jvx::propertyAddress::CjvxPropertyAddressLinear ident(idS);

		res = theTriple.theProps->description_property(callGate, theDescr, ident);
	}
	else
	{
		jvx::propertyAddress::CjvxPropertyAddressDescriptor ident(identificationTarget.c_str());
		res = theTriple.theProps->description_property(callGate, theDescr, ident);
	}
	if (res == JVX_NO_ERROR)
	{
		offsetS = jvx_string2Size(offsetStart, err);
		if (!err)
		{
			callGate.on_set.report_set = reportSet;
			res = jvx::helper::properties::fromString(theTriple, callGate, theDescr, loadTarget, loadTargetPP, offsetS);
			if (res != JVX_NO_ERROR)
			{
				errTxt = "Failed to convert expression <" + loadTarget + "> into a valid value for property with descriptor <" + identificationTarget + ">  for component <" + 
					jvxComponentIdentification_txt(theTriple.cpId.tp) + ", reason: ";
				errTxt += jvxErrorType_descr(res);
			}
		}
		else
		{
			errTxt = "Failed to convert expression <" + offsetStart + "> into a valid offset for property with descriptor <" + identificationTarget + ">  for component <" + 
				jvxComponentIdentification_txt(theTriple.cpId.tp) + ", reason: ";
			errTxt += jvxErrorType_descr(res);
		}
	}
	else
	{
		errTxt = "The property value with descriptor <" + identificationTarget + "> could not be obtained for component <" + 
			jvxComponentIdentification_txt(theTriple.cpId.tp) + ", reason: ";
		errTxt += jvxErrorType_descr(res);
	}
	
	return res;
}

jvxErrorType
CjvxHostJsonCommandsProperties::act_set_property_component_list(
	const jvx_propertyReferenceTriple& theTriple, jvxCallManagerProperties& callGate,
	const std::string& props, const std::string& loadTarget, 
	CjvxJsonElementList& jelmret, jvxBool reportSet, std::string& errTxt)
{
	CjvxJsonArray jsonArr;

	std::vector<std::string> args;
	jvxErrorType resL = jvx::helper::parseStringListIntoTokens(props, args, ',');
	for (auto str : args)
	{
		CjvxJsonArrayElement arr;
		CjvxJsonElementList locSec;
		CjvxJsonElement locElm;
		jvxBool errBracketsNotFound = false;
		// arr.makeSection()

		std::string propsBraces = jvx_parseStringFromBrackets(str, errBracketsNotFound, '(', ')');
		std::vector<std::string> elms;
		std::string idTarget = propsBraces;
		std::string load = loadTarget;
		std::string offs;
		resL = jvx::helper::parseStringListIntoTokens(propsBraces, elms, ',');
		if (elms.size() > 0)
		{
			idTarget = elms[0];
		}
		if (elms.size() > 1)
		{
			load = elms[1];
		}
		if (elms.size() > 2)
		{
			offs = elms[2];
		}

		resL = act_set_property_component_single(theTriple, callGate, idTarget, load, offs, locSec, reportSet, errTxt);
		locElm.makeAssignmentString("return_code", jvxErrorType_str[resL].friendly);
		locSec.addConsumeElement(locElm);
		JVX_CREATE_COMPONENT_DESCRIPTOR(locElm, idTarget);
		locSec.addConsumeElement(locElm);

		arr.makeSection(locSec);
		jsonArr.addConsumeElement(arr);
	}
	CjvxJsonElement locElm;
	locElm.makeArray("properties", jsonArr);
	jelmret.addConsumeElement(locElm);
	return JVX_NO_ERROR;
}

// Run the property get command
jvxErrorType
CjvxHostJsonCommandsProperties::act_get_property_component_core(jvxComponentIdentification tp, const std::string& props, const std::vector<std::string>& args,
	jvxSize off, CjvxJsonElementList& jelmret, jvxBool content_only, jvxBool compact, std::string& errTxt)
{
	if (!props.empty())
	{
		jvxBool errBracketsNotFound = false;
		std::string propBr = jvx_parseStringFromBrackets(props, errBracketsNotFound, '[', ']');
		if (errBracketsNotFound)
		{
			return show_property_component(
				tp, props, args, off, jelmret, content_only, compact, errTxt);
		}
		return show_property_component_list(
			tp, propBr, jelmret, content_only, compact, errTxt);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxHostJsonCommandsProperties::act_set_property_component_core(const jvxComponentIdentification& tp, const std::string& identificationTarget,
	const std::string& loadTarget, const std::string& offsetStart, CjvxJsonElementList& jelmret, jvxBool reportSet, jvxBool collect, std::string& errTxt)
{
	jvxErrorType res = JVX_NO_ERROR;
	jvxBool errBracketsNotFound = false;
	std::string props = jvx_parseStringFromBrackets(identificationTarget, errBracketsNotFound, '[', ']');
	jvx_propertyReferenceTriple theTriple;
	jvx_initPropertyReferenceTriple(&theTriple);
	this->requestReferencePropertiesObject(theTriple, tp);
	if (jvx_isValidPropertyReferenceTriple(&theTriple))
	{
		jvxCallManagerProperties callGate;		
		if (collect)
		{
			res = theTriple.theProps->start_property_report_collect(callGate);
		}
		if (res == JVX_NO_ERROR)
		{
			if (errBracketsNotFound)
			{
				res = act_set_property_component_single(theTriple, callGate,
					identificationTarget, loadTarget, offsetStart, jelmret, reportSet, errTxt);
			}
			else
			{
				res = act_set_property_component_list(theTriple, callGate,
					props, loadTarget, jelmret, reportSet, errTxt);
			}
			if (collect)
			{
				res = theTriple.theProps->stop_property_report_collect(callGate);
			}
		}
		this->returnReferencePropertiesObject(theTriple, tp);
	}
	else
	{
		res = JVX_ERROR_ELEMENT_NOT_FOUND;
		errTxt = "The property interface reference for component <" + jvxComponentIdentification_txt(tp) + " is not available.";
		errTxt += jvxErrorType_descr(res);
	}
	return res;
}
