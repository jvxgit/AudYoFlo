#include "jvxDevices/CjvxBareDeviceNtask.h"
#include "jvxDevices/CjvxMaster_Ntask.h"

CjvxBareDeviceNtask::CjvxBareDeviceNtask(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxObject(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
	_common_set.theComponentType.unselected(JVX_COMPONENT_UNKNOWN);
	_common_set.theObjectSpecialization = reinterpret_cast<jvxHandle*>(static_cast<IjvxDevice*>(this));
	_common_set.thisisme = static_cast<IjvxObject*>(this);

	JVX_INITIALIZE_MUTEX(_common_set_master_ntask.mut_hookup);
}

CjvxBareDeviceNtask::~CjvxBareDeviceNtask()
{
	JVX_TERMINATE_MUTEX(_common_set_master_ntask.mut_hookup);
}

jvxErrorType
CjvxBareDeviceNtask::activate() 
{
	jvxSize i,j;
	jvxErrorType res = CjvxObject::_activate();
	if (res == JVX_NO_ERROR)
	{
		for (i = 0; i < _common_set_master_ntask.fixedMaster.size(); i++)
		{
			oneMasterElement masterElm;
			CjvxMasterNtask* theNewMaster = new CjvxMasterNtask(
				_common_set_master_ntask.fixedMaster[i].idCntxt,
				_common_set_master_ntask.fixedMaster[i].nmMaster.c_str(),
				static_cast<IjvxConnectionMasterFactory*>(this),
				static_cast<CjvxMasterNtask_report*>(this),
				static_cast<CjvxObject*>(this));

			masterElm.descror = _common_set_master_ntask.fixedMaster[i].nmMaster;
			//masterElm.refCnt = 0;
			masterElm.theMaster = theNewMaster;
			//masterElm.uId = i;

			_common_set_conn_master_factory.masters[static_cast<IjvxConnectionMaster*>(theNewMaster)] = masterElm;

			_common_set_master_ntask.fixedMaster[i].theMaster = theNewMaster;
		}

		for (i = 0; i < _common_set_connector_ntask.fixedTasks.size(); i++)
		{
			jvxSize j0_ocon = JVX_SIZE_UNSELECTED;
			jvxSize j0_icon = JVX_SIZE_UNSELECTED;
			for (j = 0; j < _common_set_master_ntask.fixedMaster.size(); j++)
			{
				if(_common_set_master_ntask.fixedMaster[j].nmConnector == 
					_common_set_connector_ntask.fixedTasks[i].outputConnector.nmConnector)
				{
					j0_ocon = j;
					break;
				}
			}
			for (j = 0; j < _common_set_master_ntask.fixedMaster.size(); j++)
			{
				if (_common_set_master_ntask.fixedMaster[j].nmConnector ==
					_common_set_connector_ntask.fixedTasks[i].inputConnector.nmConnector)
				{
					j0_icon = j;
					break;
				}
			}

			if (JVX_CHECK_SIZE_SELECTED(j0_icon))
			{
				_common_set_connector_ntask.fixedTasks[i].inputConnector.assocMaster =
					_common_set_master_ntask.fixedMaster[j0_icon].theMaster;
			}
			if (JVX_CHECK_SIZE_SELECTED(j0_ocon))
			{
				_common_set_connector_ntask.fixedTasks[i].outputConnector.assocMaster =
					_common_set_master_ntask.fixedMaster[j0_ocon].theMaster;
			}

			CjvxInputConnectorNtask* theNewIcon = new CjvxInputConnectorNtask(
				_common_set_connector_ntask.fixedTasks[i].inputConnector.idCon,
				_common_set_connector_ntask.fixedTasks[i].inputConnector.nmConnector.c_str(),
				static_cast<IjvxConnectorFactory*>(this),
				static_cast<IjvxInputOutputConnectorNtask*>(this),
				_common_set_connector_ntask.fixedTasks[i].inputConnector.assocMaster,
				static_cast<IjvxObject*>(this));

			CjvxOutputConnectorNtask* theNewOcon = new CjvxOutputConnectorNtask(
				_common_set_connector_ntask.fixedTasks[i].outputConnector.idCon,
				_common_set_connector_ntask.fixedTasks[i].outputConnector.nmConnector.c_str(),
				static_cast<IjvxConnectorFactory*>(this),
				static_cast<IjvxInputOutputConnectorNtask*>(this),
				_common_set_connector_ntask.fixedTasks[i].outputConnector.assocMaster,
				static_cast<IjvxObject*>(this));

			theNewIcon->set_reference(theNewOcon);
			theNewOcon->set_reference(theNewIcon);

			_common_set_connector_ntask.fixedTasks[i].icon = theNewIcon;
			_common_set_connector_ntask.fixedTasks[i].ocon = theNewOcon;

			oneInputConnectorElement newElmIn;
			newElmIn.theConnector = theNewIcon;
			_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn;

			oneOutputConnectorElement newElmOut;
			newElmOut.theConnector = theNewOcon;
			_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut;

			if (JVX_CHECK_SIZE_SELECTED(j0_ocon))
			{
				static_cast<CjvxMasterNtask*>(_common_set_master_ntask.fixedMaster[j0_ocon].theMaster)->link_master(
					_common_set_connector_ntask.fixedTasks[i].ocon, _common_set_connector_ntask.fixedTasks[i].icon);
			}
		}
		res = CjvxConnectorFactory::_activate_factory(static_cast<IjvxObject*>(this));
		res = CjvxConnectionMasterFactory::_activate_master_factory(static_cast<IjvxObject*>(this));
	}
	return res;
}

jvxErrorType
CjvxBareDeviceNtask::deactivate()
{
	jvxSize j;
	jvxErrorType res = CjvxObject::_deactivate();
	if (res == JVX_NO_ERROR)
	{
		res = CjvxConnectionMasterFactory::_deactivate_master_factory();
		res = CjvxConnectorFactory::_deactivate_factory();

		for (j = 0; j < _common_set_master_ntask.fixedMaster.size(); j++)
		{
			static_cast<CjvxMasterNtask*>(_common_set_master_ntask.fixedMaster[j].theMaster)->unlink_master();
		}

		// =================================================================

		for (j = 0; j < _common_set_connector_ntask.fixedTasks.size(); j++)
		{
			_common_set_connector_ntask.fixedTasks[j].icon = NULL;
			_common_set_connector_ntask.fixedTasks[j].ocon = NULL;
		}
		auto elm_in = _common_set_conn_factory.input_connectors.begin();
		for (; elm_in != _common_set_conn_factory.input_connectors.end(); elm_in++)
		{
			delete(elm_in->second.theConnector);
		}
		_common_set_conn_factory.input_connectors.clear();
		auto elm_out = _common_set_conn_factory.output_connectors.begin();
		for (; elm_out != _common_set_conn_factory.output_connectors.end(); elm_out++)
		{
			delete(elm_out->second.theConnector);
		}
		_common_set_conn_factory.output_connectors.clear();

		// =================================================================
		for (j = 0; j < _common_set_master_ntask.fixedMaster.size(); j++)
		{
			_common_set_master_ntask.fixedMaster[j].theMaster = NULL;
		}
		auto elm_mas = _common_set_conn_master_factory.masters.begin();
		for (; elm_mas != _common_set_conn_master_factory.masters.end(); elm_mas++)
		{
			delete(elm_mas->second.theMaster);
		}
		_common_set_conn_master_factory.masters.clear();

		// =================================================================
	}
	return res;
}

// ==============================================================================================

jvxErrorType
CjvxBareDeviceNtask::test_chain_master_id(jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (refto)
	{
		res = refto->test_chain_master_core(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxBareDeviceNtask::prepare_chain_master_id(jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Handle automatic prepare
	switch (_common_set_min.theState)
	{
	case JVX_STATE_ACTIVE:
		assert(_common_set.stateChecks.prepare_flags == 0);

		res = prepare();
		if (res != JVX_NO_ERROR)
		{
			std::string txt = "Implicit prepare failed on object " + _common_set_min.theDescription;
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), res);
			return res;
		}

		jvx_bitSet(_common_set.stateChecks.prepare_flags, idCtxt);
		break;
	case JVX_STATE_PREPARED:
		if (_common_set.stateChecks.prepare_flags)
		{
			jvx_bitSet(_common_set.stateChecks.prepare_flags, idCtxt);
		}
		break;
	default:
		return JVX_ERROR_WRONG_STATE;
	}

	if (refto)
	{
		res = refto->prepare_chain_master_core(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

jvxErrorType
CjvxBareDeviceNtask::postprocess_chain_master_id(jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	assert(_common_set_min.theState == JVX_STATE_PREPARED);

	if (refto)
	{
		res = refto->postprocess_chain_master_core(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(res == JVX_NO_ERROR);
	}
	
	// Handle automatic postprocess
	if (_common_set.stateChecks.prepare_flags)
	{
		jvx_bitClear(_common_set.stateChecks.prepare_flags, idCtxt);
		if (_common_set.stateChecks.prepare_flags == 0)
		{
			res = postprocess();
			if (res != JVX_NO_ERROR)
			{
				std::cout << "Implicit postprocess failed on object " << _common_set_min.theDescription << std::endl;
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBareDeviceNtask::start_chain_master_id(jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto  JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	// Handle automatic prepare
	switch (_common_set_min.theState)
	{
	case JVX_STATE_PREPARED:
		assert(_common_set.stateChecks.start_flags == 0);

		res = start();
		if (res != JVX_NO_ERROR)
		{
			std::string txt = "Implicit prepare failed on object " + _common_set_min.theDescription;
			JVX_CONNECTION_FEEDBACK_SET_ERROR_STRING(fdb, txt.c_str(), res);
			return res;
		}

		jvx_bitSet(_common_set.stateChecks.start_flags, 0);
		break;
	case JVX_STATE_PROCESSING:
		if (_common_set.stateChecks.start_flags)
		{
			jvx_bitSet(_common_set.stateChecks.start_flags, idCtxt);
		}
		break;
	default:
		return JVX_ERROR_WRONG_STATE;
	}

	if (refto)
	{
		res = refto->start_chain_master_core(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}

	JVX_LOCK_MUTEX(_common_set_master_ntask.mut_hookup);
	jvx_bitSet(_common_set_master_ntask.flags_hookup, idCtxt);
	JVX_UNLOCK_MUTEX(_common_set_master_ntask.mut_hookup);

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBareDeviceNtask::stop_chain_master_id(jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, jvxSize idCtxt, CjvxMasterNtask* refto JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;

	assert(_common_set_min.theState == JVX_STATE_PROCESSING);

	JVX_LOCK_MUTEX(_common_set_master_ntask.mut_hookup);
	jvx_bitClear(_common_set_master_ntask.flags_hookup, idCtxt);
	JVX_UNLOCK_MUTEX(_common_set_master_ntask.mut_hookup);

	if (refto)
	{
		res = refto->stop_chain_master_core(JVX_CONNECTION_FEEDBACK_CALL(fdb));
		assert(res == JVX_NO_ERROR);
	}
	// Handle automatic stop
	if (_common_set.stateChecks.start_flags)
	{
		jvx_bitClear(_common_set.stateChecks.start_flags, idCtxt);

		// Very last instance shuts down state PROCESSING
		if (_common_set.stateChecks.start_flags == 0)
		{
			res = stop();
			if (res != JVX_NO_ERROR)
			{
				std::cout << "Implicit stop failed on object " << _common_set_min.theDescription << std::endl;
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxBareDeviceNtask::connect_connect_ntask(jvxLinkDataDescriptor* theData_in, 
	jvxLinkDataDescriptor* theData_out, jvxSize idTask)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxBareDeviceNtask::disconnect_connect_ntask(
	jvxLinkDataDescriptor* theData_in,
	jvxLinkDataDescriptor* theData_out, 
	jvxSize idTask)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxBareDeviceNtask::is_ready_master_id(jvxBool* is_ready, jvxApiString* reasonifnot, jvxSize idCtxt
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	return this->is_ready(is_ready, reasonifnot);
}

