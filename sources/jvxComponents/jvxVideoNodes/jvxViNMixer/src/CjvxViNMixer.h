#ifndef __CJVXVINMIXER_H__
#define __CJVXVINMIXER_H__

#include "jvxVideoNodes/CjvxVideoNode.h"
#include "common/CjvxSingleInputConnector.h"
#include "common/CjvxSingleOutputConnector.h"
#include "common/CjvxConnectorCollection.h"

#include "pcg_exports_node.h"
#include "pcg_exports_node_selected.h"

// There was a problem with the RW locks at the beginning. However, this was fixed and there should no longer be a problem
#define RW_MUTEX

#ifdef JVX_PROJECT_NAMESPACE
namespace JVX_PROJECT_NAMESPACE {
#endif

class CjvxViNMixer: public CjvxVideoNode, 
	public CjvxConnectorCollection<CjvxSingleInputConnector, CjvxSingleInputConnectorMulti>,
	public CjvxConnectorCollection<CjvxSingleOutputConnector, CjvxSingleOutputConnectorMulti>, 
	public CjvxConnectorCollection_fwd_in_lock<CjvxSingleInputConnector>,
	public CjvxConnectorCollection_fwd_in_lock<CjvxSingleOutputConnector>,
	public CjvxConnectorCollection_lock,
	public CjvxViNMixer_genpcg, CjvxViNMixer_selected
{
private:
protected:

	class oneConnectorPrivateData
	{
	public:
		jvxByte* fldRgba32 = nullptr;
		jvxSize szFldRgba32 = 0;
		jvxSize uId = JVX_SIZE_UNSELECTED;
		std::string nameInput;
		void reset()
		{
			nameInput = "not-connected";
		}
	};
	
	struct
	{
		CjvxSimplePropsPars node;
	} video_input;

	struct
	{
		CjvxSimplePropsPars node;
	} video_output;

#ifdef RW_MUTEX
	JVX_RW_MUTEX_HANDLE safeCall;
	JVX_THREAD_ID tIdEx = 0;
	JVX_THREAD_ID tIdSh = 0;
#else
	JVX_MUTEX_HANDLE safeCall;
#endif


#ifdef JVX_OPEN_BMP_FOR_TEXT

	struct
	{

		jvxByte* bufRGBA32 = nullptr;
		jvxSize szBufRGBA32 = 0;
	} test;
#endif

	jvxSize uIdGen = 1;

public:

	// ===================================================================================================
	// ===================================================================================================
	JVX_CALLINGCONVENTION CjvxViNMixer(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	virtual JVX_CALLINGCONVENTION ~CjvxViNMixer();

	jvxErrorType select(IjvxObject* owner) override;
	jvxErrorType unselect() override;

	// ===========================================================================================

	jvxErrorType activate() override;
	jvxErrorType deactivate() override;

	// ===========================================================================================

	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage) override;

	// ===========================================================================================

	// This function MUST be implemented here since in <CjvxConnectorCollection>, this call is not implemented.
	// We need this call here to set the processing arguments
	jvxErrorType report_selected_connector(CjvxSingleInputConnector* iconn) override;

	jvxErrorType report_test_connector(CjvxSingleInputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;

	// The following two functions can be used to extend the implementation in the <CjvxConnectorCollection> class.
	// There, we should always refer the base class implementation as well to use the given functionality.
	// I will not implement these functions here but, instead, install the interface to do things "in the lock"
	// -> jvxErrorType report_started_connector(CjvxSingleInputConnector* iconn) override;
	//-> jvxErrorType report_stopped_connector(CjvxSingleInputConnector* iconn) override;

	jvxErrorType report_unselected_connector(CjvxSingleInputConnector* iconn) override;

	void report_process_buffers(CjvxSingleInputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage) override;

	// ===========================================================================================

	// This function MUST be implemented here since in <CjvxConnectorCollection>, this call is not implemented.
	// We need this call here to set the processing arguments
	jvxErrorType report_selected_connector(CjvxSingleOutputConnector* iconn) override;
	jvxErrorType report_test_connector(CjvxSingleOutputConnector* iconn JVX_CONNECTION_FEEDBACK_TYPE_A(fdb)) override;
	
	// The following two functions can be used to extend the implementation in the <CjvxConnectorCollection> class.
	// There, we should always refer the base class implementation as well to use the given functionality.
	// I will not implement these functions here but, instead, install the interface to do things "in the lock"
	// -> jvxErrorType report_started_connector(CjvxSingleOutputConnector* iconn) override;
	// -> jvxErrorType report_stopped_connector(CjvxSingleOutputConnector* iconn) override;

	jvxErrorType report_unselected_connector(CjvxSingleOutputConnector* iconn) override;

	void report_process_buffers(CjvxSingleOutputConnector* iconn, jvxLinkDataDescriptor& datThisConnector, jvxSize idx_stage) override;

	void report_real_master_connect(CjvxSingleInputConnector* ioconn) override;
	void report_real_master_disconnect(CjvxSingleInputConnector* ioconn) override;

	//=======================================================================================================
	
	void activate_connectors() override;
	void deactivate_connectors() override;

	// ===============================================================================================
	// ===============================================================================================
	// FORWARD INTERFACES FORWARD INTERFACES FORWARD INTERFACES FORWARD INTERFACES FORWARD INTERFACES
	// ===============================================================================================
	// ===============================================================================================

	virtual void lock(jvxBool rwExclusive = true, jvxSize idLock = JVX_SIZE_UNSELECTED) override;
	virtual void unlock(jvxBool rwExclusive = true, jvxSize idLock = JVX_SIZE_UNSELECTED) override;
	
	virtual void report_started_connector_in_lock(CjvxSingleInputConnector* iconnPlus, jvxHandle* priv) override;
	virtual void report_before_stop_connector_in_lock(CjvxSingleInputConnector* iconnPlus, jvxHandle* priv) override;

	virtual void report_started_connector_in_lock(CjvxSingleOutputConnector* oconnPlus, jvxHandle* priv) override;
	virtual void report_before_stop_connector_in_lock(CjvxSingleOutputConnector* oconnPlus, jvxHandle* priv) override;

	void updateExposedProperties();
	
};

#ifdef JVX_PROJECT_NAMESPACE
}
#endif

#endif
