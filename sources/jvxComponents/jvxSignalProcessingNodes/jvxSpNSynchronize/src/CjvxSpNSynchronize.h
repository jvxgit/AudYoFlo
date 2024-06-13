#ifndef __CJVXSPNSYNCHRONIZE_H__
#define __CJVXSPNSYNCHRONIZE_H__

#include "jvxNodes/CjvxBareNode1ioRearrange.h"
#include "CjvxSpNSynchronize_sec.h"

#include "pcg_exports_node.h"

class CjvxSpNSynchronize : public CjvxBareNode1ioRearrange, public genSynchronize
{
	friend class CjvxSpNSynchronize_sec;
protected:

	CjvxSpNSynchronize_sec sec_master;
	jvxSynchronizeBufferMode bufferMode = jvxSynchronizeBufferMode::JVX_SYNCHRONIZE_UNBUFFERED_PUSH;

public:
	JVX_CALLINGCONVENTION CjvxSpNSynchronize(JVX_CONSTRUCTOR_ARGUMENTS_MACRO_DECLARE);
	~CjvxSpNSynchronize();

	// ===================================================

	jvxErrorType select(IjvxObject* owner)override;
	jvxErrorType unselect()override;

	// ===================================================

	jvxErrorType activate()override;
	jvxErrorType deactivate()override;

	// =================================================================================
	// Extend interfaces by master connnector factory
	// =================================================================================
	jvxErrorType request_hidden_interface(jvxInterfaceType tp, jvxHandle** hdl)override;
	jvxErrorType return_hidden_interface(jvxInterfaceType tp, jvxHandle* hdl)override;

	// Negotiations
	jvxErrorType test_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

	jvxErrorType start_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType stop_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;
	jvxErrorType process_buffers_icon(jvxSize mt_mask, jvxSize idx_stage)override;

	void fwd_report_process_buffers(jvxLinkDataDescriptor& datLink, jvxSize idx);

	jvxErrorType prepare_connect_icon(JVX_CONNECTION_FEEDBACK_TYPE(fdb)) override;

};

	// ============================================================================

	

#endif