#include "common/CjvxDataChainInterceptor.h"

CjvxDataChainInterceptor::CjvxDataChainInterceptor(
	IjvxConnectorFactory* myParentRef, IjvxObject* objPar, 
	const std::string& name_connection, 
	const std::string& name_bridge,
	jvxSize cnt_interceptor,
	IjvxDataChainInterceptorReport* repRef, jvxBool dedicatedThreadArg, jvxBool boostThreadArg) :
	myParent(myParentRef), myReport(repRef), toolsHost(NULL),
	conn_in(NULL), conn_out(NULL), 
	theData_out(NULL),theData_in(NULL), 
	theMaster_in(NULL), theMaster_out(NULL),
	theCommon_in(NULL), theCommon_out(NULL), 
	icon(NULL), ocon(NULL), objParent(objPar),
	dedicatedThread(dedicatedThreadArg), 
	boostThread(boostThreadArg)
{
	nm_process = name_connection;
	nm_bridge = name_bridge;
	id_interceptor = cnt_interceptor;

	nm_ic = nm_process + "--interceptor--#" + jvx_size2String(id_interceptor);

	isBridgeIn = false;
	isBridgeOut = false;

	isProcess = true;

	my_tag = JVX_PROCESS_MARK_INOUT_CONNECTOR;
}

CjvxDataChainInterceptor::~CjvxDataChainInterceptor()
{
}

jvxErrorType
CjvxDataChainInterceptor::outputs_data_format_group(jvxDataFormatGroup grp)
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataChainInterceptor::accepts_data_format_group(jvxDataFormatGroup grp)
{
	return JVX_NO_ERROR;
}

void
CjvxDataChainInterceptor::set_tools_host(IjvxToolsHost* toolsHostPar)
{
	toolsHost = toolsHostPar;
}

void 
CjvxDataChainInterceptor::tag(jvxDataChainConnectorTag tagParam)
{
	my_tag = tagParam;
}

void 
CjvxDataChainInterceptor::updateName(const char* newNm)
{
	nm_ic = newNm;
}

jvxErrorType
CjvxDataChainInterceptor::check_master_common()
{
	jvxErrorType res = JVX_NO_ERROR;

	if (
		(theMaster_in != NULL) && (theMaster_out != NULL))
	{
		if (theMaster_in != theMaster_out)
		{
			res = JVX_ERROR_INTERNAL;
		}
	}

	if (res == JVX_NO_ERROR)
	{
		if (
			(theCommon_in != NULL) && (theCommon_out != NULL))
		{
			if (theCommon_in != theCommon_out)
			{
				res = JVX_ERROR_INTERNAL;
			}
		}
	}
	return res;
}

jvxErrorType
CjvxDataChainInterceptor::description_interceptor(jvxApiString* str)
{
	if (str)
	{
		str->assign(nm_connect_to);
	}
	return JVX_NO_ERROR;
}

// ===========================================================================

jvxErrorType 
CjvxDataChainInterceptor::descriptor_connector(jvxApiString* str)
{
	if (str)
	{
		str->assign(nm_ic);
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataChainInterceptor::parent_factory(IjvxConnectorFactory** my_parent) 
{
	if (my_parent)
	{
		*my_parent = myParent;
	}
	return JVX_NO_ERROR;
}

// =============================================================================================

jvxErrorType 
CjvxDataChainInterceptor::associated_common_icon(IjvxDataConnectionCommon** ref)
{
	if (ref)
	{
		*ref = theCommon_in;
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataChainInterceptor::select_connect_icon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	IjvxInputConnector** replace_connector)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	conn_in = obj;
	theMaster_in = master;
	theCommon_in = ass_connection_common;

	if (isProcess)
	{
		// Check that master is setup properly
		res = check_master_common();
		assert(res == JVX_NO_ERROR);
	}
	return res;
}

jvxErrorType 
CjvxDataChainInterceptor::unselect_connect_icon(IjvxConnectorBridge* obj,
	IjvxInputConnector* replace_connector)
{
	assert(conn_in == obj);
	conn_in = NULL;
	theMaster_in = NULL;
	theCommon_in = NULL;

	return JVX_NO_ERROR;
}

// =====================================================================

jvxErrorType
CjvxDataChainInterceptor::associated_common_ocon(IjvxDataConnectionCommon** ref)
{
	if (ref)
	{
		*ref = theCommon_out;
	}
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataChainInterceptor::select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
	IjvxDataConnectionCommon* ass_connection_common,
	IjvxOutputConnector** replace_connector)
{
	jvxErrorType res = JVX_NO_ERROR;

	conn_out = obj;
	theMaster_out = master;
	theCommon_out = ass_connection_common;

	if (isProcess)
	{
		res = check_master_common();
		assert(res == JVX_NO_ERROR);
	}

	return JVX_NO_ERROR;
}

jvxErrorType
CjvxDataChainInterceptor::unselect_connect_ocon(
	IjvxConnectorBridge* obj,
	IjvxOutputConnector* replace_connector)
{
	// assert(conn_out == obj);
	conn_out = NULL;
	theMaster_out = NULL;
	theCommon_out = NULL;

	return JVX_NO_ERROR;
}

// =====================================================================

jvxErrorType 
CjvxDataChainInterceptor::connect_connect_icon(jvxLinkDataDescriptor* theData
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	conn_in->reference_connect_from(&ocon);
	assert(ocon);
	theData_in = theData;
	res = this->connect_connect_ocon(jvxChainConnectArguments(theData->con_link.uIdConn, theData->con_link.master )
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	if (res != JVX_NO_ERROR)
	{
		conn_in->return_reference_connect_from(ocon);
		ocon = NULL;
	}
	return res;
}

jvxErrorType
CjvxDataChainInterceptor::connect_connect_ocon(const jvxChainConnectArguments& args
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	IjvxConnectorFactory* parFac = NULL;
	IjvxObject* theObject = NULL;
	jvxApiString aStr;
	jvxApiString cStr;
	jvxErrorType resL = JVX_NO_ERROR;
	if (conn_out)
	{
		conn_out->reference_connect_to(&icon);
		assert(icon);

		nm_connect_to = "unknown";
		icon->descriptor_connector(&cStr);
		nm_connect_to = cStr.std_str();
		resL = icon->parent_factory(&parFac);
		if (parFac)
		{
			parFac->request_reference_object(&theObject);
			assert(theObject);
			theObject->description(&aStr);
			nm_connect_to += ":" + aStr.std_str();
			parFac->return_reference_object(theObject);
		}
		theData_out = theData_in;
		res = icon->connect_connect_icon(theData_out  JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		if (res != JVX_NO_ERROR)
		{
			conn_out->return_reference_connect_to(icon);
			icon = NULL;
			nm_connect_to = "";
		}
	}
	return res;
}

// =====================================================================

jvxErrorType 
CjvxDataChainInterceptor::disconnect_connect_icon(jvxLinkDataDescriptor* theData
	JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	jvxErrorType res = this->disconnect_connect_ocon(jvxChainConnectArguments(theData->con_link.uIdConn, theData->con_link.master)
		JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
	assert(res == JVX_NO_ERROR);
	conn_in->return_reference_connect_from(ocon);
	ocon = NULL;
	theData_in = NULL;
	return JVX_NO_ERROR;
}

jvxErrorType CjvxDataChainInterceptor::disconnect_connect_ocon(const jvxChainConnectArguments& args JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (icon)
	{
		icon->disconnect_connect_icon(theData_out JVX_CONNECTION_FEEDBACK_CALL_A(fdb));
		conn_out->return_reference_connect_to(icon);
		icon = NULL;
		theData_out = NULL;
	}
	return JVX_NO_ERROR;
}

// =====================================================================

jvxErrorType 
CjvxDataChainInterceptor::test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	std::string ret = nm_ic + "->" + nm_connect_to;
	if (objParent == NULL)
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_NO_OBJ_COMM_CONN(fdb, "default", ret.c_str(), "Entering chain interceptor input connector");
	}
	else
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, objParent, "default", "Entering chain interceptor input connector");
	}
	JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_I(fdb, theData_in);
	return test_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType CjvxDataChainInterceptor::test_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	std::string ret = nm_ic + "->" + nm_connect_to;
	if (objParent == NULL)
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_NO_OBJ_COMM_CONN(fdb, "default", ret.c_str(), "Entering chain interceptor output connector");
	}
	else
	{
		JVX_CONNECTION_FEEDBACK_ON_ENTER_OBJ_COMM_CONN(fdb, objParent, "default", "Entering chain interceptor output connector");
	}
	JVX_CONNECTION_FEEDBACK_ON_ENTER_LINKDATA_TEXT_I(fdb, theData_in);

	if (icon)
	{
		return icon->test_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return JVX_NO_ERROR;
}

// =====================================================================

jvxErrorType 
CjvxDataChainInterceptor::prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	return prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxDataChainInterceptor::prepare_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	if (icon)
	{
		return icon->prepare_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return JVX_NO_ERROR;
}

// =====================================================================

jvxErrorType 
CjvxDataChainInterceptor::start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) 
{
	return start_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxDataChainInterceptor::start_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (icon)
	{
		res = icon->start_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

// =====================================================================

jvxErrorType
CjvxDataChainInterceptor::stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return stop_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxDataChainInterceptor::stop_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (icon)
	{
		res = icon->stop_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

// =====================================================================

jvxErrorType
CjvxDataChainInterceptor::postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	return postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
}

jvxErrorType
CjvxDataChainInterceptor::postprocess_connect_ocon(JVX_CONNECTION_FEEDBACK_TYPE(fdb))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (icon)
	{
		res = icon->postprocess_connect_icon(JVX_CONNECTION_FEEDBACK_CALL(fdb));
	}
	return res;
}

// =====================================================================

jvxErrorType 
CjvxDataChainInterceptor::process_start_icon(jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{
	return process_start_ocon(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
}

jvxErrorType
CjvxDataChainInterceptor::process_start_ocon(jvxSize pipeline_offset, jvxSize* idx_stage,
	jvxSize tobeAccessedByStage,
	callback_process_start_in_lock clbk,
	jvxHandle* priv_ptr)
{

	jvxErrorType res = JVX_NO_ERROR;
	if (icon)
	{
		res = icon->process_start_icon(pipeline_offset, idx_stage, tobeAccessedByStage, clbk, priv_ptr);
	}
	return res;
}

// =================================================================================

jvxErrorType 
CjvxDataChainInterceptor::process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) 
{
	jvxErrorType res = JVX_NO_ERROR;
	
	if (myReport)
	{
		myReport->report_event_interceptor(JVX_INTERCEPTOR_REPORT_PROCESS_START_ENTER, id_interceptor);
	}
	res = process_buffers_ocon(mt_mask, idx_stage);
	if (myReport)
	{
		myReport->report_event_interceptor(JVX_INTERCEPTOR_REPORT_PROCESS_START_LEAVE, id_interceptor);
	}
	return res;
}

jvxErrorType
CjvxDataChainInterceptor::process_buffers_ocon(jvxSize mt_mask, jvxSize idx_stage)
{
	jvxErrorType res = JVX_NO_ERROR;
	
	if (myReport)
	{
		myReport->report_event_interceptor(JVX_INTERCEPTOR_REPORT_PROCESS_BUFFER_ENTER, id_interceptor);
	}
	
	if (icon)
	{
		res = icon->process_buffers_icon(mt_mask, idx_stage);
	}

	if (myReport)
	{
		myReport->report_event_interceptor(JVX_INTERCEPTOR_REPORT_PROCESS_BUFFER_LEAVE, id_interceptor);
	}

	return res;
}

// ===================================================================================

jvxErrorType 
CjvxDataChainInterceptor::process_stop_icon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr)
{
	jvxErrorType res = JVX_NO_ERROR;

	if (myReport)
	{
		myReport->report_event_interceptor(JVX_INTERCEPTOR_REPORT_PROCESS_STOP_ENTER, id_interceptor);
	}

	res = process_stop_ocon(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);

	if (myReport)
	{
		myReport->report_event_interceptor(JVX_INTERCEPTOR_REPORT_PROCESS_STOP_LEAVE, id_interceptor);
	}
	return res;
}

jvxErrorType
CjvxDataChainInterceptor::process_stop_ocon(jvxSize idx_stage, jvxBool shift_fwd,
	jvxSize tobeAccessedByStage,
	callback_process_stop_in_lock clbk,
	jvxHandle* priv_ptr )
{
	jvxErrorType res = JVX_NO_ERROR;
	if (icon)
	{
		res = icon->process_stop_icon(idx_stage, shift_fwd, tobeAccessedByStage, clbk, priv_ptr);
	}
	return res;
}

// =================================================================================

jvxErrorType 
CjvxDataChainInterceptor::transfer_backward_icon(jvxLinkDataTransferType tp, jvxHandle* hdl
	JVX_CONNECTION_FEEDBACK_TYPE_A(var)) 
{
	jvxErrorType res = JVX_NO_ERROR;
	if (ocon)
	{
		res = ocon->transfer_backward_ocon(tp, hdl JVX_CONNECTION_FEEDBACK_CALL_A(var));
	}
	return res;
}

jvxErrorType
CjvxDataChainInterceptor::transfer_backward_ocon(jvxLinkDataTransferType tp, jvxHandle* hdl
	JVX_CONNECTION_FEEDBACK_TYPE_A(var))
{
	return transfer_backward_icon(tp, hdl JVX_CONNECTION_FEEDBACK_CALL_A(var));
}

// =======================================================================================

jvxErrorType 
CjvxDataChainInterceptor::transfer_forward_icon(jvxLinkDataTransferType tp, jvxHandle* hdl
	JVX_CONNECTION_FEEDBACK_TYPE_A(var)) 
{
	return transfer_forward_ocon(tp, hdl JVX_CONNECTION_FEEDBACK_CALL_A(var));
}

jvxErrorType
CjvxDataChainInterceptor::transfer_forward_ocon(jvxLinkDataTransferType tp, jvxHandle* hdl
	JVX_CONNECTION_FEEDBACK_TYPE_A(var))
{
	jvxErrorType res = JVX_NO_ERROR;
	if (icon)
	{
		res = icon->transfer_forward_icon(tp, hdl JVX_CONNECTION_FEEDBACK_CALL_A(var));
	}
	return res;
}

// ====================================================================================================
// ====================================================================================================

jvxErrorType 
CjvxDataChainInterceptor::try_run_processing(IjvxConnectionMaster* master_ref)
{
	return JVX_ERROR_UNSUPPORTED;
}

// ====================================================================================================
// ====================================================================================================

jvxErrorType
CjvxDataChainInterceptor::number_next(jvxSize* num)
{
	if (num)
	{
		*num = 0;
		if (theData_out->con_link.connect_to)
		{
			*num = 1;
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataChainInterceptor::reference_next(jvxSize idx, IjvxConnectionIterator** next)
{
	if(idx == 0)
	{
		if (next)
		{
			*next = nullptr;
		}

		IjvxInputConnector* icon = nullptr;
		connected_icon(&icon);
		if (icon)
		{
			if (next)
			{
				*next = static_cast<IjvxConnectionIterator*>(icon);
			}			
		}
		return JVX_NO_ERROR;
	}
	return JVX_ERROR_ID_OUT_OF_BOUNDS;
}

jvxErrorType 
CjvxDataChainInterceptor::connected_icon(IjvxInputConnector** icon)
{
	if (icon)
	{
		*icon = nullptr;
		if (theData_out)
		{
			if (theData_out->con_link.connect_to)
			{
				*icon = theData_out->con_link.connect_to;
			}
		}
	}
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxDataChainInterceptor::reference_component(
	jvxComponentIdentification* cpTp, 
	jvxApiString* modName,
	jvxApiString* lContext)
{
	if (cpTp)
	{
		jvxComponentIdentification tp = JVX_COMPONENT_INTERCEPTOR;
		tp.slotid = JVX_SIZE_SLOT_OFF_SYSTEM;
		tp.slotsubid = JVX_SIZE_SLOT_OFF_SYSTEM;
		*cpTp = tp;
		if (modName)
		{
			modName->assign("CjvxDataChainInterceptor");
		}
		if (lContext)
		{
			lContext->assign("Interceptor Default Connector");
		}
	}
	return JVX_NO_ERROR;
}
