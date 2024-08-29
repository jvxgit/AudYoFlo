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

	struct
	{

		jvxByte* bufRGBA32 = nullptr;
		jvxSize szBufRGBA32 = 0;
	} test;

public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxViNMixer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxViNMixer();

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

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
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
