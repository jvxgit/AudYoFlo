#ifndef __CJVXVINMIXER_H__
#define __CJVXVINMIXER_H__

#include "jvxVideoNodes/CjvxVideoNode.h"
#include "common/CjvxSingleInputConnector.h"
#include "common/CjvxSingleOutputConnector.h"
#include "common/CjvxConnectorCollection.h"

#include "pcg_exports_node.h"

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxViNMixer: public CjvxVideoNode, 
	public CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>,
	public CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>, 
	public CjvxConnectorCollection_startstop<CjvxSingleInputConnector>,
	public CjvxConnectorCollection_startstop<CjvxSingleOutputConnector>,
	public CjvxConnectorCollection_bwd,
	public CjvxViNMixer_genpcg
{
private:
protected:

	struct
	{
		CjvxSimplePropsPars node;
	} video_input;

	struct
	{
		CjvxSimplePropsPars node;
	} video_output;

	
	JVX_RW_MUTEX_HANDLE safeCall;

#ifdef JVX_OPEN_BMP_FOR_TEXT

	struct
	{

		jvxByte* bufRGBA32 = nullptr;
		jvxSize szBufRGBA32 = 0;
	} test;
#endif

public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxViNMixer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxViNMixer();

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	// ===========================================================================================

	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;

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

	void activate_connectors() override;
	void deactivate_connectors() override;

	virtual void lock(jvxBool rwExclusive = true, jvxSize idLock = JVX_SIZE_UNSELECTED) override;
	virtual void unlock(jvxBool rwExclusive = true, jvxSize idLock = JVX_SIZE_UNSELECTED) override;

	virtual void report_proc_connector_started_in_lock(CjvxSingleInputConnector* iconnPlus) override;
	virtual void report_proc_connector_before_stop_in_lock(CjvxSingleInputConnector* iconnPlus) override;

	virtual void report_proc_connector_started_in_lock(CjvxSingleOutputConnector* oconnPlus) override;
	virtual void report_proc_connector_before_stop_in_lock(CjvxSingleOutputConnector* oconnPlus) override;

};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
