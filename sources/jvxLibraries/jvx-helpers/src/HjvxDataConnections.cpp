#include "jvx-helpers.h"

#ifndef JVX_SKIP_EVENT_JSON
#include "CjvxJson.h"
#endif

jvxErrorType 
jvx_start_process(IjvxDataConnections* allConnections, 
	oneStartedConnection& theContainer, 
	jvxBool verbose_mode, jvxBool report_global 
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxSize cnt = 0;
	IjvxDataConnectionProcess* theProcess = NULL;
	jvxSize the_conn_id = JVX_SIZE_UNSELECTED;

#ifndef JVX_SKIP_EVENT_JSON
	CjvxJsonElementList jsonElmLst;
#endif

	jvxErrorType resL = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);
	std::string warnMess;
	resL = allConnections->create_connection_process(
		theContainer.process_name.c_str(), 
		&the_conn_id, 
		theContainer.interceptors, 
		theContainer.essentialForStart,
		verbose_mode,
		report_global);
	if (resL != JVX_NO_ERROR)
	{
		warnMess = "When trying to start process <";
		warnMess += theContainer.process_name;
		warnMess += ">: Process could not be created, error reason: ";
		warnMess += jvxErrorType_txt(resL);

		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, warnMess.c_str(), resL);

		return resL;
	}

	resL = allConnections->reference_connection_process_uid(the_conn_id, &theProcess);
	assert(resL == JVX_NO_ERROR);

	theContainer.theProc = theProcess;
	theContainer.theProc_id = the_conn_id;

	resL = theProcess->associate_master(theContainer.theConnMaster);
	assert(resL == JVX_NO_ERROR);

	std::map<jvxSize, oneBridgeContainer>::iterator elmB = theContainer.lstBridgeContainers.begin();
	for (; elmB != theContainer.lstBridgeContainers.end(); elmB++, cnt++)
	{
		resL = theProcess->create_bridge(elmB->second.outputC,
			elmB->second.inputC, elmB->second.bridge_name.c_str(),
			&elmB->second.theBId, false, false);
		assert(resL == JVX_NO_ERROR);
	}

	resL = theProcess->connect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	if (resL != JVX_NO_ERROR)
	{
		warnMess = "When trying to connect process <";
		warnMess += theContainer.process_name;
		warnMess += ">: Connecting the chain failed, error reason: ";
		warnMess += jvxErrorType_txt(resL);
		warnMess += ", error description: ";
		warnMess += JVX_CONNECTION_FEEDBACK_GET_ERROR_STRING(fdb);
		warnMess += ".";

		resL = allConnections->return_reference_connection_process(theProcess);
		allConnections->remove_connection_process(theContainer.theProc_id);
		JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, warnMess.c_str(), resL);
		return resL;
	}

#ifndef JVX_SKIP_EVENT_JSON
	resL = theProcess->transfer_forward_chain(JVX_LINKDATA_TRANSFER_COLLECT_LINK_JSON, &jsonElmLst JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	assert(resL == JVX_NO_ERROR);
#else
	assert(0);
#endif

	resL = allConnections->return_reference_connection_process(theProcess);
	assert(resL == JVX_NO_ERROR);

	std::string txt;
#ifndef JVX_SKIP_EVENT_JSON
	jsonElmLst.printToStringView(txt);
#else
	txt = "JSON not compiled for this release!";
#endif

	std::cout << "Process <" << theContainer.process_name << "> created and connected: " << txt << std::endl;
	
	return JVX_NO_ERROR;
};

// =======================================================================================================

jvxErrorType jvx_put_configuration_dataproc(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theReader, jvxConfigData* sectionToLookFor,
	const char* fName, int lineno, IjvxDataConnections* theDatConnections, std::vector<std::string>& warnMess)
{
	jvxConfigData* theSection = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	std::string oneWarnMess;

	//jvxApiString errmess;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);

	// ============================================================================================
	// Group at first
	// ============================================================================================

	std::string token = "group_connect_section";
	res = theReader->getReferenceEntryCurrentSection_name(sectionToLookFor, &theSection, token.c_str());
	if (res != JVX_NO_ERROR)
	{
		oneWarnMess = "Failed to read section <group_connect_section> from file ";
		oneWarnMess += fName;
		oneWarnMess += " in section starting at line ";
		oneWarnMess += jvx_int2String(lineno);
		oneWarnMess += ".";
		warnMess.push_back(oneWarnMess);
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	res = jvx_put_configuration_dataproc_processes_groups(callConf, theReader, theSection, fName, lineno, theDatConnections, warnMess, false);
	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	// ============================================================================================
	// Processes at second
	// ============================================================================================

	token = "process_connect_section";
	res = theReader->getReferenceEntryCurrentSection_name(sectionToLookFor, &theSection, token.c_str());
	if (res != JVX_NO_ERROR)
	{
		oneWarnMess = "Failed to read section <process_connect_section> from file ";
		oneWarnMess += fName;
		oneWarnMess += " in section starting at line ";
		oneWarnMess += jvx_int2String(lineno);
		oneWarnMess += ".";
		warnMess.push_back(oneWarnMess);
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}

	res = jvx_put_configuration_dataproc_processes_groups(callConf, theReader, theSection, fName, lineno, theDatConnections, warnMess, true);
	if (res != JVX_NO_ERROR)
	{
		return res;
	}

	return JVX_NO_ERROR;
}

void put_configuration_existing_process_group(
	jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theReader,
	jvxConfigData* theProcess,
	const char* fName, int lineno,
	jvxBool isProcess,
	jvxSize uIdCreated, 
	IjvxDataConnections* theDatConnections)
{
	if (isProcess)
	{
		IjvxDataConnectionProcess* proc = NULL;
		IjvxConfiguration* cfgHdl = NULL;
		theDatConnections->reference_connection_process_uid(uIdCreated, &proc);
		proc->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, reinterpret_cast<jvxHandle**>(&cfgHdl));
		if (cfgHdl)
		{
			cfgHdl->put_configuration(callConf, theReader, theProcess, fName, lineno);
			proc->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, reinterpret_cast<jvxHandle*>(cfgHdl));
		}
		theDatConnections->return_reference_connection_process(proc);
	}
	else
	{
		IjvxDataConnectionGroup* grp = NULL;
		IjvxConfiguration* cfgHdl = NULL;
		theDatConnections->reference_connection_group_uid(uIdCreated, &grp);
		grp->request_hidden_interface(JVX_INTERFACE_CONFIGURATION, reinterpret_cast<jvxHandle**>(&cfgHdl));
		if (cfgHdl)
		{
			cfgHdl->put_configuration(callConf, theReader, theProcess, fName, lineno);
			grp->return_hidden_interface(JVX_INTERFACE_CONFIGURATION, reinterpret_cast<jvxHandle*>(cfgHdl));
		}
		theDatConnections->return_reference_connection_group(grp);
	}
}


jvxErrorType jvx_put_configuration_dataproc_processes_groups(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theReader, jvxConfigData* theSection,
	const char* fName, int lineno, IjvxDataConnections* theDatConnections, std::vector<std::string>& warnMess,
	jvxBool isProcess)
{
	
	jvxConfigData* oneBridge = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* theProcess = NULL;
	jvxSize cntP = 0;
	jvxSize i;
	std::string token;
	std::string oneWarnMess;
	//jvxApiString errmess;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb_loc);

	jvxCallManagerConfiguration confCall;
	std::string prefix = "JVX_ACTIVE_PROCESS_";
	std::string tokenEntry = "JVX_CONNECTION_PROCESS_NAME";

	if (!isProcess)
	{
		prefix = "JVX_ACTIVE_GROUP_";
		tokenEntry = "JVX_CONNECTION_GROUP_NAME";
	}

	cntP = 0;
	while (1)
	{
		std::string tokenSec = prefix + jvx_size2String(cntP);
		res = theReader->getReferenceEntryCurrentSection_name(theSection, &theProcess, tokenSec.c_str());
		if ((res == JVX_NO_ERROR) && theProcess)
		{
			jvxBool takenFromAutoSetup = false;
			jvxConfigData* autoSetup = NULL;
			std::string findEntry;
			jvxValue val;
			jvxErrorType resL = JVX_NO_ERROR;
			theReader->getReferenceEntryCurrentSection_name(theProcess, &autoSetup, "CONNECTION_FROM_AUTOSETUP");
			if (autoSetup)
			{
				resL = theReader->getAssignmentValue(autoSetup, &val);
				if (resL == JVX_NO_ERROR)
				{
					val.toContent(&takenFromAutoSetup);
				}
			}
			if (takenFromAutoSetup)
			{
				jvxApiString astr;
				jvxConfigData* nmIdentify = NULL;
				theReader->getReferenceEntryCurrentSection_name(theProcess, &nmIdentify, "CONNECTION_NAME_REFERENCE");
				if (nmIdentify)
				{
					resL = theReader->getAssignmentString(nmIdentify, &astr);
					if (resL == JVX_NO_ERROR)
					{
						findEntry = astr.std_str();
					}
				}

				if (!findEntry.empty())
				{
					jvxSize uIdCreated = JVX_SIZE_UNSELECTED;
					jvxSize num;
					if (isProcess)
					{
						theDatConnections->number_connections_process(&num);
						for (i = 0; i < num; i++)
						{
							IjvxDataConnectionProcess* theProc = NULL;
							theDatConnections->reference_connection_process(i, &theProc);
							if (theProc)
							{
								theProc->descriptor_connection(&astr);
								if (findEntry == astr.std_str())
								{
									theDatConnections->uid_for_reference(theProc, &uIdCreated);
								}
							}
							theDatConnections->return_reference_connection_process(theProc);
							if (JVX_CHECK_SIZE_SELECTED(uIdCreated))
							{
								break;
							}
						}
					}
					else
					{
						theDatConnections->number_connections_group(&num);
						for (i = 0; i < num; i++)
						{
							IjvxDataConnectionGroup* theGrp = NULL;
							theDatConnections->reference_connection_group(i, &theGrp);
							if (theGrp)
							{
								theGrp->descriptor_connection(&astr);
								if (findEntry == astr.std_str())
								{
									theDatConnections->uid_for_reference(theGrp, &uIdCreated);
								}
							}
							theDatConnections->return_reference_connection_group(theGrp);
							if (JVX_CHECK_SIZE_SELECTED(uIdCreated))
							{
								break;
							}
						}
					}
					if (JVX_CHECK_SIZE_SELECTED(uIdCreated))
					{
						put_configuration_existing_process_group(callConf, theReader,
							theProcess, fName, lineno, isProcess, uIdCreated, theDatConnections);
					}
					else
					{
						oneWarnMess = "Trying to specify configuration parameters for unknown <";
						oneWarnMess += findEntry;
						oneWarnMess += ">.";
						warnMess.push_back(oneWarnMess);
					}
				}
			}
			else
			{
				CjvxConnectionDescription theNewConnection;
				jvxErrorType resL = JVX_NO_ERROR;
				std::string compTypeName;
				jvxBool parsingOk = true;
				jvxInt32 linenosec = -1;
				jvxApiString fn;
				jvxSize catId = JVX_SIZE_UNSELECTED;
				jvxBool interceptors = false;
				jvxBool essentialForStart = false;

				theReader->getOriginSection(theProcess, &fn, &linenosec);

				// Read the name of the process
				resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, theProcess, tokenEntry, &theNewConnection.process_name);
				if (resL != JVX_NO_ERROR)
				{
					oneWarnMess = "Failed to read entry for token ";
					oneWarnMess += token;
					oneWarnMess += " in section ";
					oneWarnMess += tokenSec;
					oneWarnMess += " starting in line ";
					oneWarnMess += jvx_int2String(linenosec);
					oneWarnMess += " in file <";
					oneWarnMess += fn.std_str();
					oneWarnMess += ">.";
					warnMess.push_back(oneWarnMess);
					parsingOk = false;
				}

				if (isProcess)
				{
					jvxConfigData* icCfg = NULL;

					jvxValue val;
					theReader->getReferenceEntryCurrentSection_name(theProcess, &icCfg, "PROCESS_INTERCEPTORS");
					if (icCfg)
					{
						theReader->getAssignmentValue(icCfg, &val);
						val.toContent(&interceptors);
					}

					icCfg = NULL;
					theReader->getReferenceEntryCurrentSection_name(theProcess, &icCfg, "PROCESS_ESSENTIAL");
					if (icCfg)
					{
						theReader->getAssignmentValue(icCfg, &val);
						val.toContent(&essentialForStart);
					}
					token = "MASTER_FACTORY_NAME";
					resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, theProcess, token, &theNewConnection.master_factory_name);
					if (resL != JVX_NO_ERROR)
					{
						oneWarnMess = "Failed to read entry for token ";
						oneWarnMess += token;
						oneWarnMess += " in section ";
						oneWarnMess += tokenSec;
						oneWarnMess += " starting in line ";
						oneWarnMess += jvx_int2String(linenosec);
						oneWarnMess += " in file <";
						oneWarnMess += fn.std_str();
						oneWarnMess += ">.";
						warnMess.push_back(oneWarnMess);
						parsingOk = false;
					}

					token = "MASTER_FACTORY_COMPONENT_TYPE";
					resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, theProcess, token, &compTypeName);
					if (resL != JVX_NO_ERROR)
					{
						oneWarnMess = "Failed to read entry for token ";
						oneWarnMess += token;
						oneWarnMess += " in section ";
						oneWarnMess += tokenSec;
						oneWarnMess += " starting in line ";
						oneWarnMess += jvx_int2String(linenosec);
						oneWarnMess += " in file <";
						oneWarnMess += fn.std_str();
						oneWarnMess += ">.";
						warnMess.push_back(oneWarnMess);
						parsingOk = false;
					}

					resL = jvxComponentType_decode(&theNewConnection.master_factory_tp.tp, compTypeName);
					if (resL != JVX_NO_ERROR)
					{
						oneWarnMess = "Failed to convert expression <";
						oneWarnMess += compTypeName;
						oneWarnMess += "> obtained for entry ";
						oneWarnMess += token;
						oneWarnMess += " in section ";
						oneWarnMess += tokenSec;
						oneWarnMess += " starting in line ";
						oneWarnMess += jvx_int2String(linenosec);
						oneWarnMess += " in file <";
						oneWarnMess += fn.std_str();
						oneWarnMess += ">";
						oneWarnMess += " into a valid component type.";
						warnMess.push_back(oneWarnMess);
						parsingOk = false;
					}
					token = "MASTER_FACTORY_SLOT_ID";
					resL = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, theProcess, token, &theNewConnection.master_factory_tp.slotid);
					if (resL != JVX_NO_ERROR)
					{
						oneWarnMess = "Failed to read entry for token ";
						oneWarnMess += token;
						oneWarnMess += " in section ";
						oneWarnMess += tokenSec;
						oneWarnMess += " starting in line ";
						oneWarnMess += jvx_int2String(linenosec);
						oneWarnMess += " in file <";
						oneWarnMess += fn.std_str();
						oneWarnMess += ">.";
						warnMess.push_back(oneWarnMess);
						parsingOk = false;
					}

					token = "MASTER_FACTORY_SLOT_SUB_ID";
					resL = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, theProcess, token, &theNewConnection.master_factory_tp.slotsubid);
					if (resL != JVX_NO_ERROR)
					{
						oneWarnMess = "Failed to read entry for token ";
						oneWarnMess += token;
						oneWarnMess += " in section ";
						oneWarnMess += tokenSec;
						oneWarnMess += " starting in line ";
						oneWarnMess += jvx_int2String(linenosec);
						oneWarnMess += " in file <";
						oneWarnMess += fn.std_str();
						oneWarnMess += ">.";
						warnMess.push_back(oneWarnMess);
						parsingOk = false;
					}

					token = "MASTER_NAME";
					resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, theProcess, token, &theNewConnection.master_name);
					if (resL != JVX_NO_ERROR)
					{
						oneWarnMess = "Failed to read entry for token ";
						oneWarnMess += token;
						oneWarnMess += " in section ";
						oneWarnMess += tokenSec;
						oneWarnMess += " starting in line ";
						oneWarnMess += jvx_int2String(linenosec);
						oneWarnMess += " in file <";
						oneWarnMess += fn.std_str();
						oneWarnMess += ">.";
						warnMess.push_back(oneWarnMess);
						parsingOk = false;
					}
				}

				// =================================================================
				// Bridges
				// =================================================================
				if (parsingOk)
				{
					jvxSize cntP2 = 0;

					while (1)
					{
						jvxBool bridge_ref_from_input = false;
						jvxBool bridge_ref_to_output = false;

						std::string tokenBridge;

						resL = theReader->getReferenceEntryCurrentSection_name(theProcess, &oneBridge, tokenBridge.c_str());

						tokenBridge = ("BRIDE_ID_" + jvx_size2String(cntP2));
						resL = theReader->getReferenceEntryCurrentSection_name(theProcess, &oneBridge, tokenBridge.c_str());
						if (resL == JVX_NO_ERROR)
						{
							CjvxDescriptBridge newBridge;
							jvxBool parseOKLocal = true;
							jvxConfigData* bridge_from = NULL;
							jvxConfigData* bridge_to = NULL;
							jvxInt32 linenobrid = -1;
							jvxApiString fn;
							jvxValue val;
							theReader->getOriginSection(theProcess, &fn, &linenobrid);

							token = "JVX_CONNECTOR_BRIDGE_NAME";
							resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, oneBridge, token, &newBridge.bridge_name);
							if (resL != JVX_NO_ERROR)
							{
								oneWarnMess = "Failed to read entry for token ";
								oneWarnMess += token;
								oneWarnMess += " in section ";
								oneWarnMess += tokenBridge;
								oneWarnMess += " starting in line ";
								oneWarnMess += linenobrid;
								oneWarnMess += " in file <";
								oneWarnMess += fn.std_str();
								oneWarnMess += ">.";
								warnMess.push_back(oneWarnMess);
								parseOKLocal = false;
							}

							// Accept missing statement for compatibility to old config files. Only groups with thread parameters
							if (!isProcess)
							{
								token = "JVX_CONNECTOR_BRIDGE_DEDICATED_THREAD";
								resL = HjvxConfigProcessor_readEntry_assignment(theReader, oneBridge, token, &newBridge.ded_thread);

								// Accept missing statement for compatibility to old config files
								token = "JVX_CONNECTOR_BRIDGE_BOOST_THREAD";
								resL = HjvxConfigProcessor_readEntry_assignment(theReader, oneBridge, token, &newBridge.boost_thread);
							}
							resL = theReader->getReferenceEntryCurrentSection_name(oneBridge, &bridge_from, "JVX_CONNECTOR_BRIDGE_REF_FROM");
							if (resL != JVX_NO_ERROR)
							{
								if (
									(resL == JVX_ERROR_ELEMENT_NOT_FOUND) && (!isProcess))
								{
									bridge_ref_from_input = true;
								}
								else
								{
									oneWarnMess = "Failed to read entry for token ";
									oneWarnMess += token;
									oneWarnMess += " in section ";
									oneWarnMess += tokenBridge;
									oneWarnMess += " starting in line ";
									oneWarnMess += linenobrid;
									oneWarnMess += " in file <";
									oneWarnMess += fn.std_str();
									oneWarnMess += ">.";
									warnMess.push_back(oneWarnMess);
									parseOKLocal = false;
								}
							}

							resL = theReader->getReferenceEntryCurrentSection_name(oneBridge, &bridge_to, "JVX_CONNECTOR_BRIDGE_REF_TO");
							if (resL != JVX_NO_ERROR)
							{
								if (
									(resL == JVX_ERROR_ELEMENT_NOT_FOUND) && (!isProcess))
								{
									bridge_ref_to_output = true;
								}
								else
								{
									oneWarnMess = "Failed to read entry for token ";
									oneWarnMess += token;
									oneWarnMess += " in section ";
									oneWarnMess += tokenBridge;
									oneWarnMess += " starting in line ";
									oneWarnMess += linenobrid;
									oneWarnMess += " in file <";
									oneWarnMess += fn.std_str();
									oneWarnMess += ">.";
									warnMess.push_back(oneWarnMess);
									parseOKLocal = false;
								}
							}

							if (parseOKLocal)
							{
								if (bridge_ref_from_input)
								{
									newBridge.from.uid = JVX_SIZE_UNSELECTED;
								}
								else
								{
									token = "ID_FROM";
									resL = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, bridge_from, token, &newBridge.from.uid);
									if (resL != JVX_NO_ERROR)
									{
										token = "NAME";
										resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, bridge_from, token, &newBridge.from.connector_factory_name);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to read entry for token ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenBridge;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_FROM)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}

										token = "CONNECTOR_NAME";
										resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, bridge_from, token, &newBridge.from.connector_name);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to read entry for token ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenBridge;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_FROM)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}

										token = "COMPONENT_TYPE";
										compTypeName = "";
										resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, bridge_from, token, &compTypeName);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to read entry for token ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenBridge;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_FROM)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}

										resL = jvxComponentType_decode(&newBridge.from.tp.tp, compTypeName);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to convert expression <";
											oneWarnMess += compTypeName;
											oneWarnMess += "> obtained for entry ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenSec;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_FROM)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">";
											oneWarnMess += " into a valid component type.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}

										token = "SLOT_ID";
										resL = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, bridge_from, token, &newBridge.from.tp.slotid);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to read entry for token ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenBridge;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_FROM)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}

										token = "SLOT_SUB_ID";
										resL = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, bridge_from, token, &newBridge.from.tp.slotsubid);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to read entry for token ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenBridge;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_FROM)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}
									}
								}
							} // if (parseOKLocal)

							  // ===================================================================

							if (parseOKLocal)
							{
								if (bridge_ref_to_output)
								{
									newBridge.to.uid = JVX_SIZE_UNSELECTED;
								}
								else
								{
									token = "ID_TO";
									resL = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, bridge_to, token, &newBridge.to.uid);
									if (resL != JVX_NO_ERROR)
									{
										token = "NAME";
										resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, bridge_to, token, &newBridge.to.connector_factory_name);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to read entry for token ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenBridge;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_TO)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}

										token = "CONNECTOR_NAME";
										resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, bridge_to, token, &newBridge.to.connector_name);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to read entry for token ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenBridge;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_TO)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}

										token = "COMPONENT_TYPE";
										compTypeName = "";
										resL = HjvxConfigProcessor_readEntry_assignmentString(theReader, bridge_to, token, &compTypeName);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to read entry for token ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenBridge;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_TO)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}

										resL = jvxComponentType_decode(&newBridge.to.tp.tp, compTypeName);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to convert expression <";
											oneWarnMess += compTypeName;
											oneWarnMess += "> obtained for entry ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenSec;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_TO)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">";
											oneWarnMess += " into a valid component type.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}

										token = "SLOT_ID";
										resL = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, bridge_to, token, &newBridge.to.tp.slotid);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to read entry for token ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenBridge;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_TO)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}

										token = "SLOT_SUB_ID";
										resL = HjvxConfigProcessor_readEntry_assignment<jvxSize>(theReader, bridge_to, token, &newBridge.to.tp.slotsubid);
										if (resL != JVX_NO_ERROR)
										{
											oneWarnMess = "Failed to read entry for token ";
											oneWarnMess += token;
											oneWarnMess += " in section ";
											oneWarnMess += tokenBridge;
											oneWarnMess += " (subsection JVX_CONNECTOR_BRIDGE_REF_TO)";
											oneWarnMess += " starting in line ";
											oneWarnMess += jvx_int2String(linenobrid);
											oneWarnMess += " in file <";
											oneWarnMess += fn.std_str();
											oneWarnMess += ">.";
											warnMess.push_back(oneWarnMess);
											parseOKLocal = false;
										}
									}
								}
							} // if (parseOKLocal)

							if (parseOKLocal)
							{
								theNewConnection.connectors.push_back(newBridge);
							}
							else
							{
								parsingOk = false;
								break;
							}
						}
						else
						{
							break;
						}
						cntP2++;
					}
				}

				if (parsingOk)
				{
					jvxSize uIdCreated = JVX_SIZE_UNSELECTED;
					std::cout << "Trying to activate process " << theNewConnection.process_name << std::endl;
					res = theNewConnection.create_process_group_from_description(isProcess, theDatConnections,
						JVX_SIZE_UNSELECTED, false, // <- This is the condition to store the connection in config file. If we load it from config we store it in config
						interceptors, essentialForStart,
						&uIdCreated, false, false, catId JVX_CONNECTION_FEEDBACK_CALL_A(fdb_loc));

					if (res == JVX_NO_ERROR)
					{
						put_configuration_existing_process_group(callConf, theReader, 
							theProcess, fName, lineno, isProcess, uIdCreated, theDatConnections);
					}
					else
					{
						oneWarnMess.assign(JVX_CONNECTION_FEEDBACK_GET_ERROR_STRING(fdb_loc));
						warnMess.push_back(oneWarnMess);
					}
				}
			}
		}
		else
		{
			break;
		}
		cntP++;
	}
	return JVX_NO_ERROR;
}

jvxErrorType jvx_put_configuration_dataproc_rules(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theReader, jvxConfigData* sectionToLookForIn,
	const char* fName, int lineno, IjvxDataConnections* theDatConnections, std::vector<std::string>& warnMess)
{
	/* Connection rules can be added by program code in the project hook function
	 * OR may be stored in the config file. Typically, all rules are stored but 
	 * the program code rules are added before those from config file. As a conclusion,
	 * the rules are programmed and only if deviating from the programmed name, a rule is loaded 
	 * from file. This allows to load config files without rules added yby program code to auto connect the same 
	 * processes and groups :-)
	 */
	jvxSize cnt = 0;
	std::string token;
	jvxConfigData* datEntry = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* datTmp = NULL, *sectionRules = NULL;
	jvxApiString str;
	jvxApiString str111, str112;
	jvxApiString str121, str122;
	jvxApiString str211, str212;
	jvxApiString str221, str222;

	jvxSize uId = JVX_SIZE_UNSELECTED;
	std::string prefix;
	jvxComponentIdentification tpId1;
	jvxComponentIdentification tpId2;
	jvxValue val;

	std::string rulename;
	std::string bridgename;
	std::string wildcard11;
	std::string wildcard12;

	std::string wildcard21;
	std::string wildcard22;

	jvxBool interceptors = false;
	jvxBool essential = false;

	IjvxDataConnectionRule* theNewRule = NULL;

	res = theReader->getReferenceEntryCurrentSection_name(sectionToLookForIn, &sectionRules, "process_connect_rule_section");
	if (res == JVX_NO_ERROR)
	{
		while (1)
		{
			jvxDataConnectionRuleParameters params;
			theNewRule = NULL;
			uId = JVX_SIZE_UNSELECTED;

			token = "JVX_CONNECTION_RULE_" + jvx_size2String(cnt);
			res = theReader->getReferenceEntryCurrentSection_name(sectionRules, &datEntry, token.c_str());
			if (res != JVX_NO_ERROR)
			{
				break;
			}

			datTmp = NULL;
			res = theReader->getReferenceEntryCurrentSection_name(datEntry, &datTmp, "PROCESS_INTERCEPTORS");
			if (res == JVX_NO_ERROR)
			{
				theReader->getAssignmentValue(datTmp, &val);
				val.toContent(&interceptors);
			}
			datTmp = NULL;

			datTmp = NULL;
			res = theReader->getReferenceEntryCurrentSection_name(datEntry, &datTmp, "PROCESS_ESSENTIAL");
			if (res == JVX_NO_ERROR)
			{
				theReader->getAssignmentValue(datTmp, &val);
				val.toContent(&essential);
			}
			datTmp = NULL;

			// ===================================================================
			// One connection rule
			// ===================================================================		

			prefix = "JVX_CONNECTION_RULE_MASTER";

			res = theReader->getReferenceEntryCurrentSection_name(datEntry, &datTmp, "JVX_CONNECTION_RULE_NAME");
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			res = theReader->getAssignmentString(datTmp, &str);
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			rulename = str.std_str();

			datTmp = NULL;
			str.clear();
			token = prefix + "COMPONENT_TYPE";
			res = theReader->getReferenceEntryCurrentSection_name(datEntry, &datTmp, token.c_str());
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			res = theReader->getAssignmentString(datTmp, &str);
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			res = jvxComponentType_decode(&tpId1.tp, str.std_str());

			datTmp = NULL;
			str.clear();
			token = prefix + "SLOT_ID";
			res = theReader->getReferenceEntryCurrentSection_name(datEntry, &datTmp, token.c_str());
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			res = theReader->getAssignmentValue(datTmp, &val);
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			val.toContent(&tpId1.slotid);

			datTmp = NULL;
			str.clear();
			token = prefix + "SLOT_SUB_ID";
			res = theReader->getReferenceEntryCurrentSection_name(datEntry, &datTmp, token.c_str());
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			res = theReader->getAssignmentValue(datTmp, &val);
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			val.toContent(&tpId1.slotsubid);

			datTmp = NULL;
			str.clear();
			token = prefix + "FACTORY_WILDCARD";
			res = theReader->getReferenceEntryCurrentSection_name(datEntry, &datTmp, token.c_str());
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			res = theReader->getAssignmentString(datTmp, &str);
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			wildcard11 = str.std_str();

			datTmp = NULL;
			str.clear();
			token = prefix + "MASTER_WILDCARD";
			res = theReader->getReferenceEntryCurrentSection_name(datEntry, &datTmp, token.c_str());
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			res = theReader->getAssignmentString(datTmp, &str);
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			wildcard12 = str.std_str();

			// Apply the new rule
			params.interceptors = interceptors;
			params.essential = essential;
			res = theDatConnections->create_connection_rule(rulename.c_str(), &uId, &params);
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}
			res = theDatConnections->reference_connection_rule_uid(uId, &theNewRule);
			if (res != JVX_NO_ERROR)
			{
				goto exit_err;
			}

			theNewRule->specify_master(tpId1, wildcard11.c_str(), wildcard12.c_str());

			// The bridge rules
			jvxSize cnt2 = 0;
			jvxConfigData* datSubEntry = NULL;
			while (1)
			{
				token = ("CONNECTION_RULE_BRIDE_ID_" + jvx_size2String(cnt2));
				res = theReader->getReferenceEntryCurrentSection_name(datEntry, &datSubEntry, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					break;
				}

				// Bridge name?
				str.clear();
				token = "JVX_CONNECTION_RULE_BRIDGE_NAME";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentString(datTmp, &str);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				bridgename = str.std_str();

				// One bridge
				prefix = "CONNECTOR_FROM";

				datTmp = NULL;
				str.clear();
				token = prefix + "COMPONENT_TYPE";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentString(datTmp, &str);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = jvxComponentType_decode(&tpId1.tp, str.std_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}

				datTmp = NULL;
				str.clear();
				token = prefix + "SLOT_ID";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentValue(datTmp, &val);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				val.toContent(&tpId1.slotid);

				datTmp = NULL;
				str.clear();
				token = prefix + "SLOT_SUB_ID";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentValue(datTmp, &val);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				val.toContent(&tpId1.slotsubid);

				datTmp = NULL;
				str.clear();
				token = prefix + "FACTORY_WILDCARD";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentString(datTmp, &str);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				wildcard11 = str.std_str();

				datTmp = NULL;
				str.clear();
				token = prefix + "CONNECTOR_WILDCARD";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentString(datTmp, &str);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				wildcard12 = str.std_str();


				// ==============================================================================================

				prefix = "CONNECTOR_TO";

				datTmp = NULL;
				str.clear();
				token = prefix + "COMPONENT_TYPE";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentString(datTmp, &str);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = jvxComponentType_decode(&tpId2.tp, str.std_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}

				datTmp = NULL;
				str.clear();
				token = prefix + "SLOT_ID";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentValue(datTmp, &val);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				val.toContent(&tpId2.slotid);

				datTmp = NULL;
				str.clear();
				token = prefix + "SLOT_SUB_ID";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentValue(datTmp, &val);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				val.toContent(&tpId2.slotsubid);

				datTmp = NULL;
				str.clear();
				token = prefix + "FACTORY_WILDCARD";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentString(datTmp, &str);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				wildcard21 = str.std_str();

				datTmp = NULL;
				str.clear();
				token = prefix + "CONNECTOR_WILDCARD";
				res = theReader->getReferenceEntryCurrentSection_name(datSubEntry, &datTmp, token.c_str());
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				res = theReader->getAssignmentString(datTmp, &str);
				if (res != JVX_NO_ERROR)
				{
					goto exit_err;
				}
				wildcard22 = str.std_str();

				theNewRule->add_bridge_specification(tpId1, wildcard11.c_str(), wildcard12.c_str(),
					tpId2, wildcard21.c_str(), wildcard22.c_str(), bridgename.c_str(), false, false);
				cnt2++;
			}

			// Finally, check that this rule is not yet registered
			jvxSize numAll = theDatConnections->number_connection_rules(&numAll);
			jvxSize k;
			jvxBool removeNewOne = false;
			for (k = 0; k < numAll; k++)
			{
				IjvxDataConnectionRule* ruleCompare = NULL;
				jvxSize uIdComp;
				jvxSize numB1 = 0;
				jvxSize numB2 = 0;
				jvxSize l;

				res = theDatConnections->reference_connection_rule(k, &ruleCompare);
				assert(res == JVX_NO_ERROR);
				theDatConnections->uid_for_reference(ruleCompare, &uIdComp);

				if (uIdComp != uId)
				{
					ruleCompare->get_master(&tpId1, &str111, &str112);
					theNewRule->get_master(&tpId2, &str121, &str122);
					if (
						!(
						(tpId1 != tpId2) ||
							(str111.std_str() != str121.std_str()) ||
							(str112.std_str() != str122.std_str())
							))
					{

						ruleCompare->number_bridges(&numB1);
						theNewRule->number_bridges(&numB2);
						if (numB1 == numB2)
						{
							jvxBool loopEndedDueToDifference = false;
							for (l = 0; l < numB1; l++)
							{
								ruleCompare->get_bridge(l, NULL,
									&tpId1, &str111, &str112, NULL, NULL, NULL);
								theNewRule->get_bridge(l, NULL,
									&tpId2, &str121, &str122, NULL, NULL, NULL);
								if ((tpId1 != tpId2) ||
									(str111.std_str() != str121.std_str()) ||
									(str112.std_str() != str122.std_str()))
								{
									loopEndedDueToDifference = true;
									break;
								}

								ruleCompare->get_bridge(l, NULL, 
									NULL, NULL, NULL, &tpId1, &str111, &str112);
								theNewRule->get_bridge(l, NULL, 
									NULL, NULL, NULL, &tpId2, &str121, &str122);
								if (
									(tpId1 != tpId2) ||
									(str111.std_str() != str121.std_str()) ||
									(str112.std_str() != str122.std_str()))
								{
									loopEndedDueToDifference = true;
									break;
								}
							}
							if (!loopEndedDueToDifference)
							{
								removeNewOne = true;
							}
						}
					}
				}

				theDatConnections->return_reference_connection_rule(ruleCompare);
				ruleCompare = NULL;
				if (removeNewOne)
				{
					break;
				}
			}

			theDatConnections->return_reference_connection_rule(theNewRule);
			if (removeNewOne)
			{
				// If we are here, we found a match
				theDatConnections->remove_connection_rule(uId);
			}
			cnt++;
		}
	}
	return JVX_NO_ERROR;

exit_err:
	if (JVX_CHECK_SIZE_SELECTED(uId))
	{
		if (theNewRule)
		{
			theDatConnections->return_reference_connection_rule(theNewRule);
		}
		theDatConnections->remove_connection_rule(uId);
	}
	return res;
}

// =========================================================================================================

jvxErrorType 
jvx_match_slots(IjvxHost* theHost, jvxComponentIdentification& tpThis, const jvxComponentIdentification& tpToThis)
{
	assert(theHost);

	// linearize slot
	jvxSize i;
	jvxSize linslotidTo = 0;	
	jvxComponentIdentification tmp;
	jvxSize szSlots = 0;
	jvxSize szSubSlots = 0;
	jvxBool slotsNotFound = false;

	jvxComponentType parTp = JVX_COMPONENT_UNKNOWN;

	if (
		(tpThis.slotid == JVX_SIZE_SLOT_RETAIN) ||
		(tpThis.slotsubid == JVX_SIZE_SLOT_RETAIN))
	{
		tmp.tp = tpToThis.tp;
		theHost->role_component_system(tmp.tp, & parTp, nullptr, nullptr);
		theHost->number_slots_component_system(tmp, &szSlots, NULL,  nullptr, nullptr);
		if (parTp == JVX_COMPONENT_UNKNOWN)
		{
			if (JVX_CHECK_SIZE_UNSELECTED(tpToThis.slotid))
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
			linslotidTo = tpToThis.slotid;
		}
		else
		{
			if (
				JVX_CHECK_SIZE_UNSELECTED(tpToThis.slotid) ||
				JVX_CHECK_SIZE_UNSELECTED(tpToThis.slotsubid))
			{
				return JVX_ERROR_INVALID_ARGUMENT;
			}
			if (tpToThis.slotid < szSlots)
			{
				for (i = 0; i < tpToThis.slotid; i++)
				{
					tmp.slotid = i;
					theHost->role_component_system(tmp.tp, &parTp, nullptr, nullptr);
					theHost->number_slots_component_system(tmp, &szSlots, &szSubSlots, nullptr, nullptr);
					linslotidTo += szSubSlots;
				}
				linslotidTo += tpToThis.slotsubid;
			}
		}

		tmp.tp = tpThis.tp;
		slotsNotFound = true;
		theHost->role_component_system(tmp.tp, &parTp, nullptr, nullptr);
		theHost->number_slots_component_system(tmp, &szSlots, NULL, nullptr, nullptr);
		if (parTp == JVX_COMPONENT_UNKNOWN)
		{
			slotsNotFound = false;
			if (tpThis.slotid == JVX_SIZE_SLOT_RETAIN)
			{
				tpThis.slotid = linslotidTo;
			}
			if (tpThis.slotsubid == JVX_SIZE_SLOT_RETAIN)
			{
				tpThis.slotsubid = 0;
			}
		}
		else
		{
			for (i = 0; i < szSlots; i++)
			{
				tmp.slotid = i;
				theHost->number_slots_component_system(tmp, NULL, &szSubSlots, nullptr, nullptr);
				if (linslotidTo < szSubSlots)
				{
					slotsNotFound = false;
					if (tpThis.slotid == JVX_SIZE_SLOT_RETAIN)
					{
						tpThis.slotid = i;
					}
					if (tpThis.slotsubid == JVX_SIZE_SLOT_RETAIN)
					{
						tpThis.slotsubid = linslotidTo;
					}
					break;
				}
				else
				{
					linslotidTo -= szSubSlots;
				}
			}
		}
	}
	if (slotsNotFound)
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	return JVX_NO_ERROR;
}



jvxErrorType 
jvx_rule_from_dropzone(IjvxDataConnections* theDatConnections, jvxConnectionRuleDropzone* dropzone)
{
	jvxSize cnt = 0;
	std::string token;
	jvxConfigData* datEntry = NULL;
	jvxErrorType res = JVX_NO_ERROR;
	jvxConfigData* datTmp = NULL, *sectionRules = NULL;
	jvxApiString str;
	jvxApiString str111, str112;
	jvxApiString str121, str122;
	jvxApiString str211, str212;
	jvxApiString str221, str222;

	jvxSize uId = JVX_SIZE_UNSELECTED;
	std::string prefix;
	jvxComponentIdentification tpId1;
	jvxComponentIdentification tpId2;
	jvxValue val;

	std::string rulename;
	std::string bridgename;
	std::string wildcard11;
	std::string wildcard12;

	std::string wildcard21;
	std::string wildcard22;
	IjvxDataConnectionRule* theNewRule = NULL;
	jvxDataConnectionRuleParameters params;

	std::list<jvxConnectionRuleBridge>::iterator elm;
	jvxSize k;
	jvxBool removeNewOne = false;

	rulename = dropzone->rule_name;
	tpId1 = dropzone->cpMaster;
	wildcard11 = dropzone->masterFactoryWildcard;
	wildcard12 = dropzone->masterWildcard;

	// Apply the new rule
	params.interceptors = dropzone->interceptors;
	params.essential = dropzone->essential;
	res = theDatConnections->create_connection_rule(rulename.c_str(), &uId, &params);
	if (res != JVX_NO_ERROR)
	{
		goto exit_err;
	}
	res = theDatConnections->reference_connection_rule_uid(uId, &theNewRule);
	if (res != JVX_NO_ERROR)
	{
		goto exit_err;
	}

	theNewRule->specify_master(tpId1, wildcard11.c_str(), wildcard12.c_str());

	elm= dropzone->bridges.begin();
	for(; elm != dropzone->bridges.end(); elm++)
	{
		bridgename = elm->bridge_name;
		tpId1 = elm->fromCpTp;
		tpId2 = elm->toCpTp;
		wildcard11 = elm->fromFactoryWildcard;
		wildcard12 = elm->fromConnectorWildcard;
		wildcard21 = elm->toFactoryWildcard;
		wildcard22 = elm->toConnectorWildcard;

		theNewRule->add_bridge_specification(tpId1, wildcard11.c_str(), wildcard12.c_str(),
			tpId2, wildcard21.c_str(), wildcard22.c_str(), bridgename.c_str(), false, false);
	}

	// Finally, check that this rule is not yet registered
	jvxSize numAll;
	res = theDatConnections->number_connection_rules(&numAll);
	assert(res == JVX_NO_ERROR);
	for (k = 0; k < numAll; k++)
	{
		IjvxDataConnectionRule* ruleCompare = NULL;
		jvxSize uIdComp;
		jvxSize numB1 = 0;
		jvxSize numB2 = 0;
		jvxSize l;

		res = theDatConnections->reference_connection_rule(k, &ruleCompare);
		assert(res == JVX_NO_ERROR);
		theDatConnections->uid_for_reference(ruleCompare, &uIdComp);

		if (uIdComp != uId)
		{
			ruleCompare->get_master(&tpId1, &str111, &str112);
			theNewRule->get_master(&tpId2, &str121, &str122);
			if (
				!(
				(tpId1 != tpId2) ||
					(str111.std_str() != str121.std_str()) ||
					(str112.std_str() != str122.std_str())
					))
			{

				ruleCompare->number_bridges(&numB1);
				theNewRule->number_bridges(&numB2);
				if (numB1 == numB2)
				{
					jvxBool loopEndedDueToDifference = false;
					for (l = 0; l < numB1; l++)
					{
						ruleCompare->get_bridge(l, NULL,
							&tpId1, &str111, &str112, NULL, NULL, NULL);
						theNewRule->get_bridge(l, NULL,
							&tpId2, &str121, &str122, NULL, NULL, NULL);
						if ((tpId1 != tpId2) ||
							(str111.std_str() != str121.std_str()) ||
							(str112.std_str() != str122.std_str()))
						{
							loopEndedDueToDifference = true;
							break;
						}

						ruleCompare->get_bridge(l, NULL,
							NULL, NULL, NULL, &tpId1, &str111, &str112);
						theNewRule->get_bridge(l, NULL,
							NULL, NULL, NULL, &tpId2, &str121, &str122);
						if (
							(tpId1 != tpId2) ||
							(str111.std_str() != str121.std_str()) ||
							(str112.std_str() != str122.std_str()))
						{
							loopEndedDueToDifference = true;
							break;
						}
					}
					if (!loopEndedDueToDifference)
					{
						removeNewOne = true;
					}
				}
			}
		}

		theDatConnections->return_reference_connection_rule(ruleCompare);
		ruleCompare = NULL;
		if (removeNewOne)
		{
			break;
		}
	}

	theDatConnections->return_reference_connection_rule(theNewRule);
	if (removeNewOne)
	{
		// If we are here, we found a match
		theDatConnections->remove_connection_rule(uId);
	}
	return JVX_NO_ERROR;

exit_err:
	if (JVX_CHECK_SIZE_SELECTED(uId))
	{
		if (theNewRule)
		{
			theDatConnections->return_reference_connection_rule(theNewRule);
		}
		theDatConnections->remove_connection_rule(uId);
	}
	return res;
}

jvxErrorType jvx_find_best_match_bsize_srate(
	jvxSize targetBsize, jvxSize targetSrate,
	const std::vector<std::string>* entriesSizes,
	jvxSize& idxSize,
	const std::vector<std::string>* entriesRates,
	jvxSize& idxRate,
	jvxInt32* bsize, jvxInt32* srate)
{
	jvxInt32 oneSize = 0;
	jvxInt32 maxSize = 0;
	jvxInt32 minSize = INT_MAX;
	jvxSize i, j;
	jvxData deviationFsMax = 0.1;
	jvxSize numLoopsMax = 5;
	jvxData disadvantageLoops = 0.9;

	if (entriesSizes)
	{
		for (i = 0; i < (*entriesSizes).size(); i++)
		{
			jvxBool err = false;
			oneSize = JVX_SIZE_INT32(jvx_string2Size((*entriesSizes)[i], err));
			assert(err == false);

			if (oneSize > maxSize)
			{
				maxSize = oneSize;
			}
			if (oneSize < minSize)
			{
				minSize = oneSize;
			}
		}
	}
	int cnt = 1;
	bool doCont = true;
	int targetBSize = 0;

	jvxData relativeDeviation = -1;
	jvxData oneSamplerate = 0.0;

	jvxData deviation_min = JVX_DATA_MAX_POS;// 10 percent or less deviation
	int numFrames_min = -1;

	if (bsize)
	{
		if (!srate)
		{
			for (j = 0; j < (*entriesRates).size(); j++)
			{
				jvxBool err = false;
				jvxData rr = (jvxData)(jvx_string2Size((*entriesRates)[j], err) - targetSrate);
				if (rr < deviation_min)
				{
					deviation_min = rr;
					idxRate = j;
				}
			}
			jvxBool err = false;
			*bsize =  JVX_SIZE_INT32(JVX_DATA2SIZE((jvxData)jvx_string2Size(
				(*entriesRates)[idxRate], err) / (jvxData)targetSrate * (jvxData)targetBsize));
		}
		else
		{
		  *srate = JVX_SIZE_INT32(targetSrate);
		  *bsize = JVX_SIZE_INT32(targetBsize);
		}
	}
	else
	{
		if (srate)
		{
			for (i = 0; i < (*entriesSizes).size(); i++)
			{
				jvxBool err = false;
				jvxData ss = (jvxData)(jvx_string2Size((*entriesSizes)[i], err) - targetBsize);
				if (ss < deviation_min)
				{
					deviation_min = ss;
					idxSize = i;
				}
			}
			jvxBool err = false;
			*srate = JVX_SIZE_INT32(JVX_DATA2SIZE((jvxData)jvx_string2Size((*entriesSizes)[idxSize], err) / (jvxData)targetBsize* (jvxData)targetSrate));
		}
		else
		{
			while (doCont)
			{
			  int locTargetBsize = JVX_SIZE_INT32(cnt * targetBsize);
				if (locTargetBsize > maxSize)
				{
					doCont = false;
				}

				if (cnt == numLoopsMax)
				{
					doCont = false;
				}

				for (j = 0; j < (*entriesRates).size(); j++)
				{
					jvxBool err = false;
					jvxSize oneSamplerate = jvx_string2Size((*entriesRates)[j], err);
					assert(err == false);

					if (oneSamplerate >= targetSrate)
					{
						jvxData achievableBsize = (jvxData)oneSamplerate / (jvxData)targetSrate * locTargetBsize;

						// Do not degrade quality
						for (i = 0; i < (*entriesSizes).size(); i++)
						{
						  oneSize = JVX_SIZE_INT32(jvx_string2Size((*entriesSizes)[i], err));

							jvxData duration_setup = (jvxData)oneSize / (jvxData)oneSamplerate;
							jvxData duration_desired = (jvxData)locTargetBsize / (jvxData)targetSrate;
							//jvxData effectiveRate = (jvxData)locTargetBsize / (jvxData)(oneSize) * (jvxData)targetSrate;
							relativeDeviation = fabs(duration_setup - duration_desired) / duration_desired;

							if (deviation_min > relativeDeviation)
							{
								deviation_min = relativeDeviation;
								numFrames_min = cnt;
								idxSize = i;
								idxRate = j;
							}
						}
					}
				}
				cnt++;
				disadvantageLoops *= disadvantageLoops;
			}

			// If we found one solution
			if (numFrames_min > 0)
			{
				return(JVX_NO_ERROR);
			}

			return JVX_ERROR_ELEMENT_NOT_FOUND;
		}
	}
	return JVX_NO_ERROR;
}
