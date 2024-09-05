#ifndef __CJVXAUDIOWITHVIDEODEVICE_H__
#define __CJVXAUDIOWITHVIDEODEVICE_H__

#include "jvxAudioTechnologies/CjvxAudioDevice.h"
#include "common/CjvxSingleInputConnector.h"
#include "common/CjvxSingleOutputConnector.h"
#include "common/CjvxSingleMaster.h"

class CjvxAddVideo 
{
public:
	CjvxSingleInputConnector icon;
	CjvxSingleOutputConnector ocon;
	CjvxSingleMaster mas;

	CjvxAddVideo() : icon(false), ocon(false)
	{
		icon.allowSingleConnect = true;
		ocon.allowSingleConnect = true;
	};

	jvxErrorType is_ready(jvxBool* isReady, jvxApiString* reason)
	{
		return JVX_NO_ERROR;
	};
};

class CjvxAudioWithVideoDevice: public CjvxAudioDevice,
	public CjvxSingleConnector_report<CjvxSingleOutputConnector>,
	public CjvxSingleConnector_report<CjvxSingleInputConnector>,
	public CjvxSingleConnectorTransfer_report<CjvxSingleOutputConnector>,
	public CjvxSingleConnectorTransfer_report<CjvxSingleInputConnector>,
	public CjvxSingleMaster_report
{
protected:
	CjvxAddVideo plusVideo;
	
public:

	JVX_CALLINGCONVENTION CjvxAudioWithVideoDevice(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxAudioWithVideoDevice();

	void activate_connectors_master() override;
	void deactivate_connectors_master() override;

	// ===========================================================================================

	jvxErrorType report_selected_connector(CjvxSingleInputConnector* iconn) override;
	jvxErrorType report_test_connector(CjvxSingleInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	void request_unique_id_start(CjvxSingleInputConnector* iconn, jvxSize* uId) override;
	jvxErrorType report_started_connector(CjvxSingleInputConnector* iconn) override;

	jvxErrorType report_stopped_connector(CjvxSingleInputConnector* iconn) override;
	void release_unique_id_stop(CjvxSingleInputConnector* iconn, jvxSize uId) override;
	jvxErrorType report_unselected_connector(CjvxSingleInputConnector* iconn) override;

	void report_process_buffers(CjvxSingleInputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage) override;

	// ===========================================================================================

	jvxErrorType report_selected_connector(CjvxSingleOutputConnector* iconn) override;
	jvxErrorType report_test_connector(CjvxSingleOutputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	void request_unique_id_start(CjvxSingleOutputConnector* iconn, jvxSize* uId) override;
	jvxErrorType report_started_connector(CjvxSingleOutputConnector* iconn) override;

	jvxErrorType report_stopped_connector(CjvxSingleOutputConnector* iconn) override;
	void release_unique_id_stop(CjvxSingleOutputConnector* iconn, jvxSize uId) override;
	jvxErrorType report_unselected_connector(CjvxSingleOutputConnector* iconn) override;

	void report_process_buffers(CjvxSingleOutputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage) override;

	// ===========================================================================================
	void before_test_chain(CjvxSingleMaster* mas) override;
	void before_connect_chain() override;

};

#endif
	
