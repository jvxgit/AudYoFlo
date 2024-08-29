#include "jvxTechnologies/CjvxAudioWithVideoDevice.h"

CjvxAudioWithVideoDevice::CjvxAudioWithVideoDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE) :
	CjvxAudioDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{

}

CjvxAudioWithVideoDevice::~CjvxAudioWithVideoDevice()
{
}

void
CjvxAudioWithVideoDevice::activate_connectors_master()
{

	oneInputConnectorElement newElmIn;

	newElmIn.theConnector = static_cast<IjvxInputConnectorSelect*>(this);
	_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn;

	plusVideo.icon.activate(this, this,"video", this, 1);
	//plusVideo.icon._common_set_io_common.theMaster = &plusVideo.mas;
	//plusVideo.icon._common_set_io_common.myRuntimeId = 1;
	
	newElmIn.theConnector = static_cast<IjvxInputConnectorSelect*>(&plusVideo.icon);
	_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn;

	// ===================================================================

	oneOutputConnectorElement newElmOut;

	newElmOut.theConnector = static_cast<IjvxOutputConnectorSelect*>(this);
	_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut;

	plusVideo.ocon.activate(this, this, "video", this, 1);
	//plusVideo.ocon._common_set_io_common.theMaster = &plusVideo.mas;	
	plusVideo.ocon._common_set_io_common.myRuntimeId = 1;

	newElmOut.theConnector = static_cast<IjvxOutputConnectorSelect*>(&plusVideo.ocon);
	_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut;

	// ===================================================================

	CjvxInputOutputConnector::lds_activate(nullptr, static_cast<IjvxObject*>(this),
		static_cast<IjvxConnectorFactory*>(this), static_cast<IjvxConnectionMaster*>(this),
		"audio", static_cast<IjvxInputConnector*>(this),
		static_cast<IjvxOutputConnector*>(this));
	CjvxConnectorFactory::_activate_factory(this);
	
	// ===================================================================

	oneMasterElement newElm;

	CjvxConnectionMaster::_init_master(static_cast<CjvxObject*>(this), "audio",
		static_cast<IjvxConnectionMasterFactory*>(this)); 			
	
	newElm.descror = "audio"; 
	newElm.theMaster = static_cast<IjvxConnectionMaster*>(this); 
	_common_set_conn_master_factory.masters[newElm.theMaster] = newElm; 

	plusVideo.mas._common_set_ma_common.myParent = this;
	plusVideo.mas._common_set_ma_common.conn_out = &plusVideo.ocon;
	plusVideo.mas._common_set_ma_common.name = "CjvxAudioWithVideoDevice-video";
	plusVideo.mas._common_set_ma_common.descriptor = "video";
	plusVideo.mas._common_set_ma_common.myRuntimeId = 1;
	plusVideo.mas._common_set_ma_common.object = this;
	plusVideo.mas._common_set_ma_common.cbRef = this;

	newElm.descror = "video";
	newElm.theMaster = static_cast<IjvxConnectionMaster*>(&plusVideo.mas);
	_common_set_conn_master_factory.masters[newElm.theMaster] = newElm;

	CjvxConnectionMasterFactory::_activate_master_factory(static_cast<IjvxObject*>(this)); 	
}

void
CjvxAudioWithVideoDevice::deactivate_connectors_master()
{
	CjvxConnectorFactory::_deactivate_factory();
	_common_set_conn_factory.input_connectors.clear();
	_common_set_conn_factory.output_connectors.clear();
	CjvxInputOutputConnector::lds_deactivate();

	CjvxConnectionMasterFactory::_deactivate_master_factory();
	_common_set_conn_master_factory.masters.clear();
	CjvxConnectionMaster::_terminate_master();
}

//  ===================================================================================================
jvxErrorType 
CjvxAudioWithVideoDevice::report_selected_connector(CjvxSingleInputConnector* iconn)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioWithVideoDevice::report_test_connector(CjvxSingleInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	return JVX_NO_ERROR;
}

void 
CjvxAudioWithVideoDevice::request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId) 
{
}

jvxErrorType 
CjvxAudioWithVideoDevice::report_started_connector(CjvxSingleInputConnector* iconn) 
{
	return JVX_NO_ERROR;
}


jvxErrorType 
CjvxAudioWithVideoDevice::report_stopped_connector(CjvxSingleInputConnector* iconn) 
{
	return JVX_NO_ERROR;
}

void 
CjvxAudioWithVideoDevice::release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId) 
{
}

jvxErrorType 
CjvxAudioWithVideoDevice::report_unselected_connector(CjvxSingleInputConnector* iconn) 
{
	return JVX_NO_ERROR;
}

void 
CjvxAudioWithVideoDevice::report_process_buffers(CjvxSingleInputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage) 
{
}

// ===========================================================================================

jvxErrorType 
CjvxAudioWithVideoDevice::report_selected_connector(CjvxSingleOutputConnector* iconn) 
{
	return JVX_NO_ERROR;
}

jvxErrorType
CjvxAudioWithVideoDevice::report_test_connector(CjvxSingleOutputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) 
{
	return JVX_NO_ERROR;
}

void 
CjvxAudioWithVideoDevice::request_unique_id_start(CjvxSingleOutputConnector* iconn, jvxSize* uId) 
{
}

jvxErrorType 
CjvxAudioWithVideoDevice::report_started_connector(CjvxSingleOutputConnector* iconn)
{
	return JVX_NO_ERROR;
}

jvxErrorType 
CjvxAudioWithVideoDevice::report_stopped_connector(CjvxSingleOutputConnector* iconn) 
{
	return JVX_NO_ERROR;
}

void 
CjvxAudioWithVideoDevice::release_unique_id_stop(CjvxSingleOutputConnector* iconn, jvxSize uId)
{
}

jvxErrorType 
CjvxAudioWithVideoDevice::report_unselected_connector(CjvxSingleOutputConnector* iconn)
{
	return JVX_NO_ERROR;
}

void 
CjvxAudioWithVideoDevice::report_process_buffers(CjvxSingleOutputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage)
{
}

// ===========================================================================================
void 
CjvxAudioWithVideoDevice::before_test_chain(CjvxSingleMaster* mas)
{
}

void
CjvxAudioWithVideoDevice::before_connect_chain()
{

}