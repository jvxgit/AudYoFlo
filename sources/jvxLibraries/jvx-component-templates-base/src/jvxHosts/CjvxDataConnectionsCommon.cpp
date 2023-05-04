#include "jvxHosts/CjvxDataConnectionsCommon.h"

CjvxDataConnectionCommon::CjvxDataConnectionCommon(
	CjvxDataConnections* parentArg, 
	const std::string& nm, 
	jvxSize unique_id_system, 
	jvxBool interceptors, 
	const std::string& descr,
	jvxBool verbose_mode,
	jvxBool report_automation):
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL_EMPTY)
{
	_common_set_conn_comm.unique_id_bridge = 1;
	_common_set_conn_comm.name = nm;
	//_common_set_min.theState = JVX_STATE_INIT;
	_common_set_conn_comm.unique_id_system = unique_id_system;
	_common_set_conn_comm.interceptors = interceptors;
	_common_set_conn_comm.verbose_out = verbose_mode;
	_common_set_conn_comm.report_automation = report_automation;

	//_common_set_conn_comm.lastTestErrMess = "Test not yet done";
	
	_common_set_min.theState = JVX_STATE_NONE;

	theContext = NULL;
	theTag = nm;
	theParent = parentArg;
	connRuleId = JVX_SIZE_UNSELECTED;
	theDataConnectionGroup = NULL;

	if (descr.empty())
	{
		_common_set.theDescriptor = jvx_replaceStrInStr(nm, " ", "") + jvx_size2String(unique_id_system);
	}
	else
	{
		_common_set.theDescriptor = descr;
	}
}

CjvxDataConnectionCommon::~CjvxDataConnectionCommon()
{
}

jvxErrorType 
CjvxDataConnectionCommon::_interceptors_active(jvxBool* is_act)
{
	if (is_act)
	{
		*is_act = _common_set_conn_comm.interceptors;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_unique_id_connections(jvxSize* uid)
{
	if (uid)
	{
		*uid = _common_set_conn_comm.unique_id_system;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_descriptor_connection(jvxApiString* str)
{
	if (str)
	{
		str->assign(_common_set_conn_comm.name);
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_misc_connection_parameters(jvxSize* connRuleIdPtr, jvxBool* preventStoredInConfigPtr)
{
	if (connRuleIdPtr)
	{
		*connRuleIdPtr = this->connRuleId;
	}
	if (preventStoredInConfigPtr)
	{
		*preventStoredInConfigPtr = this->preventStoredInConfig;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_set_misc_connection_parameters(jvxSize connRuleIdArg, jvxBool preventStoredInConfigArg)
{
	this->preventStoredInConfig = preventStoredInConfigArg;
	connRuleId = connRuleIdArg;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_number_bridges(jvxSize* num)
{
	if (num)
	{
		*num = _common_set_conn_comm.bridges.size();
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_connector_factory_is_involved(IjvxConnectorFactory* rem_this)
{
	IjvxOutputConnector* oCon = NULL;
	IjvxConnectorFactory* par = NULL;
	IjvxInputConnector* iCon = NULL;
	jvxErrorType resL = JVX_NO_ERROR;
	jvxErrorType res = JVX_ERROR_ELEMENT_NOT_FOUND;

	std::map<jvxSize, oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > >::iterator elm = _common_set_conn_comm.bridges.begin();
	for (; elm != _common_set_conn_comm.bridges.end(); elm++)
	{
		oCon = NULL;
		par = NULL;

		elm->second.ptr->reference_connect_from(&oCon);
		if (oCon)
		{
			oCon->parent_factory(&par);
			if (par == rem_this)
			{
				res = JVX_NO_ERROR;
			}
			elm->second.ptr->return_reference_connect_from(oCon);
		}
		if (res == JVX_NO_ERROR)
		{
			break;
		}

		iCon = NULL;
		par = NULL;

		elm->second.ptr->reference_connect_to(&iCon);
		if (iCon)
		{
			resL = iCon->parent_factory(&par);
			assert(resL == JVX_NO_ERROR);
			if (par)
			{
				if (par == rem_this)
				{
					res = JVX_NO_ERROR;
				}
			}
			elm->second.ptr->return_reference_connect_to(iCon);
		}
		if (res == JVX_NO_ERROR)
		{
			break;
		}
	}
	return res;
};

jvxErrorType
CjvxDataConnectionCommon::_reference_bridge(jvxSize idx, IjvxConnectorBridge** theBridge)
{
	std::map<jvxSize, oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > >::iterator elm = _common_set_conn_comm.bridges.begin();
	std::advance(elm, idx);
	if (elm != _common_set_conn_comm.bridges.end())
	{
		if (theBridge)
		{
			*theBridge = static_cast<IjvxConnectorBridge*>(elm->second.ptr);
			elm->second.refCnt++;
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnectionCommon::_return_reference_bridge(IjvxConnectorBridge* theBridge)
{
	std::map<jvxSize, oneEntrySomething<CjvxConnectorBridge<CjvxDataConnectionCommon> > >::iterator elm = _common_set_conn_comm.bridges.begin();
	for (; elm != _common_set_conn_comm.bridges.end(); elm++)
	{
		if (static_cast<IjvxConnectorBridge*>(elm->second.ptr) == theBridge)
		{
			break;
		}
	}
	if (elm != _common_set_conn_comm.bridges.end())
	{
		assert(elm->second.refCnt > 0);
		elm->second.refCnt--;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType
CjvxDataConnectionCommon::_remove_connection()
{
	jvxErrorType res = JVX_NO_ERROR;
	JVX_CONNECTION_FEEDBACK_TYPE_DEFINE(fdb);

	// Remove all dependent connections first
	while(dependencies.size())
	{
		jvxErrorType resL = JVX_NO_ERROR;
		auto elm = dependencies.begin();
		assert(theContext);
		jvxSize uid = JVX_SIZE_UNSELECTED;
		jvxBool isProcess = false;
		theContext->uid_for_reference(*elm, &uid, &isProcess);
		if (JVX_CHECK_SIZE_SELECTED(uid))
		{
			if (isProcess)
			{
				resL = theContext->remove_connection_process(uid);
			}
			else
			{
				resL = theContext->remove_connection_group(uid);
			}
			if (resL != JVX_NO_ERROR)
			{
				return resL;
			}
		}
	}
	if (_common_set_min.theState >= JVX_STATE_ACTIVE)
	{				
		if (_common_set_conn_comm.verbose_out)
		{
			std::cout << "-->" << __FUNCTION__ << ": Removing connection <" << _common_set_conn_comm.name << ">." << std::endl;
		}

		res = theDataConnectionGroup->disconnect_chain(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		if (res != JVX_NO_ERROR)
		{
			return res;
		}

		theDataConnectionGroup->unlink_triggers_connection();
	}
	else
	{
		res = JVX_ERROR_WRONG_STATE;
	}

	theContext = NULL;
	res = remove_all_bridges_local();
	assert(res == JVX_NO_ERROR);
	
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_get_configuration(jvxCallManagerConfiguration* callConf,
	IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section)
{
	
	jvxConfigData* datBridge = NULL;
	jvxSize cnt;

	if (_common_set_min.theState >= JVX_STATE_ACTIVE)
	{
		get_configuration_local(callConf, theWriter, add_to_this_section);

		cnt = 0;
		if (_common_set_conn_comm.interceptors)
		{
			auto elm = _common_set_conn_comm.bridges_orig.begin();
			for (; elm != _common_set_conn_comm.bridges_orig.end(); elm++)
			{
				theWriter->createEmptySection(&datBridge, ("BRIDE_ID_" + jvx_size2String(cnt)).c_str());

				// Return config from single bridge
				elm->second.ptr->get_configuration(callConf, theWriter, datBridge);

				theWriter->addSubsectionToSection(add_to_this_section, datBridge);
				cnt++;
			}
		}
		else
		{
			auto elm = _common_set_conn_comm.bridges.begin();
			for (; elm != _common_set_conn_comm.bridges.end(); elm++)
			{
				theWriter->createEmptySection(&datBridge, ("BRIDE_ID_" + jvx_size2String(cnt)).c_str());

				// Return config from single bridge
				elm->second.ptr->get_configuration(callConf, theWriter, datBridge);

				theWriter->addSubsectionToSection(add_to_this_section, datBridge);
				cnt++;
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_set_connection_context(IjvxDataConnections* context)
{
	if (context)
	{
		if (theContext == NULL)
		{
			theContext = context;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_ALREADY_IN_USE;
	}
	else
	{
		if (theContext)
		{
			theContext = context;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_NOT_READY;
	}
}

jvxErrorType
CjvxDataConnectionCommon::_connection_context(IjvxDataConnections** context)
{
	if (context)
		*context = theContext;
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_set_connection_association(const char* tag, jvxComponentIdentification cpTp)
{
	if (tag)
	{
		theTag = tag;
		theCpTp = cpTp;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_connection_association(jvxApiString* tagOnRet, jvxComponentIdentification* cpTp)
{
	if (tagOnRet)
	{
		tagOnRet->assign(theTag);
	}
	if (cpTp)
	{
		*cpTp = theCpTp;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_status(jvxState* stat)
{
	if (stat)
	{
		*stat = _common_set_min.theState;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_remove_bridge_local(jvxSize unique_id)
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxInputConnector* icon = NULL;
	IjvxOutputConnector* ocon = NULL;
	if (_common_set_conn_comm.interceptors)
	{
		jvxBool noErr = true;

		// Remove both bridges and finally also the orig bridge for config file write
		auto elm = _common_set_conn_comm.bridges.find(unique_id);
		if (elm == _common_set_conn_comm.bridges.end())
		{
			noErr = false;
		}
		else
		{
			assert(elm->second.refCnt == 0);

			elm->second.ptr->reference_connect_from(&ocon);
			elm->second.ptr->reference_connect_to(&icon);
			remove_interceptor_if(icon, ocon);

			res = elm->second.ptr->terminate();
			assert(res == JVX_NO_ERROR);

			JVX_DSP_SAFE_DELETE_OBJECT(elm->second.ptr);
			_common_set_conn_comm.bridges.erase(elm);
		}

		elm = _common_set_conn_comm.bridges.find(unique_id + 1);
		if (elm == _common_set_conn_comm.bridges.end())
		{
			noErr = false;
		}
		else
		{
			assert(elm->second.refCnt == 0);

			elm->second.ptr->reference_connect_from(&ocon);
			elm->second.ptr->reference_connect_to(&icon);
			remove_interceptor_if(icon, ocon);

			res = elm->second.ptr->terminate();
			assert(res == JVX_NO_ERROR);

			JVX_DSP_SAFE_DELETE_OBJECT(elm->second.ptr);
			_common_set_conn_comm.bridges.erase(elm);
		}

		// ==================================================================

		elm = _common_set_conn_comm.bridges_orig.find(unique_id);
		if (elm == _common_set_conn_comm.bridges_orig.end())
		{
			noErr = false;
		}
		else
		{
			assert(elm->second.refCnt == 0);

			// The objects here are only stored for config file storage, none has been initialized
			//res = elm->second.ptr->terminate();
			//assert(res == JVX_NO_ERROR);

			JVX_DSP_SAFE_DELETE_OBJECT(elm->second.ptr);
			_common_set_conn_comm.bridges_orig.erase(elm);
		}
	}
	else
	{
		auto elm = _common_set_conn_comm.bridges.find(unique_id);
		if (elm != _common_set_conn_comm.bridges.end())
		{
			assert(elm->second.refCnt == 0);
			res = elm->second.ptr->terminate();
			assert(res == JVX_NO_ERROR);

			JVX_DSP_SAFE_DELETE_OBJECT(elm->second.ptr);
			_common_set_conn_comm.bridges.erase(elm);
			return JVX_NO_ERROR;
		}
	}
	return JVX_ERROR_ELEMENT_NOT_FOUND;
}

/*
jvxErrorType
CjvxDataConnectionCommon::_set_reference_report(IjvxDataConnection_report* hdlArg)
{
	if (_common_set_conn_comm.rep_handle == nullptr)
	{
		_common_set_conn_comm.rep_handle = hdlArg;
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ALREADY_IN_USE;
}
*/

jvxErrorType
CjvxDataConnectionCommon::_remove_all_bridges_local()
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxInputConnector* icon = NULL;
	IjvxOutputConnector* ocon = NULL;
	auto elm = _common_set_conn_comm.bridges.begin();
	while (_common_set_conn_comm.bridges.size())
	{
		elm = _common_set_conn_comm.bridges.begin();
		assert(elm->second.refCnt == 0);

		elm->second.ptr->reference_connect_from(&ocon);
		elm->second.ptr->reference_connect_to(&icon);
		elm->second.ptr->return_reference_connect_from(ocon);
		elm->second.ptr->return_reference_connect_to(icon);

		res = elm->second.ptr->terminate();
		assert(res == JVX_NO_ERROR);

		JVX_DSP_SAFE_DELETE_OBJECT(elm->second.ptr);
		_common_set_conn_comm.bridges.erase(elm);

		// Remove it here, not before!
		remove_interceptor_if(icon, ocon);

	}

	elm = _common_set_conn_comm.bridges_orig.begin();
	while (_common_set_conn_comm.bridges_orig.size())
	{
		elm = _common_set_conn_comm.bridges_orig.begin();
		assert(elm->second.refCnt == 0);

		JVX_DSP_SAFE_DELETE_OBJECT(elm->second.ptr);
		_common_set_conn_comm.bridges_orig.erase(elm);
	}

	assert(theInterceptors.size() == 0);

	return JVX_NO_ERROR;
};

void
CjvxDataConnectionCommon::remove_interceptor_if(IjvxInputConnector* icon, IjvxOutputConnector* ocon)
{
	IjvxDataConnectionCommon* refI = NULL;
	IjvxDataConnectionCommon* refO = NULL;
	auto elmI = theInterceptors.begin();
	for (; elmI != theInterceptors.end(); elmI++)
	{
		if (icon == static_cast<IjvxInputConnector*>(elmI->ptr))
		{
			elmI->fromConnected = false;
		}
		if (ocon == static_cast<IjvxOutputConnector*>(elmI->ptr))
		{
			elmI->toConnected = false;
		}
		
		if (
			(!elmI->toConnected) &&
			(!elmI->fromConnected))
		{
			about_release_intereptor(elmI->ptr);
			// Release interceptor
			JVX_DSP_SAFE_DELETE_OBJECT(elmI->ptr);
			theInterceptors.erase(elmI);
			break;
		}
	}
}

void 
CjvxDataConnectionCommon::about_release_intereptor(CjvxDataChainInterceptor* ptr)
{

}

jvxErrorType
CjvxDataConnectionCommon::_link_triggers_connection()
{
	for (auto& elm : _common_set_conn_comm.bridges)
	{
		IjvxInputConnector* icon = nullptr;
		IjvxTriggerOutputConnector* otcon = nullptr;

		IjvxOutputConnector* ocon = nullptr;
		IjvxTriggerInputConnector* itcon = nullptr;

		jvxSize itrigId = JVX_SIZE_UNSELECTED;
		jvxSize otrigId = JVX_SIZE_UNSELECTED;

		jvxBool pushNewElement = true;
		oneChainTriggerIdElement newElement;
		oneChainTriggerIdElement* workHere = &newElement;

		elm.second.ptr->icon_trig_id(&icon, &itrigId);
		elm.second.ptr->ocon_trig_id(&ocon, &otrigId);

		if (JVX_CHECK_SIZE_SELECTED(itrigId))
		{
			if (icon)
			{
				icon->request_trigger_otcon(&otcon);
			}
		}

		if (JVX_CHECK_SIZE_SELECTED(otrigId))
		{
			if (ocon)
			{
				ocon->request_trigger_itcon(&itcon);
			}
		}

		if(itcon)
		{
			auto elmMap = mapIdTriggerConnect.find(itrigId);
			if (elmMap != mapIdTriggerConnect.end())
			{
				workHere = &elmMap->second;
				pushNewElement = false;
			}
			workHere->itcons.push_back(itcon);
			if (pushNewElement)
			{
				mapIdTriggerConnect[itrigId] = *workHere;
			}
		}
		if (otcon)
		{
			auto elmMap = mapIdTriggerConnect.find(otrigId);
			if (elmMap != mapIdTriggerConnect.end())
			{
				workHere = &elmMap->second;
				pushNewElement = false;
			}
			workHere->otcons.push_back(otcon);
			if (pushNewElement)
			{
				mapIdTriggerConnect[otrigId] = *workHere;
			}
		}
	}

	// All bridges prepared here, next run the linking from the maps
	for (auto elmMap : mapIdTriggerConnect)
	{
		for (auto elmpMapItcon : elmMap.second.itcons)
		{
			for (auto elmpMapOtcon : elmMap.second.otcons)
			{
				elmpMapItcon->link_connect_tcon(elmpMapOtcon);
			}
		}

		for (auto elmpMapOtcon : elmMap.second.otcons)
		{
			for (auto elmpMapItcon : elmMap.second.itcons)
			{
				elmpMapOtcon->link_connect_tcon(elmpMapItcon);
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataConnectionCommon::_unlink_triggers_connection()
{
	// All bridges prepared here, next run the linking from the maps
	for (auto elmMap : mapIdTriggerConnect)
	{
		for (auto elmpMapItcon : elmMap.second.itcons)
		{
			for (auto elmpMapOtcon : elmMap.second.otcons)
			{
				elmpMapItcon->unlink_connect_tcon(elmpMapOtcon);
			}
		}

		for (auto elmpMapOtcon : elmMap.second.otcons)
		{
			for (auto elmpMapItcon : elmMap.second.itcons)
			{
				elmpMapOtcon->unlink_connect_tcon(elmpMapItcon);
			}
		}
	}
	mapIdTriggerConnect.clear();
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataConnectionCommon::_add_dependency(IjvxDataConnectionCommon* dependent)
{
	auto elm = dependencies.begin();
	for (; elm != dependencies.end(); elm++)
	{
		if(*elm == dependent)
		{
			break;
		}
	}

	if (elm != dependencies.end())
	{
		return JVX_ERROR_DUPLICATE_ENTRY;
	}
	dependencies.push_back(dependent);
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataConnectionCommon::_remove_dependency(IjvxDataConnectionCommon* dependent)
{
	auto elm = dependencies.begin();
	for (; elm != dependencies.end(); elm++)
	{
		if (*elm == dependent)
		{
			break;
		}
	}

	if (elm == dependencies.end())
	{
		return JVX_ERROR_ELEMENT_NOT_FOUND;
	}
	dependencies.erase(elm);
	return JVX_NO_ERROR;
}
