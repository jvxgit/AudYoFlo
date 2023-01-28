#ifndef __CJVXCONNECTORBRIDGE_H__
#define __CJVXCONNECTORBRIDGE_H__

template <typename T>
class CjvxConnectorBridge : public IjvxConnectorBridge
{
protected:
	class oneBridgeElement
	{
	public:
		IjvxOutputConnector* from;
		IjvxInputConnector* to;
		IjvxOutputConnector* from_origin;
		IjvxInputConnector* to_origin;
		oneBridgeElement()
		{
			from = NULL;
			to = NULL;
			from_origin = NULL;
			to_origin = NULL;
		};
	} ;
	
	class oneBridgeDefinition
	{
	public:
		oneBridgeElement theLink;
		T* parent;
		std::string name;
		jvxSize refCnt_from;
		jvxSize refCnt_to;
		jvxBool ded_thread;
		jvxBool boost_thread;
		jvxSize uId_from;
		jvxSize uId_to;
		jvxBool interceptor_bridge;

		oneBridgeDefinition()
		{
			parent = NULL;
			refCnt_from = JVX_SIZE_UNSELECTED;
			refCnt_to = JVX_SIZE_UNSELECTED;
			ded_thread = false;
			boost_thread = false;
			uId_from = JVX_SIZE_UNSELECTED;
			uId_to = JVX_SIZE_UNSELECTED;
			interceptor_bridge = false;
		};
	};
	
	oneBridgeDefinition _common_set_conn_bridge;

public:
	CjvxConnectorBridge(IjvxOutputConnector* from, 
		IjvxInputConnector* to, T* parent, 
		const std::string& nm, 
		jvxBool dedThread, jvxBool 
		boostThread, 
		jvxBool interceptorBridge)
	{
		_common_set_conn_bridge.theLink.from = from;
		_common_set_conn_bridge.theLink.to = to;
		_common_set_conn_bridge.parent = parent;
		_common_set_conn_bridge.name = nm;
		_common_set_conn_bridge.refCnt_from = 0;
		_common_set_conn_bridge.refCnt_to = 0;

		_common_set_conn_bridge.ded_thread = dedThread;
		_common_set_conn_bridge.boost_thread = boostThread;
		_common_set_conn_bridge.interceptor_bridge = interceptorBridge;

		_common_set_conn_bridge.uId_from = JVX_SIZE_UNSELECTED;
		_common_set_conn_bridge.uId_to = JVX_SIZE_UNSELECTED;
	};
	~CjvxConnectorBridge() 
	{
	};

	void append_name_postfix(const char* nm)
	{
		_common_set_conn_bridge.name += nm;
	}

	void set_uid_from_to(jvxSize uid_from, jvxSize uid_to)
	{
		_common_set_conn_bridge.uId_from = uid_from;
		_common_set_conn_bridge.uId_to = uid_to;
	};

	jvxBool check_exist(IjvxOutputConnector* from, IjvxInputConnector* to)
	{
		if ((from != NULL) && (from != NULL))
		{
			return(
				(from == _common_set_conn_bridge.theLink.from) &&
				(to == _common_set_conn_bridge.theLink.to));
		}
		if (from != NULL)
		{
			return(from == _common_set_conn_bridge.theLink.from);
		}
		if (to != NULL)
		{
			return(to == _common_set_conn_bridge.theLink.to);
		}
		return false;
	}
	
	jvxErrorType init(IjvxConnectionMaster* master, IjvxDataConnectionCommon* ass_connection_common)
	{
		jvxErrorType res = JVX_NO_ERROR;
		
#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "Bridge <" << _common_set_conn_bridge.name << ">" << std::endl;
		std::cout << "++ connect from " << std::flush;
		std::cout << JVX_DISPLAY_CONNECTOR(static_cast<IjvxCommonConnector*>(_common_set_conn_bridge.theLink.from)) << std::endl;
#endif

		_common_set_conn_bridge.theLink.from_origin = _common_set_conn_bridge.theLink.from;
		_common_set_conn_bridge.theLink.to_origin = _common_set_conn_bridge.theLink.to;

		res = _common_set_conn_bridge.theLink.from_origin->select_connect_ocon(static_cast<IjvxConnectorBridge*>(this), master, ass_connection_common,
			&_common_set_conn_bridge.theLink.from);
		if(res != JVX_NO_ERROR)
		{
			return res;
		}

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "++ to " << std::flush;
		std::cout << JVX_DISPLAY_CONNECTOR(static_cast<IjvxCommonConnector*>(_common_set_conn_bridge.theLink.to)) << std::endl;
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif

		_common_set_conn_bridge.theLink.to_origin->select_connect_icon(static_cast<IjvxConnectorBridge*>(this), master, ass_connection_common,
			&_common_set_conn_bridge.theLink.to);
		if(res == JVX_NO_ERROR)
		{
			_common_set_conn_bridge.refCnt_from = 0;
			_common_set_conn_bridge.refCnt_to = 0;
		}
		return res;
	};
	
	jvxErrorType terminate()
	{
		jvxErrorType res = JVX_NO_ERROR;

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
		std::cout << "Bridge <" << _common_set_conn_bridge.name << ">" << std::endl;
		std::cout << "++ disconnect connection from " << std::flush;
		std::cout << JVX_DISPLAY_CONNECTOR(static_cast<IjvxCommonConnector*>(_common_set_conn_bridge.theLink.from));
#endif
		assert(_common_set_conn_bridge.refCnt_from == 0);
		assert(_common_set_conn_bridge.refCnt_to == 0);

		res = _common_set_conn_bridge.theLink.from_origin->unselect_connect_ocon(static_cast<IjvxConnectorBridge*>(this),
			_common_set_conn_bridge.theLink.from);
		if(res != JVX_NO_ERROR)
		{
			// It may be such that a select had failed. In that case, we need to 
			// silently accept this
			std::cout << "WARNING: Bridge <" << _common_set_conn_bridge.name << "> failed to unselect of connector <from>." << std::endl;
			//return res;
		}

#ifdef JVX_DATACONNECTIONS_VERBOSE
		std::cout << "++ to " << std::flush;
		std::cout << JVX_DISPLAY_CONNECTOR(static_cast<IjvxCommonConnector*>(_common_set_conn_bridge.theLink.to));
		std::cout << "+++++++++++++++++++++++++++++ " << std::endl;
#endif
		// WARNING: _common_set_conn_bridge.theLink.to_origin may be NULL if from connector select failed
		res = _common_set_conn_bridge.theLink.to_origin->unselect_connect_icon(static_cast<IjvxConnectorBridge*>(this),
			_common_set_conn_bridge.theLink.to);
		if (res != JVX_NO_ERROR)
		{
			// It may be such that a select had failed. In that case, we need to 
			// silently accept this
			std::cout << "WARNING: Bridge <" << _common_set_conn_bridge.name << "> failed to unselect of connector <to>." << std::endl;
			//return res;
		}
	
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION descriptor_bridge(jvxApiString* str)override
	{
		if (str)
			str->assign(_common_set_conn_bridge.name);
		return JVX_NO_ERROR;
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION properties_bridge(jvxBool* dedicatedThread, jvxBool* boost, jvxBool* interceptor) override
	{
		if (dedicatedThread)
			*dedicatedThread = _common_set_conn_bridge.ded_thread;
		if(boost)
			*boost = _common_set_conn_bridge.boost_thread;
		if (interceptor)
		{
			*interceptor = _common_set_conn_bridge.interceptor_bridge;
		}
		return JVX_NO_ERROR;
	};

	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connect_to(IjvxInputConnector** theToRef)override
	{
		if(_common_set_conn_bridge.theLink.to)
		{
			if(theToRef)
			{
				*theToRef = _common_set_conn_bridge.theLink.to;
				_common_set_conn_bridge.refCnt_to ++;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	};
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connect_to(IjvxInputConnector* theToRef)override
	{
		if(_common_set_conn_bridge.theLink.to == theToRef)
		{
			assert(_common_set_conn_bridge.refCnt_to > 0);
			_common_set_conn_bridge.refCnt_to--;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_ARGUMENT;
	};
		
	virtual jvxErrorType JVX_CALLINGCONVENTION reference_connect_from(IjvxOutputConnector** theFromRef)override
	{
		if(_common_set_conn_bridge.theLink.from)
		{
			if(theFromRef)
			{
				*theFromRef = _common_set_conn_bridge.theLink.from;
				_common_set_conn_bridge.refCnt_from ++;
			}
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_WRONG_STATE;
	};
	
	virtual jvxErrorType JVX_CALLINGCONVENTION return_reference_connect_from(IjvxOutputConnector* theFromRef)override
	{
		if(_common_set_conn_bridge.theLink.from == theFromRef)
		{
			assert(_common_set_conn_bridge.refCnt_from > 0);
			_common_set_conn_bridge.refCnt_from--;
			return JVX_NO_ERROR;
		}
		return JVX_ERROR_INVALID_ARGUMENT;
	};

	virtual jvxErrorType get_configuration(jvxCallManagerConfiguration* callConf, 
		IjvxConfigProcessor* theWriter, jvxConfigData* add_to_this_section)
	{
		jvxConfigData* datTmp = NULL;
		jvxConfigData* datSubTmp = NULL;
		IjvxConnectorFactory* conFac = NULL;
		jvxComponentIdentification tpId = JVX_COMPONENT_UNKNOWN;
		jvxApiString strMF, strCo;
		jvxValue val;

		val.assign(_common_set_conn_bridge.ded_thread);
		theWriter->createAssignmentValue(&datTmp, "JVX_CONNECTOR_BRIDGE_DEDICATED_THREAD",val);
		theWriter->addSubsectionToSection(add_to_this_section, datTmp);

		val.assign(_common_set_conn_bridge.boost_thread);
		theWriter->createAssignmentValue(&datTmp, "JVX_CONNECTOR_BRIDGE_BOOST_THREAD", val);
		theWriter->addSubsectionToSection(add_to_this_section, datTmp);

		theWriter->createAssignmentString(&datTmp, "JVX_CONNECTOR_BRIDGE_NAME", _common_set_conn_bridge.name.c_str());
		theWriter->addSubsectionToSection(add_to_this_section, datTmp);
		datTmp = NULL;

		if (_common_set_conn_bridge.theLink.from != NULL)
		{
			theWriter->createEmptySection(&datTmp, "JVX_CONNECTOR_BRIDGE_REF_FROM");
			_common_set_conn_bridge.theLink.from->parent_factory(&conFac);
			if (conFac)
			{
				jvx_request_interfaceToObject(conFac, NULL, &tpId, &strMF);

				theWriter->createAssignmentString(&datSubTmp, "NAME", strMF.std_str().c_str());
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;

				theWriter->createAssignmentString(&datSubTmp, "COMPONENT_TYPE", jvxComponentType_txt(tpId.tp));
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;

				val.assign((jvxSize)tpId.slotid);
				theWriter->createAssignmentValue(&datSubTmp, "SLOT_ID", val);
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;

				val.assign((jvxSize)tpId.slotsubid);
				theWriter->createAssignmentValue(&datSubTmp, "SLOT_SUB_ID", val);
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;

				_common_set_conn_bridge.theLink.from->descriptor_connector(&strCo);
				theWriter->createAssignmentString(&datSubTmp, "CONNECTOR_NAME", strCo.std_str().c_str());
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;
			}
			else
			{
				theWriter->createComment(&datSubTmp, "Output Connector with no object reference.");
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;

				val.assign(_common_set_conn_bridge.uId_from);
				theWriter->createAssignmentValue(&datSubTmp, "ID_FROM", val);
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;
			}
			theWriter->addSubsectionToSection(add_to_this_section, datTmp);
			datTmp = NULL;
		}

		// ===================================================================

		if (_common_set_conn_bridge.theLink.to)
		{
			theWriter->createEmptySection(&datTmp, "JVX_CONNECTOR_BRIDGE_REF_TO");
			_common_set_conn_bridge.theLink.to->parent_factory(&conFac);
			if (conFac)
			{

				jvx_request_interfaceToObject(conFac, NULL, &tpId, &strMF);

				theWriter->createAssignmentString(&datSubTmp, "NAME", strMF.std_str().c_str());
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;

				theWriter->createAssignmentString(&datSubTmp, "COMPONENT_TYPE", jvxComponentType_txt(tpId.tp));
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;

				val.assign((jvxSize)tpId.slotid);
				theWriter->createAssignmentValue(&datSubTmp, "SLOT_ID", val);
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;

				val.assign((jvxSize)tpId.slotsubid);
				theWriter->createAssignmentValue(&datSubTmp, "SLOT_SUB_ID", val);
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;

				_common_set_conn_bridge.theLink.to->descriptor_connector(&strCo);
				theWriter->createAssignmentString(&datSubTmp, "CONNECTOR_NAME", strCo.std_str().c_str());
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;
			}
			else
			{
				theWriter->createComment(&datSubTmp, "Input Connector with no object reference.");
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;

				val.assign(_common_set_conn_bridge.uId_to);
				theWriter->createAssignmentValue(&datSubTmp, "ID_TO", val);
				theWriter->addSubsectionToSection(datTmp, datSubTmp);
				datSubTmp = NULL;
			}

			theWriter->addSubsectionToSection(add_to_this_section, datTmp);
			datTmp = NULL;
		}
		return JVX_NO_ERROR;
	};
};

#endif
