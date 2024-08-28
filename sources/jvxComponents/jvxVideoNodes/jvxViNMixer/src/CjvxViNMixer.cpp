#include "CjvxViNMixer.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

	CjvxViNMixer::CjvxViNMixer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE):
	CjvxVideoNode(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_CALL)
{
}

	CjvxViNMixer::~CjvxViNMixer()
{
}

//  ===================================================================================================

void
CjvxViNMixer::activate_connectors()
{

	oneInputConnectorElement newElmIn; 

	newElmIn.theConnector = static_cast<IjvxInputConnectorSelect*>(this);
	_common_set_conn_factory.input_connectors[newElmIn.theConnector] = newElmIn;

	CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::conName = "mix-in";
	JVX_SAFE_ALLOCATE_OBJECT((CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen),
		CjvxSingleInputConnectorMulti(true));
	CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen->activate(this, this,
		CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::conName, this, 0);
	newElmIn.theConnector = CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen;

	// Functional part "CjvxConnectorFactory"
	_common_set_conn_factory.input_connectors[CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen] = newElmIn;

	// ===================================================================

	oneOutputConnectorElement newElmOut;

	newElmOut.theConnector = static_cast<IjvxOutputConnectorSelect*>(this);
	_common_set_conn_factory.output_connectors[newElmOut.theConnector] = newElmOut;

	CjvxConnectorCollection < CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::conName = "mix-out";
	JVX_SAFE_ALLOCATE_OBJECT((CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen), CjvxSingleOutputConnectorMulti(true));
	CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen->activate(this, this,
		CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::conName, this, 0);
	newElmOut.theConnector = CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen;
	// Functional part "CjvxConnectorFactory"
	_common_set_conn_factory.output_connectors[CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen] = newElmOut;
	// ===================================================================

	CjvxInputOutputConnector::lds_activate(nullptr, static_cast<IjvxObject*>(this),
		static_cast<IjvxConnectorFactory*>(this), nullptr,
		"video", static_cast<IjvxInputConnector*>(this),
		static_cast<IjvxOutputConnector*>(this));
	CjvxConnectorFactory::_activate_factory(this);

}

	void
	CjvxViNMixer::deactivate_connectors()
	{
		CjvxConnectorFactory::_deactivate_factory();
		_common_set_conn_factory.input_connectors.clear();
		_common_set_conn_factory.output_connectors.clear();
		CjvxInputOutputConnector::lds_deactivate();

		CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen->deactivate();
		JVX_SAFE_DELETE_OBJECT((CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>::extra_iocon_gen));

		CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen->deactivate();
		JVX_SAFE_DELETE_OBJECT((CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>::extra_iocon_gen));
	}

	//  ===================================================================================================

	jvxErrorType
	CjvxViNMixer::report_selected_connector(CjvxSingleInputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType
	CjvxViNMixer::report_test_connector(CjvxSingleInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId)
	{
	}

	jvxErrorType
	CjvxViNMixer::report_started_connector(CjvxSingleInputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}


	jvxErrorType
	CjvxViNMixer::report_stopped_connector(CjvxSingleInputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId)
	{
	}

	jvxErrorType
	CjvxViNMixer::report_unselected_connector(CjvxSingleInputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::report_process_buffers(CjvxSingleInputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage)
	{
	}

	// ===========================================================================================

	jvxErrorType
	CjvxViNMixer::report_selected_connector(CjvxSingleOutputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType
	CjvxViNMixer::report_test_connector(CjvxSingleOutputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb))
	{
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::request_unique_id_start(CjvxSingleOutputConnector* iconn, jvxSize* uId)
	{
	}

	jvxErrorType
	CjvxViNMixer::report_started_connector(CjvxSingleOutputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	jvxErrorType
	CjvxViNMixer::report_stopped_connector(CjvxSingleOutputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::release_unique_id_stop(CjvxSingleOutputConnector* iconn, jvxSize uId)
	{
	}

	jvxErrorType
	CjvxViNMixer::report_unselected_connector(CjvxSingleOutputConnector* iconn)
	{
		return JVX_NO_ERROR;
	}

	void
	CjvxViNMixer::report_process_buffers(CjvxSingleOutputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage)
	{
	}


#ifdef JVX_PROJECT_NAMESPACE
}
#endif
